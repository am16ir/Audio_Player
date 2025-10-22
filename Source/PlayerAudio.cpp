#include "PlayerAudio.h"

PlayerAudio::PlayerAudio() {
	formatManager.registerBasicFormats();

}

PlayerAudio::~PlayerAudio() {
    transportSource.stop();
    transportSource.setSource(nullptr);
    readerSource.reset();
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    transportSource.getNextAudioBlock(bufferToFill);
}

void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
}

bool PlayerAudio::LoadFile(const juce::File& file) {
    if (file.existsAsFile())
    {
        if (auto* reader = formatManager.createReaderFor(file))
        {
            //  Disconnect old source first
            transportSource.stop();
            transportSource.setSource(nullptr);
            readerSource.reset();

            // Create new reader source
            readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

            // Attach safely
            transportSource.setSource(readerSource.get(),
                0,
                nullptr,
                reader->sampleRate);
            transportSource.start();

            if (logBox) {
                logBox("File name: " + file.getFileName());

                if (reader->metadataValues.getAllKeys() > 0) {
                    for (auto &key : reader->metadataValues.getAllKeys()) {
                        logBox("Metadata: " + key + " : " + reader->metadataValues[key]);
					}
                }
				double duration = reader->lengthInSamples / reader->sampleRate;
                int min = duration / 60; int sec = (int)duration % 60; int hours = duration / 3600;
                if (hours > 0)
                    logBox("Duration: " + juce::String(hours) + ":" + juce::String(min % 60) + ":" + juce::String(sec));
                else if (min > 0)
                    logBox("Duration: " + juce::String(min) + ":" + juce::String(sec));
                else
					logBox("Duration: " + juce::String(sec) + "s");
            }
            return true;
        }
    }
    if (logBox)
        logBox("Error: could not open file " + file.getFileName());
    return false;
}

void PlayerAudio::pause() {
    transportSource.stop();
}

void PlayerAudio::play() {
    transportSource.start();
}

void PlayerAudio::restart() {
    transportSource.setPosition(0.0);
    transportSource.start();
}

void PlayerAudio::setGain(float gain) {
    transportSource.setGain(gain);
}

void PlayerAudio::setPosition(double pos) {
    transportSource.setPosition(pos);
}

double PlayerAudio::getPosition() const {
    return transportSource.getCurrentPosition();
}

double PlayerAudio::getLength() const {
    return transportSource.getLengthInSeconds();
}
void PlayerAudio::goEnd() {
    return transportSource.setPosition(getLength());
}

void PlayerAudio::setLooping(bool shouldLoop) {
    if (readerSource) {
        transportSource.setLooping(shouldLoop);
    }
}


bool PlayerAudio::isLooping() const {
    if (readerSource) {
        return transportSource.isLooping();
    }
    return false;
}

void PlayerAudio::forward() {
	double pos = getPosition();
    if (pos + 10 <= getLength())
        setPosition(pos + 10);
    else
        setPosition(getLength());
}
void PlayerAudio::backward() {
    double pos = getPosition();
    if (pos >= 10)
        setPosition(pos - 10);
    else
        setPosition(0.0);
}

void PlayerAudio::mute() {
    if (isMuted)
        transportSource.setGain(0.0f);
    else
        transportSource.setGain(prevGain);
}

