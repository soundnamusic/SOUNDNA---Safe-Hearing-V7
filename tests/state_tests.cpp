#include "../Source/StateCodec.h"
#include <iostream>
#include <stdexcept>
void check(bool ok,const char* what){if(!ok)throw std::runtime_error(what);}
int main()
{
    using namespace SoundnaState;
    State a;a.project=juce::String::fromUTF8("Mix città");a.timer.begin(1000000);
    a.timer.advance(3000,2700);a.timer.startBreak(4000000);a.timer.advance(600,0);
    a.timer.intervalMinutes=45;
    a.savedAt=4600000;
    const auto xml=encode(a);
    auto parsed=juce::XmlDocument::parse(xml->toString());check(parsed!=nullptr,"XML parsing");
    State b;check(decode(*parsed,b,10000000),"Roundtrip");
    check(b.timer.intervalMinutes==45,"Saved project interval overrides new default 30");
    check(b.project==a.project && b.timer.pausesStarted==1 && b.timer.pausesCompleted==0,"Title and pause counts");
    check(b.timer.session==3000 && b.timer.pauseTotal==600 && b.timer.onBreak,"Pause and totals persisted");
    check(b.timer.history.size()==2 && b.timer.active.continuation,"Checkpoint seals current segment");
    b.timer.advance(60,0);b.timer.endBreak(10060000);
    check(b.timer.cycle==0 && b.timer.session==3000 && b.timer.pauseTotal==660,"Resume excludes closed-project time");
    check(b.timer.pausesStarted==1 && b.timer.pausesCompleted==1,"Reopened pause counted once");
    check(b.timer.history.back().seconds==60 && b.timer.history.back().pause,"Chronology resume segment");
    State c; auto saved=encode(b);check(decode(*saved,c,20000000),"Second roundtrip");
    check(c.timer.session==3000 && c.timer.listening==2700,"Second restore totals stable");
    juce::XmlElement old("SOUNDNA_LISTEN");old.setAttribute("version",1);old.setAttribute("session",900.0);
    old.setAttribute("listening",700.0);old.setAttribute("cycle",100.0);old.setAttribute("interval",45);
    State migrated;check(decode(old,migrated,30000000),"v1 migration");
    check(migrated.timer.session==900 && migrated.timer.cycle==100 && migrated.timer.legacyTotals,"v1 totals retained");
    check(migrated.timer.history.empty() && migrated.timer.pausesStarted==0,"No invented old pauses");
    old.setAttribute("version",2); old.setAttribute("project","Existing manual name"); old.setAttribute("interval",17);
    check(decode(old,migrated,30000000) && migrated.timer.intervalMinutes==17,"Old manual names and custom intervals retained");
    old.setAttribute("followHostName",true);
    check(decode(old,migrated,30000000) && migrated.project.isEmpty(),"Discard automatically imported 1.4 titles");
    check(migrated.timer.session==900 && migrated.timer.cycle==100,"Title migration preserves timers");
    auto manual=encode(a);check(!manual->getBoolAttribute("followHostName",true),"Saved titles remain manual in older versions too");
    old.setAttribute("version",99);check(!decode(old,migrated,30000000),"Reject unknown state version");
    std::cout<<"State roundtrip, checkpoint reopen and v1 migration passed\n";
}
