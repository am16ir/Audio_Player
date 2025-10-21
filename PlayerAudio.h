#pragma once

#include <JuceHeader.h>
#include <functional>
#include <memory>

#ifdef SOUND_TOUCH_AVAILABLE
// إذا فعلت SoundTouch: تأكد أن المسار صحيح
#include "SoundTouch.h"
using namespace soundtouch;
#endif

class PlayerAudio
{
public:
    PlayerAudio();
    ~PlayerAudio();

    // audio lifecycle (called by AudioAppComponent)
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void releaseResources();
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);

    // controls
    bool LoadFile(const juce::File& file); // returns true if loaded successfully
    void play();
    void pause();
    void restart();
    void goEnd();

    void setGain(float gain);
    void setPosition(double pos); // seconds
    double getPosition() const;
    double getLength() const;

    void setLooping(bool shouldLoop);
    bool isLooping() const;

    void setSpeed(double ratio); // 1.0 = normal
    double getSpeed() const;

    void setMute(bool shouldMute);
    bool isMuted() const;

    bool isLoaded() const;

    // logging callback (GUI sets this)
    std::function<void(const juce::String&)> logBox;

private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;

    // TransportSource used to manage reading/position
    juce::AudioTransportSource transportSource;

    // If SoundTouch not present we use ResamplingAudioSource to change rate (this changes pitch).
    std::unique_ptr<juce::ResamplingAudioSource> resampler; // wraps transportSource

#ifdef SOUND_TOUCH_AVAILABLE
    // SoundTouch path (time-stretch preserving pitch)
    std::unique_ptr<SoundTouch> soundTouch;
    juce::AudioBuffer<float> scratchBuffer; // temp for soundtouch I/O if needed
#endif

    // state
    bool loaded{ false };
    bool muted{ false };
    float lastGainBeforeMute{ 0.8f };

    // remember last prepare values to prepare resampler/soundtouch if created later
    int lastBlockSizeExpected{ 0 };
    double lastSampleRate{ 0.0 };
};
