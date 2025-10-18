#include "PlayerGui.h"


PlayerGui::PlayerGui() {
    for (auto* btn : { &loadButton, &restartButton , &pauseButton ,&goEndButton ,&playButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    // Volume slider
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    setSize(1000, 500);
    setAudioChannels(0, 2);

	statusBox.setMultiLine(true);
	statusBox.setReadOnly(true);
    statusBox.setScrollbarsShown(true);
	addAndMakeVisible(statusBox);

    player1.logBox = [this](const juce::String& message)
    {
            statusBoxMessage(message); 
    };

}


PlayerGui::~PlayerGui()
{
    shutdownAudio();
}



void PlayerGui::resized()
{
    int y = 20;
    loadButton.setBounds(20, y, 80, 40);
    restartButton.setBounds(120, y, 80, 40);
    pauseButton.setBounds(220, y, 80, 40);
    playButton.setBounds(220, 60, 80, 40);
    goEndButton.setBounds(320, y, 80, 40);
	statusBox.setBounds(20, 140, getWidth() - 40, 40);
    volumeSlider.setBounds(20, 100, getWidth() - 40, 30);
}

void PlayerGui::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void PlayerGui::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        juce::FileChooser chooser("Select audio files...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file.existsAsFile()) {
                    statusBox.clear();
                    player1.LoadFile(file);                    
                    player1.play();
                }
            });
    }

    if (button == &restartButton)
    {
        player1.restart();
    }

    if (button == &pauseButton)
    {
        player1.pause();
    }
    if (button == &playButton) {
        player1.play();
    }
  
    if (button == &goEndButton) {
        player1.goEnd();
    }

}


void PlayerGui::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerGui::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    player1.getNextAudioBlock(bufferToFill);
}

void PlayerGui::releaseResources()
{
    player1.releaseResources();
}

void PlayerGui::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
        player1.setGain((float)slider->getValue());
}
void PlayerGui::statusBoxMessage(const juce::String& message) {
    statusBox.insertTextAtCaret(message + "\n");
}
