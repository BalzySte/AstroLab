#include "QFitsZoomDock.h"
#include <iostream>
#include <string>



QFitsZoomDock::QFitsZoomDock(QWidget* parent)
{
	setParent(parent);
	setFeatures(QDockWidget::NoDockWidgetFeatures);
	
	QWidget* fakeTitleBar = new QWidget();
	
	setTitleBarWidget (fakeTitleBar);
	
	_zoomSlider = new QZoomSlider(Qt::Horizontal, this);
	
	_zoomSlider->setMinimum(25);
	_zoomSlider->setMaximum(400);
	_zoomSlider->setValue(100);
		
	_container = new QWidget(this);
	_containerLayout = new QGridLayout(_container);
	
	_containerLayout->addWidget(_zoomSlider, 0, 0, 1, 1/*, Qt::AlignTop | Qt::AlignHCenter*/);	
	_container->setLayout(_containerLayout);
//	_container->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	
 	setWidget(_container);
	setWidget(_zoomSlider);
	
	setFixedWidth(500);
}


void QFitsZoomDock::previewImage()
{
	double newZoomValue = _zoomSlider->value()/100.;
	
	if (dynamic_cast<FitsViewer*>(parent())->getFocusedWindow() != NULL)
		dynamic_cast<FitsViewer*>(parent())->getFocusedWindow()->previewZoom(newZoomValue);
}


void QFitsZoomDock::updateImage()
{	
	double newZoomValue = _zoomSlider->value()/100.;
	
	if (dynamic_cast<FitsViewer*>(parent())->getFocusedWindow() != NULL)
		dynamic_cast<FitsViewer*>(parent())->getFocusedWindow()->updateZoom(newZoomValue);
}


// *********   QZoomSlider   ********* //
QZoomSlider::QZoomSlider(Qt::Orientation orientation, QWidget* parent) :
											QSlider(orientation, parent)
{
	_dockPtr = dynamic_cast<QFitsZoomDock*>(this->parent());
	
	// Connect signals to slots
	connect(this, SIGNAL(sliderMoved(int)), this, SLOT(onSliderChange()));
	connect(this, SIGNAL(sliderReleased()), this, SLOT(onSliderSet()));
}


void QZoomSlider::onSliderChange()
{
	if (_dockPtr != NULL)
		_dockPtr->previewImage();
}


void QZoomSlider::onSliderSet()
{
	if (_dockPtr != NULL)
		_dockPtr->updateImage();	
}