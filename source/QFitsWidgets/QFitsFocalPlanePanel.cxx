#include "QFitsFocalPlanePanel.h"
#include <algorithm>
//#include <iostream>
#include <cmath>
#include <chrono>

#ifdef LINUX_PLATFORM
	#include <thread>
	#include <chrono>
#endif
#ifdef WINDOWS_PLATFORM
	#include <windows.h>
#endif

#include <thread>

//#include <QtDebug>

#define PI 3.14159265


QFitsFocalPlanePanel::QFitsFocalPlanePanel(QWidget* parent) : QWidget(parent), _worker(NULL)
{	
	// Including DejaVuSans font for windows platform
	#ifdef WINDOWS_PLATFORM
	QFontDatabase::addApplicationFont(":/fonts/DejaVuSans.ttf");
	setFont(QFont("DejaVu Sans"));
	#endif
	
	// Creates panel grid to arrange widgets
	_panelGrid = new QGridLayout(this);
	
	// Creates alignment window widgets
	_starFieldImage = new QFitsLabel(this);
	_colorMapPlot = new QCustomPlot(this);
	_numericalInfo = new QLabel(this);
	_tempWidget = new QLabel(this);
	_colorMap= new QCPColorMap(_colorMapPlot->xAxis, _colorMapPlot->yAxis);
	
	// Widgets are placed in grid layout
	//	panelGrid->addWidget(starFieldImage, 0, 0, 1, 1);
	_panelGrid->addWidget(_colorMapPlot, 0, 1, 1, 1);
	_panelGrid->addWidget(_numericalInfo, 1, 0, 1, 2);
	
	// Some attributes are set
	// COLOR MAP PLOT & COLOR MAP
	_colorMapPlot->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	_colorMapPlot->yAxis->setRangeReversed(true);
	_colorMapPlot->xAxis->setVisible(false);	
	_colorMapPlot->xAxis2->setVisible(true);
	_colorMapPlot->xAxis2->setTickLabels(true);
	connect(_colorMapPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), _colorMapPlot->xAxis2, SLOT(setRange(QCPRange)));
	
	_colorMap->setGradient(QCPColorGradient::gpPolar);	
	
	// NUMERICAL INFO
	_numericalInfo->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);	
	QFont font = _numericalInfo->font();
	font.setPointSize(11);
	_numericalInfo->setFont(font);
	
	// STAR FIELD IMAGE
	_starFieldImage->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	
	//this->resize(x, y);
	resize (1200, 500);
	
	// TODO: Put this show in the right place
	show();
}


QFitsFocalPlanePanel::~QFitsFocalPlanePanel()
{
	qDebug() << "QFitsFocalPanel DESTROYED";
}


void QFitsFocalPlanePanel::closeEvent(QCloseEvent *event)
{
	if(_worker != NULL)
	{
		_worker->setQuitFlag();
		qDebug() << "CloseEvent";
		while (_worker->isRunning())
		#ifdef LINUX_PLATFORM
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		#endif
		#ifdef WINDOWS_PLATFORM
			Sleep(50);
		#endif
	}
	deleteLater();
	event->accept();
}


