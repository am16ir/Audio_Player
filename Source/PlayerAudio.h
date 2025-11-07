#pragma once
#include <JuceHeader.h>
#include <unordered_map>
#include <map>
class info {
    juce::String filename;
    double duration = 0;
    juce::StringPairArray metadata;
    juce::String durationMessage;
    juce::String filepath;
    juce::String message;

    void durationFormat() {
        int hours = duration / 3600;
        int min = duration / 60;
        int sec = (int)duration % 60;
        if (hours > 0)
            durationMessage = "Duration: " + juce::String(hours) + ":" + juce::String(min % 60) + ":" + juce::String(sec);
        else if (min > 0)
            durationMessage = "Duration: " + juce::String(min) + ":" + juce::String(sec);
        else
            durationMessage = "Duration: " + juce::String(sec) + "s";
    }

    void metadataFormat() {
        if (metadata.getAllKeys() > 0) {
            for (auto& key : metadata.getAllKeys()) {
                message += "Metadata: " + key + " : " + metadata[key];
            }
        }
    }

public:
    info(juce::String fn, double dur, juce::StringPairArray meta, juce::String fp);
    info() = default;
    juce::String get_filename();
    juce::String get_duration();
    juce::String get_metadata();
    juce::String get_path();

    void set_filename(juce::String f);
    void set_duration(double d);
    void set_metadata(juce::StringPairArray m);
    void set_filepath(juce::String fp);
};

class PlayerAudio {
private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;

    juce::ResamplingAudioSource resampleSource{ &transportSource, false }; // ***Sayed***

    float outputGain = 1.0f; // used by mixer
    double speed = 1.0; // default playback speed (1.0 = normal)


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)

public:
    PlayerAudio();
    ~PlayerAudio();

    juce::AudioFormatManager& getFormatManager() { return formatManager; }//////*****Sayed to Wave


    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

    float prevGain;
    std::unordered_map<juce::String, float>songs;
    std::unordered_map<juce::String, std::vector<double>> markers;
    juce::String currentFileName;

    double startPoint = 0.0;
    double endPoint = 0.0;
    bool isSegmentLooping = false;


    info LoadFile(const juce::File& file);
    void pause();
    void play();
    void goEnd();
    void restart();
    void forward();
    void backward();
    void setGain(float gain);
    void setPosition(double pos);
    double getPosition() const;
    double getLength() const;
    void mute();
    void complete(juce::String name);
    bool isMuted = false;
    void setLooping(bool shouldLoop);
    bool isLooping() const;

    void setStartPoint(double pos);
    void setEndPoint(double pos);
    void clearSegmentPoints();
    void setSegmentLooping(bool shouldLoop);
    double getStartPoint() const { return startPoint; }
    double getEndPoint() const { return endPoint; }
    bool getSegmentLooping() const { return isSegmentLooping; }

    void clearMarkers();

    void setSpeed(double ratio);   
    double getSpeed() const { return speed; }



    // *** MIXER ADDED BY SAYED ***
    void setOutputGain(float gain) { outputGain = gain; }
    float getOutputGain() const { return outputGain; }



};