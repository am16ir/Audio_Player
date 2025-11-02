#include "PlayerAudio.h"

juce::String info::get_filename() {
    return filename;
}

juce::String info::get_duration() {
    return durationMessage;
}

juce::String info::get_metadata() {
    return message;
}
juce::String info::get_path() {
    return filepath;
}
info::info(juce::String fn, double dur, juce::StringPairArray meta,juce::String fp) : filename(fn),duration(dur),
metadata(meta),filepath(fp){
    
    durationFormat();
    metadataFormat();
}

void info::set_filename(juce::String f) {
    filename = f;
}

void info::set_duration(double d) {
    duration = d;
	durationFormat();
}

void info::set_metadata(juce::StringPairArray m) {
    metadata = m;
	metadataFormat();
}
void info::set_filepath(juce::String fp) {
    filepath = fp;
}

PlayerAudio::PlayerAudio() {
	formatManager.registerBasicFormats();

}

PlayerAudio::~PlayerAudio() {
    transportSource.stop();
    transportSource.setSource(nullptr);
    readerSource.reset();
}


void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    transportSource.getNextAudioBlock(bufferToFill);
}

void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
}

info PlayerAudio::LoadFile(const juce::File& file) {
    info myinfo;
    if (file.existsAsFile())
    {
        if (auto* reader = formatManager.createReaderFor(file))
        {
            //  Disconnect old source first
            if (readerSource != nullptr && !currentFileName.isEmpty()) {
                songs[currentFileName] = transportSource.getCurrentPosition();
            }
            transportSource.stop();
            transportSource.setSource(nullptr);
            readerSource.reset();

            // Create new reader source
            readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

            // Attach safely
            transportSource.setSource(readerSource.get(),
                0,
                nullptr,
                reader->sampleRate);
			currentFileName = file.getFileName();
            transportSource.start();
           
            myinfo.set_filename( file.getFileName());
            myinfo.set_metadata ( reader->metadataValues);
            myinfo.set_duration ( reader->lengthInSamples / reader->sampleRate);
			myinfo.set_filepath(file.getFullPathName());
			
          
        }
    }
	return myinfo;
}

void PlayerAudio::pause() {
    transportSource.stop();
}

void PlayerAudio::play() {
    transportSource.start();
}

void PlayerAudio::complete(juce::String name) {
    if (songs.count(name))
		transportSource.setPosition(0.0);
    else 
        transportSource.setPosition(songs[name]);
	transportSource.start();
}

void PlayerAudio::restart() {
    transportSource.setPosition(0.0);
    transportSource.start();
}

void PlayerAudio::setGain(float gain) {
    transportSource.setGain(gain);
}

void PlayerAudio::setPosition(double pos) {
    transportSource.setPosition(pos);
}

double PlayerAudio::getPosition() const {
    return transportSource.getCurrentPosition();
}

double PlayerAudio::getLength() const {
    return transportSource.getLengthInSeconds();
}
void PlayerAudio::goEnd() {
    return transportSource.setPosition(getLength());
}

void PlayerAudio::setLooping(bool shouldLoop) {
    
	if (readerSource != nullptr)
        readerSource->setLooping(shouldLoop);
   
}


bool PlayerAudio::isLooping() const {
    if (readerSource == nullptr)
		return false;

    return readerSource->isLooping();
   
}

void PlayerAudio::forward() {
	double pos = getPosition();
    if (pos + 10 <= getLength())
        setPosition(pos + 10);
    else
        setPosition(getLength());
}
void PlayerAudio::backward() {
    double pos = getPosition();
    if (pos >= 10)
        setPosition(pos - 10);
    else
        setPosition(0.0);
}

void PlayerAudio::mute() {
    if (isMuted)
        transportSource.setGain(0.0);
    else
        transportSource.setGain(prevGain);
}
