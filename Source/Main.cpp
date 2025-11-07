#include <JuceHeader.h>
#include "PlayerGui.h"

// Our application class
class SimpleAudioPlayer : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override { return "Spotify++"; }
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

            centreWithSize(1400, 700); 
            setVisible(true);
        }

        void closeButtonPressed() override
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        class DualPlayerComponent : public juce::Component
        {
        public:
            DualPlayerComponent()
            {
                player1 = std::make_unique<PlayerGui>();
                player2 = std::make_unique<PlayerGui>();


                
                addAndMakeVisible(player1.get());
                addAndMakeVisible(player2.get());

				
            }

            void resized() override
            {
                auto area = getLocalBounds();
                player1->setBounds(area.removeFromLeft(getWidth() / 2));
                player2->setBounds(area); 
            }

            /*void paint(juce::Graphics& g) override
            {
                g.fillAll(juce::Colours::lightgreen);

                int spacing = 20;
                int playerWidth = (getWidth() - spacing) / 2;
                auto spacingArea = juce::Rectangle<int>(playerWidth, 0, spacing, getHeight());
                g.setColour(juce::Colours::orangered);
                g.fillRect(spacingArea);
            }*/

        private:
            std::unique_ptr<PlayerGui> player1;
            std::unique_ptr<PlayerGui> player2;
           
        };
    };

    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(SimpleAudioPlayer)