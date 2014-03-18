MODIFICHE / PROBLEMI / OTTIMIZZAZIONI NECESSARIE

- Efficienza filtro mediana (std::vector sembra essere lento)

- ColorMap definita una sola volta per l'intero programma e non 
  generata ogni volta che si rende necessaria

- Rivedere algoritmo trova-stelle: Non deve trovare tutti i massimi
  e poi tagliare le stelle al di sotto del parametro inserito, ma
  direttamente scartarle senza inserirle nella lista. Lo stesso algoritmo
  è presente anche nella funzione che analizza le singole stele,
  modificare anche quella parte di codice.

- L'algoritmo che calcola centroide e FWHM dovrebbe scartare le stelle
  che si sovrappongono (attualmente è necessario controllare visivamente
  che non succeda). 

- Migliorare la finestra di export PNG

- Commentare tutto ciò che riguarda le funzioni di export.

- La finestra di info aperta dalla funzione "Star Analysis" dovrebbe
  dipendere dal programma principale e chiudersi automaticamente alla
  sua chiusura

- Potrebbe essere utile usare una lista per contenere gli oggetti star
