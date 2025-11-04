#include "PlayerGui.h"
#include <vector>
using namespace std;

// ****Sayed**** -Start Waveform
WaveformDisplay::WaveformDisplay(PlayerAudio& audio) :
    playerAudio(audio),
    thumbnail(512, formatManager, thumbnailCache)
{
    formatManager.registerBasicFormats();
    thumbnail.addChangeListener(this);
    startTimer(40);
}

WaveformDisplay::~WaveformDisplay()
{
    thumbnail.removeChangeListener(this);
}

void WaveformDisplay::paint(juce::Graphics& g)
{
    auto area = getLocalBounds().toFloat();

    g.setColour(juce::Colours::darkgrey.darker(0.5f));
    g.fillRoundedRectangle(area, 4.0f);

    if (thumbnail.getTotalLength() > 0.0)
    {
        auto audioLength = thumbnail.getTotalLength();
        auto drawArea = area.reduced(2.0f);

        auto intDrawArea = drawArea.toNearestInt();

        auto playHeadPosition = juce::jmin(playerAudio.getPosition() / audioLength, 1.0);

        g.setColour(juce::Colours::lightgrey.withAlpha(0.4f));
        thumbnail.drawChannel(g, intDrawArea, 0.0, audioLength, 0, 0.4f);

        if (playHeadPosition > 0.0)
        {
            auto playedWidth = static_cast<int>(intDrawArea.getWidth() * playHeadPosition);
            auto playedArea = intDrawArea.withWidth(playedWidth);
            g.setColour(juce::Colours::lightblue.withAlpha(0.7f));
            thumbnail.drawChannel(g, playedArea, 0.0, audioLength, 0, 0.8f);
        }

        g.setColour(juce::Colours::red);
        auto playHeadX = drawArea.getX() + (drawArea.getWidth() * playHeadPosition);
        g.drawLine(playHeadX, drawArea.getY(), playHeadX, drawArea.getBottom(), 2.0f);

        drawTimeMarkers(g, drawArea);
    }
    else
    {
        g.setColour(juce::Colours::lightgrey);
        g.setFont(14.0f);
        g.drawFittedText("No audio file loaded", getLocalBounds(), juce::Justification::centred, 1);
    }

    g.setColour(juce::Colours::white.withAlpha(0.3f));
    g.drawRoundedRectangle(area, 4.0f, 1.0f);
}

void WaveformDisplay::loadFile(const juce::File& file)
{
    thumbnail.setSource(new juce::FileInputSource(file));
    repaint();
}

void WaveformDisplay::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &thumbnail)
        repaint();
}

void WaveformDisplay::timerCallback()
{
    repaint();
}

void WaveformDisplay::mouseDown(const juce::MouseEvent& event)
{
    if (thumbnail.getTotalLength() > 0.0)
    {
        auto clickPosition = event.position.x;
        auto proportion = clickPosition / getWidth();
        auto newPosition = proportion * thumbnail.getTotalLength();
        playerAudio.setPosition(newPosition);
    }
}

void WaveformDisplay::drawTimeMarkers(juce::Graphics& g, juce::Rectangle<float> area)
{
    auto duration = thumbnail.getTotalLength();
    if (duration <= 0) return;

    g.setColour(juce::Colours::white.withAlpha(0.5f));
    g.setFont(10.0f);

    for (int seconds = 30; seconds < duration; seconds += 30)
    {
        auto xPos = area.getX() + (seconds / duration) * area.getWidth();
        g.drawLine(xPos, area.getBottom() - 8, xPos, area.getBottom(), 1.0f);

        if (seconds % 60 == 0)
        {
            auto minutes = seconds / 60;
            auto timeText = juce::String(minutes) + ":" + juce::String(seconds % 60).paddedLeft('0', 2);
            g.drawText(timeText, xPos - 20, area.getBottom() - 20, 40, 12, juce::Justification::centred);
        }
    }
}

//***Sayed***  end waveform display class




PlayerGui::PlayerGui() : waveformDisplay(player1)// **** editing by Sayed**** 

