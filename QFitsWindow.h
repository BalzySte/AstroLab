//A qt implementation of FitsPhoto
#ifndef __QFitsWindow_h__
#define __QFitsWindow_h__

#include <QtGui>

#include "FitsPhoto.h"
#include "QFitsStretchDock.h"
#include "fitsviewer.h"
#include "astroAnalizer.h"

//#include "common.h"

// 8 bit monocolor FitsPhoto

// closeEvent should be redefined in order
// to save changes

class QFitsLabel;
class QFitsScrollArea;

class QFitsWindow : public QMdiSubWindow
{
	Q_OBJECT
	
public:
	QFitsWindow(std::list<QFitsWindow*>& winList, QWidget* parent = 0);
	void open(QString filename);
	void createFromFitsPhoto(FitsPhoto& fitsPhoto, QString imageName = "New Image");
	
	// Previews and updates image stretch 
	void previewStretch(pixelT min, pixelT max);
	void updateStretch(pixelT min, pixelT max);
	
	// Previews and updates image zoom
	void previewZoom(double zoomFactor);
	void updateZoom(double zoomFactor);
	
	// Methods for accessing private FitsPhoto member.
	FitsPhoto& getFitsPhoto();
	const FitsPhoto& getFitsPhoto() const;
	
	// Accessing image window title
	const QString& getImageTitle() const;
	QString& getImageTitle();
	
	// Methods returning the current rendering values
	pixelT getCurrentMaxStretch();
	pixelT getCurrentMinStretch();
	double getCurrentZoom();
	
	// Directly drawing on image
	void circleStars(std::vector<star>& vector, int radius);
																	// Used to temporarily point out something
																	// on an image. Drawn circle will disappear
																	// upon QLabel redrawing.
	
private:	
	// Reference to opened Fits Windows list container
	std::list<QFitsWindow*>& subWindowsList;
	
	// Image title
	QString _imageTitle;
	
	// QListWidgetItem object, used to generate opened images list i.e. when adding.
	QListWidgetItem* _listItem;
	
	// Variables for keeping the stretch set through the stretch box's sliders
	pixelT _currentMinStretch, _currentMaxStretch;
	
	// Qt Widget drawn inside fits window
	QFitsLabel* _imageLabel;
	QFitsScrollArea* _scrollArea;
	
	QImage _image;
	
	// Fits Image
	FitsPhoto _fitsPhoto;
	
	// 8 bit gray shades Fits Image representation
	std::vector<uchar> _8bitImageArray;

	// Zoom value
	double _zoomFactor;
	
private slots:	
	// Slots	
	void closeEvent(QCloseEvent* closeEvent);
	void focusInEvent();	
//	void focusOutEvent(QFocusEvent * focusOutEvent);
};


// Function for creating QImage from 8bit values vector
void generateQPixmap(QPixmap& pixmap);


class QFitsLabel : public QLabel	// NOT NECESSARY
{
	Q_OBJECT
	
public:
	QFitsLabel(QFitsWindow* parent);
	
private:
	QFitsWindow* _fitsWindowParent;
	
private slots:
	//void focusInEvent(QFocusEvent * focusInEvent);
};


class QFitsScrollArea : public QScrollArea
{
	Q_OBJECT
	
public:
	QFitsScrollArea(QFitsWindow* parent);
	
private:
	QFitsWindow* _fitsWindowParent;
	
private slots:
	void focusInEvent();	
};

#endif // __QFitsWindow_h__