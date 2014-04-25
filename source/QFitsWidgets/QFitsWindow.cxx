#include "QFitsWindow.h"

#include "FitsToImage.h"
#include "fitsviewer.h"


QFitsWindow::QFitsWindow(std::list<QFitsWindow*>& winList, QWidget* parent) :
				QMdiSubWindow(parent), subWindowsList(winList)
{
	setMouseTracking(true);		// Always track mouse position inside widget
	
	// Instance must be deleted on close action
	setAttribute(Qt::WA_DeleteOnClose);
	
	// Created object is added to image windows list
	subWindowsList.push_back(this);
	
	// Adds a scroll area containing the label
	_scrollArea = new QFitsScrollArea(this);
	_scrollArea->setBackgroundRole(QPalette::Dark);
	
	// Creates image label for showing image
	_imageLabel = new QFitsLabel(_scrollArea);
	_imageLabel->setBackgroundRole(QPalette::Base);
	_imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
 	_imageLabel->setScaledContents(true);	// Adapts to image size
	
	// Adds image label to scroll area
 	_scrollArea->setWidget(_imageLabel);
	
	// Adds scroll area to the window
	setWidget(_scrollArea);
}


void QFitsWindow::open(QString filename)
{
	// TODO: Should rename images with same name.
	_windowTitle = filename.section('/' , -1, -1);
	setWindowTitle(_windowTitle);
	
	_imageLabel->open(filename);
	
	resize(300, 250);
	show();
}


void QFitsWindow::createFromFitsPhoto(FitsPhoto& fitsPhoto, QString imageName)
{
	// TODO: Should rename images with same name.
	_windowTitle = imageName;
	setWindowTitle(_windowTitle);
	
	_imageLabel->createFromFitsPhoto(fitsPhoto);
	
	resize(300, 250);
	show();
}


const QFitsLabel* QFitsWindow::getImageLabel() const
{
	return _imageLabel;	
}


QFitsLabel* QFitsWindow::getImageLabel()
{
	return _imageLabel;
}


void QFitsLabel::open(QString filename)
{		
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
	setPixmap(imagePixmap);
	adjustSize();
		
}


// TODO: DELETE THIS 2 LINES
// void QFitsWindow::exportPixmapToFile(QString filename);


void QFitsLabel::exportPixmapToFile(QString filename)
{
	pixmap()->toImage().convertToFormat(QImage::Format_ARGB32).save(filename);
}


void QFitsLabel::previewStretch(pixelT min, pixelT max)
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
	
	setPixmap(imagePixmap);
	adjustSize();
	
	show();
}


void QFitsLabel::updateStretch(pixelT min, pixelT max)
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
	
	setPixmap(imagePixmap);
	adjustSize();
	
	show();
}


void QFitsLabel::previewZoom(double zoomFactor)
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
	setPixmap(imagePixmap);
	adjustSize();
	
	show();
}


void QFitsLabel::updateZoom(double zoomFactor)
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
	setPixmap(imagePixmap);
	adjustSize();
	
	show();
}


pixelT QFitsLabel::getCurrentMaxStretch()
{
	return _currentMaxStretch;
}


pixelT QFitsLabel::getCurrentMinStretch()
{
	return _currentMinStretch;
}


double QFitsLabel::getCurrentZoom()
{
	return _zoomFactor;
}


// ATTENTION: Function "steals" FitsPhoto parameter through std::move
void QFitsLabel::createFromFitsPhoto(FitsPhoto& fitsPhoto)
{
	// See QFitsWindow::open() for detailed description	
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
		
	setPixmap(imagePixmap);
	adjustSize();
}


const FitsPhoto& QFitsLabel::getFitsPhoto() const
{
	return _fitsPhoto;
}


const QString& QFitsWindow::getImageTitle() const
{
	return _windowTitle;
}


QString& QFitsWindow::getImageTitle()
{
	return _windowTitle;
}


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
	focusEvent->accept();
}


void QFitsLabel::circleStars(const std::vector<star>& vector, int radius)
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
	for (std::vector<star>::const_iterator it = vector.cbegin(); it != vector.cend(); ++it)
	{
		QRectF rectangle((it->x-radius)*_zoomFactor, (it->y-radius)*_zoomFactor, (2*radius)*_zoomFactor, (2*radius)*_zoomFactor);	
		painter.drawArc(rectangle, startAngle, spanAngle);
	}
	
	// Finished to draw
	painter.end();
	
	setPixmap(imagePixmap);
	adjustSize();
	
	show();
}


