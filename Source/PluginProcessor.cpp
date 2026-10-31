#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "StateCodec.h"

ListenProcessor::ListenProcessor()
 : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                  .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{ model.intervalMinutes=preferences->defaultInterval(); model.begin(juce::Time::currentTimeMillis()); startTimerHz(20); }
ListenProcessor::~ListenProcessor() { stopTimer(); }
void ListenProcessor::prepareToPlay(double rate, int) { sampleRate = rate > 0 ? rate : 44100; }
bool ListenProcessor::isBusesLayoutSupported(const BusesLayout& b) const
{
    const auto out = b.getMainOutputChannelSet();
    return b.getMainInputChannelSet() == out
        && (out == juce::AudioChannelSet::mono() || out == juce::AudioChannelSet::stereo());
}
void ListenProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    // No DSP, no locks, no allocations, no UI access. Input samples remain untouched.
    const bool offline = isNonRealtime();
    bool playing = false, available = false;
    if (auto* head = getPlayHead())
        if (auto position = head->getPosition())
        { available = true; playing = position->getIsPlaying(); }
    transport.store(offline ? 3 : !available ? 4 : playing ? 2 : 1, std::memory_order_relaxed);
    lastCallback.store(juce::Time::getMillisecondCounterHiRes(), std::memory_order_relaxed);
    if (playing && !offline)
        playbackNanos.fetch_add(static_cast<uint64_t>(std::llround(buffer.getNumSamples() * 1.0e9 / sampleRate)),
                               std::memory_order_relaxed);
}
int ListenProcessor::transportStatus() const
{
    return juce::Time::getMillisecondCounterHiRes() - lastCallback.load(std::memory_order_relaxed) > 1500
        ? 0 : transport.load(std::memory_order_relaxed);
}
void ListenProcessor::flushLocked()
{
    const auto now = Clock::now();
    double wall = std::chrono::duration<double>(now - lastTick).count();
    lastTick = now;
    // Ignore long suspended-message-loop gaps (sleep / host suspension).
    if (wall > 2.0) wall = 0;
    const double audio = playbackNanos.exchange(0, std::memory_order_relaxed) / 1.0e9;
    model.advance(wall, audio);
}
void ListenProcessor::markChanged()
{ updateHostDisplay(ChangeDetails().withNonParameterStateChanged(true)); }
void ListenProcessor::timerCallback()
{
    { std::lock_guard<std::mutex> lock(stateMutex); flushLocked(); }
    if (++dirtyTicks >= 600) { dirtyTicks=0; markChanged(); }
}
TimerStats ListenProcessor::snapshot()
{ std::lock_guard<std::mutex> lock(stateMutex); flushLocked(); return model; }
SoundnaReport::ReportData ListenProcessor::reportSnapshot()
{
    std::lock_guard<std::mutex> lock(stateMutex); flushLocked();
    return { model, projectTitle.toStdString(), juce::Time::currentTimeMillis() };
}
juce::String ListenProcessor::getProjectTitle()
{ std::lock_guard<std::mutex> lock(stateMutex); return projectTitle; }
void ListenProcessor::setProjectTitle(const juce::String& name)
{
    { std::lock_guard<std::mutex> lock(stateMutex); projectTitle=name.substring(0,80); }
    markChanged();
}
void ListenProcessor::toggleBreak()
{
    {
        std::lock_guard<std::mutex> lock(stateMutex); flushLocked();
        const auto now=juce::Time::currentTimeMillis();
        if (model.onBreak) model.endBreak(now); else model.startBreak(now);
    }
    markChanged();
}
void ListenProcessor::setInterval(int minutes)
{
    { std::lock_guard<std::mutex> lock(stateMutex); flushLocked(); model.intervalMinutes = juce::jlimit(1, 180, minutes); }
    markChanged();
}
void ListenProcessor::resetTotals()
{
    {
        std::lock_guard<std::mutex> lock(stateMutex); flushLocked();
        const int interval=model.intervalMinutes; model={}; model.intervalMinutes=interval;
        model.begin(juce::Time::currentTimeMillis());
    }
    markChanged();
}
void ListenProcessor::getStateInformation(juce::MemoryBlock& dest)
{
    SoundnaState::State state;
    {
        std::lock_guard<std::mutex> lock(stateMutex); flushLocked();
        state={model,projectTitle,juce::Time::currentTimeMillis()};
    }
    auto xml=SoundnaState::encode(state); copyXmlToBinary(*xml,dest);
}
void ListenProcessor::setStateInformation(const void* data, int size)
{
    auto xml=getXmlFromBinary(data,size); if(!xml)return;
    SoundnaState::State state;
    if(!SoundnaState::decode(*xml,state,juce::Time::currentTimeMillis()))return;
    std::lock_guard<std::mutex> lock(stateMutex);
    model=std::move(state.timer); projectTitle=state.project;
    playbackNanos.exchange(0,std::memory_order_relaxed); lastTick=Clock::now();
}
juce::AudioProcessorEditor* ListenProcessor::createEditor() { return new ListenEditor(*this); }
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new ListenProcessor(); }
