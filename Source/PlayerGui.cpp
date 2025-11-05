#include "PlayerGui.h"
#include <vector>
using namespace std;


PlayerGui::PlayerGui()
    : waveformDisplay(player1, player1.getFormatManager()) // *****Sayed******

{
    for (auto* btn : { &loadButton, &restartButton , &pauseButton ,&goEndButton ,&playButton ,
        &forward ,&backward,&loopButton,&mute,&startingpoint,&endingpoint,&Deletepoints,&looponpoints,
        &markerButton,&clearMarkers,&clear,&removeSelected })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }
    mute.setClickingTogglesState(true);
    looponpoints.setClickingTogglesState(true);

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
    //setAudioChannels(0, 2);

    statusBox.setMultiLine(true);
    statusBox.setReadOnly(true);
    statusBox.setScrollbarsShown(true);
    addAndMakeVisible(statusBox);

    // position slider 
    positionslider.setRange(0.0, 1, 0.01);
    positionslider.setValue(0.0);
    positionslider.addListener(this);
    addAndMakeVisible(positionslider);

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
        player1.complete(fileinfo.get_filename());

        looponpoints.setToggleState(false, juce::dontSendNotification);
        looponpoints.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
        player1.clearSegmentPoints();
        };

    startTimer(50);
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
    loopButton.setBounds(420, y, 80, 40);
    mute.setBounds(420, 70, 80, 40);
    startingpoint.setBounds(520, y, 80, 40);
    endingpoint.setBounds(520, 70, 80, 40);
    looponpoints.setBounds(630, 20, 80, 40);
    Deletepoints.setBounds(630, 70, 80, 40);
    markerButton.setBounds(730, 20, 80, 40);
    clearMarkers.setBounds(730, 70, 80, 40);


    waveformDisplay.setBounds(20, 130, getWidth() - 40, 80);// ****Sayed****

    volumeSlider.setBounds(20, 220, getWidth() - 40, 30);
    positionslider.setBounds(20, 260, getWidth() - 40, 30);
    speedSlider.setBounds(20, 300, getWidth() - 40, 30); // ****Sayed****

    statusBox.setBounds(20, 390, 350, 70);
    mytable.setBounds(400, 390, 360, 150);
    clear.setBounds(400, 350, 80, 30);
    removeSelected.setBounds(500, 350, 120, 30);



}

void PlayerGui::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::lightblue);
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

                    looponpoints.setToggleState(false, juce::dontSendNotification);
                    looponpoints.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
                    player1.clearSegmentPoints();
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

    if (button == &startingpoint) {
        player1.setStartPoint(player1.getPosition());
        statusBoxMessage("Point A set to: " + juce::String(player1.getStartPoint()) + "s");
    }
    if (button == &endingpoint) {
        player1.setEndPoint(player1.getPosition());
        statusBoxMessage("Point B set to: " + juce::String(player1.getEndPoint()) + "s");
    }
    if (button == &Deletepoints) {
        player1.clearSegmentPoints();
        looponpoints.setToggleState(false, juce::dontSendNotification);
        looponpoints.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
        statusBoxMessage("Segment loop points cleared.");
    }
    if (button == &looponpoints) {
        bool newState = looponpoints.getToggleState();

        double A = player1.getStartPoint();
        double B = player1.getEndPoint();
        double L = player1.getLength();

        if (B > A && L > 0.0) {
            player1.setSegmentLooping(newState);
            if (newState) {
                button->setColour(juce::TextButton::buttonColourId, juce::Colours::red);
                statusBoxMessage("Segment looping ACTIVATED: " + juce::String(A) + "s to " + juce::String(B) + "s");
            }
            else {
                button->setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
                statusBoxMessage("Segment looping DEACTIVATED.");
            }
        }
        else {
            player1.setSegmentLooping(false);
            looponpoints.setToggleState(false, juce::dontSendNotification);
            button->setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
            statusBoxMessage("Cannot activate segment loop: Set valid A and B points first (A < B).");
        }
        button->repaint();
    }
    if (button == &markerButton) {
        double pos = player1.getPosition();
        juce::String markerName = "Marker at " + juce::String(pos, 1) + "s";
        player1.markers[player1.currentFileName].push_back(pos);
        statusBoxMessage("Added " + markerName);
        repaint();
    }
    if (button == &clearMarkers) {
        player1.clearMarkers();
        statusBoxMessage("All markers cleared.");
        repaint();
    }
    if (button == &clear) {
        model.clear();
        player1.goEnd();
        mytable.updateContent();
    }
    if (button == &removeSelected) {
        int selectedRow = mytable.getSelectedRow();
        if (selectedRow >= 0) {
            juce::String filename = model.files[selectedRow].first;
            if (filename == player1.currentFileName) {
                player1.goEnd();
                player1.currentFileName = "";
                positionslider.setValue(0.0);
            }
            model.remove(selectedRow);
            mytable.updateContent();
            statusBoxMessage("Removed selected file from the list.");
        }
        else {
            statusBoxMessage("No row selected to remove.");
        }
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

void PlayerGui::sliderDragStarted(juce::Slider* slider)
{
    if (slider == &positionslider)
    {
        isSliderDragging = true;
    }
    // Add speed slider handling ****Sayed****
    if (slider == &speedSlider)
    {
        player1.setSpeed((float)slider->getValue());
    }
}

void PlayerGui::sliderDragEnded(juce::Slider* slider)
{
    if (slider == &positionslider)
    {
        isSliderDragging = false;
        double totalLength = player1.getLength();
        if (totalLength > 0.0) {
            double newPosition = slider->getValue();
            player1.setPosition(newPosition);

            if (player1.getSegmentLooping()) {
                if (newPosition < player1.getStartPoint() || newPosition >= player1.getEndPoint()) {
                    player1.setPosition(player1.getStartPoint());
                }
            }
        }
    }
}


void PlayerGui::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider && !player1.isMuted) {
        player1.setGain((float)slider->getValue());
        player1.prevGain = slider->getValue();
    }
    if (slider == &positionslider) {
        double totalLength = player1.getLength();
        if (totalLength > 0.0) {
            double newPosition = slider->getValue();
            player1.setPosition(newPosition);

        }
    }
}

void PlayerGui::statusBoxMessage(const juce::String& message) {
    statusBox.insertTextAtCaret(message + "\n");
}

void PlayerGui::timerCallback()
{
    double currentPosition = player1.getPosition();
    double totalLength = player1.getLength();

    if (player1.getSegmentLooping() && totalLength > 0.0)
    {
        double A = player1.getStartPoint();
        double B = player1.getEndPoint();

        if (currentPosition >= B)
        {
            player1.setPosition(A);
            currentPosition = A;
        }
    }



    if (totalLength > 0.0)
    {
        if (!isSliderDragging) {
            double sliderValue = currentPosition;
            positionslider.setRange(0.0, totalLength, 1.0);
            positionslider.setValue(sliderValue, juce::dontSendNotification);
        }
    }
}

void PlayerGui::paintOverChildren(juce::Graphics& g) {
    auto totalLength = player1.getLength();
    if (totalLength <= 0.0)
        return;

    auto it = player1.markers.find(player1.currentFileName);
    if (it == player1.markers.end())
        return;

    g.setColour(juce::Colours::orange);

    auto sliderBounds = positionslider.getBounds();


    for (auto& pos : it->second) {

        double fraction = (pos) / totalLength;
        fraction = juce::jlimit(0.0, 1.0, fraction);


        int x = positionslider.getPositionOfValue(pos);


        g.fillRect(x + 15, sliderBounds.getY(), 2, sliderBounds.getHeight());
    }
}