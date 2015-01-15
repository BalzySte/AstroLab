#ifndef __QFitsFocalPlanePanel_h__
#define __QFitsFocalPlanePanel_h__


#include <QtWidgets>
#include "qcustomplot.h"
#include "FitsPhoto.h"
#include "astroAnalyzer.h"
#include "QFitsWindow.h"
#include "focalPlaneRegression.h"


class QFitsFocalPlaneWorker;

// TODO: THIS WIDGET SHOULD BLOCK PARENT WINDOW AND MEMORY SHOULD BE DEALLOCATED WHEN CLOSED
class QFitsFocalPlanePanel : public QWidget
{
friend class QFitsFocalPlaneWorker;
	Q_OBJECT

public:
	QFitsFocalPlanePanel(QWidget* parent = 0);
	virtual ~QFitsFocalPlanePanel();
	void analyseFitsFile(QString filename, double topThreshold, double bottomThreshold);
	void continuousAnalysis(QString folderPath, double topThreshold, double bottomThreshold);
	
protected:
	virtual void closeEvent(QCloseEvent *event);

private slots:
	void updateView(QString fileName, std::vector<star> starVector, FocalPlane focalPlane, pixelT backgroundIntensity);

private:
	QFitsFocalPlaneWorker* _worker;
	
	QGridLayout* _panelGrid;
	QFitsLabel* _starFieldImage;
	QCustomPlot* _colorMapPlot;
	QLabel* _numericalInfo;
	QCPColorMap* _colorMap;
	QLabel* _tempWidget;
};

class QFitsFocalPlaneWorker : public QThread
{
	Q_OBJECT

public:
	QFitsFocalPlaneWorker(QString folderPath, double topThreshold, double bottomThreshold, QWidget* parent = 0);
	virtual ~QFitsFocalPlaneWorker();
	void setQuitFlag();

signals:
	void finished(QString fileName, std::vector<star> starVector, FocalPlane focalPlane, pixelT backgroundIntensity);
	
protected:
	void run();
	
private:
	bool _quit;
	QString _oldFilename;
	QString _folderPath;
	QStringList _fileFilters;
	double _topThreshold;
	double _bottomThreshold;
};

#endif //__QFitsFocalPlanePanel_h__