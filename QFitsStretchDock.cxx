#include "QFitsStretchDock.h"

#include "QFitsWindow.h"
#include "settings.h"
#include "fitsviewer.h"
#include <iomanip>
#include <sstream>
#include <algorithm>


QFitsStretchDock::QFitsStretchDock(QWidget* parent)
{
	setParent(parent);
	setFeatures(QDockWidget::NoDockWidgetFeatures);
	
	// Generates new "fake" titlebar with 0 px thickness
	QWidget* fakeTitleBar = new QWidget();
	
	// ATTENTION: Are both new and old original title bars deleted on exit?
	setTitleBarWidget(fakeTitleBar);
	
	_topSlider = new QStretchSlider(Qt::Vertical, this);
	_bottomSlider = new QStretchSlider(Qt::Vertical, this);

	// Setting top slider features
	_topSlider->setMinimum(0);
	_topSlider->setMaximum(1000);
	_topSlider->setValue(500);
	_topSlider->setMinimumHeight(512*2/3);
	_topSlider->setMaximumHeight(512*2/3);
	
	// Setting bottom slider features
	_bottomSlider->setMinimum(0);
	_bottomSlider->setMaximum(1000);
	_bottomSlider->setValue(500);
	_bottomSlider->setMinimumHeight(512*2/3);
	_bottomSlider->setMaximumHeight(512*2/3);
	
	// Draws default sliders pixmap
	createSliderPixmap(_stretchPixmap);
	
	// Sliders pixmap is displayed in a QLabel
	_stretchImageLabel = new QLabel;
	_stretchImageLabel->setPixmap(_stretchPixmap);
	_stretchImageLabel->QLabel::setScaledContents(true);
	_stretchImageLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
	
	// Creating a separator
	_separator = new QFrame;
	_separator->setFrameShape(QFrame::HLine);

	// Creating Stretch labels
	_maxValueLabel = new QLabel("Max");
	_minValueLabel = new QLabel("Min");;
	_maxValueBox = new QLineEdit("-");
	_minValueBox = new QLineEdit("-");
	
	// Setting text labels features
	_maxValueBox->setReadOnly(true);
	_maxValueBox->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	_minValueBox->setReadOnly(true);
	_minValueBox->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	
	// Creating container widgets and its layout
	_container = new QWidget(this);
	_containerLayout = new QGridLayout(_container);
	
	// Set left and right container's margins (appearance purposes)
	_containerLayout->setContentsMargins(0, 8, 0, 8);

	// All previously created widgets are added to the layout
	_containerLayout->addWidget(_topSlider, 0, 0, 1, 1, Qt::AlignTop/* | Qt::AlignHCenter*/);
	_containerLayout->addWidget(_stretchImageLabel, 0, 1);
	_containerLayout->addWidget(_bottomSlider, 0, 2, 1, 1, Qt::AlignBottom/* | Qt::AlignHCenter*/);
	_containerLayout->addWidget(_separator, 1, 0, 1, 3, Qt::AlignBottom);
	_containerLayout->addWidget(_maxValueLabel, 2, 0, 1, 3, Qt::AlignVCenter | Qt::AlignHCenter);
	_containerLayout->addWidget(_maxValueBox, 3, 0, 1, 3);
	_containerLayout->addWidget(_minValueLabel, 4, 0, 1, 3, Qt::AlignVCenter | Qt::AlignHCenter);
	_containerLayout->addWidget(_minValueBox, 5, 0, 1, 3);
	
	// Layout is set as container's layout
	_container->setLayout(_containerLayout);
	_container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	
	// Container is set as dock widget
	setWidget(_container);
	
	// DockSpace width is set to 500 pixels
	setFixedWidth(85);
}


void QFitsStretchDock::update()
{
	// "Asks" to the main program object current active window
	QFitsWindow* fitsWindow = dynamic_cast<FitsViewer*>(parent())->getFocusedWindow();
	
 	// If an image is currently selected updates min and max shown values
	if (fitsWindow != NULL)
	{
		pixelT maxVal = dynamic_cast<FitsViewer*>(parent())->getFocusedWindow()->getCurrentMaxStretch();
		pixelT minVal = dynamic_cast<FitsViewer*>(parent())->getFocusedWindow()->getCurrentMinStretch();
		
		std::stringstream maxStr;
		std::stringstream minStr;
		
		// If max and min are in a range suitable for fixed representation
		if (fabs(maxVal) < pow(10., 7) && fabs(maxVal) > pow(10., -3)
			&& fabs(minVal) < pow(10., 7) && fabs(minVal) > pow(10., -3))
		{
			// Calculates maximum number of decimals
			int prec = static_cast<int>(std::min(std::min(3., 1000./fabs(minVal)), 1000./fabs(maxVal)));
			
			// Formats values strings
			maxStr << std::setprecision(prec) << std::fixed << maxVal;
			minStr << std::setprecision(prec) << std::fixed << minVal;
		}
		else
		// otherwise
		{
			//Numbers are shown using scientific notation
			maxStr << std::setprecision(1) << std::scientific << maxVal;
			minStr << std::setprecision(1) << std::scientific << minVal;
		}
		
		_maxValueBox->setText(QString::fromStdString(maxStr.str()));
		_minValueBox->setText(QString::fromStdString(minStr.str()));
			
		}
	// Or just sets text boxes to "-"
	else
	{
		_maxValueBox->setText("-");
		_minValueBox->setText("-");
	}
}


