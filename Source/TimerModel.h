#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

struct TimerStats
{
    double listening = 0, session = 0, cycle = 0, breakTime = 0, pauseTotal = 0;
    double completedCycleTotal = 0, longestCycle = 0;
    int intervalMinutes = 30, pausesStarted = 0, pausesCompleted = 0;
    bool onBreak = false, legacyTotals = false;
    static double sane(double x) { return std::isfinite(x) ? std::clamp(x, 0.0, 315360000.0) : 0.0; }
    void sanitise()
    {
        listening = sane(listening); session = sane(session); cycle = sane(cycle);
        breakTime = sane(breakTime); pauseTotal = sane(pauseTotal);
        completedCycleTotal = sane(completedCycleTotal); longestCycle = sane(longestCycle);
        intervalMinutes = std::clamp(intervalMinutes, 1, 180);
        pausesStarted = std::clamp(pausesStarted, 0, 1000000);
        pausesCompleted = std::clamp(pausesCompleted, 0, pausesStarted);
    }
    double remaining() const { return std::max(0.0, intervalMinutes * 60.0 - cycle); }
    bool due() const { return !onBreak && cycle >= intervalMinutes * 60.0; }
};
struct TimelineSegment
{
    int64_t startMs = 0, endMs = 0;
    double seconds = 0, playback = 0;
    bool pause = false, continuation = false;
};
struct TimerModel : TimerStats
{
    std::vector<TimelineSegment> history;
    TimelineSegment active;
    bool initialised = false;
    void begin(int64_t now)
    {
        if (initialised) return;
        active = {}; active.startMs = now; active.pause = onBreak; initialised = true;
    }
    void advance(double wall, double playback)
    {
        wall = sane(wall); playback = sane(playback);
        listening += playback;
        if (onBreak) { breakTime += wall; pauseTotal += wall; }
        else { session += wall; cycle += playback; }
        active.seconds += wall; active.playback += playback;
    }
    void closeSegment(int64_t now)
    {
        active.endMs = std::max(active.startMs, now);
        history.push_back(active);
    }
    void startBreak(int64_t now = 0)
    {
        if (onBreak) return;
        begin(now); closeSegment(now);
        completedCycleTotal += cycle; longestCycle = std::max(longestCycle, cycle);
        ++pausesStarted; onBreak = true; breakTime = 0;
        active = {}; active.pause = true; active.startMs = now;
    }
    void endBreak(int64_t now = 0)
    {
        if (!onBreak) return;
        begin(now); closeSegment(now); ++pausesCompleted;
        onBreak = false; cycle = 0;
        active = {}; active.startMs = now;
    }
    // A saved checkpoint is the last observed time, never an invented DAW close time.
    void resumeFromCheckpoint(int64_t savedAt, int64_t now)
    {
        if (initialised && (active.seconds > 0 || active.playback > 0)) closeSegment(savedAt);
        active = {}; active.startMs = now; active.pause = onBreak; active.continuation = true;
        initialised = true;
    }
};
