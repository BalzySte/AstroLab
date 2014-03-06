#include "QFitsStretchDock.h"
#include <iostream>
#include <string>
//#include <unistd.h>

QFitsStretchDock::QFitsStretchDock(QWidget* parent)
{
	setParent(parent);
	_topSlider = new QStretchSlider(Qt::Vertical, this);
	_bottomSlider = new QStretchSlider(Qt::Vertical, this);
		
	_topSlider->setMinimum(0);
	_topSlider->setMaximum(stretchResolution);
	_topSlider->setValue(stretchResolution/2);
	_topSlider->setMinimumHeight(512*2/3);
	_topSlider->setMaximumHeight(512*2/3);
	
	_bottomSlider->setMinimum(0);
	_bottomSlider->setMaximum(stretchResolution);
	_bottomSlider->setValue(stretchResolution/2);
	_bottomSlider->setMinimumHeight(512*2/3);
	_bottomSlider->setMaximumHeight(512*2/3);
	
	
	createSliderPixmap(_stretchPixmap);
	
	_stretchImageLabel = new QLabel;
	_stretchImageLabel->setPixmap(_stretchPixmap);
	_stretchImageLabel->QLabel::setScaledContents(true);
	_stretchImageLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
	
	_separator = new QFrame;
	_separator->setFrameShape(QFrame::HLine);

	_maxValueLabel = new QLabel("Max");
	_minValueLabel = new QLabel("Min");;
	_maxValueBox = new QLineEdit("-");
	_minValueBox = new QLineEdit("-");
	
	_maxValueBox->setReadOnly(true);
	_maxValueBox->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	_minValueBox->setReadOnly(true);
	_minValueBox->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	
	_container = new QWidget(this);
	_containerLayout = new QGridLayout(_container);

	_containerLayout->addWidget(_topSlider, 0, 0, 1, 1, Qt::AlignTop/* | Qt::AlignHCenter*/);
	_containerLayout->addWidget(_stretchImageLabel, 0, 1);
	_containerLayout->addWidget(_bottomSlider, 0, 2, 1, 1, Qt::AlignBottom/* | Qt::AlignHCenter*/);
	_containerLayout->addWidget(_separator, 1, 0, 1, 3, Qt::AlignBottom);
	_containerLayout->addWidget(_maxValueLabel, 2, 0, 1, 3, Qt::AlignVCenter | Qt::AlignHCenter);
	_containerLayout->addWidget(_maxValueBox, 3, 0, 1, 3);
	_containerLayout->addWidget(_minValueLabel, 4, 0, 1, 3, Qt::AlignVCenter | Qt::AlignHCenter);
	_containerLayout->addWidget(_minValueBox, 5, 0, 1, 3);
		
	_container->setLayout(_containerLayout);
	_container->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	
	setWidget(_container);
}


void QFitsStretchDock::update()
{
	QFitsWindow* fitsWindow = dynamic_cast<FitsViewer*>(parent())->getFocusedWindow();
	std::cout << "Active Fits windows set properly" << std::endl;
	
	if (fitsWindow != NULL)
	{
		_maxValueBox->setText(QString::fromStdString(std::to_string(dynamic_cast<FitsViewer*>(parent())->getFocusedWindow()->getCurrentMaxStretch())));
		_minValueBox->setText(QString::fromStdString(std::to_string(dynamic_cast<FitsViewer*>(parent())->getFocusedWindow()->getCurrentMinStretch())));
	}
	else
	{
		_maxValueBox->setText("-");
		_minValueBox->setText("-");
	}
}


void QFitsStretchDock::previewImage()
{
	int stretchStart = 341 - (341 * _topSlider->value()/1000);
	int stretchEnd = 512 - (341 * _bottomSlider->value()/1000);
//	int stretchStart = (512-10)-(512*_topSlider->value())/stretchResolution;
//	int stretchEnd = (512+10)-(512*_bottomSlider->value())/stretchResolution;

	createSliderPixmap(_stretchPixmap, stretchStart, stretchEnd);
	_stretchImageLabel->setPixmap(_stretchPixmap);
	_stretchImageLabel->update();

	if (dynamic_cast<FitsViewer*>(parent())->getFocusedWindow() != NULL)
	{
		//	TODO: update stretch image
		pixelT currentMax = (dynamic_cast<FitsViewer*>(parent())->getFocusedWindow())->getCurrentMaxStretch();
		pixelT currentMin = (dynamic_cast<FitsViewer*>(parent())->getFocusedWindow())->getCurrentMinStretch();
		std::cout << "passed" << std::endl;
		pixelT stretch = currentMax - currentMin;
		
		pixelT maxVariation = (_topSlider->value() - 500) * stretch / stretchResolution;
		pixelT minVariation = (_bottomSlider->value() - 500) * stretch / stretchResolution;
		
		dynamic_cast<FitsViewer*>(parent())->getFocusedWindow()->previewStretch(currentMin + minVariation, currentMax + maxVariation);

		_maxValueBox->setText(QString::fromStdString(std::to_string(currentMax + maxVariation)));
		_minValueBox->setText(QString::fromStdString(std::to_string(currentMin + minVariation)));
	}
}


