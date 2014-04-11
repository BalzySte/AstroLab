#include "QFitsWindow.h"

#include "FitsToImage.h"
#include "fitsviewer.h"


QFitsWindow::QFitsWindow(std::list<QFitsWindow*>& winList, QWidget* parent) :
				QMdiSubWindow(parent), subWindowsList(winList), _zoomFactor(1.),
				_xPointedPixel(-1), _yPointedPixel(-1)
{
	// Instance must be deleted on close action
	setAttribute(Qt::WA_DeleteOnClose);
	
	// Created object is added to image windows list
	subWindowsList.push_back(this);
	
	// Creates image label for showing image
	_imageLabel = new QFitsLabel(this);
	_imageLabel->setBackgroundRole(QPalette::Base);
	_imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
 	_imageLabel->setScaledContents(true);	// Adapts to image size
	
	// Adds a scroll area containing the label
	_scrollArea = new QFitsScrollArea(this);
	_scrollArea->setBackgroundRole(QPalette::Dark);
	_scrollArea->setWidget(_imageLabel);
	
	// Adds scroll area to the window
	setWidget(_scrollArea);
}


void QFitsWindow::open(QString filename)
{	
	// TODO: Should rename images with same name.
	_imageTitle = filename.section('/' , -1, -1);
	
	// Open fit file with fitsPhoto lib.
	_fitsPhoto.open(filename.toStdString());

	// Set min - max stretch and render 8bit grayscale image
	_currentMinStretch = _fitsPhoto.getImageMinValue();
	_currentMaxStretch = _fitsPhoto.getImageMaxValue();
	
	// Sets 8bit shades vector size according to opened image
	_8bitImageArray.resize(_fitsPhoto.getPixelNum());
	
	// Populates 8 bit shades vector with max - min stretch
	FitsTo8BitScale(_8bitImageArray, _fitsPhoto);
	
	// Generates default full scale image
	_image = QImage(_8bitImageArray.data(), _fitsPhoto.getWidth(), _fitsPhoto.getHeight(),
				 _fitsPhoto.getWidth(), QImage::Format_Indexed8);
	
	// Generates colortable ATTENTION: should be created once for entire program
	QVector<QRgb> colorTable(256);
	for(int i = 0; i < 256; ++i)
		colorTable[i] = qRgb(i,i,i);
	
	_image.setColorCount(256);	
	_image.setColorTable(colorTable);
	
	// Generates QPixmap from image
	QPixmap imagePixmap = QPixmap::fromImage(_image);
	
	// Apply QPixmap to label and adjust its size
	_imageLabel->setPixmap(imagePixmap);
	_imageLabel->adjustSize();
		
	setWindowTitle(_imageTitle);
	resize(300, 250);
	show();
}	void _exportPixmapToFile(QString filename);


void QFitsWindow::exportPixmapToFile(QString filename)
{
	_imageLabel->pixmap()->toImage().convertToFormat(QImage::Format_ARGB32).save(filename);
}


void QFitsWindow::previewStretch(pixelT min, pixelT max)
{
	// See QFitsWindow::open() for detailed description
	// Populates 8 bit shades vector with restretched image
	FitsTo8BitScale(_8bitImageArray, _fitsPhoto, min, max);
	
	int _width = _image.width();
	int _height = _image.height();
	
	// Generates scaled image according to zoom
	QImage scaledImg = _image.scaled(static_cast<int>(_width*_zoomFactor),
									 static_cast<int>(_height*_zoomFactor), Qt::KeepAspectRatio, Qt::FastTransformation);
	
	QVector<QRgb> colorTable(256);
	for(int i = 0; i < 256; ++i)
		colorTable[i] = qRgb(i,i,i);
	
	scaledImg.setColorCount(256);	
	scaledImg.setColorTable(colorTable);
	
	QPixmap imagePixmap = QPixmap::fromImage(scaledImg);
	
	_imageLabel->setPixmap(imagePixmap);
	_imageLabel->adjustSize();
	
	show();
}


