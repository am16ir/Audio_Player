#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"
#include <vector>

using namespace std;

class tablemodel : public juce::TableListBoxModel {

public:
    function<void(juce::String)> doubleClick;
    vector<pair<juce::String, juce::String>> files;
    tablemodel() = default;
    void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override {
        if (columnId == 1) {
            juce::File f(files[rowNumber].first);
            g.drawText(f.getFileName(), juce::Rectangle<int>(0, 0, width, height), juce::Justification::centredLeft);
        }
        else if (columnId == 2)
            g.drawText((files[rowNumber].second), juce::Rectangle<int>(0, 0, width, height), juce::Justification::centredLeft);
    }

    int getNumRows() override {
        return files.size();
    }
    void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override {
        if (rowIsSelected)
            g.fillAll(juce::Colours::lightblue);
        else
            g.fillAll(juce::Colours::grey);
    }
    void cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent& e) override {
        if (rowNumber >= 0 && rowNumber < files.size()) {
            juce::String filename = files[rowNumber].first;
            if (doubleClick)
                doubleClick(filename);
        }
    }

};

// ****Sayed**** - start of WaveformDisplay class
class WaveformDisplay : public juce::Component,
    public juce::ChangeListener,
    public juce::Timer
{
public:
    WaveformDisplay(PlayerAudio& audio);
    ~WaveformDisplay() override;

    void paint(juce::Graphics& g) override;
    void resized() override {}

    void loadFile(const juce::File& file);
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    void timerCallback() override;
    void mouseDown(const juce::MouseEvent& event) override;

private:
    PlayerAudio& playerAudio;
    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumbnailCache{ 512 };
    juce::AudioThumbnail thumbnail;

    void drawTimeMarkers(juce::Graphics& g, juce::Rectangle<float> area);
};

///***Sayed*** - End of WaveformDisplay class

class PlayerGui : public juce::AudioAppComponent,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    // ****Sayed**** - Added Timer inheritance for continuous UI updates
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
  
    juce::Slider speedSlider;  // ****Sayed**** 

    
    juce::Label currentTimeLabel;  // ****Sayed**** 
    juce::Label totalTimeLabel;  // ****Sayed**** 
    WaveformDisplay waveformDisplay;  // ****Sayed**** 

    juce::TableListBox mytable;
    tablemodel model;
    std::unique_ptr<juce::FileChooser> fileChooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGui)

public:
    PlayerGui();
    ~PlayerGui() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    void timerCallback() override;  // ****Sayed**** 

    void statusBoxMessage(const juce::String& message);
    void updateProgressDisplay();  // ****Sayed**** 
    juce::String formatTime(double seconds);  // ****Sayed**** 
};