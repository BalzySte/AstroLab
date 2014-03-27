Buongiorno,

all'interno di questo archivio sono presenti tutti i sorgenti scritti
da me per l'intero progetto.
Ho utilizzato due librerie, in aggiunta a quella standard del C++(11):

	- CCFits (versione 2.4, NASA)
	- Qt 5


Ho provato, compatibilmente con il tempo a disposizione a commentare
tutto il codice così da rendere più facile la lettura ad altri e
ricordarmi io stesso cos'ho scritto. Commenti e variabili sono scritti
in inglese (spero corretto) per varie ragioni tra cui quella di poter
condividere facilmente il codice.

Noterà tra i commenti alcune annotazioni del tipo:
	TODO:
	ATTENTION:

Alcune di esse sono semplici annotazioni per futuri miglioramenti
altre sono informazioni utili ad un utilizzo corretto delle funzioni
implementate.

Ho cercato il più possibile di scrivere del codice che fosse ben
strutturato ed efficiente, sempre nel tempo che avevo a disposizione.

Allo stesso modo ho tentato di rendere il design il più semplice
e funzionale possibile. Naturalmente è richiesta all'utilizzatore una
certa "consapevolezza" delle operazioni che svolge.
In alcuni casi il programma previene operazioni errate come la divisione
di un immagine per 0.


Spero che il codice sia sufficientemente chiaro e comprensibile.


Brevemente una descrizione di quello che sono i vari files che
trova allegati:

	fitsviewer.pro		: File progetto QT, genera tramite
				  qmake il Makefile per compilare

	main.cxx		: Programma principale, sono 5 righe
				  di codice che generano ed eseguono
				  l'applicazione

	fitsviewer.h		: Classe derivata da QApplication, è
				  l'applicazione "vera e propria"

	QFits***.h		: Classi QT reimplementate per creare
				  gli elementi del programma (barre,
				  finestre di informazioni, liste di
				  immagini aperte, etc.)

	FitsPhoto.h		: Gestisce le immagini fits. Costruita
				  sulla CCFits (molto articolata e di
				  basso livello) permette di gestire
				  agilmente il caricamento e le operaz-
				  basilari tra immagini fits.

	AstroAnalizer.h		: Funzioni specifiche per l'analisi
				  delle immagini.


Spero di esser stato breve ed esaustivo nell'illustrare come si articola
il progetto.
In allegato nell'archivio trova inoltre un paio di screenshots.


Cordiali saluti

Stefano Balzan
