# SOUNDNA - Safe Hearing 1.5.1

Plugin AU e VST3 per Mac; VST3 per Windows. Nessun installer.
Il plugin lascia l'audio invariato. Conta il tempo in Play della DAW: non misura
il volume reale nelle cuffie/casse e non è un dispositivo di protezione dell'udito.

## Aggiornamento 1.5.1
- Barra inferiore: slider, selettore minuti e conteggio pause in un gruppo centrato.
- Selettore minuti allargato da 62 a 100 px logici, inclusi valori personalizzati a tre cifre.
- Lingua e Azzera: entrambi 90x28 px logici, con margini simmetrici.
- Esporta resoconto: sfondo rosso SOUNDNA #E64949, testo chiaro.

## Interfaccia
- SOUNDNA in rosso maiuscolo; titolo continuo senza vuoti artificiali.
- Layout conforme al riferimento: nome manuale in alto, export a destra,
  ascolto a sinistra, pausa al centro, rimanente/totale a destra, controlli in basso.
- Interfaccia ridimensionabile: base 660x204; larghezza precedente conservata.
- Pulsante centrale sempre disponibile: Inizio la pausa / Si torna al lavoro.
- Durante la pausa, durata crescente in grande al centro; sfondo oscurato del 28%, senza blur.
  Durata e pulsante restano nitidi; le impostazioni di sfondo sono disabilitate durante la pausa.
- Totale progetto sospeso durante la pausa e conservato al rientro.
- Blocco di ascolto azzerato quando si torna al lavoro.
- Alla scadenza, bordo dell'intera finestra e pulsante pulsano lentamente in rosso
  (ciclo 2,8 secondi), senza suoni. La pausa richiede sempre un clic.
- Primo utilizzo 30 minuti. Slider a quattro scatti e menu 15/30/45/60.
- L'ultima durata scelta diventa automaticamente il default per nuove istanze.
  Un progetto salvato conserva il proprio intervallo. Cambiare durata non azzera
  il tempo già conteggiato; una soglia inferiore può attivare subito il promemoria.
- Un clic sul grande display ascolto apre le scelte rapide; il doppio clic modifica
  direttamente i MINUTI PRIMA DELLA PAUSA (1–180), senza finestra aggiuntiva.
  Invio conferma, Esc o perdita del fuoco annullano. Il tempo ascoltato non viene alterato.
  Il clic singolo attende brevemente per distinguerlo dal doppio clic.
  Anche le durate personalizzate diventano il default e sono salvate nel progetto.
- TEMPO RIMANENTE / TIME UNTIL BREAK mostra il conto alla rovescia prima della pausa,
  sotto il quale resta il totale progetto. Il rimanente si ferma a zero alla scadenza.
- Esporta resoconto / Export report: PDF scuro, cronologia e statistiche IT/EN.
- Testi IT/EN incorporati, uso offline, nessuna traduzione via Internet.
- Dimensioni, lingua e durata predefinita salvate sul computer, fuori dal progetto.
  Su Mac: ~/Library/Application Support/SOUNDNA/SafeHearing.settings.
  Non sono sincronizzate tra computer.

## Aggiornare GitHub
1. Estrai lo ZIP. Apri la cartella SOUNDNA_Listen.
2. Carica Source, tests, scripts e CMakeLists.txt nella RADICE del repository,
   sostituendo i file precedenti. Non caricare la cartella esterna come sottocartella.
3. Usa .github/workflows/build.yml incluso. È lo stesso workflow della versione
   1.2.1: macos-15-intel per Mac universale, windows-2022 per Windows x64.
4. Salva con Commit changes su main.
5. Actions > Build SOUNDNA - Safe Hearing > Run workflow > main.
   Non usare Re-run jobs sui vecchi commit.
6. Nel log Mac deve apparire:
   SOUNDNA 1.5.1 - script corretto, verifica architetture
7. Scarica gli Artifacts della nuova esecuzione riuscita:
   SOUNDNA-Safe-Hearing-macOS-universal (AU + VST3 arm64 e x86_64)
   SOUNDNA-Safe-Hearing-Windows-x64 (VST3 x64)

La riga lipo corretta è inclusa nello script. Non sostituirlo con copie vecchie.
I job Mac e Windows sono indipendenti. Firma Mac ad hoc, non notarizzazione Apple.
Gli esempi PDF inclusi contengono dati dimostrativi e sono generati dal plugin.

## Installazione senza installer
Chiudi le DAW prima di sostituire i plugin.
Mac: copia .component in ~/Library/Audio/Plug-Ins/Components/
e .vst3 in ~/Library/Audio/Plug-Ins/VST3/.
Windows: copia il bundle .vst3 in C:\Program Files\Common Files\VST3\.
Rimuovi dalle cartelle scandite le vecchie copie chiamate SOUNDNA Listen:
condividono gli stessi identificativi. Riavvia la DAW e riesegui la scansione se serve.
Il nome delle cartelle sorgenti resta tecnico; il prodotto si chiama SOUNDNA - Safe Hearing.

## Uso e limiti da conoscere
Inserisci una sola istanza sul master per monitorare il progetto.
Salva il progetto DAW per conservare timer e cronologia.
Il timer generale parte quando viene creata l'istanza del plugin, non prima.
Il promemoria visivo è visibile soltanto quando la finestra del plugin è aperta.
Il conteggio prosegue con editor chiuso; host, bypass e sospensione possono
influire sulle chiamate audio. L'export offline è escluso quando segnalato dalla DAW.
Se lasci il Play attivo durante una pausa, il totale di riproduzione del PDF
continua a contarlo; blocco e totale progetto restano sospesi.
Il plugin non ferma il trasporto e non silenzia l'audio.
Il nome del progetto viene inserito esclusivamente dall'utente e salvato con il progetto DAW.
Non viene letto alcun nome traccia/contesto: sincronizzazione e pulsante rimossi.
I nomi manuali precedenti vengono mantenuti. I nomi importati automaticamente dalla
versione 1.4 vengono svuotati, senza alterare timer e cronologia.
Le durate escludono le lunghe sospensioni del thread messaggi; orari PDF in UTC.
Azzera cancella conteggi e cronologia con conferma; non serve per fare una pausa.
I documenti AGGIORNAMENTO_1_1.md e AGGIORNAMENTO_1_2.md sono storici:
per installare/compilare seguire questo README.

## Compilazione e verifica
Mac: bash scripts/build-mac.command (Xcode Command Line Tools e CMake 3.22+).
Windows: scripts/build-windows.ps1 (Visual Studio C++ e CMake).
JUCE 8.0.12 viene scaricato in fase di compilazione; il plugin finale lavora offline.
AAX è opzionale e richiede SDK/licenze/firma Avid; non incluso nei job standard.
Consultare VERIFICHE.md per i controlli eseguiti e i limiti della validazione.
