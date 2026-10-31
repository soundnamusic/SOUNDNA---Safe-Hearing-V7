#include "../Source/UiPreferences.h"
#include <iostream>
#include <stdexcept>
void check(bool good,const char* message){if(!good)throw std::runtime_error(message);}
int main()
{
    const auto path=juce::File::getSpecialLocation(juce::File::tempDirectory)
        .getNonexistentChildFile("safehearing-preference-test",".settings");
    {
        UiPreferences first(path);
        check(first.width()==660 && first.language()==SafeHearing::Language::Italian,"Default preferences");
        check(first.defaultInterval()==30,"First-use default interval");
        first.setDefaultInterval(45);
        first.setWidth(990); first.setLanguage(SafeHearing::Language::English);
        check(first.flush(),"Preference save");
    }
    {
        UiPreferences nextProject(path);
        check(nextProject.defaultInterval()==45,"Interval must survive new project");
        for(int value : {15,30,45,60}) { nextProject.setDefaultInterval(value); check(nextProject.defaultInterval()==value,"Allowed intervals"); }
        nextProject.setDefaultInterval(17); check(nextProject.defaultInterval()==17,"Custom default accepted");
        nextProject.setDefaultInterval(181); check(nextProject.defaultInterval()==17,"Reject out-of-range default");
        check(nextProject.width()==990,"Width must survive new instance/project");
        check(UiPreferences::heightFor(nextProject.width())==306,"Aspect ratio must survive");
        check(nextProject.language()==SafeHearing::Language::English,"Language must survive restart");
        nextProject.setWidth(90000);check(nextProject.width()==1320,"Upper bound");
        nextProject.setWidth(1);check(nextProject.width()==528,"Lower bound");
        nextProject.setLanguage(SafeHearing::Language::Italian);check(nextProject.flush(),"Second save");
    }
    {
        UiPreferences reopened(path);
        check(reopened.defaultInterval()==17,"Chosen interval persists after restart");
        check(reopened.width()==528 && reopened.language()==SafeHearing::Language::Italian,"Updated preference roundtrip");
    }
    check(path.deleteFile(),"Temporary test file cleanup");
    for(const auto& text:SafeHearing::translations)
        check(text.italian && *text.italian && text.english && *text.english,"Every embedded translation must exist");
    std::cout<<"Global size/language persistence, bounds and translation coverage passed\n";
}
