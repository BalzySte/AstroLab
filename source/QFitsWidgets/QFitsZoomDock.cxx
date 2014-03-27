#include "QFitsZoomDock.h"

#include "QFitsWindow.h"
#include "fitsviewer.h"
#include "settings.h"


QFitsZoomDock::QFitsZoomDock(QWidget* parent)
{
	setParent(parent);
	setFeatures(QDockWidget::NoDockWidgetFeatures);

	// Generates new "fake" titlebar with 0 px thickness
	QWidget* fakeTitleBar = new QWidget();
	// ATTENTION: Are both new and old original title bars deleted on exit?
	setTitleBarWidget(fakeTitleBar);
	
	// Creates zoom titlebar
	_zoomSlider = new QZoomSlider(Qt::Horizontal, this);
	
	// Setting zoom slider features
	_zoomSlider->setMinimum(0);
	_zoomSlider->setMaximum(400);
	_zoomSlider->setValue(200);
	
	// Creates min and max zoom values labels
	_minZoomLabel = new QLabel("20 %");
	_maxZoomLabel = new QLabel("180 %");
	
	// Creates Container and its layout
	_container = new QWidget(this);
	_containerLayout = new QGridLayout(_container);
	
	// Set left and right container's margins (appearance purposes)
	_containerLayout->setContentsMargins(8, 0, 8, 0);
	
	// All previously created widgets are added to the layout
	_containerLayout->addWidget(_minZoomLabel, 0, 0, 1, 1);
	_containerLayout->addWidget(_zoomSlider, 0, 1, 1, 1);
	_containerLayout->addWidget(_maxZoomLabel, 0, 2, 1, 1);

	// Layout is set as container's layout	
	_container->setLayout(_containerLayout);
	
	// Container is set in a way that it can expand and occupy entire dock space
	_container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	
	// Container is set as dock widget
 	setWidget(_container);
	
	// DockSpace width is set to 500 pixels
	setFixedWidth(500);
}


void QFitsZoomDock::update()
{
	// "Asks" to the main program object current active window
	QFitsWindow* fitsWindow = dynamic_cast<FitsViewer*>(parent())->getFocusedWindow();
	
	// If an image is currently selected brings zoom bar to image's zoom value
	if (fitsWindow != NULL)
	{
		_zoomSlider->setValue(static_cast<int>(250 * (fitsWindow->getCurrentZoom() - 1) + 200));
	}
	// Or just sets it to 100 %
	else
	{
		_zoomSlider->setValue(200);
	}
}


void QFitsZoomDock::previewImage()
{
	// Calculates new zoom value
	double newZoomValue = 1 + ((_zoomSlider->value() - 200)/250.);
	
	// If an image is focused previews zoomed image
	if (dynamic_cast<FitsViewer*>(parent())->getFocusedWindow() != NULL)
		dynamic_cast<FitsViewer*>(parent())->getFocusedWindow()->previewZoom(newZoomValue);
}


void QFitsZoomDock::updateImage()
{	
	// Calculates new zoom value
	double newZoomValue = 1 + ((_zoomSlider->value() - 200)/250.);
	
	// If an image is focused updates zoomed image
	if (dynamic_cast<FitsViewer*>(parent())->getFocusedWindow() != NULL)
		dynamic_cast<FitsViewer*>(parent())->getFocusedWindow()->updateZoom(newZoomValue);
}


// *********   QZoomSlider   ********* //

QZoomSlider::QZoomSlider(Qt::Orientation orientation, QWidget* parent) :
											QSlider(orientation, parent)
{
	// TODO: Display shorter ticks
 	//setTickPosition(QSlider::TicksBelow);
 	//setTickInterval(200);
	_dockPtr = dynamic_cast<QFitsZoomDock*>(this->parent());
	
	// Connect signals to slots
	connect(this, SIGNAL(sliderMoved(int)), this, SLOT(onSliderChange()));
	connect(this, SIGNAL(sliderReleased()), this, SLOT(onSliderSet()));
}


void QZoomSlider::onSliderChange()
{
	_dockPtr->previewImage();
}


void QZoomSlider::onSliderSet()
{
	_dockPtr->updateImage();	
}