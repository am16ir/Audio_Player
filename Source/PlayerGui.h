#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"
#include <vector>

using namespace std;

class tablemodel : public juce::TableListBoxModel {

public:
	function<void(juce::String)> doubleClick;
	vector<pair<juce::String, juce::String>> files;// filename , duration
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
	void remove(int rowNumber) {
		if (rowNumber >= 0 && rowNumber < files.size()) {
			files.erase(files.begin() + rowNumber);
		}
	}
	void clear() {
		files.clear();
	}

};


//////////************* class wave Sayed  ***************/////////////////////
class WaveformDisplay : public juce::Component,
	private juce::ChangeListener,
	private juce::Timer
{
public:
	WaveformDisplay(PlayerAudio& audio, juce::AudioFormatManager& formatManager)
		: audioPlayer(audio),
		thumbnailCache(5),
		thumbnail(512, formatManager, thumbnailCache)
	{
		thumbnail.addChangeListener(this);
		startTimer(40);
	}

	void paint(juce::Graphics& g) override
	{
		g.fillAll(juce::Colours::darkgrey);
		g.setColour(juce::Colours::lightgrey);

		if (thumbnail.getTotalLength() > 0.0)
		{
			auto audioLength = thumbnail.getTotalLength();
			auto thumbArea = getLocalBounds();

			g.setColour(juce::Colours::lightblue);
			thumbnail.drawChannels(g, thumbArea, 0.0, audioLength, 1.0f);

			auto audioPosition = audioPlayer.getPosition();
			auto drawPosition = (audioPosition / audioLength) * thumbArea.getWidth();

			g.setColour(juce::Colours::red);
			g.drawLine(drawPosition, 0, drawPosition, getHeight(), 2.0f);

			g.setColour(juce::Colours::white);
			g.drawText(formatTime(audioPosition) + " / " + formatTime(audioLength),
				getLocalBounds(), juce::Justification::centredBottom);
		}
		else
		{
			g.setColour(juce::Colours::white);
			g.drawText("No audio file loaded", getLocalBounds(), juce::Justification::centred);
		}
	}

	void setFile(const juce::File& file)
	{
		thumbnail.setSource(new juce::FileInputSource(file));
	}

	void mouseDown(const juce::MouseEvent& event) override
	{
		if (thumbnail.getTotalLength() > 0.0)
		{
			auto clickPosition = event.position.x;
			auto thumbWidth = getWidth();
			auto newPosition = (clickPosition / thumbWidth) * thumbnail.getTotalLength();
			audioPlayer.setPosition(newPosition);
		}
	}

private:
	PlayerAudio& audioPlayer;
	juce::AudioThumbnailCache thumbnailCache;
	juce::AudioThumbnail thumbnail;

	void changeListenerCallback(juce::ChangeBroadcaster* source) override
	{
		if (source == &thumbnail)
			repaint();
	}

	void timerCallback() override
	{
		repaint();
	}

	juce::String formatTime(double seconds)
	{
		auto minutes = (int)(seconds / 60);
		auto secs = (int)seconds % 60;
		return juce::String::formatted("%02d:%02d", minutes, secs);
	}
};


///////// end of wave Sayed  ////////////////////////



class PlayerGui : public juce::AudioAppComponent,
	public juce::Button::Listener,
	public juce::Slider::Listener,
	public juce::Timer
{
private:

	PlayerAudio player1;
	PlayerAudio player2;
	juce::TextButton loadButton{ "Load Files" };
	juce::TextButton restartButton{ "Restart" };
	juce::TextButton pauseButton{ "Pause" };
	juce::TextButton playButton{ "Play" };
	juce::TextButton goEndButton{ "End" };
	juce::TextButton forward{ "Forward  +10" };
	juce::TextButton backward{ "Backward -10" };
	juce::TextButton loopButton{ "Loop" };
	juce::TextButton mute{ "Mute" };
	juce::TextButton startingpoint{ "Starting point" };
	juce::TextButton endingpoint{ "Ending point" };
	juce::TextButton Deletepoints{ "Delete points" };
	juce::TextButton looponpoints{ "LOOP ON POINTS" };
	juce::TextButton markerButton{ "Add Marker" };
	juce::TextButton clearMarkers{ "Clear Markers" };
	juce::TextButton removeSelected{ "Remove File" };
	juce::TextButton clear{ "Clear Table" };
	juce::TextEditor statusBox;
	juce::Slider volumeSlider;
	juce::Slider speedSlider; // ****Sayed****
	juce::TableListBox mytable;
	juce::Slider positionslider;
	tablemodel model;
	std::unique_ptr<juce::FileChooser> fileChooser;

	bool isSliderDragging = false;
	WaveformDisplay waveformDisplay; // ************Sayed***************


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


	void sliderDragStarted(juce::Slider* slider) override;
	void sliderDragEnded(juce::Slider* slider) override;


	void statusBoxMessage(const juce::String& message);

	void timerCallback() override;
	void paintOverChildren(juce::Graphics& g) override;


};