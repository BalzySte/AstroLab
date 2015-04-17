#ifndef __fitsviewer_h__
#define __fitsviewer_h__

#include <QMainWindow>
#include <QMdiSubWindow>

#include "QFitsWindow.h"
#include "QFitsStretchDock.h"
#include "QFitsZoomDock.h"
#include "QMousePointerDock.h"
#include "settings.h"


class FitsViewer : public QMainWindow
{
  Q_OBJECT
  
public:
	FitsViewer();
	
	// Sets currently selected image window
	void setFocusedWindow(QFitsWindow* window);
	
	// Gets currently selected image window
	QFitsWindow* getFocusedWindow() const;
	
	// Updates mouse Pointer Dock
	void updateMousePointerDock();
		
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
	void polarWarp();
	void createMedianFilter();
	void createLowPassFilter();
	void findStars();
	void analyseStar();
	void focalPlaneEvaluation();
	void opticsAlignment();
// 	void openSettingsWindow();
// 	void drawValidArea();
	void quit();
	void about();
	
	// Mouse move event
// 	void mouseMoveEvent(QMouseEvent *event);
    
private:
	void createMenus();
	void createActions();

	// Fits image windows list and pointer to active window
	std::list<QFitsWindow*> imageWindowsList;
	QFitsWindow* _currentFitsImage;
	
	// Child widgets & docks
	QMdiArea *workspace;
	QBoxLayout *mainLayout;
	QFitsStretchDock *stretchDock;
	QFitsZoomDock *zoomDock;
	QMousePointerDock *mousePointerDock;
	
	// Settings object
// 	QSettings settings;
	
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
	QAction *polarWarpAct;

	
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
	QAction *focalPlaneEvaluationAct;
	QAction *opticsAlignmentAct;
// 	QAction *drawValidAreaAct;

	// Settings Menu actions
// 	QAction *openSettingsWindowAct;
	
	// Help Menu actions
	QAction *aboutAct;

	// Menus
	QMenu *fileMenu;
	QMenu *viewMenu;
	QMenu *operationsMenu;
	QMenu *filtersMenu;
	QMenu *analysisMenu;
// 	QMenu *settingsMenu;
	QMenu *helpMenu;
};



class QFitsMdiArea : public QMdiArea
{
public:
	QFitsMdiArea(QWidget* parent);
};
  
#endif // __fitsviewer_h__