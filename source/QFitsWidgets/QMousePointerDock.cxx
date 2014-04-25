#include "QMousePointerDock.h"

#include "QFitsWindow.h"
#include "fitsviewer.h"
#include "settings.h"

#include <iomanip>


QMousePointerDock::QMousePointerDock(QWidget* parent)
{
	setParent(parent);
	setFeatures(QDockWidget::NoDockWidgetFeatures);

	// Generates new "fake" titlebar with 0 px thickness
	QWidget* fakeTitleBar = new QWidget();
	// ATTENTION: Are both new and old original title bars deleted on exit?
	setTitleBarWidget(fakeTitleBar);

	// Generating text
	_textStream << " (  -  ,  -  )      Value:   -   ";

	// Creating text label
	_textLabel = new QLabel(this);
	_textLabel->setText(QString::fromStdString(_textStream.str()));

	setWidget(_textLabel);
}


void QMousePointerDock::update()
{
	_textStream.str("");
// 	_textStream.clear();

	QFitsWindow* currentWindow = dynamic_cast<QFitsWindow*>(
		dynamic_cast<FitsViewer*>(parent())->getFocusedWindow());

	if (currentWindow != NULL)
	{
		const QFitsLabel* imageLabel = currentWindow->getImageLabel();
		QPoint mousePos = imageLabel->getPointedPixel();
		
		//TODO: Value should be formatted according to it's dimension
		if (mousePos == QPoint(-1, -1))
			_textStream << " (  -  ,  -  )      Value:   -   ";
		
		else
		{
		pixelT value = imageLabel->getFitsPhoto()(mousePos.x(),mousePos.y());	
  		_textStream << " (" << std::setw(4) << mousePos.x() + 1 << ", "
					<< std::setw(4) << mousePos.y() + 1 << ')' << " Value: "
					<< value;
		}
	}
	else
	{
  		_textStream << " (  -  ,  -  )      Value:   -   ";
	}

  	_textLabel->setText(QString::fromStdString(_textStream.str()));
	_textLabel->update();
}