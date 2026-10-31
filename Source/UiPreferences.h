#pragma once
#include "Translations.h"
#include <juce_data_structures/juce_data_structures.h>

// Shared by editors in this process; stored per OS user, outside any DAW project.
// Read when creating a processor; preference edits happen on the UI thread. Never used in processBlock.
class UiPreferences
{
public:
    static constexpr int baseWidth=660, baseHeight=204, minWidth=528, maxWidth=1320;
    static juce::PropertiesFile::Options options()
    {
        juce::PropertiesFile::Options o;
        o.applicationName="SafeHearing"; o.filenameSuffix="settings";
        o.folderName="SOUNDNA"; o.osxLibrarySubFolder="Application Support";
        o.storageFormat=juce::PropertiesFile::storeAsXML;
        o.millisecondsBeforeSaving=-1; // explicit, debounced writes in the editor timer
        o.commonToAllUsers=false;
        return o;
    }
    UiPreferences() : file(options()) {}
    // Injection point for a real file persistence test, without touching user settings.
    explicit UiPreferences(const juce::File& path) : file(path,options()) {}
    static bool validInterval(int minutes) { return minutes>=1 && minutes<=180; }
    int defaultInterval() const
    {
        const int value=file.getIntValue("defaultIntervalMinutes",30);
        return validInterval(value)?value:30;
    }
    void setDefaultInterval(int minutes)
    {
        if(!validInterval(minutes))return;
        file.setValue("defaultIntervalMinutes",minutes);
        lastEditMs=juce::Time::getMillisecondCounterHiRes();
    }
    int width() const { return juce::jlimit(minWidth,maxWidth,file.getIntValue("editorWidth",baseWidth)); }
    static int heightFor(int width) { return juce::roundToInt(width * (double)baseHeight/baseWidth); }
    SafeHearing::Language language() const
    { return file.getIntValue("language",0)==1?SafeHearing::Language::English:SafeHearing::Language::Italian; }
    void setWidth(int w)
    {
        const auto width=juce::jlimit(minWidth,maxWidth,w);
        file.setValue("editorWidth",width); file.setValue("editorHeight",heightFor(width));
        lastEditMs=juce::Time::getMillisecondCounterHiRes();
    }
    void setLanguage(SafeHearing::Language language)
    { file.setValue("language",language==SafeHearing::Language::English?1:0); lastEditMs=juce::Time::getMillisecondCounterHiRes(); }
    bool flushIfReady()
    { return juce::Time::getMillisecondCounterHiRes()-lastEditMs>=500?file.saveIfNeeded():true; }
    bool flush() { return file.saveIfNeeded(); }
private:
    juce::PropertiesFile file;
    double lastEditMs=0;
};
