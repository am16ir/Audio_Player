#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"


class PlayerGui	: public juce::AudioAppComponent,
				  public juce::Button::Listener,
				  public juce::Slider::Listener 
{
private:

	PlayerAudio player1;
	juce::TextButton loadButton{ "Load Files" };
	juce::TextButton restartButton{ "Restart" };
	juce::TextButton stopButton{ "Stop" };
	juce::TextButton prevButton{ "Previous" };
	juce::TextButton nextButton{ "Next" };
	juce::Slider volumeSlider;

	std::unique_ptr<juce::FileChooser> fileChooser;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGui)

public:
	PlayerGui();
	~PlayerGui();


	void prepareToPlay(int samplesPerBlockExpected, double sampleRate)override;
	void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)override;
	void releaseResources()override;
	
	void paint(juce::Graphics& g) override;
	void resized() override;

	void buttonClicked(juce::Button* button)override;
	void sliderValueChanged(juce::Slider* slider)override;


};