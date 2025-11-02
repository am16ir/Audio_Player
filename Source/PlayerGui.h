#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"


class PlayerGui	: public juce::AudioAppComponent,
				  public juce::Button::Listener,
				  public juce::Slider::Listener,
				  public juce::Timer
{
private:

	PlayerAudio player1;
	juce::TextButton loadButton{ "Load Files" };
	juce::TextButton restartButton{ "Restart" };
	juce::TextButton pauseButton{ "Pause" };
	juce::TextButton playButton{ "Play" };
	juce::TextButton goEndButton{ "End" };
	juce::TextButton forward{ "Forward  +10" };
	juce::TextButton backward{ "Backward -10" };
	juce::TextButton loopButton{ "Loop" };
	juce::TextButton mute{ "Mute" };
	juce::TextEditor statusBox;
	juce::Slider volumeSlider;
	juce::Slider positionslider;

	std::unique_ptr<juce::FileChooser> fileChooser;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGui)

public:
	PlayerGui();
	~PlayerGui()override;


	void prepareToPlay(int samplesPerBlockExpected, double sampleRate)override;
	void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)override;
	void releaseResources()override;
	
	void paint(juce::Graphics& g) override;
	void resized() override;

	void buttonClicked(juce::Button* button)override;
	void sliderValueChanged(juce::Slider* slider)override;

	void timerCallback() override;

	void statusBoxMessage(const juce::String& message);
};
