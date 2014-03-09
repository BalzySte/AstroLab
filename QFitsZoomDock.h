#ifndef __QFitsZoomDock_h__
#define __QFitsZoomDock_h__

#include <QtGui>
#include "QFitsWindow.h"
#include "fitsviewer.h"
#include "settings.h"
#include "common.h"


class QZoomSlider;	// Declaration

class QFitsZoomDock : public QDockWidget
{
	Q_OBJECT
	
public:
	QFitsZoomDock(QWidget* parent = 0);
	void previewImage();
	void updateImage();
	
private:
	QWidget* _container;
	QGridLayout* _containerLayout;
	QZoomSlider* _zoomSlider;
};


class QZoomSlider : public QSlider
{
	Q_OBJECT
	
public:
	QZoomSlider(Qt::Orientation orientation, QWidget* parent);
	
private:
	QFitsZoomDock* _dockPtr;		// Pointer to Stretch dock (Sliders' parent)
	
private slots:	
	void onSliderChange();	// virtual protected in inherited class.
							// Updates displayed image according to
							// new zoom value.
	
	void onSliderSet();
};


#endif	//__QFitsZoomDock_h__