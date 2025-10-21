#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGui : public juce::AudioAppComponent,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::Timer
{
public:
    explicit PlayerGui(PlayerAudio& playerRef);
    ~PlayerGui() override;

    // Audio callbacks (forward to player)
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    // UI events
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    void timerCallback() override;

    void statusBoxMessage(const juce::String& message);

private:
    PlayerAudio& player;

    // buttons
    juce::TextButton loadButton{ "Load Files" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton pauseButton{ "Pause" };
    juce::TextButton playButton{ "Play" };
    juce::TextButton goEndButton{ "End" };
    juce::TextButton loopButton{ "Loop" };
    juce::TextButton muteButton{ "Mute" };

    // displays
    juce::TextEditor statusBox;

    // sliders
    juce::Slider volumeSlider;
    juce::Slider speedSlider;
    juce::Slider positionSlider;
    juce::Label timeLabel;

    std::unique_ptr<juce::FileChooser> fileChooser;

    bool userDraggingPosition{ false };
    bool isMutedLocal{ false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGui)
};
