#pragma once
#include <JuceHeader.h>

class PlayerAudio {
private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)

public:
    PlayerAudio();
    ~PlayerAudio();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

    float prevGain;
    bool LoadFile(const juce::File& file);
    void pause();
    void play();
    void goEnd();
    void restart();
    void forward();
    void backward();
    void setGain(float gain);
    void setPosition(double pos);
    double getPosition() const;
    double getLength() const;
    void mute();
    bool isMuted;
    void setLooping(bool shouldLoop);
    bool isLooping() const;


    std::function<void(const juce::String&)>logBox;


};