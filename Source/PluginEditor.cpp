#include "PluginEditor.h"

using SafeHearing::Text;
namespace {
const juce::Colour bg(0xff121317), panel(0xff1c1e24), red(0xffe64949), white(0xfff4f4f5), grey(0xffa2a5af), line(0xff363a42);
}
ListenEditor::ListenEditor(ListenProcessor& p) : AudioProcessorEditor(p), listenProcessor(p)
{
    look.setColour(juce::TextButton::buttonColourId, line);
    look.setColour(juce::TextButton::textColourOffId, white);
    look.setColour(juce::Slider::textBoxTextColourId, white);
    look.setColour(juce::Slider::textBoxBackgroundColourId, panel);
    look.setColour(juce::Slider::textBoxOutlineColourId, line);
    look.setColour(juce::Slider::backgroundColourId, panel);
    look.setColour(juce::Slider::thumbColourId, red);
    look.setColour(juce::Slider::trackColourId, red);
    look.setColour(juce::ComboBox::backgroundColourId,panel);
    look.setColour(juce::ComboBox::textColourId,white);
    look.setColour(juce::ComboBox::outlineColourId,line);
    setLookAndFeel(&look);
    language=preferences->language();
    addAndMakeVisible(content);
    content.setInterceptsMouseClicks(false,true);
    content.addAndMakeVisible(pauseButton); content.addAndMakeVisible(resetButton); content.addAndMakeVisible(interval);
    content.addAndMakeVisible(exportButton); content.addAndMakeVisible(project);
    content.addAndMakeVisible(languageSelector);
    content.addAndMakeVisible(intervalSelector);
    content.addAndMakeVisible(listeningDisplay);
    listeningDisplay.setFont(juce::FontOptions(32));
    listeningDisplay.setColour(juce::Label::textColourId,white);
    listeningDisplay.setColour(juce::Label::backgroundColourId,juce::Colours::transparentBlack);
    listeningDisplay.setColour(juce::Label::outlineColourId,juce::Colours::transparentBlack);
    listeningDisplay.setColour(juce::Label::textWhenEditingColourId,white);
    listeningDisplay.setColour(juce::Label::backgroundWhenEditingColourId,panel);
    listeningDisplay.setColour(juce::Label::outlineWhenEditingColourId,juce::Colours::transparentBlack);
    listeningDisplay.setEditable(false,true,true);
    listeningDisplay.addMouseListener(this,false);
    listeningDisplay.onEditorShow=[this] {
        pendingClickAt=0;
        if(auto* editor=listeningDisplay.getCurrentTextEditor()) {
            editor->setInputRestrictions(3,"0123456789");
            editor->setText(juce::String(state.intervalMinutes)); editor->selectAll();
            editor->setColour(juce::TextEditor::outlineColourId,juce::Colours::transparentBlack);
            editor->setColour(juce::TextEditor::focusedOutlineColourId,juce::Colours::transparentBlack);
        }
    };
    listeningDisplay.onTextChange=[this] {
        const auto value=listeningDisplay.getText();
        if(value.containsOnly("0123456789") && value.isNotEmpty())chooseInterval(value.getIntValue());
        timerCallback();
    };
    for(int minutes : {15,30,45,60})intervalSelector.addItem(juce::String(minutes)+" min",minutes);
    intervalSelector.onChange=[this] { chooseInterval(intervalSelector.getSelectedId()); };
    languageSelector.addItem("Italiano",1); languageSelector.addItem("English",2);
    languageSelector.onChange=[this] {
        language=languageSelector.getSelectedId()==2?SafeHearing::Language::English:SafeHearing::Language::Italian;
        preferences->setLanguage(language); applyLanguage();
    };
    project.setTextToShowWhenEmpty(t(Text::ProjectPlaceholder),grey);
    project.setInputRestrictions(80); project.setMultiLine(false);
    project.setFont(juce::FontOptions(14));
    project.setColour(juce::TextEditor::backgroundColourId,bg);
    project.setColour(juce::TextEditor::textColourId,white);
    project.setColour(juce::TextEditor::outlineColourId,bg);
    project.setColour(juce::TextEditor::focusedOutlineColourId,line);
    project.setText(listenProcessor.getProjectTitle(),false);
    project.onTextChange=[this] { listenProcessor.setProjectTitle(project.getText()); };
    project.onReturnKey=[this] { project.giveAwayKeyboardFocus(); };
    pauseButton.setTitle(t(Text::PauseControl));
    pauseButton.onClick=[this] { listenProcessor.toggleBreak(); timerCallback(); };
    exportButton.setColour(juce::TextButton::buttonColourId,red);
    exportButton.setColour(juce::TextButton::textColourOffId,white);
    exportButton.onClick=[this] { exportReport(); };
    resetButton.setColour(juce::TextButton::buttonColourId,bg);
    resetButton.setColour(juce::TextButton::textColourOffId,grey);
    resetButton.onClick=[this] {
        juce::Component::SafePointer<ListenEditor> safe(this);
        juce::AlertWindow::showAsync(juce::MessageBoxOptions().withIconType(juce::MessageBoxIconType::QuestionIcon)
            .withTitle(t(Text::ResetTitle))
            .withMessage(t(Text::ResetMessage))
            .withButton(t(Text::ResetConfirm)).withButton(t(Text::Cancel)).withAssociatedComponent(this),
            [safe](int result) { if(safe!=nullptr && result==1) { safe->listenProcessor.resetTotals(); safe->timerCallback(); } });
    };
    interval.setSliderStyle(juce::Slider::LinearHorizontal);
    interval.setRange(15,60,15); interval.setTextBoxStyle(juce::Slider::NoTextBox,false,0,0);
    interval.setTextValueSuffix(" min"); interval.setTitle(t(Text::IntervalTitle));
    interval.onValueChange=[this] { chooseInterval(static_cast<int>(interval.getValue())); };
    applyLanguage();
    setResizable(true,true);
    setResizeLimits(UiPreferences::minWidth,UiPreferences::heightFor(UiPreferences::minWidth),
                    UiPreferences::maxWidth,UiPreferences::heightFor(UiPreferences::maxWidth));
    getConstrainer()->setFixedAspectRatio((double)UiPreferences::baseWidth/UiPreferences::baseHeight);
    setSize(preferences->width(),UiPreferences::heightFor(preferences->width()));
    storeResize=true;
    timerCallback(); startTimerHz(20);
}
ListenEditor::~ListenEditor() { stopTimer(); preferences->flush(); fileChooser.reset(); setLookAndFeel(nullptr); }
juce::String ListenEditor::t(Text key) const { return juce::String::fromUTF8(SafeHearing::tr(language,key)); }
void ListenEditor::applyLanguage()
{
    languageSelector.setSelectedId(language==SafeHearing::Language::English?2:1,juce::dontSendNotification);
    languageSelector.setTitle(t(Text::LanguageTitle));
    listeningDisplay.setTooltip(t(Text::EditInterval)); listeningDisplay.setTitle(t(Text::IntervalTitle));
    project.setTextToShowWhenEmpty(t(Text::ProjectPlaceholder),grey);
    pauseButton.setTitle(t(Text::PauseControl));
    interval.setTitle(t(Text::IntervalTitle));
    intervalSelector.setTitle(t(Text::IntervalTitle));
    intervalSelector.setTooltip(t(Text::DefaultIntervalHint));
    resetButton.setButtonText(t(Text::ResetButton)); exportButton.setButtonText(t(Text::ExportPdf));
    pauseButton.setButtonText(state.onBreak?t(Text::EndBreak):t(Text::StartBreak));
    exportStatus.clear(); statusTicks=0; repaint();
}
void ListenEditor::mouseUp(const juce::MouseEvent& e)
{
    if(e.eventComponent==&listeningDisplay && !state.onBreak)
        pendingClickAt=e.getNumberOfClicks()==1 ? juce::Time::getMillisecondCounterHiRes()+juce::MouseEvent::getDoubleClickTimeout()+50 : 0;
}
void ListenEditor::showIntervalMenu()
{
    juce::PopupMenu menu;
    for(int m : {15,30,45,60})menu.addItem(m,juce::String(m)+" min",true,state.intervalMinutes==m);
    juce::Component::SafePointer<ListenEditor> safe(this);
    menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(&listeningDisplay),
        [safe](int m){if(safe!=nullptr && m>0 && !safe->state.onBreak)safe->chooseInterval(m);});
}
void ListenEditor::chooseInterval(int minutes)
{
    if(!UiPreferences::validInterval(minutes))return;
    listenProcessor.setInterval(minutes);
    preferences->setDefaultInterval(minutes);
    timerCallback();
}
void ListenEditor::exportReport()
{
    if(fileChooser)return;
    auto name=juce::File::createLegalFileName(listenProcessor.getProjectTitle().trim());
    if(name.isEmpty())name=t(Text::ProjectFile);
    fileChooser=std::make_unique<juce::FileChooser>(t(Text::ExportTitle),
        juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("SOUNDNA_"+name+".pdf"),"*.pdf");
    exportButton.setEnabled(false);
    juce::Component::SafePointer<ListenEditor> safe(this);
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles
        | juce::FileBrowserComponent::warnAboutOverwriting, [safe](const juce::FileChooser& chooser) {
        if(safe==nullptr)return;
        const auto target=chooser.getResult();
        safe->exportButton.setEnabled(true);
        if(target!=juce::File()) {
            if(!target.hasFileExtension("pdf"))safe->exportStatus=safe->t(Text::RequirePdf);
            else {
                auto report=safe->listenProcessor.reportSnapshot(); report.language=safe->language;
                const auto bytes=SoundnaReport::createPdf(report);
                safe->exportStatus=target.replaceWithData(bytes.data(),bytes.size()) ? safe->t(Text::PdfSaved) : safe->t(Text::PdfFailed);
            }
            safe->statusTicks=160;
        }
        // Release after the native chooser's callback has returned.
        juce::MessageManager::callAsync([safe] { if(safe!=nullptr)safe->fileChooser.reset(); });
    });
}
void ListenEditor::timerCallback()
{
    if(language!=preferences->language()) { language=preferences->language(); applyLanguage(); }
    preferenceWriteFailed=!preferences->flushIfReady();
    if(preferenceWriteFailed && !preferenceWarningShown) {
        preferenceWarningShown=true; exportStatus=t(Text::PreferencesFailed); statusTicks=160;
    }
    if(!preferenceWriteFailed)preferenceWarningShown=false;
    state=listenProcessor.snapshot();
    if(!listeningDisplay.isBeingEdited())listeningDisplay.setText(timeText(state.cycle),juce::dontSendNotification);
    for(auto* c : std::initializer_list<juce::Component*>{&project,&listeningDisplay,&interval,&intervalSelector,&resetButton,&languageSelector})c->setEnabled(!state.onBreak);
    if(state.onBreak)pendingClickAt=0;
    if(pendingClickAt>0 && juce::Time::getMillisecondCounterHiRes()>=pendingClickAt) {
        pendingClickAt=0; if(!listeningDisplay.isBeingEdited())showIntervalMenu();
    }
    interval.setValue(state.intervalMinutes,juce::dontSendNotification);
    if(state.intervalMinutes==15 || state.intervalMinutes==30 || state.intervalMinutes==45 || state.intervalMinutes==60)
        intervalSelector.setSelectedId(state.intervalMinutes,juce::dontSendNotification);
    else
        intervalSelector.setText(juce::String(state.intervalMinutes)+" min",juce::dontSendNotification);
    if(!project.hasKeyboardFocus(true)) {
        const auto title=listenProcessor.getProjectTitle();
        if(project.getText()!=title)project.setText(title,false);
    }
    pauseButton.setButtonText(state.onBreak?t(Text::EndBreak):t(Text::StartBreak));
    reminderOpacity=juce::jlimit(0.0f,1.0f,reminderOpacity+(state.due()?0.025f:-0.1f));
    const auto phase=juce::Time::getMillisecondCounterHiRes() * (juce::MathConstants<double>::twoPi/2800.0);
    reminderGlow=reminderOpacity*static_cast<float>(0.72+0.28*std::sin(phase));
    pauseButton.setColour(juce::TextButton::buttonColourId,
        state.onBreak?red.darker(0.35f):line.interpolatedWith(red,reminderGlow));
    pauseButton.setColour(juce::TextButton::textColourOffId,white);
    if(statusTicks>0 && --statusTicks==0)exportStatus.clear();
    repaint();
}
juce::String ListenEditor::timeText(double seconds) { return SoundnaReport::duration(seconds); }
void ListenEditor::text(juce::Graphics& g,juce::String value,juce::Rectangle<int> box,float size,
                        juce::Colour colour,juce::Justification align)
{
    g.setColour(colour); g.setFont(juce::FontOptions(size)); g.drawText(value,box,align,true);
}
void ListenEditor::resized()
{
    const float scale=getWidth()/(float)UiPreferences::baseWidth;
    content.setBounds(0,0,UiPreferences::baseWidth,UiPreferences::baseHeight);
    content.setTransform(juce::AffineTransform::scale(scale));
    project.setBounds(242,10,194,28);
    languageSelector.setBounds(20,164,90,28); listeningDisplay.setBounds(28,82,190,42);
    exportButton.setBounds(452,10,180,28);
    pauseButton.setBounds(234,112,198,35);
    // A 350 px group centred at x=330; equal 45 px gaps to the outer controls.
    interval.setBounds(155,164,100,28); intervalSelector.setBounds(267,164,100,28);
    resetButton.setBounds(550,164,90,28);
    if(storeResize)preferences->setWidth(getWidth());
}
void ListenEditor::paint(juce::Graphics& g)
{
    g.fillAll(bg);
    g.addTransform(juce::AffineTransform::scale(getWidth()/(float)UiPreferences::baseWidth));
    // One text layout keeps the two colours without a fixed-width gap in the brand.
    juce::AttributedString brand;
    const juce::Font brandFont(juce::FontOptions(18));
    brand.append("SOUNDNA",brandFont,red);
    brand.append(" - Safe Hearing",brandFont,white);
    juce::TextLayout brandLayout; brandLayout.createLayout(brand,1000.0f);
    {
        juce::Graphics::ScopedSaveState titleState(g);
        const auto titleScale=std::min(1.0f,202.0f / std::max(1.0f,brandLayout.getWidth()));
        g.addTransform(juce::AffineTransform::scale(titleScale).translated(20.0f,11.0f));
        brandLayout.draw(g,{0.0f,0.0f,brandLayout.getWidth()+1.0f,25.0f/titleScale});
    }
    g.setColour(red); g.fillRect(230,16,2,15);
    g.setColour(line); g.drawHorizontalLine(46,20,644);
    g.setColour(panel); g.fillRoundedRectangle(16,52,210,95,5);
    text(g,t(Text::ListeningLabel),{30,60,190,17},10,grey);
    const float progress=static_cast<float>(juce::jlimit(0.0,1.0,state.cycle/(state.intervalMinutes*60.0)));
    g.setColour(line); g.fillRect(30,134,186,3);
    g.setColour(red); g.fillRect(30.0f,134.0f,186.0f*progress,3.0f);
    text(g,t(Text::Remaining),{456,55,178,14},10,grey);
    text(g,timeText(std::ceil(state.remaining())),{454,70,180,30},27,white);
    text(g,t(Text::ProjectTotal),{456,105,178,14},10,grey);
    text(g,timeText(state.session),{454,119,180,28},25,white);
    const int transport=listenProcessor.transportStatus();
    if(state.onBreak) {
        text(g,t(Text::BreakDuration),{234,55,198,15},10,grey,juce::Justification::centred);
        text(g,timeText(state.breakTime),{234,73,198,34},32,white,juce::Justification::centred);
    } else {
        const juce::String status=state.due()?t(Text::Reminder):
            transport==2?t(Text::Playing):transport==3?t(Text::Offline):
            transport==4?t(Text::NoTransport):t(Text::Waiting);
        text(g,status,{234,67,198,30},12,state.due()?red:grey,juce::Justification::centred);
    }
    g.setColour(line); g.drawHorizontalLine(156,20,644);
    juce::String footer=exportStatus.isNotEmpty()?exportStatus:
        state.onBreak && transport==2?t(Text::PlayDuringBreak):
        juce::String(state.pausesCompleted)+" "+t(state.pausesCompleted==1?Text::BreakCompleted:Text::BreaksCompleted);
    text(g,footer,{379,165,126,26},11,grey);
}
void ListenEditor::paintOverChildren(juce::Graphics& g)
{
    if(state.onBreak) {
        // Dim the background while keeping the active break controls fully readable.
        juce::Graphics::ScopedSaveState save(g);
        const float s=getWidth()/(float)UiPreferences::baseWidth;
        g.excludeClipRegion(juce::Rectangle<float>(230,52,206,98).transformedBy(juce::AffineTransform::scale(s)).getSmallestIntegerContainer());
        g.setColour(juce::Colours::black.withAlpha(0.28f)); g.fillAll();
    }
    // Paint over child components so the reminder is visible around the whole editor.
    // A slow 2.8-second pulse, with no sound and no automatic break.
    if(reminderGlow<=0.001f)return;
    const float scale=getWidth()/(float)UiPreferences::baseWidth;
    const auto bounds=getLocalBounds().toFloat().reduced(2.0f*scale);
    for(int i=4;i>=1;--i) {
        g.setColour(red.withAlpha(reminderGlow*0.035f));
        g.drawRoundedRectangle(bounds,7.0f*scale,(float)(i*3)*scale);
    }
    g.setColour(red.withAlpha(reminderGlow));
    g.drawRoundedRectangle(bounds,7.0f*scale,2.5f*scale);
}