void QFitsStretchDock::previewImage()
{
	// Heights at which shade of slider image begins and end
	// so that it follows slider move.
	int shadeStart = 341 - (341 * _topSlider->value()/1000);
	int shadeEnd = 512 - (341 * _bottomSlider->value()/1000);

	// Updating slider image
	createSliderPixmap(_stretchPixmap, shadeStart, shadeEnd);
	_stretchImageLabel->setPixmap(_stretchPixmap);
	_stretchImageLabel->update();

	// Gets current selected image pointer
	QFitsWindow* currentImage = dynamic_cast<FitsViewer*>(parent())->getFocusedWindow();
	
	// If an image is currently focused
	if (currentImage != NULL)
	{
		// Gets from selected image last stretch values
		pixelT currentMax = currentImage->getCurrentMaxStretch();
		pixelT currentMin = currentImage->getCurrentMinStretch();
		pixelT stretch = currentMax - currentMin;
		
		// Calculates stretch variation from current sliders position.
		// At one time, only a slider can be moved, the other produces
		// no increment or decrement. Either maxVariation or minVariation
		// is exclusively 0.
		// TODO: Increment is linear. Quadratic would suit better
		pixelT maxVariation = (_topSlider->value() - 500) * stretch / 1000;
		pixelT minVariation = (_bottomSlider->value() - 500) * stretch / 1000;
		
		// Updates displayed image calling its preview method
		currentImage->previewStretch(currentMin + minVariation, currentMax + maxVariation);

		// Min and Max stretch values are updated
		pixelT maxVal = currentMax + maxVariation;
		pixelT minVal = currentMin + minVariation;
		
		std::stringstream maxStr;
		std::stringstream minStr;
		
		// If max and min are in a range suitable for fixed representation
		if (fabs(maxVal) < pow(10., 7) && fabs(maxVal) > pow(10., -3)
			&& fabs(minVal) < pow(10., 7) && fabs(minVal) > pow(10., -3))
		{
			// Calculates maximum number of decimals
			int prec = static_cast<int>(std::min(std::min(3., 1000./fabs(minVal)), 1000./fabs(maxVal)));
			
			// Formats values strings
			maxStr << std::setprecision(prec) << std::fixed << maxVal;
			minStr << std::setprecision(prec) << std::fixed << minVal;
		}
		else
			// otherwise
		{
			//Numbers are shown using scientific notation
			maxStr << std::setprecision(1) << std::scientific << maxVal;
			minStr << std::setprecision(1) << std::scientific << minVal;
		}
		
		_maxValueBox->setText(QString::fromStdString(maxStr.str()));
		_minValueBox->setText(QString::fromStdString(minStr.str()));
	}
}


void QFitsStretchDock::updateImage()
{
	// Default slider image drawn and displayed
	createSliderPixmap(_stretchPixmap);
	_stretchImageLabel->setPixmap(_stretchPixmap);
	_stretchImageLabel->update();
	
	// Gets current selected image pointer
	QFitsWindow* currentImage = dynamic_cast<FitsViewer*>(parent())->getFocusedWindow();
	
	// If an image is currently focused
	// Same as 
	if (currentImage != NULL)
	{
		// Same considerations as of QFitsStretchDock::previewImage()
		pixelT currentMax = currentImage->getCurrentMaxStretch();
		pixelT currentMin = currentImage->getCurrentMinStretch();
		pixelT stretch = currentMax - currentMin;
		
		pixelT maxVariation = (_topSlider->value() - 500) * stretch / 1000;
		pixelT minVariation = (_bottomSlider->value() - 500) * stretch / 1000;
		
		// Updates displayed image calling its update method
		currentImage->updateStretch(currentMin + minVariation, currentMax + maxVariation);
		
		// Min and Max stretch values are updated
		pixelT maxVal = dynamic_cast<FitsViewer*>(parent())->getFocusedWindow()->getCurrentMaxStretch();
		pixelT minVal = dynamic_cast<FitsViewer*>(parent())->getFocusedWindow()->getCurrentMinStretch();
		
		std::stringstream maxStr;
		std::stringstream minStr;
		
		// If max and min are in a range suitable for fixed representation
		if (fabs(maxVal) < pow(10., 7) && fabs(maxVal) > pow(10., -3)
			&& fabs(minVal) < pow(10., 7) && fabs(minVal) > pow(10., -3))
		{
			// Calculates maximum number of decimals
			int prec = static_cast<int>(std::min(std::min(3., 1000./fabs(minVal)), 1000./fabs(maxVal)));
			
			// Formats values strings
			maxStr << std::setprecision(prec) << std::fixed << maxVal;
			minStr << std::setprecision(prec) << std::fixed << minVal;
		}
		else
		// otherwise
		{
			//Numbers are shown using scientific notation
			maxStr << std::setprecision(1) << std::scientific << maxVal;
			minStr << std::setprecision(1) << std::scientific << minVal;
		}
		
		_maxValueBox->setText(QString::fromStdString(maxStr.str()));
		_minValueBox->setText(QString::fromStdString(minStr.str()));
		
	}
	
	// Sliders are brought back to their "rest" positions
	_bottomSlider->setValue(500);
	_topSlider->setValue(500);
}


// Draws sliders Pixmap
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
	
	// ATTENTION: COLORTABLE SHOULD BE DEFINED AS AN EXTERNAL CONSTANT OBJECT
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
	_dockPtr = dynamic_cast<QFitsStretchDock*>(this->parent());

	// Connects signals to slots
	connect(this, SIGNAL(sliderMoved(int)), this, SLOT(onSliderChange()));
	connect(this, SIGNAL(sliderReleased()), this, SLOT(onSliderSet()));
}


void QStretchSlider::onSliderChange()
{
	_dockPtr->previewImage();
}


void QStretchSlider::onSliderSet()
{
	_dockPtr->updateImage();	
}
