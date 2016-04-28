#ifndef __fitsviewer_h__
#define __fitsviewer_h__

#include <QMainWindow>
#include <QMdiSubWindow>
#include <QCoreApplication>

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
	// File menu
	void open();
	void exportImage();
	void quit();
	// Operations menu
	void addition();
	void subtraction();
	void scalarMultiplication();
	void scalarDivision();
	void multiplication();
	void division();
	// Filters menu
	void createMedianFilter();
	void createLowPassFilter();
	void cometAzAvg();
	void cometAzMed();
	void cometAzRenorm();
	void cometInvRho();
	void cometRvsf();
	void cometRvsfMosaic();
	// Analysis menu
	void findStars();
	void analyseStar();
	void focalPlaneEvaluation();
	void opticsAlignment();
// 	void openSettingsWindow();
// 	void drawValidArea();
	// Help menu
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
	
	/* View menu actions
	QAction *zoomInAct;
	QAction *zoomOutAct;
	QAction *normalSizeAct;
	QAction *fitToWindowAct;
	*/
	
	// Filters menu actions
	QAction *createMedianFilterAct;
	QAction *createLowPassFilterAct;
	                                 // Cometary filters
	QAction *cometAzAvgAct;          // Division by Azimuthal Average
	QAction *cometAzMedAct;          // Division by Azimuthal Median
	QAction *cometAzRenormAct;       // Azimuthal Renormalization
	QAction *cometInvRhoAct;         // Division by 1/ρ
	QAction *cometRvsfAct;           // Radially Variable Spatial Filtering
	QAction *cometRvsfMosaicAct;     // Radially Variable Spatial Filtering Mosaic


	// Analysis menu actions
	QAction *findStarsAct;
	QAction *starAnalysisAct;
	QAction *focalPlaneEvaluationAct;
	QAction *opticsAlignmentAct;
// 	QAction *drawValidAreaAct;

	// Settings Menu actions
// 	QAction *openSettingsWindowAct;
	
	// Help Menu actions
	QAction *aboutAct;

	// Sub-menus
	QMenu *cometaryFilters; // Analysis submenus

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