void QFitsLabel::circleNumberStars(const std::vector<star>& vector, int radius)
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
	for (std::vector<star>::const_iterator it = vector.cbegin(); it != vector.cend(); ++it)
	{
		QRectF rectangle((it->x-radius)*_zoomFactor, (it->y-radius)*_zoomFactor, (2*radius)*_zoomFactor, (2*radius)*_zoomFactor);	
		painter.drawArc(rectangle, startAngle, spanAngle);
		painter.drawText(QPoint((it->x+radius)*_zoomFactor, (it->y+radius)*_zoomFactor), std::to_string(++count).c_str());
	}
	
	// Finished to draw
	painter.end();
	
	setPixmap(imagePixmap);
	adjustSize();
	
	show();	
}


/*
void QFitsLabel::drawValidArea()
{	// Conceptually similar to circleStars method.
	// Draws a coloured rectangle on the image to
	// highlights the valid area of the image
	
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
	painter.setPen(Qt::red);
	
	
	int x1 = _fitsPhoto.validArea().x1;
	int y1 = _fitsPhoto.validArea().y1;
	int x2 = _fitsPhoto.validArea().x2;
	int y2 = _fitsPhoto.validArea().y2;
	
	int topLeftX, topLeftY, width, height;
	
	if (x1 == 0)
		topLeftX = 0;
	else
		topLeftX = static_cast<int>(x1*_zoomFactor  + 0.5);
	
	if (y1 == 0)
		topLeftY = 0;
	else
		topLeftY = static_cast<int>(y1*_zoomFactor  + 0.5);
	
	if (x2 ==  _fitsPhoto.getWidth() - 1)
		width = imagePixmap.width() - 1;
	else
		width = static_cast<int>((x2 - x1)*_zoomFactor + 0.5);
	
	if (y2 ==  _fitsPhoto.getHeight() - 1)
		height = imagePixmap.height() - 1;
	else
		height = static_cast<int>((y2 - y1)*_zoomFactor + 0.5);
	
	// Draws rectangle
	QRectF rectangle(topLeftX, topLeftY, width, height);	
	painter.drawRect(rectangle);
	
	// Finished to draw
	painter.end();
	
	setPixmap(imagePixmap);
	adjustSize();
	
	show();
}
*/


void QFitsWindow::mouseMoveEvent(QMouseEvent* event)
{
	QMdiSubWindow::mouseMoveEvent(event);
	dynamic_cast<FitsViewer*>(parent()->parent()->parent())->updateMousePointerDock(); // Calls Mouse Pointer dock update method
	event->accept();
}


// --- QFitsLabel ---

QFitsLabel::QFitsLabel(QWidget* parent) :
			QLabel(parent), _zoomFactor(1.), _pointedPixel(QPoint(-1,-1))
{
	setFocusPolicy(Qt::NoFocus);
//	setAttribute(Qt::WA_NoMousePropagation);	//No propagation of mouse event
	setMouseTracking(true);		// Always track mouse position inside widget
}


QPoint QFitsLabel::getPointedPixel()
{
	return _pointedPixel;
}


QPoint QFitsLabel::getPointedPixel() const
{
	return _pointedPixel;
}


// ATTENTION: TODO: pos() is mouse position. It should
// be converted to image coordinates.
void QFitsLabel::mouseMoveEvent(QMouseEvent *event)
{
	_pointedPixel = (event->pos() / _zoomFactor);
	
  	event->ignore();
}


void QFitsLabel::leaveEvent(QEvent *event)
{
	_pointedPixel = QPoint(-1, -1);
	event->accept();
}



// ******* QFitsScrollArea ******* //

QFitsScrollArea::QFitsScrollArea(QFitsWindow* fitsWinPtr) : QScrollArea()
{
	setMouseTracking(true);		// Always track mouse position inside widget
	setFocusPolicy(Qt::NoFocus);
	_fitsWindowParent = fitsWinPtr;		// Useless, should use parent.
}


void QFitsScrollArea::mouseMoveEvent(QMouseEvent* event)
{
	QScrollArea::mouseMoveEvent(event);
	event->ignore();
}


/*
void QFitsScrollArea::focusInEvent(QFocusEvent* focusEvent)
	// Although not used, focusInEvent parameter is necessary for
	// signal - slot connection*/
	/*
	QObject* tmpMdiArea = (parent()->parent())->parent();
	FitsViewer* tmpMainWin = dynamic_cast<FitsViewer*> (tmpMdiArea->parent());
	tmpMainWin->setFocusedWindow(_fitsWindowParent);	
	*/
// 	dynamic_cast<QWidget*>(parent())->focusInEvent(focusEvent);
// }