{
    for (auto* btn : { &loadButton, &restartButton, &pauseButton, &goEndButton, &playButton,
        &forward, &backward, &loopButton, &mute })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }
    mute.setClickingTogglesState(true);

    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    // ****Sayed**** 
    speedSlider.setRange(0.25, 4.0, 0.05);
    speedSlider.setValue(1.0);
    speedSlider.addListener(this);
    // ****Sayed**** 
    speedSlider.setSkewFactorFromMidPoint(1.0);
    // ****Sayed**** 
    speedSlider.setTextValueSuffix("x");
    addAndMakeVisible(speedSlider);

    // ****Sayed**** 
    currentTimeLabel.setText("0:00", juce::dontSendNotification);
    currentTimeLabel.setJustificationType(juce::Justification::centred);
    currentTimeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(currentTimeLabel);

    // ****Sayed**** 
    totalTimeLabel.setText("0:00", juce::dontSendNotification);
    totalTimeLabel.setJustificationType(juce::Justification::centred);
    totalTimeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(totalTimeLabel);

    // ****Sayed**** 
    addAndMakeVisible(waveformDisplay);

    // ****edit by Sayed**** 
    setSize(850, 600);
    setAudioChannels(0, 2);

    statusBox.setMultiLine(true);
    statusBox.setReadOnly(true);
    statusBox.setScrollbarsShown(true);
    addAndMakeVisible(statusBox);

    mytable.getHeader().addColumn("Filename", 1, 180);
    mytable.getHeader().addColumn("Duration", 2, 180);
    mytable.setModel(&model);
    addAndMakeVisible(mytable);

    model.doubleClick = [this](juce::String filename) {
        juce::File file(filename);
        info fileinfo = player1.LoadFile(file);
        statusBox.clear();
        statusBoxMessage("FILE: " + fileinfo.get_filename());
        statusBoxMessage(fileinfo.get_duration());

        // ****Sayed****
        totalTimeLabel.setText(formatTime(player1.getLength()), juce::dontSendNotification);
        waveformDisplay.loadFile(file);

        player1.complete(fileinfo.get_filename());
        startTimer(40);
        };

    startTimer(40);
}

PlayerGui::~PlayerGui()
{
    stopTimer();// ****Sayed****
    shutdownAudio();
}

void PlayerGui::resized()
{
    int y = 20;
    int buttonWidth = 80;
    int buttonHeight = 40;

    loadButton.setBounds(20, y, buttonWidth, buttonHeight);
    backward.setBounds(120, y, buttonWidth, buttonHeight);
    pauseButton.setBounds(220, y, buttonWidth, buttonHeight);
    restartButton.setBounds(320, y, buttonWidth, buttonHeight);
    loopButton.setBounds(420, y, buttonWidth, buttonHeight);

    forward.setBounds(120, y + 50, buttonWidth, buttonHeight);
    playButton.setBounds(220, y + 50, buttonWidth, buttonHeight);
    goEndButton.setBounds(320, y + 50, buttonWidth, buttonHeight);
    mute.setBounds(420, y + 50, buttonWidth, buttonHeight);

    // ****Sayed****
    int waveformY = 130;
    waveformDisplay.setBounds(20,130, getWidth() - 40, 120);

    // ****Sayed**** 
    int progressY = waveformY + 130;
    currentTimeLabel.setBounds(20, progressY, 50, 20);
    totalTimeLabel.setBounds(getWidth() - 70, 260, 50, 20);

    // ****Sayed**** 
    speedSlider.setBounds(20, 330, getWidth() - 40, 30);
    volumeSlider.setBounds(20, 290, getWidth() - 40, 30);

    statusBox.setBounds(20, 370, 350, 80);
    mytable.setBounds(400, 370, 360, 150);
}









void PlayerGui::paint(juce::Graphics& g)
{
    // ****Sayed**** - Create gradient background for modern visual appeal
    juce::ColourGradient gradient(
        juce::Colours::darkgrey.darker(0.2f), 0, 0,
        juce::Colours::darkgrey.darker(0.5f), 0, (float)getHeight(),
        false
    );
    g.setGradientFill(gradient);
    g.fillAll();

    // ****Sayed**** - Draw section headers for better UI organization
    g.setColour(juce::Colours::white.withAlpha(0.8f));
    g.setFont(14.0f);

    g.drawText("Waveform Display", 20, 115, 200, 15, juce::Justification::centredLeft);


}









