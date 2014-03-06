#ifndef __fitsviewer_h__
#define __fitsviewer_h__

#include <vector>
#include <QMainWindow>
#include <QMdiSubWindow>

#include "QFitsWindow.h"
#include "QFitsStretchDock.h"
#include "common.h"


class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;


class FitsViewer : public QMainWindow
{
  Q_OBJECT
  
public:
  FitsViewer();
  void setFocusedWindow(QFitsWindow* window);
  QFitsWindow* getFocusedWindow() const;
  void updateStretchDock();
  
private slots:
  void open();
  void save();
  void addition();
  void subtraction();
  void multiplication();
  void division();
  void quit();
  void about();
    
private:
  void createMenus();
  void createActions();
  void createStretchSlider();

  // Fits image windows list and pointer to active window
  std::list<QFitsWindow*> imageWindowsList;
  QFitsWindow* _currentFitsImage;
  
  QMdiArea *workspace;
  QBoxLayout *mainLayout;
  QFitsStretchDock *stretchDock;
  
  QAction *openAct;
  QAction *exitAct;

  // Operations
  QAction *addAct;
  QAction *subtractAct;
  QAction *multiplyAct;
  QAction *divideAct;
  
/*
  QAction *zoomInAct;
  QAction *zoomOutAct;
  QAction *normalSizeAct;
  QAction *fitToWindowAct;
*/

  QAction *aboutAct;

  QMenu *fileMenu;
  QMenu *viewMenu;
  QMenu *operationsMenu;
  QMenu *helpMenu;

  double scaleFactor;
};
  
#endif // __fitsviewer_h__