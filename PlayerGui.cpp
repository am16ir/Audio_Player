#include "PlayerGui.h"
#include <vector>
using namespace std;

PlayerGui::PlayerGui()
    : waveformDisplay(player1, player1.getFormatManager()) // *****Sayed******
{
    for (auto* btn : { &loadButton, &restartButton , &pauseButton ,&goEndButton ,&playButton ,
        &forward ,&backward,&loopButton,&mute })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }
    mute.setClickingTogglesState(true);

    // Volume slider
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    // Speed slider ****sayed****
    speedSlider.setRange(0.25, 4.0, 0.05); // Range from 0.25x to 4.0x speed
    speedSlider.setValue(1.0); // Default normal speed
    speedSlider.addListener(this);
    speedSlider.setSkewFactorFromMidPoint(1.0); // Makes slider more sensitive around normal speed
    speedSlider.setTextValueSuffix("x"); // Display suffix
    addAndMakeVisible(speedSlider);

    addAndMakeVisible(waveformDisplay);	// ******Sayed******

    setSize(850, 600);//***Editing by sayed
    setAudioChannels(0, 2);

    statusBox.setMultiLine(true);
    statusBox.setReadOnly(true);
    statusBox.setScrollbarsShown(true);
    addAndMakeVisible(statusBox);


    // Table
    mytable.getHeader().addColumn("Filename", 1, 180);
    mytable.getHeader().addColumn("Duration", 2, 180);
    mytable.setModel(&model);
    addAndMakeVisible(mytable);
    model.doubleClick = [this](juce::String filename) {
        info fileinfo = player1.LoadFile(juce::File(filename));
        statusBox.clear();
        statusBoxMessage("FILE: " + fileinfo.get_filename());
        statusBoxMessage(fileinfo.get_duration());

        // NEW: Update waveform display when file is loaded
        waveformDisplay.setFile(juce::File(filename));

        player1.complete(fileinfo.get_filename());
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
    backward.setBounds(120, y, 80, 40);
    forward.setBounds(120, 70, 80, 40);
    pauseButton.setBounds(220, y, 80, 40);
    playButton.setBounds(220, 70, 80, 40);
    restartButton.setBounds(320, y, 80, 40);
    goEndButton.setBounds(320, 70, 80, 40);
    statusBox.setBounds(20, 150, getWidth() - 40, 40);
    loopButton.setBounds(420, y, 80, 40);
    mute.setBounds(420, 70, 80, 40);


    waveformDisplay.setBounds(20, 120, getWidth() - 40, 80);// ****Sayed****
    speedSlider.setBounds(20, 250, getWidth() - 40, 30); // ****Sayed****

    statusBox.setBounds(20, 300, 350, 70);
    volumeSlider.setBounds(20, 220, getWidth() - 40, 30);
    mytable.setBounds(400, 300, 360, 150);
}

void PlayerGui::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);

    // Add label for speed slider     ****Sayed****
    g.setColour(juce::Colours::white);
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


                    info fileinfo = player1.LoadFile(file);
                    statusBoxMessage("FILE: " + fileinfo.get_filename());
                    statusBoxMessage(fileinfo.get_duration());
                    statusBoxMessage(fileinfo.get_metadata());

                    //  waveform Sayed
                    waveformDisplay.setFile(file);

                    model.files.push_back({ fileinfo.get_path(),fileinfo.get_duration() });
                    mytable.updateContent();
                }
                else {
                    statusBoxMessage("Choose Again! ");
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
    if (button == &forward) {
        player1.forward();
    }
    if (button == &backward) {
        player1.backward();
    }
    if (button == &loopButton)
    {
        bool newState = !player1.isLooping();
        player1.setLooping(newState);

        if (newState) {
            button->setColour(juce::TextButton::buttonColourId, juce::Colours::darkblue);
        }
        else {
            button->setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
        }
        button->repaint();
    }
    if (button == &mute) {
        player1.isMuted = mute.getToggleState();
        player1.mute();
        if (player1.isMuted)
            mute.setButtonText("Unmute");
        else
            mute.setButtonText("Mute");
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
    if (slider == &volumeSlider && !player1.isMuted)
        player1.setGain((float)slider->getValue());
    player1.prevGain = slider->getValue();

    // Add speed slider handling ****Sayed****
    if (slider == &speedSlider)
    {
        player1.setSpeed((float)slider->getValue());
    }
}

void PlayerGui::statusBoxMessage(const juce::String& message) {
    statusBox.insertTextAtCaret(message + "\n");
}