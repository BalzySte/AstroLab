//A qt implementation of FitsPhoto
#ifndef __QFitsWindow_h__
#define __QFitsWindow_h__

#include <QtWidgets>
#include "FitsPhoto.h"
#include "astroAnalyzer.h"


//#include "common.h"

// 8 bit monocolor FitsPhoto

// closeEvent should be redefined in order
// to save changes

class QFitsLabel;
class QFitsScrollArea;
class QFitsListWidgetItem;

class QFitsWindow : public QMdiSubWindow
{
	Q_OBJECT
	
public:
	void open(QString filename);
	void createFromFitsPhoto(FitsPhoto& fitsPhoto, QString imageName = "New Image");
	
	QFitsWindow(std::list<QFitsWindow*>& winList, QWidget* parent = 0);
	
	// Accessing image window title
	const QString& getImageTitle() const;
	QString& getImageTitle();
	
	const QFitsLabel* getImageLabel() const;
	QFitsLabel* getImageLabel();
	
private:	
	// Reference to opened Fits Windows list container
	std::list<QFitsWindow*>& subWindowsList;
	
	// Image title
	QString _windowTitle;
	
	QFitsLabel *_imageLabel;
	QFitsScrollArea *_scrollArea;
	
	
	// ATTENTION: PROBABLY USELESS
	// QListWidgetItem object, used to generate opened images list i.e. when adding.
	QFitsListWidgetItem* _listItem;
				
private slots:	
	// Slots
  	void mouseMoveEvent(QMouseEvent* event);
	void closeEvent(QCloseEvent* closeEvent);
	void focusInEvent(QFocusEvent* focusEvent);
};


// Function for creating QImage from 8bit values vector
void generateQPixmap(QPixmap& pixmap);


class QFitsLabel : public QLabel	// NOT NECESSARY
{
	Q_OBJECT
	
public:
	QFitsLabel(QWidget* parent);
	void open(QString filename);
	void createFromFitsPhoto(FitsPhoto& fitsPhoto);
	void exportPixmapToFile(QString filename);
	
	
	// Previews and updates image stretch
	void previewStretch(pixelT min, pixelT max);
	void updateStretch(pixelT min, pixelT max);
	
	// Previews and updates image zoom
	void previewZoom(double zoomFactor);
	void updateZoom(double zoomFactor);
	//	void updateMousePosition(int x, int y);
	
	// Methods returning the current rendering values
	pixelT getCurrentMaxStretch();
	pixelT getCurrentMinStretch();
	double getCurrentZoom();
		
	// Methods for accessing private FitsPhoto member.
//  	FitsPhoto& getFitsPhoto();
	const FitsPhoto& getFitsPhoto() const;
	
	// Method for accessing pointed pixel
	QPoint getPointedPixel();
	QPoint getPointedPixel() const;
	
	
	// Circles stars on image direcly.
	void circleStars(const std::vector<star>& vector, int radius);
	void circleNumberStars(const std::vector<star>& vector, int radius);
	// Used to temporarily point out stars
	// on an image. Drawn circles will disappear
	// upon QLabel redrawing.
	
	// Display valid area of focused image
	void drawValidArea();
	
private:
	QImage _image;
	
	// Fits Image
	FitsPhoto _fitsPhoto;
	
	// 8 bit gray shades Fits Image representation
	std::vector<uchar> _8bitImageArray;
	
	// Variables for keeping the stretch set through the sliders of the stretch box
	pixelT _currentMinStretch, _currentMaxStretch;
	
	// Zoom value
	double _zoomFactor;
	
	// Pointed pixel (mouse position on image)
	// NOTE: (-1,-1) means mouse is outside image, top left pixel is (0, 0)
	// 		 this notation is used internally to comply with indexes used by
	//		 valarrays and other objects. User may see top left pixel as (1, 1). 	
	QPoint _pointedPixel;
	
protected slots:
// 	void focusInEvent(QFocusEvent * focusEvent);
	void mouseMoveEvent(QMouseEvent *event);
	void leaveEvent(QEvent *event);
};


class QFitsScrollArea : public QScrollArea
{
	Q_OBJECT
	
public:
	QFitsScrollArea(QFitsWindow* parent);
	
private:
	QFitsWindow* _fitsWindowParent;
	
private slots:
  	void mouseMoveEvent(QMouseEvent *event);
//  	void focusInEvent(QFocusEvent* focusEvent);	
};

#endif // __QFitsWindow_h__
