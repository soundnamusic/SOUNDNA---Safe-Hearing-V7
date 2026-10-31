# Verifiche 1.5.1
- Modifiche solo visive: gruppo inferiore centrato (155–505 su larghezza 660),
  estremi Lingua/Azzera 90x28, selettore minuti 100x28, export rosso.
- La verifica visiva nel sistema operativo/DAW non è eseguibile in questo ambiente.
- Sintassi C++ di processor/editor verificata con JUCE 8.0.12 (commit fissato nel CMake).
- 28 controlli timer: conto alla rovescia basato sul Play, pausa, reset del blocco,
  soglie personalizzate, totali e cronologia.
- Test stato: nomi manuali conservati, rimozione dei nomi automatici 1.4 senza
  modificare i timer, intervalli personalizzati e riapertura della pausa.
- Test preferenze: default 30, valori personalizzati 1–180 persistenti,
  rifiuto valori fuori limite, dimensione e lingua.
- Test report: totali, traduzioni, codifica e paginazione.
- bash -n sullo script Mac. Ordine input-prima di -verify_arch conservato.
- Workflow invariato rispetto alla 1.3.0. Nessuna esecuzione GitHub da questo ambiente.
- Da verificare su Mac/Windows: compilazione nativa, caricamento AU/VST3,
  clic/doppio clic e focus tastiera, ridimensionamento e oscuramento pausa,
  nome manuale e riapertura progetto. Non è stata eseguita una prova GUI nativa.
