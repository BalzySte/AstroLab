// not implemented

#include "QFitsWindow.h"
#include "FitsPhoto.h"
#include "FitsToImage.h"
#include "fitsviewer.h"

#include <iostream>

QFitsWindow::QFitsWindow(std::list<QFitsWindow*>& winList, QWidget* parent) :
					subWindowsList(winList), QMdiSubWindow(parent), _zoomFactor(1.)
{
	setAttribute(Qt::WA_DeleteOnClose);
	subWindowsList.push_back(this);
	
	_imageLabel = new QFitsLabel(this);
	_imageLabel->setBackgroundRole(QPalette::Base);
	_imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
 	_imageLabel->setScaledContents(true);		// Would stretch the image and it's not desirable
	
	_scrollArea = new QFitsScrollArea(this);
	_scrollArea->setBackgroundRole(QPalette::Dark);
	_scrollArea->setWidget(_imageLabel);
	
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


void QFitsWindow::previewStretch(pixelT min, pixelT max)
{
	FitsTo8BitScale(_8bitImageArray, _fitsPhoto, min, max);
	int _width = _image.width();
	int _height = _image.height();
	
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


// ATTENTION: Function "steals" FitsPhoto parameter through std::move
void QFitsWindow::createFromFitsPhoto(FitsPhoto& fitsPhoto, QString imageName)
{
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
	subWindowsList.remove(this); //remove this QFitsWindow instance from windows list.
	if (subWindowsList.size() != 0)
		subWindowsList.back()->setFocus();
	else
	{
		QObject* tmpMdiArea = (parent())->parent();
		FitsViewer* tmpMainWin = dynamic_cast<FitsViewer*> (tmpMdiArea->parent());
		tmpMainWin->setFocusedWindow(NULL);
	}
	closeEvent->accept();
}

void QFitsWindow::focusInEvent(QFocusEvent* focusInEvent)
{
	std::cout << "Focused" << std::endl;
	QObject* tmpMdiArea = (parent())->parent();
	FitsViewer* tmpMainWin = dynamic_cast<FitsViewer*> (tmpMdiArea->parent());
	tmpMainWin->setFocusedWindow(this);
}


void QFitsWindow::circleStars(std::vector<star>& vector, int radius)
{
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
	QPainter painter(&imagePixmap);
	painter.setPen(Qt::blue);
	
	int startAngle = 0;
	int spanAngle = 5760;
	
	for (std::vector<star>::iterator it = vector.begin(); it != vector.end(); ++it)
	{
		QRectF rectangle((it->x-radius)*_zoomFactor, (it->y-radius)*_zoomFactor, (2*radius)*_zoomFactor, (2*radius)*_zoomFactor);	
		painter.drawArc(rectangle, startAngle, spanAngle);
	}
	
	painter.end();
		
	_imageLabel->setPixmap(imagePixmap);
	_imageLabel->adjustSize();
	
	show();
}


// --- QFitsLabel ---

QFitsLabel::QFitsLabel(QFitsWindow* parent) : QLabel()
{
	_fitsWindowParent = parent;
	//connect(this, SIGNAL(focusInEvent(QFocusEvent)), _fitsWindowParent, SLOT(focusInEvent(QFocusEvent)));
}


// --- QFitsScrollArea ---
QFitsScrollArea::QFitsScrollArea(QFitsWindow* fitsWinPtr) : QScrollArea()
{
	_fitsWindowParent = fitsWinPtr;		//Useless, should use parent.
}


void QFitsScrollArea::focusInEvent(QFocusEvent * focusInEvent)
{
	QObject* tmpMdiArea = (parent()->parent())->parent();
	FitsViewer* tmpMainWin = dynamic_cast<FitsViewer*> (tmpMdiArea->parent());

	tmpMainWin->setFocusedWindow(_fitsWindowParent);	
}