#include <JuceHeader.h>
#include "PlayerGui.h"

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
        class DualPlayerComponent : public juce::Component, public juce::Button::Listener
        {
        public:
            DualPlayerComponent()
            {
                player1 = std::make_unique<PlayerGui>();
                player2 = std::make_unique<PlayerGui>();

                addAndMakeVisible(player1.get());
                addAndMakeVisible(player2.get());

                mixAllButton = std::make_unique<juce::TextButton>("Mix All");
                mixAllButton->addListener(this);
                mixAllButton->setColour(juce::TextButton::buttonColourId, juce::Colours::green);
                addAndMakeVisible(mixAllButton.get());
            }

            void resized() override
            {
                auto area = getLocalBounds();

                auto buttonArea = area.removeFromTop(50);
                mixAllButton->setBounds(buttonArea.withSizeKeepingCentre(100, 30));

                player1->setBounds(area.removeFromLeft(getWidth() / 2));
                player2->setBounds(area);
            }

            void buttonClicked(juce::Button* button) override
            {
                if (button == mixAllButton.get())
                {
                    bool mixState = !isMixing;
                    isMixing = mixState;

                    if (mixState)
                    {
                        player1->getPlayer1().play();
                        player2->getPlayer1().play();
                        mixAllButton->setButtonText("Stop Mix");
                        mixAllButton->setColour(juce::TextButton::buttonColourId, juce::Colours::red);
                    }
                    else
                    {
                        player1->getPlayer1().pause();
                        player2->getPlayer1().pause();
                        mixAllButton->setButtonText("Mix All");
                        mixAllButton->setColour(juce::TextButton::buttonColourId, juce::Colours::green);
                    }
                }
            }

        private:
            std::unique_ptr<PlayerGui> player1;
            std::unique_ptr<PlayerGui> player2;
            std::unique_ptr<juce::TextButton> mixAllButton;
            bool isMixing = false;
        };
    };

    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(SimpleAudioPlayer)