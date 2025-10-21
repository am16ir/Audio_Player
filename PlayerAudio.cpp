#include "PlayerAudio.h"

PlayerAudio::PlayerAudio()
{
    formatManager.registerBasicFormats();
}

PlayerAudio::~PlayerAudio()
{
    // stop and disconnect before destroying sources
    transportSource.stop();
    transportSource.setSource(nullptr);

    resampler.reset();

#ifdef SOUND_TOUCH_AVAILABLE
    soundTouch.reset();
    scratchBuffer.setSize(0, 0);
#endif

    readerSource.reset();
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    lastBlockSizeExpected = samplesPerBlockExpected;
    lastSampleRate = sampleRate;

    // prepare transport (transport reads from readerSource)
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

    // if we have resampler (non-SoundTouch path), prepare it too
    if (resampler)
        resampler->prepareToPlay(samplesPerBlockExpected, sampleRate);

#ifdef SOUND_TOUCH_AVAILABLE
    if (soundTouch)
    {
        soundTouch->setSampleRate((uint)lastSampleRate);
        // allocate scratch buffer for safety if needed
        scratchBuffer.setSize(2, lastBlockSizeExpected); // we'll resize later dynamically if needed
    }
#endif
}

void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
    if (resampler)
        resampler->releaseResources();

#ifdef SOUND_TOUCH_AVAILABLE
    // nothing special
    scratchBuffer.setSize(0, 0);
#endif
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();

    if (!loaded || !readerSource)
        return;

#ifdef SOUND_TOUCH_AVAILABLE
    // If SoundTouch available and configured, use time-stretch processing path
    if (soundTouch)
    {
        // We'll read raw samples via transportSource into a temp buffer, feed soundTouch, then pull output.
        // Prepare temp buffer sized by requested frames and channels
        const int outChannels = bufferToFill.buffer->getNumChannels();
        const int numFrames = bufferToFill.numSamples;

        scratchBuffer.setSize(juce::jmax(fileNumChannels, outChannels), numFrames, false, true, true);

        // Read raw frames: use readerSource directly (it is PositionableAudioSource)
        juce::AudioSourceChannelInfo tmpReadInfo(&scratchBuffer, 0, numFrames);
        // Important: readerSource->getNextAudioBlock will advance its internal position
        readerSource->getNextAudioBlock(tmpReadInfo);

        // interleave and feed soundTouch
        // SoundTouch expects interleaved floats if channels>1, but its C++ interface also supports putSamples with planar? We'll use interleaved.
        // Build interleaved vector
        std::vector<float> interleaved((size_t)numFrames * (size_t)fileNumChannels);
        for (int ch = 0; ch < fileNumChannels; ++ch)
        {
            const float* src = scratchBuffer.getReadPointer(ch);
            for (int n = 0; n < numFrames; ++n)
                interleaved[(size_t)n * fileNumChannels + ch] = src[n];
        }

        // feed to soundTouch
        soundTouch->putSamples(interleaved.data(), numFrames);

        // receive processed frames (up to numFrames)
        const int maxOutFrames = numFrames * 2; // safe upper bound
        std::vector<float> outInterleaved((size_t)maxOutFrames * (size_t)fileNumChannels);
        int got = soundTouch->receiveSamples(outInterleaved.data(), maxOutFrames);

        if (got <= 0)
        {
            // no samples ready => output silence
            return;
        }

        // deinterleave to output buffer
        const int useCh = juce::jmin(outChannels, fileNumChannels);
        for (int ch = 0; ch < useCh; ++ch)
        {
            float* dst = bufferToFill.buffer->getWritePointer(ch, bufferToFill.startSample);
            int n = 0;
            for (; n < got && n < bufferToFill.numSamples; ++n)
                dst[n] = outInterleaved[(size_t)n * fileNumChannels + ch];
            // zero remainder if any
            for (; n < bufferToFill.numSamples; ++n)
                dst[n] = 0.0f;
        }

        // apply gain/mute
        float gain = muted ? 0.0f : lastGainBeforeMute;
        if (gain != 1.0f)
            bufferToFill.buffer->applyGain(0, bufferToFill.startSample, bufferToFill.numSamples, gain);

        return;
    }
#endif

    // Fallback (ResamplingAudioSource path): use resampler wrapping transportSource
    if (resampler)
    {
        resampler->getNextAudioBlock(bufferToFill);
        // apply mute/gain (transportSource's gain already applied if setGain used)
        if (muted)
            bufferToFill.buffer->applyGain(0, bufferToFill.startSample, bufferToFill.numSamples, 0.0f);
        return;
    }

    // If neither path exists, try to read directly from transportSource
    transportSource.getNextAudioBlock(bufferToFill);
    if (muted)
        bufferToFill.buffer->applyGain(0, bufferToFill.startSample, bufferToFill.numSamples, 0.0f);
}

