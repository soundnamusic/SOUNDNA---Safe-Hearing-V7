#pragma once
#include "TimerModel.h"
#include <juce_core/juce_core.h>

namespace SoundnaState {
struct State { TimerModel timer; juce::String project; int64_t savedAt=0; };
inline void writeSegment(juce::XmlElement& x, const TimelineSegment& s)
{
    x.setAttribute("start",juce::String(s.startMs)); x.setAttribute("end",juce::String(s.endMs));
    x.setAttribute("seconds",s.seconds); x.setAttribute("playback",s.playback);
    x.setAttribute("pause",s.pause); x.setAttribute("continuation",s.continuation);
}
inline TimelineSegment readSegment(const juce::XmlElement& x)
{
    TimelineSegment s;
    s.startMs=std::max<int64_t>(0,x.getStringAttribute("start").getLargeIntValue());
    s.endMs=std::max<int64_t>(0,x.getStringAttribute("end").getLargeIntValue());
    s.seconds=TimerStats::sane(x.getDoubleAttribute("seconds"));
    s.playback=TimerStats::sane(x.getDoubleAttribute("playback"));
    s.pause=x.getBoolAttribute("pause"); s.continuation=x.getBoolAttribute("continuation");
    return s;
}
inline std::unique_ptr<juce::XmlElement> encode(const State& s)
{
    auto x=std::make_unique<juce::XmlElement>("SOUNDNA_LISTEN"); const auto& m=s.timer;
    x->setAttribute("version",2); x->setAttribute("project",s.project);
    // Mark manual for compatibility if this state is opened by version 1.4.
    x->setAttribute("followHostName",false);
    x->setAttribute("savedAt",juce::String(s.savedAt));
    x->setAttribute("listening",m.listening); x->setAttribute("session",m.session);
    x->setAttribute("cycle",m.cycle); x->setAttribute("break",m.breakTime);
    x->setAttribute("paused",m.onBreak); x->setAttribute("interval",m.intervalMinutes);
    x->setAttribute("pauseTotal",m.pauseTotal); x->setAttribute("started",m.pausesStarted);
    x->setAttribute("completed",m.pausesCompleted); x->setAttribute("cycleTotal",m.completedCycleTotal);
    x->setAttribute("longest",m.longestCycle); x->setAttribute("legacy",m.legacyTotals);
    auto* log=x->createNewChildElement("History");
    for(const auto& r:m.history)writeSegment(*log->createNewChildElement("Segment"),r);
    writeSegment(*x->createNewChildElement("Active"),m.active);
    return x;
}
inline bool decode(const juce::XmlElement& x, State& out, int64_t now)
{
    const int version=x.getIntAttribute("version");
    if(!x.hasTagName("SOUNDNA_LISTEN") || (version!=1 && version!=2))return false;
    State next; auto& m=next.timer;
    next.project=x.getStringAttribute("project").substring(0,80);
    // Discard titles automatically imported by 1.4; keep every manual title.
    if(x.getBoolAttribute("followHostName",false))next.project.clear();
    m.listening=x.getDoubleAttribute("listening"); m.session=x.getDoubleAttribute("session");
    m.cycle=x.getDoubleAttribute("cycle"); m.breakTime=x.getDoubleAttribute("break");
    m.onBreak=x.getBoolAttribute("paused"); m.intervalMinutes=x.getIntAttribute("interval",45);
    if(version==2) {
        m.pauseTotal=x.getDoubleAttribute("pauseTotal"); m.pausesStarted=x.getIntAttribute("started");
        m.pausesCompleted=x.getIntAttribute("completed"); m.completedCycleTotal=x.getDoubleAttribute("cycleTotal");
        m.longestCycle=x.getDoubleAttribute("longest"); m.legacyTotals=x.getBoolAttribute("legacy");
        if(auto* log=x.getChildByName("History"))
            for(auto* row:log->getChildIterator()) if(row->hasTagName("Segment")) {
                if(m.history.size()>=100000)return false;
                m.history.push_back(readSegment(*row));
            }
        if(auto* active=x.getChildByName("Active")) { m.active=readSegment(*active); m.initialised=true; }
        next.savedAt=x.getStringAttribute("savedAt").getLargeIntValue();
        m.resumeFromCheckpoint(next.savedAt,now);
    } else {
        m.legacyTotals=true;
        // v1 has no history or pause totals. Do not fabricate old break events.
        m.breakTime=0; m.pausesStarted=m.onBreak?1:0;
        m.begin(now);
    }
    m.sanitise(); next.savedAt=now; out=std::move(next); return true;
}
}
