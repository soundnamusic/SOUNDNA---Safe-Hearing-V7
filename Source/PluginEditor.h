#pragma once
#include "PluginProcessor.h"
#include "UiPreferences.h"
#include <juce_gui_basics/juce_gui_basics.h>

class ListenEditor final : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit ListenEditor(ListenProcessor&);
    ~ListenEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;
    void paintOverChildren(juce::Graphics&) override;
    void mouseUp(const juce::MouseEvent&) override;
private:
    ListenProcessor& listenProcessor;
    TimerStats state;
    juce::SharedResourcePointer<UiPreferences> preferences;
    SafeHearing::Language language=SafeHearing::Language::Italian;
    juce::Component content;
    juce::ComboBox languageSelector, intervalSelector;
    bool storeResize=false;
    bool preferenceWriteFailed=false;
    bool preferenceWarningShown=false;
    juce::String t(SafeHearing::Text key) const;
    void applyLanguage();
    juce::LookAndFeel_V4 look;
    juce::TextButton pauseButton, resetButton, exportButton;
    juce::Label listeningDisplay;
    double pendingClickAt=0;
    void showIntervalMenu();
    juce::TextEditor project;
    juce::Slider interval;
    std::unique_ptr<juce::FileChooser> fileChooser;
    juce::String exportStatus;
    int statusTicks = 0;
    float reminderOpacity = 0;
    float reminderGlow = 0;
    void chooseInterval(int minutes);
    void exportReport();
    void timerCallback() override;
    static juce::String timeText(double seconds);
    void text(juce::Graphics&, juce::String, juce::Rectangle<int>, float, juce::Colour,
              juce::Justification = juce::Justification::centredLeft);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ListenEditor)
};
