#include <JuceHeader.h>
#include "PlayerGui.h"

// Our application class
class SimpleAudioPlayer : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override { return "Spotify++ Dual Player"; } // Updated name
    const juce::String getApplicationVersion() override { return "3.2"; }

    void initialise(const juce::String&) override
    {
        mainWindow = std::make_unique<MainWindow>(getApplicationName());
    }

    void shutdown() override
    {
        mainWindow = nullptr;
    }

private:
    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow(juce::String name)
            : DocumentWindow(name,
                juce::Colours::lightgrey,
                DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);

            auto dualPlayerComponent = std::make_unique<DualPlayerComponent>();
            setContentOwned(dualPlayerComponent.release(), true);

            centreWithSize(1400, 600); // Double width for two players
            setVisible(true);
        }

        void closeButtonPressed() override
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }



        //////****Multiplayer Component *****//////


    private:
        // NEW: Component that contains two PlayerGui instances side by side
        class DualPlayerComponent : public juce::Component
        {
        public:
            DualPlayerComponent()
            {
                // Create two PlayerGui instances
                player1 = std::make_unique<PlayerGui>();
                player2 = std::make_unique<PlayerGui>();

                addAndMakeVisible(player1.get());
                addAndMakeVisible(player2.get());
            }

            void resized() override
            {
                // Split the window horizontally for two players
                auto area = getLocalBounds();
                player1->setBounds(area.removeFromLeft(getWidth() / 2));
                player2->setBounds(area); // Remaining right half
            }

        private:
            std::unique_ptr<PlayerGui> player1;
            std::unique_ptr<PlayerGui> player2;
        };
    };


    //////****End of Multiplayer Component *****//////

    std::unique_ptr<MainWindow> mainWindow;
};

// This macro starts the app
START_JUCE_APPLICATION(SimpleAudioPlayer)