void QFitsFocalPlanePanel::analyseFitsFile(QString filename, double topThreshold, double bottomThreshold)
{
	_starFieldImage->open(filename);
	// BEGIN Field Image Setup
	
	// Use a blank widget to evaluate space available in window
	// TODO: replace this trick with some good code
	// 		 implementing QFitsLabel resize event cuold be a smart solution
	_panelGrid->addWidget(_tempWidget, 0, 0, 1, 1);
		
	// Gets star vector returned by extractStarProfiles function
	// TESTING:
//	std::vector<star> starVector = extractStarProfiles(_starFieldImage->getFitsPhoto(), threshold, true);
	std::vector<star> starVector = extractStarProfiles(_starFieldImage->getFitsPhoto(), topThreshold, bottomThreshold, true);
	
	// Field Image stretch is adjusted to display selected stars in bright white 
	//	pixelT brightestStar = starVector.back().intensity;
	pixelT dimmestStar = starVector.front().intensity;
	pixelT backgroundIntensity = _starFieldImage->getFitsPhoto().extractMedianFiltered(9).getImageMeanValue();
	// NOTE: This median filter is already applied by Star Detect function 
	//       therefore this calculation is a waste of resources. Anyway
	//       that's the price to pay for encapsulation and code reuse.
	// TODO: In the future all these resource demanding procedures should be
	//       organized in a proper way to reduce computing power wastes and
	//       speed up the program.	
	// END Field Image Setup
	
	// BEGIN Color Map Setup	
	int imageHeight = _starFieldImage->getFitsPhoto().getHeight();
	int imageWidth = _starFieldImage->getFitsPhoto().getWidth();
	
	// Focal plane equaqtion is calculated with multiple linear regression
	FocalPlane focalPlane = focalPlaneRegression(starVector);
	
	_colorMap->data()->setSize(50, 50);		
	_colorMap->data()->setRange(QCPRange(0, imageWidth-1), QCPRange(0, imageHeight-1));
	for (int x=0; x<imageWidth; ++x)
		for (int y=0; y<imageHeight; ++y)
			_colorMap->data()->setData(x, y, focalPlane.c + x*focalPlane.a + y*focalPlane.b);
	
	double FWHM_Center = focalPlane.c + focalPlane.a * (imageWidth / 2) +
	focalPlane.b * (imageHeight/2);
		
	double plotMaxRange = FWHM_Center + 2*pow(focalPlane.var_FWHM, 0.5);
	double plotMinRange = FWHM_Center - 2*pow(focalPlane.var_FWHM, 0.5);
	
	_colorMapPlot->addPlottable(_colorMap);
	_colorMapPlot->rescaleAxes();
//	_colorMap->rescaleDataRange(true);
	_colorMap->setDataRange((QCPRange(plotMinRange, plotMaxRange)));
	_colorMapPlot->replot();
	// END Color Plot Setup
	
	// BEGIN Numerical Info Setup
	double nablaModulus = pow(pow(focalPlane.a, 2) + pow(focalPlane.b, 2), .5);
	double nablaTheta = atan2(focalPlane.b, focalPlane.a) * 180 / PI;
	double FWHM_TopLeft = focalPlane.c;
	double FWHM_BottomLeft = focalPlane.c + focalPlane.b * imageHeight;
	double FWHM_BottomRight = focalPlane.c + focalPlane.a * imageWidth +
	focalPlane.b * imageHeight;
	double FWHM_TopRight = focalPlane.c + focalPlane.a * imageWidth;
	
	QString numericalInfoText = QString(QString::fromUtf8(
		"\u2207: (%1, %2)       |\u2207|, \u03D1: %3, %4 \u00B0       "
		"FWHM:  \u25F0: %5    \u25F1: %6    \u25F2: %7    \u25F3: %8    \u25AB : %9    SigmaFWHM : %10"))
	.arg(focalPlane.a, 5, 'E', 3).arg(focalPlane.b, 5, 'E', 3)
	.arg(nablaModulus, 5, 'E', 3).arg(nablaTheta, 5, 'g', 4)
	.arg(FWHM_TopLeft, 5, 'g', 3).arg(FWHM_BottomLeft, 5, 'g', 3).arg(FWHM_BottomRight, 5, 'g', 3)
	.arg(FWHM_TopRight, 5, 'g', 3).arg(FWHM_Center, 5, 'g', 3).arg(pow(focalPlane.var_FWHM, 0.5), 5, 'g', 3);
	
	_numericalInfo->setText(numericalInfoText);
	// END Numerical Info setup
	
	// this->show()
	show();
	
	// SETUP starFieldImage
	// After the layout geometry is set and the parent is visible,
	// the star field image can be resized according to available space
	// and the tempWidget is removed from layout.
	// NOTE: tempWidget is still child of "this" therefore is destroyed
	//       upon parent destruction.
	
		
	//TODO: This tempWidget workaround could be avoided if a proper resize event
	//		for QFitsLabel is reimplemented.
	
	// get label dimensions
	int w = _panelGrid->cellRect(0, 0).width();
	int h = _panelGrid->cellRect(0, 0).height();
	
	qDebug() << "W " << w << "H " << h;
		
	// set a scaled pixmap to a w x h window keeping its aspect ratio 
	double zoomFactor = std::min(static_cast<double>(w) / imageWidth,
								 static_cast<double>(h) / imageHeight);

	//starFieldImage->setPixmap(starFieldImage->pixmap()->scaled(w,h,Qt::KeepAspectRatio));
	_starFieldImage->updateZoom(zoomFactor);
	
	// Image is stretched so that all selected stars are 255 white and background
	// noise is removed (0 black)
	_starFieldImage->updateStretch(backgroundIntensity, dimmestStar+backgroundIntensity); 
	
	// Calls circleNumberStars QFitsWindow's method to circle them and assign a number
	_starFieldImage->circleNumberStars(starVector, 10);
	
	_panelGrid->removeWidget(_tempWidget);
	_tempWidget->hide();
	
	_panelGrid->addWidget(_starFieldImage, 0, 0, 1, 1);
	_starFieldImage->show();
	// END starFieldImage

	show();
}


