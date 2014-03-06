#include "fitsviewer.h"
#include "FitsPhoto.h"
#include "QFitsListWidgetItem.h"

#include <iostream>
#include <QtGui>

FitsViewer::FitsViewer() : _currentFitsImage(NULL)
{
	workspace = new QMdiArea(this);
	workspace->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	createActions();  
	createMenus();

	stretchDock = new QFitsStretchDock(this);
		
	setCentralWidget(workspace);
	addDockWidget(Qt::RightDockWidgetArea, stretchDock);
	
	setWindowTitle("Fits Viewer");
	
	resize(800, 600);
}


void FitsViewer::setFocusedWindow(QFitsWindow* window)
{
	_currentFitsImage = window;
}


QFitsWindow* FitsViewer::getFocusedWindow() const
{
	return _currentFitsImage;
}


void FitsViewer::updateStretchDock()
{
	if (stretchDock != NULL)
		stretchDock->update();
}


void FitsViewer::open()
{

	QStringList openFileNames = QFileDialog::getOpenFileNames(this,
                                     "Open Images", QDir::currentPath());
  
	if (!openFileNames.isEmpty())
	{
		//Creates images
		for (int count = 0; count < openFileNames.length(); ++count)
		{
			QFitsWindow *newFitsWindow = new QFitsWindow(imageWindowsList, workspace);
			newFitsWindow->open(openFileNames[count]);
		//	workspace->addSubWindow(newFitsWindow);
			newFitsWindow->show();
		}
	}
	
	imageWindowsList.back()->setFocus();
/*  
  if (image.isNull())
  {
    QMessageBox::information(this, "Image Viewer",
                            QString("Cannot load %1.").arg(fileName));
    return;
  }
*/

//		QImage image(fileName);


//   scaleFactor = 1.0;
  

//   fitToWindowAct->setEnabled(true);
//   updateActions();

//   if (!fitToWindowAct->isChecked())
}


void FitsViewer::save()
{ 
  
}


void FitsViewer::addition()
{
	// TODO: check first whether there are only 2 images opened -> sumImg = img1+img2.
	QListWidget* selectionList = new QListWidget;
	
	// Check wheter an image is focused:
	if (_currentFitsImage == NULL)
		return;
	
	for (std::list<QFitsWindow*>::iterator it = imageWindowsList.begin(); it != imageWindowsList.end(); ++it)
	{
		QFitsListWidgetItem* _imageEntry = new QFitsListWidgetItem((*it)->getImageTitle(), *it);
											// Becoming selectionList's children
											// they are destroied aas the function
											// returns together with their father

		selectionList->addItem((_imageEntry)); //Maybe needs a dynamic cast to QListWidgetItem?
	}

	// Selection list was populated, now creating dialog to prompt the user
	QDialog *imgDialog = new QDialog(this, Qt::Dialog | Qt::FramelessWindowHint);

	// Add a layout for QDialog
	QVBoxLayout *dialog_layout = new QVBoxLayout(imgDialog);
	dialog_layout->addWidget(selectionList);
	
	QDialogButtonBox *buttonBox = new QDialogButtonBox();
	buttonBox->addButton("Continue", QDialogButtonBox::AcceptRole);
	buttonBox->addButton("Cancel", QDialogButtonBox::RejectRole);

	// Connects buttonBox signals to their slots
	connect(buttonBox, SIGNAL(accepted()), imgDialog, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), imgDialog, SLOT(reject()));
	
	dialog_layout->addWidget(buttonBox);

	imgDialog->setLayout(dialog_layout);
	int result = imgDialog->exec();

	if (result == QDialog::Accepted)
	{
		std::cout << selectionList->currentItem() << std::endl;
		// Create new image
		FitsPhoto newFitsPhoto = const_cast<const QFitsWindow*>(_currentFitsImage)->getFitsPhoto() + const_cast<const QFitsWindow*>(dynamic_cast<
				QFitsListWidgetItem*>(selectionList->currentItem())->getFitsWindowPtr())->getFitsPhoto();
		
		QFitsWindow *newFitsWindow = new QFitsWindow(imageWindowsList, workspace);
		newFitsWindow->createFromFitsPhoto(newFitsPhoto, "SumImage");
	}
	else
	{
		// Do nothing
	}
	delete imgDialog;	// Delete object from heap
}


void FitsViewer::subtraction()
{
	// TODO: check first whether there are only 2 images opened -> sumImg = img1+img2.
	QListWidget* selectionList = new QListWidget;
	
	// Check wheter an image is focused:
	if (_currentFitsImage == NULL)
		return;
	
	for (std::list<QFitsWindow*>::iterator it = imageWindowsList.begin(); it != imageWindowsList.end(); ++it)
	{
		QFitsListWidgetItem* _imageEntry = new QFitsListWidgetItem((*it)->getImageTitle(), *it);
		// Becoming selectionList's children
		// they are destroied aas the function
		// returns together with their father
		
		selectionList->addItem((_imageEntry)); //Maybe needs a dynamic cast to QListWidgetItem?
	}
	
	// Selection list was populated, now creating dialog to prompt the user
	QDialog *imgDialog = new QDialog(this, Qt::Dialog | Qt::FramelessWindowHint);
	
	// Add a layout for QDialog
	QVBoxLayout *dialog_layout = new QVBoxLayout(imgDialog);
	dialog_layout->addWidget(selectionList);
	
	QDialogButtonBox *buttonBox = new QDialogButtonBox();
	buttonBox->addButton("Continue", QDialogButtonBox::AcceptRole);
	buttonBox->addButton("Cancel", QDialogButtonBox::RejectRole);
	
	// Connects buttonBox signals to their slots
	connect(buttonBox, SIGNAL(accepted()), imgDialog, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), imgDialog, SLOT(reject()));
	
	dialog_layout->addWidget(buttonBox);
	
	imgDialog->setLayout(dialog_layout);
	int result = imgDialog->exec();
	
	if (result == QDialog::Accepted)
	{
		std::cout << selectionList->currentItem() << std::endl;
		// Create new image
		FitsPhoto newFitsPhoto = const_cast<const QFitsWindow*>(_currentFitsImage)->getFitsPhoto() - const_cast<const QFitsWindow*>(dynamic_cast<
		QFitsListWidgetItem*>(selectionList->currentItem())->getFitsWindowPtr())->getFitsPhoto();
		
		QFitsWindow *newFitsWindow = new QFitsWindow(imageWindowsList, workspace);
		newFitsWindow->createFromFitsPhoto(newFitsPhoto, "SubtractImage");
	}
	else
	{
		// Do nothing
	}
	delete imgDialog;	// Delete object from heap
}