void QFitsStretchDock::updateImage()
{
	createSliderPixmap(_stretchPixmap);
	_stretchImageLabel->setPixmap(_stretchPixmap);
	_stretchImageLabel->update();

	if (dynamic_cast<FitsViewer*>(parent())->getFocusedWindow() != NULL)
	{
		pixelT currentMax = dynamic_cast<FitsViewer*>(parent())->getFocusedWindow()->getCurrentMaxStretch();
		pixelT currentMin = dynamic_cast<FitsViewer*>(parent())->getFocusedWindow()->getCurrentMinStretch();
		pixelT stretch = currentMax - currentMin;
		
		pixelT maxVariation = (_topSlider->value() - 500) * stretch / stretchResolution;
		pixelT minVariation = (_bottomSlider->value() - 500) * stretch / stretchResolution;
				
		dynamic_cast<FitsViewer*>(parent())->getFocusedWindow()->updateStretch(currentMin + minVariation, currentMax + maxVariation);
		
		_maxValueBox->setText(QString::fromStdString(std::to_string(currentMax + maxVariation)));
		_minValueBox->setText(QString::fromStdString(std::to_string(currentMin + minVariation)));
	}
		
	_bottomSlider->setValue(500);
	_topSlider->setValue(500);
}

/*
void QFitsStretchDock::setSliders(pixelT absoluteMin, pixelT min,
									pixelT absoluteMax, pixelT max)
{
	_min = min;
	_max = max;
	pixelT absoluteStretch = (absoluteMax-absoluteMin);
	pixelT topValue = RESOLUTION*(max-absoluteMin)/absoluteStretch;
	pixelT bottomValue = RESOLUTION*(min-absoluteMin)/absoluteStretch;
	_topSlider->setValue(static_cast<int>(topValue));
	_bottomSlider->setValue(static_cast<int>(bottomValue));
}
*/


void createSliderPixmap(QPixmap& pixmap, int shadeStartPos, int shadeEndPos)
{	
	int pxWidth = stretchDockImagewidth;
	int shadeLength = shadeEndPos-shadeStartPos;
	
	std::vector<uchar> imgVector(pxWidth*512);
	
	for (int px = 0; px < pxWidth*shadeStartPos; ++px)
		imgVector[px] = 255;
	for (int px = 0; px < pxWidth*shadeLength; ++px)
		imgVector[shadeStartPos*pxWidth + px] = 255 - static_cast<uchar>(px/(pxWidth)*255./(shadeLength));		
	for (int px = shadeEndPos*pxWidth; px < pxWidth*512; ++px)
		imgVector[px] = 0;

	QImage image(imgVector.data(), pxWidth, 512, pxWidth, QImage::Format_Indexed8);
	
	QVector<QRgb> colorTable(256);
	for(int i = 0; i < 256; ++i)
		colorTable[i] = qRgb(i,i,i);
		
	image.setColorCount(256);	
	image.setColorTable(colorTable);
	
	pixmap = QPixmap::fromImage(image);
}


// *********   QStretchSlider   ********* //

QStretchSlider::QStretchSlider(Qt::Orientation orientation, QWidget* parent) :
											QSlider(orientation, parent)
{
//	QWidget* wid = dynamic_cast<QWidget*>(this->parent());
//	_dockPtr = dynamic_cast<QFitsStretchDock*>(wid->parent());
	_dockPtr = dynamic_cast<QFitsStretchDock*>(this->parent());

	// Connect signals to slots
	connect(this, SIGNAL(sliderMoved(int)), this, SLOT(onSliderChange(/*int*/)));
	connect(this, SIGNAL(sliderReleased()), this, SLOT(onSliderSet()));
}


void QStretchSlider::onSliderChange(/*int newValue*/)
{
	if (_dockPtr != NULL)
	//if (change == QAbstractSlider::SliderValueChange/* && dock->dynamic_cast<FitsViewer*>(parent())->getFocusedWindow() != NULL*/)
		_dockPtr->previewImage();
}


void QStretchSlider::onSliderSet(/*int newValue*/)
{
	if (_dockPtr != NULL)
		_dockPtr->updateImage();	
}