void QFitsFocalPlanePanel::continuousAnalysis(QString folderPath, double topThreshold, double bottomThreshold)
{
	_colorMapPlot->addPlottable(_colorMap);
	_worker = new QFitsFocalPlaneWorker(folderPath, topThreshold, bottomThreshold, this);
	qRegisterMetaType<std::vector<star>>("std::vector<star>");
	qRegisterMetaType<FocalPlane>("FocalPlane");
	qRegisterMetaType<pixelT>("pixelT");
	connect(_worker, SIGNAL(finished(QString, std::vector<star>, FocalPlane, pixelT)), this,
		SLOT(updateView(QString, std::vector<star>, FocalPlane, pixelT)), Qt::QueuedConnection);
	
	// Starts worker thread
	_worker->start();
}


void QFitsFocalPlanePanel::updateView(QString fileName, std::vector<star> starVector, FocalPlane focalPlane, pixelT backgroundIntensity)
{	
	qDebug() << "SIGNAL RECEIVED";

	_starFieldImage->open(fileName);
	
	// BEGIN Field Image Setup
	
	_panelGrid->removeWidget(_starFieldImage);
	_starFieldImage->hide();
	
	// Use a blank widget to evaluate space available in window
	// TODO: replace this trick with some good code
	_panelGrid->addWidget(_tempWidget, 0, 0, 1, 1);
	
	// Field Image stretch is adjusted to display selected stars in bright white 
	//	pixelT brightestStar = starVector.back().intensity;
	pixelT dimmestStar = starVector.front().intensity;
	// END Field Image Setup
	
	// BEGIN Color Map Setup	
	int imageHeight = _starFieldImage->getFitsPhoto().getHeight();
	int imageWidth = _starFieldImage->getFitsPhoto().getWidth();	
	
	double FWHM_Center = focalPlane.c + focalPlane.a * (imageWidth / 2) +
	focalPlane.b * (imageHeight / 2);	
	
	double plotMaxRange = FWHM_Center + 2*pow(focalPlane.var_FWHM, 0.5);
	double plotMinRange = FWHM_Center - 2*pow(focalPlane.var_FWHM, 0.5);
	
	_colorMap->data()->clear();
	_colorMap->data()->setSize(50, 50);		
	_colorMap->data()->setRange(QCPRange(0, imageWidth-1), QCPRange(0, imageHeight-1));
	for (int x=0; x<imageWidth; ++x)
		for (int y=0; y<imageHeight; ++y)
			_colorMap->data()->setData(x, y, focalPlane.c + x*focalPlane.a+y*focalPlane.b);
	
	_colorMapPlot->rescaleAxes();
// 	_colorMap->rescaleDataRange(true);
	_colorMap->setDataRange((QCPRange(plotMinRange, plotMaxRange)));
	_colorMapPlot->replot();
	// END Color Plot Setup
	
	// BEGIN Numerical Info Setup
	double nablaModulus = pow(pow(focalPlane.a, 2) + pow(focalPlane.b, 2), .5);
	double nablaTheta = atan2(focalPlane.b, focalPlane.a) * 180 / PI;
	double FWHM_TopLeft = focalPlane.c;
	double FWHM_BottomLeft = focalPlane.c + focalPlane.b * imageHeight;
	double FWHM_BottomRight = focalPlane.c + focalPlane.a * imageWidth +
	focalPlane.b * imageHeight;
	double FWHM_TopRight = focalPlane.c + focalPlane.a * imageWidth;
	
	QString numericalInfoText = QString(QString::fromUtf8(
		"\u2207: (%1, %2)       |\u2207|, \u03D1: %3, %4 \u00B0       "
		"FWHM:  \u25F0: %5    \u25F1: %6    \u25F2: %7    \u25F3: %8    \u25AB : %9    SigmaFWHM : %10"))
	.arg(focalPlane.a, 5, 'E', 3).arg(focalPlane.b, 5, 'E', 3)
	.arg(nablaModulus, 5, 'E', 3).arg(nablaTheta, 5, 'g', 4)
	.arg(FWHM_TopLeft, 5, 'g', 3).arg(FWHM_BottomLeft, 5, 'g', 3).arg(FWHM_BottomRight, 5, 'g', 3)
	.arg(FWHM_TopRight, 5, 'g', 3).arg(FWHM_Center, 5, 'g', 3).arg(pow(focalPlane.var_FWHM, 0.5), 5, 'g', 3);
	
	_numericalInfo->setText(numericalInfoText);
	// END Numerical Info setup
	
	show();
	
	// SETUP starFieldImage
	// After the layout geometry is set and the parent is visible,
	// the star field image can be resized according to available space
	// and the tempWidget is removed from layout.
	// NOTE: tempWidget is still child of "this" therefore is destroyed
	//       upon parent destruction.
	
	
	//TODO: This tempWidget workaround could be avoided if a proper resize event
	//		for QFitsLabel is reimplemented.
	
	// get label dimensions
	int w = _panelGrid->cellRect(0, 0).width();
	int h = _panelGrid->cellRect(0, 0).height();
	
	qDebug() << "W " << w << "H " << h;
	
	// set a scaled pixmap to a w x h window keeping its aspect ratio 
	double zoomFactor = std::min(static_cast<double>(w) / imageWidth,
								 static_cast<double>(h) / imageHeight);
	
	//starFieldImage->setPixmap(starFieldImage->pixmap()->scaled(w,h,Qt::KeepAspectRatio));
	_starFieldImage->updateZoom(zoomFactor);
	
	// Image is stretched so that all selected stars are 255 white and background
	// noise is removed (0 black)
	_starFieldImage->updateStretch(backgroundIntensity, dimmestStar+backgroundIntensity); 
	
	// Calls circleNumberStars QFitsWindow's method to circle them and assign a number
	_starFieldImage->circleNumberStars(starVector, 10);
	
	
	_panelGrid->removeWidget(_tempWidget);
	_tempWidget->hide();
	
	_panelGrid->addWidget(_starFieldImage, 0, 0, 1, 1);
	
	_starFieldImage->show();
	// END starFieldImage

	show();
	
	qDebug() << "Is worker running? " << _worker->isRunning();
	qDebug() << "Has worker finished? " << _worker->isFinished();
	
	// Starts worker thread again
	_worker->start();
	
	qDebug() << "Is worker running? " << _worker->isRunning();
	qDebug() << "Has worker finished? " << _worker->isFinished();
	
}



