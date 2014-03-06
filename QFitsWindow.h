//A qt implementation of FitsPhoto
#ifndef __QFitsWindow_h__
#define __QFitsWindow_h__

#include <QtGui>

#include "FitsPhoto.h"
#include "QFitsStretchDock.h"
#include "fitsviewer.h"
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
	void previewStretch(pixelT min, pixelT max);
	void updateStretch(pixelT min, pixelT max);
	FitsPhoto& getFitsPhoto();
	const FitsPhoto& getFitsPhoto() const;
	const QString& getImageTitle() const;
	QString& getImageTitle();
	
	
	pixelT getCurrentMaxStretch();
	pixelT getCurrentMinStretch();
	
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
	QImage* _image;
	
	// Fits Image
	FitsPhoto _fitsPhoto;
	
	// 8 bit gray shades Fits Image representation
	std::vector<uchar> _8bitImageArray;
	
private slots:	
	// Slots	
	void closeEvent(QCloseEvent* closeEvent);
	void focusInEvent(QFocusEvent * focusInEvent);	
//	void focusOutEvent(QFocusEvent * focusOutEvent);
	
};

// Function for creating QImage from 8bit values vector
QImage* generateQImage(FitsPhoto& fitsPhoto, std::vector<uchar>& charVector);


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
	void focusInEvent(QFocusEvent * focusInEvent);	
};

#endif // __QFitsWindow_h__