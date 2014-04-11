#ifndef __fitsviewer_h__
#define __fitsviewer_h__

#include <QMainWindow>
#include <QMdiSubWindow>

#include "QFitsWindow.h"
#include "QFitsStretchDock.h"
#include "QFitsZoomDock.h"


class FitsViewer : public QMainWindow
{
  Q_OBJECT
  
public:
	FitsViewer();
	
	// Sets currently selected image window
	void setFocusedWindow(QFitsWindow* window);
	
	// Gets currently selected image window
	QFitsWindow* getFocusedWindow() const;
	
	// Updates stretch dock when focus changes
//	void updateStretchDock();
  
private slots:
	void open();
	void exportImage();
	void addition();
	void subtraction();
	void scalarMultiplication();
	void scalarDivision();
	void multiplication();
	void division();
	void createMedianFilter();
	void createLowPassFilter();
	void findStars();
	void analyseStar();
	void quit();
	void about();
    
private:
	void createMenus();
	void createActions();

	// Fits image windows list and pointer to active window
	std::list<QFitsWindow*> imageWindowsList;
	QFitsWindow* _currentFitsImage;
	
	QMdiArea *workspace;
	QBoxLayout *mainLayout;
	QFitsStretchDock *stretchDock;
	QFitsZoomDock *zoomDock;
	
	// File menu actionsimgM
	QAction *openAct;
	QAction *exportImageAct;
	QAction *exitAct;

	// Operations menu actions
	QAction *addAct;
	QAction *subtractAct;
	QAction *scalarMultiplyAct;
	QAction *scalarDivideAct;
	QAction *multiplyAct;
	QAction *divideAct;
	
	/* View menu actions
	QAction *zoomInAct;
	QAction *zoomOutAct;
	QAction *normalSizeAct;
	QAction *fitToWindowAct;
	*/
	
	QAction *findStarsAct;
	QAction *createMedianFilterAct;
	QAction *createLowPassFilterAct;
	QAction *starAnalysisAct;

	// Help Menu actions
	QAction *aboutAct;

	// Menus
	QMenu *fileMenu;
	QMenu *viewMenu;
	QMenu *operationsMenu;
	QMenu *filtersMenu;
	QMenu *analysisMenu;
	QMenu *helpMenu;
};
  
#endif // __fitsviewer_h__