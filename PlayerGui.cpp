#include "PlayerGui.h"

PlayerGui::PlayerGui(PlayerAudio& playerRef)
    : player(playerRef)
{
    // Buttons
    for (auto* b : { &loadButton, &restartButton, &pauseButton, &playButton, &goEndButton, &loopButton, &muteButton })
    {
        b->addListener(this);
        addAndMakeVisible(b);
        b->setEnabled(true);
    }

    // Volume slider
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    // Speed slider (0.5x - 2.0x)
    speedSlider.setRange(0.5, 2.0, 0.01);
    speedSlider.setValue(1.0);
    speedSlider.addListener(this);
    speedSlider.setTextValueSuffix("x");
    addAndMakeVisible(speedSlider);

    // Position slider
    positionSlider.setRange(0.0, 1.0, 0.0001);
    positionSlider.setValue(0.0);
    positionSlider.addListener(this);
    addAndMakeVisible(positionSlider);

    timeLabel.setText("00:00 / 00:00", juce::dontSendNotification);
    addAndMakeVisible(timeLabel);

    // Status box
    statusBox.setMultiLine(true);
    statusBox.setReadOnly(true);
    statusBox.setScrollbarsShown(true);
    addAndMakeVisible(statusBox);

    setSize(1000, 500);

    // Start audio (no inputs, 2 outputs)
    setAudioChannels(0, 2);

    // connect logging
    player.logBox = [this](const juce::String& msg) { statusBoxMessage(msg); };

    // timer for updating position and time
    startTimerHz(10);

    // defaults
    isMutedLocal = player.isMuted();
    // ensure player gain matches slider initially
    player.setGain((float)volumeSlider.getValue());
}

PlayerGui::~PlayerGui()
{
    shutdownAudio();
}

void PlayerGui::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    player.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerGui::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    player.getNextAudioBlock(bufferToFill);
}

void PlayerGui::releaseResources()
{
    player.releaseResources();
}

void PlayerGui::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void PlayerGui::resized()
{
    int y = 20;
    int btnW = 90;
    int btnH = 40;
    int gap = 20;
    int x = 20;

    loadButton.setBounds(x, y, btnW, btnH); x += btnW + 10;
    restartButton.setBounds(x, y, btnW, btnH); x += btnW + 10;
    pauseButton.setBounds(x, y, btnW, btnH); x += btnW + 10;
    playButton.setBounds(x, y, btnW, btnH); x += btnW + 10;
    goEndButton.setBounds(x, y, btnW, btnH); x += btnW + 10;
    loopButton.setBounds(x, y, btnW, btnH); x += btnW + 10;
    muteButton.setBounds(x, y, btnW, btnH);

    // sliders and status area
    volumeSlider.setBounds(20, 100, getWidth() - 40, 30);
    speedSlider.setBounds(20, 140, getWidth() - 40, 30);

    positionSlider.setBounds(20, 180, getWidth() - 160, 24);
    timeLabel.setBounds(getWidth() - 130, 180, 110, 24);

    statusBox.setBounds(20, 220, getWidth() - 40, getHeight() - 240);
}

void PlayerGui::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>("Select audio files...", juce::File{}, "*.wav;*.mp3;*.flac;*.aiff");
        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file.existsAsFile()) {
                    statusBox.clear();
                    bool ok = player.LoadFile(file);
                    if (ok)
                    {
                        // set initial gain & speed from UI
                        player.setGain((float)volumeSlider.getValue());
                        player.setSpeed(speedSlider.getValue());
                        // ensure muted state is consistent
                        if (isMutedLocal)
                            player.setMute(true);
                        else
                            player.setMute(false);

                        player.play();
                        statusBoxMessage("Loaded: " + file.getFileName());
                    }
                    else
                        statusBoxMessage("Failed to load file.");
                }
            });
    }
    else if (button == &restartButton)
    {
        player.restart();
    }
    else if (button == &pauseButton)
    {
        player.pause();
    }
    else if (button == &playButton)
    {
        player.play();
    }
    else if (button == &goEndButton)
    {
        player.goEnd();
    }
    else if (button == &loopButton)
    {
        bool newState = !player.isLooping();
        player.setLooping(newState);
        loopButton.setColour(juce::TextButton::buttonColourId, newState ? juce::Colours::blue : juce::Colours::grey);
        loopButton.setButtonText(newState ? "Loop: ON" : "Loop: OFF");
    }
    else if (button == &muteButton)
    {
        // Toggle mute. Before muting, save current slider volume into PlayerAudio
        float currentVol = (float)volumeSlider.getValue();
        player.setGain(currentVol); // save lastGainBeforeMute inside PlayerAudio

        isMutedLocal = !isMutedLocal;
        player.setMute(isMutedLocal);

        // Update UI text/color
        muteButton.setButtonText(isMutedLocal ? "Unmute" : "Mute");
        muteButton.setColour(juce::TextButton::buttonColourId, isMutedLocal ? juce::Colours::red : juce::Colours::grey);
    }
}

void PlayerGui::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        float val = (float)volumeSlider.getValue();

        // If currently muted and user moves slider > 0, unmute automatically
        if (isMutedLocal && val > 0.0f)
        {
            isMutedLocal = false;
            player.setMute(false);                    // IMPORTANT: unmute in PlayerAudio
            muteButton.setButtonText("Mute");
            muteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
        }

        // Always update saved gain (PlayerAudio will apply if not muted)
        player.setGain(val);
    }
    else if (slider == &speedSlider)
    {
        double s = speedSlider.getValue();
        player.setSpeed(s);
    }
    else if (slider == &positionSlider)
    {
        // Seeking: only update position while user is dragging
        if (positionSlider.isMouseButtonDown())
        {
            double frac = positionSlider.getValue();
            double len = player.getLength();
            if (len > 0.0)
                player.setPosition(frac * len);
        }
    }
}

void PlayerGui::timerCallback()
{
    if (!player.isLoaded()) return;

    double len = player.getLength();
    double pos = player.getPosition();

    if (len > 0.0)
    {
        double frac = pos / len;
        positionSlider.setValue(frac, juce::dontSendNotification);

        int totalSec = (int)std::round(len);
        int posSec = (int)std::round(pos);

        auto formatTime = [](int seconds) {
            int h = seconds / 3600;
            int m = (seconds / 60) % 60;
            int s = seconds % 60;
            if (h > 0)
                return juce::String(h) + ":" + juce::String(m).paddedLeft('0', 2) + ":" + juce::String(s).paddedLeft('0', 2);
            else
                return juce::String(m) + ":" + juce::String(s).paddedLeft('0', 2);
            };

        timeLabel.setText(formatTime(posSec) + " / " + formatTime(totalSec), juce::dontSendNotification);
    }
}

void PlayerGui::statusBoxMessage(const juce::String& message)
{
    statusBox.insertTextAtCaret(message + "\n");
}
