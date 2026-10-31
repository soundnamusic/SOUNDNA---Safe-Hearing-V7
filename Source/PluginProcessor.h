#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "TimerModel.h"
#include "PdfReport.h"
#include "UiPreferences.h"
#include <atomic>
#include <chrono>
#include <mutex>

class ListenProcessor final : public juce::AudioProcessor, private juce::Timer
{
public:
    ListenProcessor();
    ~ListenProcessor() override;
    void prepareToPlay(double, int) override;
    void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    bool isBusesLayoutSupported(const BusesLayout&) const override;
    const juce::String getName() const override { return "SOUNDNA - Safe Hearing"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0; }
    bool hasEditor() const override { return true; }
    juce::AudioProcessorEditor* createEditor() override;
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;
    TimerStats snapshot();
    SoundnaReport::ReportData reportSnapshot();
    juce::String getProjectTitle();
    void setProjectTitle(const juce::String&);
    void toggleBreak();
    void setInterval(int minutes);
    void resetTotals();
    // 0=no recent callback, 1=stop, 2=play, 3=offline, 4=transport unavailable
    int transportStatus() const;
private:
    using Clock = std::chrono::steady_clock;
    static_assert(std::atomic<uint64_t>::is_always_lock_free, "64-bit target required");
    std::atomic<uint64_t> playbackNanos {0};
    std::atomic<int> transport {0};
    std::atomic<double> lastCallback {0};
    double sampleRate = 44100;
    std::mutex stateMutex; // Never acquired on the audio thread.
    juce::SharedResourcePointer<UiPreferences> preferences;
    TimerModel model;
    juce::String projectTitle;
    int dirtyTicks = 0;
    void markChanged();
    Clock::time_point lastTick = Clock::now();
    void flushLocked();
    void timerCallback() override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ListenProcessor)
};
