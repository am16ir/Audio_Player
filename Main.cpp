#include <JuceHeader.h>
#include "PlayerGui.h"
#include "PlayerAudio.h"

// Our application class
class SimpleAudioPlayer : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override { return "Simple Audio Player"; }
    const juce::String getApplicationVersion() override { return "1.0"; }

    void initialise(const juce::String&) override
    {
        mainWindow = std::make_unique<MainWindow>(getApplicationName());
    }

    void shutdown() override
    {
        mainWindow = nullptr; // Clean up
    }

private:
    // The main window of the app
    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow(juce::String name)
            : DocumentWindow(name,
                juce::Colours::lightgrey,
                DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);

            // Create PlayerAudio instance (owned by MainWindow)
            playerAudio = std::make_unique<PlayerAudio>();

            // Create GUI and pass PlayerAudio by reference
            setContentOwned(new PlayerGui(*playerAudio), true);

            centreWithSize(1000, 500);
            setResizable(true, true);
            setVisible(true);
        }

        void closeButtonPressed() override
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        std::unique_ptr<PlayerAudio> playerAudio;
    };

    std::unique_ptr<MainWindow> mainWindow;
};

// This macro starts the app
START_JUCE_APPLICATION(SimpleAudioPlayer)