void FitsViewer::multiplication()
{
	bool ok;
	double factor = QInputDialog::getDouble(this, "Multiply by:",
									   tr("Factor:"), 1, -2147483647, 2147483647, 3, &ok);
	if (ok)
	{
		if (_currentFitsImage == NULL)
			return;
		
		FitsPhoto newFitsPhoto = const_cast<const QFitsWindow*>(_currentFitsImage)->getFitsPhoto() * factor;
		
		QFitsWindow *newFitsWindow = new QFitsWindow(imageWindowsList, workspace);
		newFitsWindow->createFromFitsPhoto(newFitsPhoto, "ProductImage");
	}
}


void FitsViewer::division()
{
	bool ok = 0;
	double divisor = QInputDialog::getDouble(this, "Multiply by:",
											tr("Factor:"), 1, -2147483647, 2147483647, 3, &ok);
	if (ok)
	{
		if (_currentFitsImage == NULL)
			return;

		if (divisor == 0.)
		{
			QMessageBox msgBox(QMessageBox::Warning, "Division by 0",
							   "You cannot divide by <b><i>0</i></b>", 0, this);
			msgBox.addButton(tr("&Continue"), QMessageBox::AcceptRole);
			//msgBox.exec();
			return;
		}

		FitsPhoto newFitsPhoto = const_cast<const QFitsWindow*>(_currentFitsImage)->getFitsPhoto() / divisor;
		
		QFitsWindow* newFitsWindow = new QFitsWindow(imageWindowsList, workspace);
		newFitsWindow->createFromFitsPhoto(newFitsPhoto, "DivisionImage");
	}
}

void FitsViewer::quit()
{
  
}


void FitsViewer::about()
{
  QMessageBox::about(this, "About Image Viewer",
			"<b>FitsViewer</b> example program. "
			"Test version. "
			"Can open, add and subtract fits images. "
			"Fits image array is displayed in 8 bit grayscale.");
}


void FitsViewer::createMenus()
{ 
	fileMenu = new QMenu("File", this);
	fileMenu->addAction(openAct);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);
/*
	viewMenu = new QMenu("View", this);
	viewMenu->addAction(zoomInAct);
	viewMenu->addAction(zoomOutAct);
	viewMenu->addAction(normalSizeAct);
	viewMenu->addSeparator();
	viewMenu->addAction(fitToWindowAct);
*/
	operationsMenu = new QMenu("Operations", this);
	operationsMenu->addAction(addAct);
	operationsMenu->addAction(subtractAct);
	operationsMenu->addAction(multiplyAct);
	operationsMenu->addAction(divideAct);

	helpMenu = new QMenu("Help", this);
	helpMenu->addAction(aboutAct);

	menuBar()->addMenu(fileMenu);
//	menuBar()->addMenu(viewMenu);
	menuBar()->addMenu(operationsMenu);
	menuBar()->addMenu(helpMenu);
}


void FitsViewer::createActions()
{
	openAct = new QAction("Open...", this);
//	openAct->setShortcut(tr("Ctrl+O"));
	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
	
	exitAct = new QAction("Exit", this);
//	exitAct->setShortcut(tr("Ctrl+Q"));
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

	addAct = new QAction("Add ...", this);
	connect(addAct, SIGNAL(triggered()), this, SLOT(addition()));
	
	subtractAct = new QAction("Subtract ...", this);
	connect(subtractAct, SIGNAL(triggered()), this, SLOT(subtraction()));
	
	multiplyAct = new QAction("Multiply ...", this);
	connect(multiplyAct, SIGNAL(triggered()), this, SLOT(multiplication()));
	
	divideAct = new QAction("Divide ...", this);
	connect(divideAct, SIGNAL(triggered()), this, SLOT(division()));
/*
	zoomInAct = new QAction("Zoom In (25%)", this);
	zoomInAct->setShortcut(tr("Ctrl++"));
	zoomInAct->setEnabled(false);
	connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));
	
	zoomOutAct = new QAction(tr("Zoom &Out (25%)"), this);
	zoomOutAct->setShortcut(tr("Ctrl+-"));
	zoomOutAct->setEnabled(false);
	connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));
	
	normalSizeAct = new QAction(tr("&Normal Size"), this);
	normalSizeAct->setShortcut(tr("Ctrl+S"));
	normalSizeAct->setEnabled(false);
	connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));

	fitToWindowAct = new QAction(tr("&Fit to Window"), this);
	fitToWindowAct->setEnabled(false);
	fitToWindowAct->setCheckable(true);
	fitToWindowAct->setShortcut(tr("Ctrl+F"));
	connect(fitToWindowAct, SIGNAL(triggered()), this, SLOT(fitToWindow()));
*/
	aboutAct = new QAction(tr("About"), this);
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
}


void FitsViewer::createStretchSlider()
{
	
}