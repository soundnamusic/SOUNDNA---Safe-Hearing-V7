#pragma once
#include <array>
#include <cstddef>

namespace SafeHearing {
enum class Language { Italian=0, English=1 };
#define SAFEHEARING_TEXTS(X) \
 X(ProjectPlaceholder,"Nome progetto per il report...","Project name for the report...") \
 X(PauseControl,"Controllo pausa","Break control") \
 X(StartBreak,"Inizio la pausa","Start my break") \
 X(EndBreak,"Si torna al lavoro","Back to work") \
 X(ExportPdf,"Esporta resoconto","Export report") \
 X(ResetButton,"Azzera...","Reset...") \
 X(ResetTitle,"Azzerare il progetto?","Reset this project?") \
 X(ResetMessage,"Totali e cronologia saranno cancellati. Esporta prima il PDF se vuoi conservarli. Nome e intervallo restano invariati.","Totals and history will be cleared. Export the PDF first to keep a record. The project name and reminder interval will stay unchanged.") \
 X(ResetConfirm,"Azzera","Reset") \
 X(Cancel,"Annulla","Cancel") \
 X(IntervalTitle,"Intervallo promemoria in minuti","Reminder interval in minutes") \
 X(LanguageTitle,"Lingua dell'interfaccia e del report","Interface and report language") \
 X(ExportTitle,"Esporta resoconto","Export report") \
 X(ProjectFile,"Progetto","Project") \
 X(RequirePdf,"Usa un nome con estensione .pdf","Use a filename ending in .pdf") \
 X(PdfSaved,"PDF salvato","PDF saved") \
 X(PdfFailed,"PDF non salvato: controlla il percorso","PDF not saved: check the location") \
 X(PreferencesFailed,"Preferenze non salvate","Preferences could not be saved") \
 X(ListeningLabel,"ASCOLTO / BLOCCO ATTUALE","LISTENING / CURRENT BLOCK") \
 X(Remaining,"TEMPO RIMANENTE","TIME UNTIL BREAK") \
 X(EditInterval,"Un clic: scelte rapide. Doppio clic: minuti liberi (1–180). Invio conferma, Esc annulla.","Click: presets. Double-click: custom minutes (1–180). Enter confirms, Esc cancels.") \
 X(ProjectTotal,"TOTALE PROGETTO","PROJECT TOTAL") \
 X(SessionPaused,"sospeso durante la pausa","paused during your break") \
 X(SessionDescription,"lavoro + editing, pause escluse","work + editing, excluding breaks") \
 X(BreakDuration,"DURATA PAUSA","BREAK DURATION") \
 X(DefaultIntervalHint,"La scelta viene ricordata per i nuovi progetti","Your choice is remembered for new projects") \
 X(Break,"Pausa","Break") \
 X(Reminder,"È il momento di una pausa","Time for a break") \
 X(Playing,"PLAY - ascolto in corso","PLAY - listening") \
 X(Offline,"Export offline escluso","Offline export excluded") \
 X(NoTransport,"Trasporto non disponibile","Transport unavailable") \
 X(Waiting,"In attesa del Play","Waiting for Play") \
 X(BreakEvery,"Pausa ogni","Break every") \
 X(PlayDuringBreak,"Play attivo durante la pausa","Playback active during break") \
 X(BreakCompleted,"pausa conclusa","break completed") \
 X(BreaksCompleted,"pause concluse","breaks completed") \
 X(UntitledProject,"Progetto senza nome","Untitled project") \
 X(Exported,"Esportato: ","Exported: ") \
 X(WorkMetric,"LAVORO DEL PROGETTO","PROJECT WORK TIME") \
 X(PlaybackMetric,"RIPRODUZIONE TOTALE","TOTAL PLAYBACK") \
 X(BreakMetric,"TEMPO IN PAUSA","TIME ON BREAK") \
 X(BreakCountsMetric,"PAUSE AVVIATE / CONCLUSE","BREAKS STARTED / FINISHED") \
 X(BlockMetric,"BLOCCO DI ASCOLTO ATTUALE","CURRENT LISTENING BLOCK") \
 X(BreakAverageMetric,"PAUSA MEDIA CONCLUSA","AVERAGE COMPLETED BREAK") \
 X(WorkflowHeading,"LETTURA DEL FLUSSO","WORKFLOW INSIGHTS") \
 X(BlockAverage,"Ascolto medio dei blocchi conclusi: ","Average listening in completed blocks: ") \
 X(LongestBlock,"   |   Blocco piu lungo: ","   |   Longest block: ") \
 X(CurrentReminder,"Promemoria attuale: ","Current reminder: ") \
 X(ReminderNote," min di Play. Stop/editing inclusi nel lavoro."," min of playback. Work includes Stop/editing time.") \
 X(PlaybackNote,"Il Play durante una pausa resta nella riproduzione totale, non nel lavoro del progetto.","Playback during a break counts towards total playback, but not towards project work time.") \
 X(LegacyNote,"Totali v1 conservati. Cronologia precedente non disponibile; statistiche pause parziali.","v1 totals retained. Earlier history is unavailable; break statistics are partial.") \
 X(MeasurementNote,"Durate misurate dal plugin. Tempo a progetto chiuso escluso; salvataggio nella DAW.","Durations measured by the plugin. Closed-project time excluded; save your DAW project.") \
 X(Timeline,"CRONOLOGIA","TIMELINE") \
 X(Activity,"ATTIVITA","ACTIVITY") \
 X(StartUtc,"INIZIO UTC","START UTC") \
 X(EndUtc,"FINE UTC","END UTC") \
 X(Duration,"DURATA","DURATION") \
 X(Work,"Lavoro","Work") \
 X(InProgress,"in corso","in progress") \
 X(Checkpoint,"ripresa salvata","saved resume") \
 X(Completed,"concluso","completed") \
 X(TimelineNote,"'In corso': fotografia al momento dell'export. 'Ripresa salvata': segmento dopo riapertura.","'In progress': snapshot at export. 'Saved resume': a segment after reopening the project.") \
 X(DurationNote,"Le durate escludono sospensioni e possono differire dall'intervallo tra gli orari UTC.","Durations exclude suspensions and may differ from the interval between UTC timestamps.") \
 X(ReportFooter,"Analisi del flusso di lavoro","Workflow analysis") \
 X(Page,"Pagina ","Page ")

enum class Text {
#define SAFEHEARING_ENUM(key,it,en) key,
 SAFEHEARING_TEXTS(SAFEHEARING_ENUM)
#undef SAFEHEARING_ENUM
 Count
};
struct Translation { const char* italian; const char* english; };
inline constexpr std::array<Translation,static_cast<std::size_t>(Text::Count)> translations {{
#define SAFEHEARING_ROW(key,it,en) {it,en},
 SAFEHEARING_TEXTS(SAFEHEARING_ROW)
#undef SAFEHEARING_ROW
}};
#undef SAFEHEARING_TEXTS
inline const char* tr(Language language,Text key)
{
    const auto& entry=translations[static_cast<std::size_t>(key)];
    return language==Language::English?entry.english:entry.italian;
}
}
