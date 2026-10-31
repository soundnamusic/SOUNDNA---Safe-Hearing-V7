> Documento storico. Per compilare seguire README_IT.md e il workflow incluso aggiornato.

# SOUNDNA - Safe Hearing 1.2

## Nome

Il nome esatto del prodotto e SOUNDNA - Safe Hearing. Compare nell'interfaccia,
nel nome restituito alla DAW, nei report e nei bundle compilati:

- SOUNDNA - Safe Hearing.component
- SOUNDNA - Safe Hearing.vst3

Gli identificativi AU/VST3/AAX e lo stato interno sono conservati per mantenere
le istanze dei progetti precedenti. Le cartelle sorgenti/target CMake e il nome
ZIP tecnico restano quelli precedenti per non rompere il workflow gia esistente.

## Italiano / English, tutto offline

Il selettore in alto cambia immediatamente lingua di etichette, pulsanti,
conferme, messaggi di stato e PDF. Tutte le traduzioni sono incluse nel plugin:
nessuna richiesta Internet, traduzione esterna o download di dizionari.

- Inizio la pausa / Start my break
- Si torna al lavoro / Back to work
- Esporta PDF / Export PDF

Il nome del progetto inserito dall'utente non viene tradotto. Le finestre native
per scegliere il percorso di salvataggio usano la lingua del sistema operativo.
La lingua selezionata viene ricordata anche aprendo un progetto nuovo.

## Ridimensionamento persistente

Trascina l'angolo inferiore destro della finestra. Tutta l'interfaccia si scala
insieme, mantenendo il formato orizzontale e le proporzioni.

- Dimensione iniziale: 660 x 172.
- Minimo: circa 528 x 138 (80%).
- Massimo: 1320 x 344 (200%).

Dimensione e lingua sono preferenze dell'utente del computer, non del progetto.
Sono salvate dopo mezzo secondo di inattivita del ridimensionamento e quando
chiudi l'editor. Un nuovo progetto e una nuova istanza riprendono le preferenze.
Su Mac il file e ~/Library/Application Support/SOUNDNA/SafeHearing.settings.
Su Windows e nella cartella dati applicazioni dell'utente, sotto SOUNDNA.
Le finestre gia aperte conservano la propria dimensione; una nuova finestra usa
l'ultima dimensione scelta. I testi cambiano anche nelle altre finestre aperte
nello stesso processo. Le preferenze non si sincronizzano tra computer.

I timer e la cronologia restano associati al progetto DAW come nella v1.1.
Caricare un vecchio progetto non ripristina la dimensione di un altro progetto.
Il salvataggio delle preferenze richiede che la cartella utente sia scrivibile.

## Aggiornamento GitHub

1. In Code > Add file > Upload files sostituisci Source, tests, scripts e
   CMakeLists.txt con quelli di questo archivio. In questa versione serve anche
   scripts, perche i nomi dei bundle sono cambiati.
2. Conferma il commit e avvia una NUOVA esecuzione del workflow sul ramo main.
   Il workflow che richiama bash scripts/build-mac.command resta valido.
3. Scarica il nuovo artifact. Lo ZIP di output ha ancora il nome tecnico
   SOUNDNA_Listen_macOS_arm64.zip per compatibilita con il workflow precedente,
   ma contiene i bundle con il nome SOUNDNA - Safe Hearing.
4. Salva il progetto e chiudi Logic/Ableton. Sposta i vecchi SOUNDNA Listen.component
   e SOUNDNA Listen.vst3 fuori dalle cartelle scandite dalle DAW, poi copia i nuovi.
   NON tenere vecchia e nuova copia insieme: condividono gli stessi identificativi.
5. Riavvia la DAW ed eventualmente riesegui la scansione dei plugin.

Nessun installer e nessuna modifica automatica alle cartelle dei plugin.
Gli esempi PDF IT/EN inclusi usano dati dimostrativi.
