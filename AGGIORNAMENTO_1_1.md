> Documento storico. Per compilare seguire README_IT.md e il workflow incluso aggiornato.

# SOUNDNA Listen 1.1 — aggiornamento

## Cosa cambia

- Finestra orizzontale 660 x 172 pixel (prima 480 x 548).
- Il timer grande mostra l'ascolto del BLOCCO ATTUALE, non il totale di tutti i blocchi.
- “Si torna al lavoro” lo azzera; il totale progetto resta conservato e riprende
  dal valore precedente. La pausa continua a sospendere il totale progetto.
- Esportazione PDF direttamente dal plugin, offline, senza librerie da installare.
- Campo per il nome del progetto; cronologia salvata nello stato della DAW.
- Migrazione dei vecchi progetti: totali e intervallo v1 vengono conservati.
  Le pause precedenti all'aggiornamento non possono essere ricostruite.

## Aggiornare il repository GitHub esistente

1. Estrai questo ZIP e apri la cartella SOUNDNA_Listen.
2. Nel repository SOUNDNA-SAFE-HEARING, scheda Code, scegli Add file > Upload files.
3. Trascina Source, tests, CMakeLists.txt e i documenti aggiornati nella radice
   del repository, nella stessa posizione dei precedenti. Conferma il commit.
   Non caricare il file ZIP né la cartella esterna SOUNDNA_Listen come sottocartella.
4. Il workflow Mac gia presente resta valido. Non devi ricrearlo o modificare scripts.
5. Actions > Build SOUNDNA Listen > Run workflow sul ramo main.
   Avvia una NUOVA esecuzione sul commit aggiornato: “Re-run” di una vecchia
   esecuzione ricompila il vecchio commit.
6. Scarica l'artifact SOUNDNA-Listen-macOS-arm64 dalla nuova esecuzione riuscita.
7. Salva i progetti DAW, chiudi Logic/Ableton e sostituisci i due bundle plugin
   .component e .vst3 con quelli nuovi. Riavvia ed eventualmente riesegui la scansione.

Gli identificativi del plugin non sono cambiati, per conservare le istanze gia
presenti nei progetti. Se la DAW mostra ancora la vecchia interfaccia, verifica
la scansione e che non ci siano copie duplicate del plugin in altre cartelle.

## Usare il report

Scrivi il nome nel campo superiore e clicca “Esporta PDF”. Scegli dove salvare
il file .pdf. Non occorre aprire un browser o installare un generatore PDF.
Il report fotografa la cronologia fino al momento in cui confermi il salvataggio.

Include:
- tempo totale progetto (lavoro + editing, escluse pause manuali);
- riproduzione totale di tutti i blocchi, incluso eventuale Play durante le pause;
- tempo totale in pausa, pause avviate/concluse e durata media delle pause concluse;
- media e massimo di ascolto nei blocchi conclusi con “Inizio la pausa”;
- cronologia con inizio/fine in UTC, durata misurata e tempo in Play di ogni segmento.

Un blocco o una pausa possono avere piu segmenti dopo salvataggio e riapertura.
Una pausa riaperta conta comunque UNA sola volta. Il tempo a progetto chiuso non
viene aggiunto. L'ultimo segmento e marcato “in corso”, non come evento concluso.
Gli orari usano UTC per evitare ambiguita tra fusi e cambi dell'ora; le durate
non dipendono dal fuso. Il nome progetto supporta i caratteri italiani/occidentali;
nel PDF i caratteri fuori da Windows-1252 vengono sostituiti da ?. Nomi molto lunghi
sono abbreviati nel titolo del PDF, senza cambiare il nome salvato nel plugin.

Salva il progetto nella DAW per conservare la cronologia. Esportare un PDF non
salva automaticamente il progetto DAW. Se carichi lo stato precedente v1, il
report segnala che la cronologia storica non e disponibile. Il totale progetto
preesistente viene mantenuto. Il nome viene inserito manualmente perche il plugin
non riceve un nome progetto affidabile e uniforme da tutte le DAW.

“Azzera...” cancella tutti i conteggi e la cronologia dopo conferma. Non e il
pulsante da usare per le pause: “Si torna al lavoro” azzera solo il blocco attuale.

Esempio_Report.pdf contiene dati inventati per mostrare il layout, non dati del
progetto dell'utente.
