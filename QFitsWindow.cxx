// not implemented

#include "QFitsWindow.h"
#include "FitsPhoto.h"
#include "FitsToImage.h"
#include "fitsviewer.h"

#include <iostream>

QFitsWindow::QFitsWindow(std::list<QFitsWindow*>& winList, QWidget* parent) :
					subWindowsList(winList), QMdiSubWindow(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	subWindowsList.push_back(this);
	
	_imageLabel = new QFitsLabel(this);
	_imageLabel->setBackgroundRole(QPalette::Base);
	_imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	//_imageLabel->setScaledContents(true);		// Would stretch the image and it's not desirable
	
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
	
	_image = generateQImage(_fitsPhoto, _8bitImageArray);
	
	_imageLabel->setPixmap(QPixmap::fromImage(*_image));
	_imageLabel->adjustSize();
		
	setWindowTitle(_imageTitle);
	resize(300, 250);
	show();
}


void QFitsWindow::previewStretch(pixelT min, pixelT max)
{
	FitsTo8BitScale(_8bitImageArray, _fitsPhoto, min, max);
	
	_image = generateQImage(_fitsPhoto, _8bitImageArray);
	
	_imageLabel->setPixmap(QPixmap::fromImage(*_image));
	_imageLabel->adjustSize();
	
	show();
}


void QFitsWindow::updateStretch(pixelT min, pixelT max)
{
	_currentMinStretch = min;
	_currentMaxStretch = max;
	
	FitsTo8BitScale(_8bitImageArray, _fitsPhoto, min, max);
	
	_image = generateQImage(_fitsPhoto, _8bitImageArray);
	
	_imageLabel->setPixmap(QPixmap::fromImage(*_image));
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


// ATTENTION: Function "steal" FitsPhoto parameter through std::move
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
	
	_image = generateQImage(_fitsPhoto, _8bitImageArray);
	
	_imageLabel->setPixmap(QPixmap::fromImage(*_image));
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
		//There's only a Stretch box child, no need to name it.
	//	tmpMainWin->findChild<QFitsStretchDock*>("")->setActiveFitsImage(NULL);
		tmpMainWin->setFocusedWindow(NULL);
	}
	closeEvent->accept();
}

void QFitsWindow::focusInEvent(QFocusEvent* focusInEvent)
{
	std::cout << "Focused" << std::endl;
	QObject* tmpMdiArea = (parent())->parent();
	FitsViewer* tmpMainWin = dynamic_cast<FitsViewer*> (tmpMdiArea->parent());
	/*
	tmpMainWin->findChild<QFitsStretchDock*>("")->setActiveFitsImage(this);	//There's only a Stretch box child, no need to name it.
	std::cout << "QFitsDock found and updated with current Sub Window" << std::endl;
	*/
	tmpMainWin->setFocusedWindow(this);
//	tmpMainWin->updateStretchDock();
//	QMdiSubWindow::focusInEvent(focusInEvent);
}

/*
void QFitsWindow::focusOutEvent ( QFocusEvent * focusOutEvent )
{
	std::cout << "Defocused" << std::endl;
	QObject* tmpMdiArea = (parent())->parent();
	FitsViewer* tmpMainWin = dynamic_cast<FitsViewer*> (tmpMdiArea->parent());
	
	tmpMainWin->findChild<QFitsStretchDock*>("")->setActiveFitsImage(NULL);	//There's only a Stretch box child, no need to name it.
	std::cout << "QFitsDock found and updated with current Sub Window" << std::endl;
}
*/

QImage* generateQImage(FitsPhoto& fitsPhoto, std::vector<uchar>& charVector)
{
	QImage* imagePtr = new QImage(charVector.data(), fitsPhoto.getWidth(),
		fitsPhoto.getHeight(), fitsPhoto.getWidth(), QImage::Format_Indexed8);

	QVector<QRgb> colorTable(256);
	for(int i = 0; i < 256; ++i)
		colorTable[i] = qRgb(i,i,i);
	
	imagePtr->setColorCount(256);	
	imagePtr->setColorTable(colorTable);
	
	return imagePtr;
}


// --- QFitsLabel --- // NOT NECESSARY
QFitsLabel::QFitsLabel(QFitsWindow* parent) : QLabel()
{
	_fitsWindowParent = parent;
	//connect(this, SIGNAL(focusInEvent(QFocusEvent)), _fitsWindowParent, SLOT(focusInEvent(QFocusEvent)));
}

/*
void QFitsLabel::focusInEvent(QFocusEvent * focusInEvent)
{
	//_fitsWindowParent->setFocus();
	std::cout << "Focusing from Label" << std::endl;
}
*/

// --- QFitsScrollArea ---
QFitsScrollArea::QFitsScrollArea(QFitsWindow* fitsWinPtr) : QScrollArea()
{
	_fitsWindowParent = fitsWinPtr;
	//connect(this, SIGNAL(focusInEvent(QFocusEvent)), _fitsWindowParent, SLOT(focusInEvent(QFocusEvent)));
}


void QFitsScrollArea::focusInEvent(QFocusEvent * focusInEvent)
{
	QObject* tmpMdiArea = (parent()->parent())->parent();
	FitsViewer* tmpMainWin = dynamic_cast<FitsViewer*> (tmpMdiArea->parent());
//	if (tmpMainWin!=0) //check if dynamic_cast worked properly.
//		tmpMainWin->findChild<QFitsStretchDock*>("")->setActiveFitsImage(_fitsWindowParent);	//There's only a Stretch box child, no need to name it.
	std::cout << "Focusing from Scroll Area" << std::endl;

	tmpMainWin->setFocusedWindow(_fitsWindowParent);	
}