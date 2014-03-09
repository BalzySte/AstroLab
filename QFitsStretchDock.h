#ifndef __QFitsStretchDock_h__
#define __QFitsStretchDock_h__

#include <QtGui>
#include "QFitsWindow.h"
#include "settings.h"
#include "fitsviewer.h"

#include "common.h"

#define RESOLUTION 1000

class QStretchSlider;	//declaration

class QFitsStretchDock : public QDockWidget
{
	Q_OBJECT
	
public:
	QFitsStretchDock(QWidget* parent = 0);
	void update();	// Updates text boxes when image window is focused
	void previewImage();
	void updateImage();

private:
	QWidget* _container;
	QGridLayout* _containerLayout;
	QStretchSlider* _topSlider;
	QStretchSlider* _bottomSlider;
	
	// Main stretch tools (bars and gradient image)
	QPixmap _stretchPixmap;
	QLabel* _stretchImageLabel;
	
	// Stretch tools / values separator
	QFrame* _separator;
	
	// Stretch values Widgets (lower dock part)
	QLabel* _maxValueLabel;
	QLabel* _minValueLabel;
	QLineEdit* _maxValueBox;
	QLineEdit* _minValueBox;
};


void createSliderPixmap(QPixmap& pixmap, int shadeStartPos = 161, int shadeEndPos = 351);


// QStretchSlider is a small floating-point version of a QSlider.
// Only few necessary functions have been redefined to provide the
// functionality shown in the program.
class QStretchSlider : public QSlider
{
	Q_OBJECT
	
public:
	QStretchSlider(Qt::Orientation orientation, QWidget* parent);
	
private:
	QFitsStretchDock* _dockPtr;		// Pointer to Stretch dock (Sliders' parent)
	
private slots:	
	void onSliderChange(/*SliderChange change int newValue*/); 	// virtual protected in inherited class.
																// Updates displayed image according to
																// new stretching values.

	void onSliderSet(/*int newValue*/);
};


#endif	// __QFitsStretchDock_h__