#include "../Source/TimerModel.h"
#include <cstdlib>
#include <iostream>
#include <limits>

int checks = 0;
void check(bool ok, const char* message) { ++checks; if (!ok) { std::cerr << message << '\n'; std::exit(1); } }
int main()
{
    TimerModel countdown; countdown.intervalMinutes=17;
    countdown.advance(300,120); check(countdown.remaining()==900,"Remaining uses playback, not editing time");
    countdown.startBreak(); countdown.advance(90,90); check(countdown.remaining()==900,"Remaining freezes during break");
    countdown.endBreak(); check(countdown.remaining()==1020,"Remaining resets after break");
    countdown.advance(1100,1100); check(countdown.remaining()==0,"Overdue remaining never negative");
    TimerModel fresh;
    check(fresh.intervalMinutes==30,"First-use interval must be 30 minutes");
    fresh.advance(1799,1799); check(!fresh.due(),"No reminder before 30 playback minutes");
    fresh.advance(1,1); check(fresh.due(),"Reminder at 30 playback minutes");
    fresh.intervalMinutes=45; check(!fresh.due() && fresh.cycle==1800,"Increasing interval preserves elapsed listening");
    fresh.intervalMinutes=15; check(fresh.due() && fresh.cycle==1800,"Lowering interval can immediately trigger reminder");
    TimerModel early; early.advance(100,60); early.startBreak(); early.advance(20,0); early.endBreak();
    check(early.session==100 && early.cycle==0 && early.breakTime==20,"Early break resets only block and preserves project total");
    TimerModel m; m.intervalMinutes=45;
    m.advance(600, 0);
    check(m.session == 600 && m.listening == 0, "Editing must not count as listening");
    m.advance(2700, 2700);
    check(m.due() && m.listening == 2700, "45 playback minutes must trigger reminder");
    m.startBreak(); m.advance(300, 0);
    check(m.session == 3300 && m.breakTime == 300 && !m.due(), "Break must freeze general timer");
    m.startBreak();
    check(m.breakTime == 300, "Repeated start must preserve break");
    m.advance(10, 10);
    check(m.listening == 2710 && m.cycle == 2700 && m.session == 3300, "Playback during break still counts as listening only");
    m.endBreak();
    check(m.cycle == 0 && m.listening == 2710 && m.session == 3300, "Return must retain totals and reset reminder");
    m.advance(60, 20);
    check(m.session == 3360 && m.listening == 2730 && m.cycle == 20, "Resume must continue totals");
    m.endBreak();
    check(m.cycle == 20, "Repeated end must not reset a working cycle");
    m.intervalMinutes = 1; m.advance(40, 40);
    check(m.due(), "Custom interval applies to playback");
    TimerModel restored = m; restored.sanitise();
    check(restored.due() && restored.session == m.session, "State roundtrip preserves timing");
    restored.listening = std::numeric_limits<double>::quiet_NaN(); restored.session = -10;
    restored.intervalMinutes = 0; restored.sanitise();
    check(restored.listening == 0 && restored.session == 0 && restored.intervalMinutes == 1, "Invalid state must be sanitised");
    TimerModel loop;
    for (int i=0; i<44100; ++i) loop.advance(1.0/44100, 1.0/44100);
    check(std::abs(loop.listening - 1) < 1e-9, "Playback duration must not depend on timeline position");
    check(m.pausesStarted == 1 && m.pausesCompleted == 1, "Pause counts must be exact");
    check(m.pauseTotal == 310, "Pause time must include full break");
    TimerModel n; n.begin(1000); n.advance(300, 200); n.startBreak(301000);
    n.advance(60, 10); n.endBreak(361000);
    check(n.cycle == 0 && n.session == 300 && n.listening == 210, "Main block resets, project total preserved");
    check(n.history.size() == 2 && n.history[0].playback == 200 && n.history[1].seconds == 60, "Chronological work and pause segments");
    n.advance(120, 80);
    check(n.cycle == 80 && n.session == 420, "New block starts from zero");
    check(n.completedCycleTotal == 200 && n.longestCycle == 200, "Block analysis");
    std::cout << checks << " checks passed\n";
}
