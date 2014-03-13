#ifndef __QFitsZoomDock_h__
#define __QFitsZoomDock_h__

#include <QtGui>


class QZoomSlider;	// Declaration

class QFitsZoomDock : public QDockWidget
{
	Q_OBJECT
	
public:
	QFitsZoomDock(QWidget* parent = 0);
	void update();	// Updates zoom bar position
	void previewImage();
	void updateImage();
	
private:
	QWidget* _container;
	QGridLayout* _containerLayout;
	QZoomSlider* _zoomSlider;
	QLabel* _minZoomLabel;
	QLabel* _maxZoomLabel;
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