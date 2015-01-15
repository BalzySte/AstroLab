#ifndef __QFitsStretchDock_h__
#define __QFitsStretchDock_h__

#include <QtWidgets>


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
	QWidget* _container;	// Main widget containing QGridLayout
	QGridLayout* _containerLayout;	// Layout containing all Dock widgets
	QStretchSlider* _topSlider;
	QStretchSlider* _bottomSlider;
	
	// Sliders gradient image
	QPixmap _stretchPixmap;
	QLabel* _stretchImageLabel;
	
	// Horizontal separator
	QFrame* _separator;
	
	// Stretch values Widgets (lower dock part)
	QLabel* _maxValueLabel;
	QLabel* _minValueLabel;
	QLineEdit* _maxValueBox;
	QLineEdit* _minValueBox;
};


// Draws slider gradient pixmap. Called upon sliders move.
void createSliderPixmap(QPixmap& pixmap, int shadeStartPos = 161, int shadeEndPos = 351);


class QStretchSlider : public QSlider
{
	Q_OBJECT
	
public:
	QStretchSlider(Qt::Orientation orientation, QWidget* parent);
	
private:
	QFitsStretchDock* _dockPtr;		// Pointer to Stretch dock (Sliders' parent)
									// Could be used instead "parent" member
	
private slots:
	// Called upon slider moving. Calls methods
	// to provide a quick preview of the image
	void onSliderChange();

	// Called upon slider released. Calls methods
	// to display final re-stretched image.
	void onSliderSet();
};


#endif	// __QFitsStretchDock_h__