void PlayerGui::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select audio files...",
            juce::File{},
            // ****Sayed**** - Support multiple audio formats including high-quality formats
            "*.wav;*.mp3;*.aiff;*.flac");

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

                    // ****Sayed**** - Update total time display for new file
                    totalTimeLabel.setText(formatTime(player1.getLength()), juce::dontSendNotification);
                    // ****Sayed**** - Generate and display waveform for loaded file
                    waveformDisplay.loadFile(file);

                    model.files.push_back({ fileinfo.get_path(), fileinfo.get_duration() });
                    mytable.updateContent();

                    // ****Sayed**** - Restart timer for UI updates with new file
                    startTimer(40);
                }
                else {
                    statusBoxMessage("No file selected!");
                }
            });
    }
    else if (button == &restartButton)
    {
        player1.restart();
    }
    else if (button == &pauseButton)
    {
        player1.pause();
    }
    else if (button == &playButton)
    {
        player1.play();
    }
    else if (button == &goEndButton)
    {
        player1.goEnd();
    }
    else if (button == &forward)
    {
        player1.forward();
    }
    else if (button == &backward)
    {
        player1.backward();
    }
    else if (button == &loopButton)
    {
        bool newState = !player1.isLooping();
        player1.setLooping(newState);

        // ****Sayed**** - Provide visual feedback for loop state with color change
        if (newState) {
            button->setColour(juce::TextButton::buttonColourId, juce::Colours::darkblue);
            button->setColour(juce::TextButton::textColourOnId, juce::Colours::white);
        }
        else {
            button->setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
            button->setColour(juce::TextButton::textColourOnId, juce::Colours::white);
        }
    }
    else if (button == &mute)
    {
        player1.isMuted = mute.getToggleState();
        player1.mute();
        // ****Sayed**** - Provide visual feedback for mute state with color and text change
        if (player1.isMuted) {
            mute.setButtonText("Unmute");
            mute.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
        }
        else {
            mute.setButtonText("Mute");
            mute.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
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

void PlayerGui::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider && !player1.isMuted)
        player1.setGain((float)slider->getValue());
    player1.prevGain = slider->getValue();

    // ****Sayed**** - Handle speed slider changes and apply to audio playback
    if (slider == &speedSlider)
    {
        player1.setSpeed((float)slider->getValue());
    }

}

void PlayerGui::statusBoxMessage(const juce::String& message)
{
    statusBox.insertTextAtCaret(message + "\n");
}

// ****Sayed**** - Timer callback for continuous UI updates (time labels, waveform playhead)
void PlayerGui::timerCallback()
{
    updateProgressDisplay();
}

// ****Sayed**** - Update time displays and waveform visualization

void PlayerGui::updateProgressDisplay()
{
    double currentPosition = player1.getPosition();
    double totalLength = player1.getLength();

    // ****Sayed**** - Progress bar removed, waveform handles visual progress

    // ****Sayed**** - Update current time label with formatted time string
    currentTimeLabel.setText(formatTime(currentPosition), juce::dontSendNotification);

    // ****Sayed**** - Only update total time label when duration changes significantly
    static double lastTotalLength = 0.0;
    if (std::abs(totalLength - lastTotalLength) > 0.1)
    {
        totalTimeLabel.setText(formatTime(totalLength), juce::dontSendNotification);
        lastTotalLength = totalLength;
    }
}

// ****Sayed**** - Convert seconds to formatted time string (MM:SS or HH:MM:SS)
juce::String PlayerGui::formatTime(double seconds)
{
    if (seconds < 0) seconds = 0;

    int minutes = static_cast<int>(seconds) / 60;
    int secs = static_cast<int>(seconds) % 60;

    // ****Sayed**** - Handle long tracks by including hours when needed
    if (minutes >= 60)
    {
        int hours = minutes / 60;
        minutes = minutes % 60;
        return juce::String::formatted("%d:%02d:%02d", hours, minutes, secs);
    }

    return juce::String::formatted("%d:%02d", minutes, secs);
}