void QFitsWindow::updateStretch(pixelT min, pixelT max)
{
	// See QFitsWindow::open() for detailed description
	
	_currentMinStretch = min;
	_currentMaxStretch = max;
	
	FitsTo8BitScale(_8bitImageArray, _fitsPhoto, min, max);
	
	_image = QImage(_8bitImageArray.data(), _fitsPhoto.getWidth(), _fitsPhoto.getHeight(),
				 _fitsPhoto.getWidth(), QImage::Format_Indexed8);

	int _width = _image.width();
	int _height = _image.height();
	
	QImage scaledImg = _image.scaled(static_cast<int>(_width*_zoomFactor),
		static_cast<int>(_height*_zoomFactor), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	
	QVector<QRgb> colorTable(256);
	for(int i = 0; i < 256; ++i)
		colorTable[i] = qRgb(i,i,i);
	
	scaledImg.setColorCount(256);	
	scaledImg.setColorTable(colorTable);
	
	QPixmap imagePixmap = QPixmap::fromImage(scaledImg);
	
	_imageLabel->setPixmap(imagePixmap);
	_imageLabel->adjustSize();
	
	show();
}


void QFitsWindow::previewZoom(double zoomFactor)
{
	// See QFitsWindow::open() for detailed description
	int _width = _image.width();
	int _height = _image.height();
	
	QImage scaledImg = _image.scaled(static_cast<int>(_width*zoomFactor),
								 static_cast<int>(_height*zoomFactor), Qt::KeepAspectRatio, Qt::FastTransformation);

	QVector<QRgb> colorTable(256);
	for(int i = 0; i < 256; ++i)
		colorTable[i] = qRgb(i,i,i);
	
	scaledImg.setColorCount(256);	
	scaledImg.setColorTable(colorTable);
	
	QPixmap imagePixmap = QPixmap::fromImage(scaledImg);
	_imageLabel->setPixmap(imagePixmap);
	_imageLabel->adjustSize();
	
	show();
}


void QFitsWindow::updateZoom(double zoomFactor)
{
	// See QFitsWindow::open() for detailed description
	_zoomFactor = zoomFactor;
	
	int _width = _image.width();
	int _height = _image.height();
	
	QImage scaledImg = _image.scaled(static_cast<int>(_width*_zoomFactor),
		static_cast<int>(_height*_zoomFactor), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	
	QVector<QRgb> colorTable(256);
	for(int i = 0; i < 256; ++i)
		colorTable[i] = qRgb(i,i,i);
	
	scaledImg.setColorCount(256);	
	scaledImg.setColorTable(colorTable);
	
	QPixmap imagePixmap = QPixmap::fromImage(scaledImg);	// Assignment from temporary should be
															// optimised by compiler
	_imageLabel->setPixmap(imagePixmap);
	_imageLabel->adjustSize();
	
	show();
}


pixelT QFitsWindow::getCurrentMaxStretch()
{
	return _currentMaxStretch;
}


pixelT QFitsWindow::getCurrentMinStretch()
{
	return _currentMinStretch;
}


double QFitsWindow::getCurrentZoom()
{
	return _zoomFactor;
}


// ATTENTION: Function "steals" FitsPhoto parameter through std::move
void QFitsWindow::createFromFitsPhoto(FitsPhoto& fitsPhoto, QString imageName)
{
	// See QFitsWindow::open() for detailed description
	// TODO: Should rename images with same name.
	_imageTitle = imageName;
	
	// create new fit image with fitsPhoto lib.
	_fitsPhoto = std::move(fitsPhoto);
	
	// Set min - max stretch and render 8bit grayscale image
	_currentMinStretch = _fitsPhoto.getImageMinValue();
	_currentMaxStretch = _fitsPhoto.getImageMaxValue();
	
	_8bitImageArray.resize(_fitsPhoto.getPixelNum());
	
	FitsTo8BitScale(_8bitImageArray, _fitsPhoto);
	
	_image = QImage(_8bitImageArray.data(), _fitsPhoto.getWidth(), _fitsPhoto.getHeight(),
 				 _fitsPhoto.getWidth(), QImage::Format_Indexed8);

	QVector<QRgb> colorTable(256);
	for(int i = 0; i < 256; ++i)
		colorTable[i] = qRgb(i,i,i);
	
	_image.setColorCount(256);	
	_image.setColorTable(colorTable);
	
	
	QPixmap imagePixmap = QPixmap::fromImage(_image);
		
	_imageLabel->setPixmap(imagePixmap);
	_imageLabel->adjustSize();
	
	setWindowTitle(_imageTitle);
	resize(300, 250);
	show();
}


const FitsPhoto& QFitsWindow::getFitsPhoto() const
{
	return _fitsPhoto;
}


const QString& QFitsWindow::getImageTitle() const
{
	return _imageTitle;
}


QString& QFitsWindow::getImageTitle()
{
	return _imageTitle;
}


/*
QImage& QFitsWindow::getQImage(pixelT bottom, pixelT top)
{
}
*/

void QFitsWindow::closeEvent(QCloseEvent* closeEvent)
{
	// TODO : here ask the user about closing the project without saving
	
	// Removes this QFitsWindow instance from windows list.
	subWindowsList.remove(this);
	if (subWindowsList.size() != 0)
		subWindowsList.back()->setFocus();
	
	// If image was the only one left
	else
	{
		QObject* tmpMdiArea = (parent())->parent();
		FitsViewer* tmpMainWin = dynamic_cast<FitsViewer*> (tmpMdiArea->parent());
		tmpMainWin->setFocusedWindow(NULL);
	}
	closeEvent->accept();
}


void QFitsWindow::focusInEvent(QFocusEvent* focusEvent)
{
	// Although not used, focusEvent parameter is necessary for
	// signal - slot connection
	QObject* tmpMdiArea = (parent())->parent();
	FitsViewer* tmpMainWin = dynamic_cast<FitsViewer*>(tmpMdiArea->parent());
	tmpMainWin->setFocusedWindow(this);
	_imageLabel->setFocus();
	focusEvent->accept();
}


void QFitsWindow::circleStars(std::vector<star>& vector, int radius)
{
// Draws blue circles around stars contained in vector.
// Circles are temporary drawn directly on the image pixmap.
// They disappear as the user changes either stretch or zoom.
	
	// See QFitsWindow::open() for detailed description on how image is drawn
	int _width = _image.width();
	int _height = _image.height();
	
	QImage scaledImg = _image.scaled(static_cast<int>(_width*_zoomFactor),
		static_cast<int>(_height*_zoomFactor), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	
	QVector<QRgb> colorTable(256);
	for(int i = 0; i < 256; ++i)
		colorTable[i] = qRgb(i,i,i);
	
	scaledImg.setColorCount(256);	
	scaledImg.setColorTable(colorTable);
	
	QPixmap imagePixmap = QPixmap::fromImage(scaledImg);	// Assignment from temporary should be
															// optimised by compiler
	// QPainter object is used to draw
	QPainter painter(&imagePixmap);
	// Setting drawing tool (blue pen)
	painter.setPen(Qt::blue);
	
	// An arc is drawn. 0 and 5760 values draw a full circle
	int startAngle = 0;
	int spanAngle = 5760;
	
	// For each element in vector draws the circle
	for (std::vector<star>::iterator it = vector.begin(); it != vector.end(); ++it)
	{
		QRectF rectangle((it->x-radius)*_zoomFactor, (it->y-radius)*_zoomFactor, (2*radius)*_zoomFactor, (2*radius)*_zoomFactor);	
		painter.drawArc(rectangle, startAngle, spanAngle);
	}
	
	// Finished to draw
	painter.end();
	
	_imageLabel->setPixmap(imagePixmap);
	_imageLabel->adjustSize();
	
	show();
}


void QFitsWindow::circleNumberStars(std::vector<star>& vector, int radius)
{
	// Works as previous functions but draws also a small number
	// near the top right corner of the circle to identify the star
	
	// See QFitsWindow::open() for detailed description on how image is drawn
	int _width = _image.width();
	int _height = _image.height();
	
	QImage scaledImg = _image.scaled(static_cast<int>(_width*_zoomFactor),
									 static_cast<int>(_height*_zoomFactor),
									 Qt::KeepAspectRatio,
									 Qt::SmoothTransformation);
	
	QVector<QRgb> colorTable(256);
	for(int i = 0; i < 256; ++i)
		colorTable[i] = qRgb(i,i,i);
	
	scaledImg.setColorCount(256);	
	scaledImg.setColorTable(colorTable);
	
	QPixmap imagePixmap = QPixmap::fromImage(scaledImg);
	// Assignment from temporary should be
	// optimised by compiler
	// QPainter object is used to draw
	QPainter painter(&imagePixmap);
	// Setting drawing tool (blue pen)
	painter.setPen(Qt::blue);
	painter.setFont(QFont("Arial"));
	
	// An arc is drawn. 0 and 5760 values draw a full circle
	int startAngle = 0;
	int spanAngle = 5760;
	
	// For each element in vector draws the circle
	int count = 0;
	for (std::vector<star>::iterator it = vector.begin(); it != vector.end(); ++it)
	{
		QRectF rectangle((it->x-radius)*_zoomFactor, (it->y-radius)*_zoomFactor, (2*radius)*_zoomFactor, (2*radius)*_zoomFactor);	
		painter.drawArc(rectangle, startAngle, spanAngle);
		painter.drawText(QPoint((it->x+radius)*_zoomFactor, (it->y+radius)*_zoomFactor), std::to_string(++count).c_str());
	}
	
	// Finished to draw
	painter.end();
	
	_imageLabel->setPixmap(imagePixmap);
	_imageLabel->adjustSize();
	
	show();	
}


// --- QFitsLabel ---

QFitsLabel::QFitsLabel(QFitsWindow* parent) : QLabel()
{
	setFocusPolicy(Qt::ClickFocus);
//	setAttribute(Qt::WA_NoMousePropagation);	//No propagation of mouse event
	setMouseTracking(true);		// Always track mouse position inside widget
	_fitsWindowParent = parent;
}


QPoint QFitsLabel::pointedPixel()
{
	return _pointedPixel;
}


/*
void QFitsLabel::focusInEvent(QFocusEvent *focusEvent)
{
}
*/

#include <iostream>
// ATTENTION: TODO: pos() is mouse position. It should
// be converted to image coordinates.
void QFitsLabel::mouseMoveEvent(QMouseEvent *event)
{
	int x = event->x();
	int y = event->y();
	
	if (hasFocus())
	{
		// Mouse inside of widget
		if (this->rect().contains(event->pos())) {
			_pointedPixel = event->pos();
			std::cout << x << ' ' << y << std::endl;
		}
		// Mouse out of Widget
		else
		{
			_pointedPixel = QPoint(-1, -1);
			std::cout << x << ' ' << y << std::endl;	
		}
	}
	event->accept();
}



// ******* QFitsScrollArea ******* //

QFitsScrollArea::QFitsScrollArea(QFitsWindow* fitsWinPtr) : QScrollArea()
{
	_fitsWindowParent = fitsWinPtr;		//Useless, should use parent.
}

/*
void QFitsScrollArea::focusInEvent(QFocusEvent* focusEvent)
{
	// Although not used, focusInEvent parameter is necessary for
	// signal - slot connection*/
	/*
	QObject* tmpMdiArea = (parent()->parent())->parent();
	FitsViewer* tmpMainWin = dynamic_cast<FitsViewer*> (tmpMdiArea->parent());
	tmpMainWin->setFocusedWindow(_fitsWindowParent);	
	*/
// 	dynamic_cast<QWidget*>(parent())->focusInEvent(focusEvent);
// }