QFitsFocalPlaneWorker::QFitsFocalPlaneWorker(QString folderPath, double topThreshold, double bottomThreshold, QWidget* parent) :
				QThread(parent), _quit(false), _oldFilename	(""), _folderPath(folderPath),
				_topThreshold(topThreshold), _bottomThreshold(bottomThreshold)
{
	_fileFilters << "*.fit" << "*.fits";
	setTerminationEnabled(true);
}


void QFitsFocalPlaneWorker::setQuitFlag()
{
	_quit = true;
}
	

void QFitsFocalPlaneWorker::run()
{
	QDir workingFolder(_folderPath, "", QDir::Time/*|QDir::Reversed*/, QDir::Files);
	workingFolder.setNameFilters(_fileFilters);
	QStringList fileList = workingFolder.entryList();
		
	while (fileList.size() == 0 || fileList.at(0) == _oldFilename)
	// The power of short circuit evaluation!
	{	
		if (_quit == true)
			return;
		
		workingFolder.refresh();
		fileList = workingFolder.entryList();
// 		qDebug() << "FileListSize " << fileList.size() << " fileList[0] "
// 					<< fileList.at(0) << " _oldFilename " << _oldFilename; 
		
	#ifdef LINUX_PLATFORM
		std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Sleeps for 500 milliseconds
	#endif
	#ifdef WINDOWS_PLATFORM
		Sleep(500);
	#endif
	}
	
	// Latest file is selected to be analysed.
	QString fileName = fileList.at(0);
	_oldFilename = fileName;
	
	FitsPhoto fitsFile;
	fitsFile.open(workingFolder.absoluteFilePath(fileName).toStdString());
	
	// Gets star vector returned by extractStarProfiles function
	// TESTING:
	std::vector<star> starVector = extractStarProfiles(fitsFile, _topThreshold, _bottomThreshold, true);
// 	std::vector<star> starVector = extractStarProfiles(fitsFile, _threshold, true);
	
	if (_quit == true)
		return;
	
	// Focal plane equaqtion is calculated with multiple linear regression
	FocalPlane focalPlane = focalPlaneRegression(starVector);
	
	pixelT backgroundIntensity = fitsFile.extractMedianFiltered(9).getImageMeanValue();
	// NOTE: This median filter is already applied by Star Detect function 
	//       therefore this calculation is a waste of resources. Anyway
	//       that's the price to pay for encapsulation and code reuse.
	// TODO: In the future all these resource demanding procedures should be
	//       organized in a proper way to reduce computing power wastes and
	//       speed up the program.
	
	if (_quit == true)
		return;
		
	emit finished(workingFolder.absoluteFilePath(fileName),
					starVector, focalPlane, backgroundIntensity);
}


QFitsFocalPlaneWorker::~QFitsFocalPlaneWorker()
{
	qDebug() << "DESTRUCTOR INVOKED";
	qDebug() << isRunning();	
	qDebug() << "WORKER DESTROYED";
}