bool PlayerAudio::LoadFile(const juce::File& file)
{
    if (!file.existsAsFile())
    {
        if (logBox) logBox("Error: file does not exist - " + file.getFullPathName());
        loaded = false;
        return false;
    }

    // create reader (owned by AudioFormatReaderSource when we pass true)
    if (auto* reader = formatManager.createReaderFor(file))
    {
        // stop & detach previous
        transportSource.stop();
        transportSource.setSource(nullptr);

        // reset existing
#ifdef SOUND_TOUCH_AVAILABLE
        soundTouch.reset();
        scratchBuffer.setSize(0, 0);
#endif
        resampler.reset();
        readerSource.reset();

        readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

        // Attach readerSource to transportSource (PositionableAudioSource)
        transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);

#ifdef SOUND_TOUCH_AVAILABLE
        // create soundTouch and configure
        soundTouch = std::make_unique<SoundTouch>();
        soundTouch->setSampleRate((uint)reader->sampleRate);
        soundTouch->setChannels(juce::jmax(1, reader->numChannels));
        soundTouch->setTempo(1.0f);
        soundTouch->setPitchSemiTones(0.0f);

        // prepare scratch buffer
        scratchBuffer.setSize(juce::jmax(1, reader->numChannels), juce::jmax(512, lastBlockSizeExpected > 0 ? lastBlockSizeExpected : 512));
#else
        // create resampler that wraps transportSource so we can change resampling ratio
        // note: deleteInputWhenDeleted = false because transportSource is owned here
        resampler = std::make_unique<juce::ResamplingAudioSource>(&transportSource, false, reader->numChannels);

        // if we already had prepareToPlay called earlier, prepare resampler
        if (lastSampleRate > 0.0 && lastBlockSizeExpected > 0)
            resampler->prepareToPlay(lastBlockSizeExpected, lastSampleRate);
#endif

        // set defaults
        transportSource.setGain(muted ? 0.0f : lastGainBeforeMute);
        transportSource.setPosition(0.0);

        loaded = true;

        if (logBox)
        {
            logBox("Loaded file: " + file.getFileName());
            auto keys = reader->metadataValues.getAllKeys();
            if (keys.size() > 0)
            {
                for (auto& k : keys)
                    logBox("Metadata: " + k + " : " + reader->metadataValues[k]);
            }
            double duration = 0.0;
            if (reader->sampleRate > 0.0)
                duration = static_cast<double>(reader->lengthInSamples) / reader->sampleRate;

            int totalSec = static_cast<int>(std::round(duration));
            int hours = totalSec / 3600;
            int minutes = (totalSec / 60) % 60;
            int seconds = totalSec % 60;

            if (hours > 0)
                logBox("Duration: " + juce::String(hours) + ":" + juce::String(minutes).paddedLeft('0', 2) + ":" + juce::String(seconds).paddedLeft('0', 2));
            else if (minutes > 0)
                logBox("Duration: " + juce::String(minutes) + ":" + juce::String(seconds).paddedLeft('0', 2));
            else
                logBox("Duration: " + juce::String(seconds) + "s");
        }

        return true;
    }

    if (logBox)
        logBox("Error: could not create reader for file " + file.getFileName());

    loaded = false;
    return false;
}

void PlayerAudio::play()
{
    // For resampler path we start transportSource; for SoundTouch we read from readerSource inside getNextAudioBlock
    transportSource.start();
}

void PlayerAudio::pause()
{
    transportSource.stop();
#ifdef SOUND_TOUCH_AVAILABLE
    if (soundTouch)
        soundTouch->clear();
#endif
}

void PlayerAudio::restart()
{
    if (readerSource)
    {
        readerSource->setNextReadPosition(0);
#ifdef SOUND_TOUCH_AVAILABLE
        if (soundTouch) soundTouch->clear();
#endif
    }
}

void PlayerAudio::goEnd()
{
    double len = getLength();
    if (len > 0.0)
    {
        readerSource->setNextReadPosition(readerSource->getTotalLength());
#ifdef SOUND_TOUCH_AVAILABLE
        if (soundTouch) soundTouch->clear();
#endif
    }
    transportSource.stop();
}

void PlayerAudio::setGain(float gain)
{
    lastGainBeforeMute = gain;
    if (!muted)
        transportSource.setGain(juce::jlimit(0.0f, 1.0f, gain));
}

void PlayerAudio::setPosition(double pos)
{
    if (readerSource)
        readerSource->setNextReadPosition(static_cast<int64_t>(pos * (readerSource->getAudioFormatReader()->sampleRate)));
}

double PlayerAudio::getPosition() const
{
    if (readerSource)
        return (double)readerSource->getNextReadPosition() / (readerSource->getAudioFormatReader()->sampleRate > 0.0 ? readerSource->getAudioFormatReader()->sampleRate : 1.0);
    return 0.0;
}

double PlayerAudio::getLength() const
{
    if (readerSource)
        return (readerSource->getAudioFormatReader()->lengthInSamples > 0 && readerSource->getAudioFormatReader()->sampleRate > 0.0)
        ? static_cast<double>(readerSource->getAudioFormatReader()->lengthInSamples) / readerSource->getAudioFormatReader()->sampleRate
        : 0.0;
    return 0.0;
}

void PlayerAudio::setLooping(bool shouldLoop)
{
    if (readerSource)
        readerSource->setLooping(shouldLoop);
}

bool PlayerAudio::isLooping() const
{
    if (readerSource)
        return readerSource->isLooping();
    return false;
}

void PlayerAudio::setSpeed(double ratio)
{
#ifdef SOUND_TOUCH_AVAILABLE
    if (soundTouch)
        soundTouch->setTempo((float)juce::jlimit(0.01, 4.0, ratio));
    else
        ; // soundTouch not initialized
#else
    if (resampler)
        resampler->setResamplingRatio(juce::jlimit(0.01, 4.0, ratio));
#endif
}

double PlayerAudio::getSpeed() const
{
#ifdef SOUND_TOUCH_AVAILABLE
    if (soundTouch) return soundTouch->getTempo();
    return 1.0;
#else
    if (resampler) return resampler->getResamplingRatio();
    return 1.0;
#endif
}

void PlayerAudio::setMute(bool shouldMute)
{
    muted = shouldMute;
    if (muted)
        transportSource.setGain(0.0f);
    else
        transportSource.setGain(juce::jlimit(0.0f, 1.0f, lastGainBeforeMute));
}

bool PlayerAudio::isMuted() const
{
    return muted;
}

bool PlayerAudio::isLoaded() const
{
    return loaded;
}
