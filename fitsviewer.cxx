#include "fitsviewer.h"

#include "FitsPhoto.h"
#include "QFitsListWidgetItem.h"
#include "astroAnalizer.h"
#include <vector>
#include <sstream>


FitsViewer::FitsViewer() : _currentFitsImage(NULL)
{
	// Creates main area, containing Fits images Windows
	workspace = new QMdiArea(this);
	workspace->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	// Connects signals to slots and creates menus
	createActions();  
	createMenus();

	// Creates sliders dock, which is used to stretch the focused image.
	stretchDock = new QFitsStretchDock(this);
	
	// Creates zoom dock
	zoomDock = new QFitsZoomDock(this);
	
	// Adds created widget to the main window
	setCentralWidget(workspace);
	
	// Sets right dock area ownership of bottom right corner
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
	
	// Adds docks
	addDockWidget(Qt::RightDockWidgetArea, stretchDock);
	addDockWidget(Qt::BottomDockWidgetArea, zoomDock);
	
	setWindowTitle("Fits Viewer");
	
	// Sets main window size
	resize(800, 600);
}


void FitsViewer::setFocusedWindow(QFitsWindow* window)
{
	_currentFitsImage = window;
	
	// Updates docks
	stretchDock->update();
	zoomDock->update();
}


QFitsWindow* FitsViewer::getFocusedWindow() const
{
	return _currentFitsImage;
}


void FitsViewer::open()
{
	// Prompts the user to select file(s) to open
	QStringList openFileNames = QFileDialog::getOpenFileNames(this,
		"Open Images", QDir::currentPath(), "Fits Images (*.fit *.fits)");
	
	// If user selected at least 1 file opens it creating a FitsWindow.
	if (!openFileNames.isEmpty())
	{
		//Creates images
		for (int count = 0; count < openFileNames.length(); ++count)
		{
			QFitsWindow *newFitsWindow = new QFitsWindow(imageWindowsList, workspace);
			newFitsWindow->open(openFileNames[count]);
			newFitsWindow->show();
		}
	}
	else
		return;
	
	imageWindowsList.back()->setFocus();

// TODO: Show a proper message in case of error
//    QMessageBox::information(this, "Image Viewer",
//						QString("Cannot load %1.").arg(fileName));
}


void FitsViewer::save()
{ 
  
}


void FitsViewer::addition()
{
	// TODO: check first whether there are only 2 images opened -> sumImg = img1+img2.
	QListWidget* selectionList = new QListWidget;
	
	// Checks whether an image is focused:
	if (_currentFitsImage == NULL)
		return;
	
	for (std::list<QFitsWindow*>::iterator it = imageWindowsList.begin(); it != imageWindowsList.end(); ++it)
	{
		QFitsListWidgetItem* _imageEntry = new QFitsListWidgetItem((*it)->getImageTitle(), *it);
											// Becoming selectionList's children
											// they are destroyed as the function
											// returns together with their father

		selectionList->addItem((_imageEntry));
	}

	// Selection list was populated, now creating dialog to prompt the user
	QDialog *imgDialog = new QDialog(this, Qt::Dialog | Qt::FramelessWindowHint);

	// Creates a vertical box layout for QDialog
	QVBoxLayout *dialog_layout = new QVBoxLayout(imgDialog);
	dialog_layout->addWidget(selectionList);
	
	// Creates button box with 2 buttons
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
		// Creates new sum image
		// TODO: Simplify this unreadable messy assignment.
		FitsPhoto newFitsPhoto = const_cast<const QFitsWindow*>(_currentFitsImage)->getFitsPhoto() +
				const_cast<const QFitsWindow*>(dynamic_cast<QFitsListWidgetItem*>(
				selectionList->currentItem())->getFitsWindowPtr())->getFitsPhoto();
		
		// Creates new fits image window
		QFitsWindow *newFitsWindow = new QFitsWindow(imageWindowsList, workspace);
		newFitsWindow->createFromFitsPhoto(newFitsPhoto, "SumImage");
	}

	delete imgDialog;	// Delete image dialog object from heap
						// together with its children
}


void FitsViewer::subtraction()
{
	// TODO: check first whether there are only 2 images opened -> sumImg = img1+img2.
	QListWidget* selectionList = new QListWidget;
	
	// Check whether an image is focused:
	if (_currentFitsImage == NULL)
		return;
	
	for (std::list<QFitsWindow*>::iterator it = imageWindowsList.begin(); it != imageWindowsList.end(); ++it)
	{
		QFitsListWidgetItem* _imageEntry = new QFitsListWidgetItem((*it)->getImageTitle(), *it);
		// Becoming selectionList's children
		// they are destroied aas the function
		// returns together with their father
		
		selectionList->addItem((_imageEntry));
	}
	
	// Selection list was populated, now creating dialog to prompt the user
	QDialog *imgDialog = new QDialog(this, Qt::Dialog | Qt::FramelessWindowHint);
	
	// Creates a vertical box layout for QDialog
	QVBoxLayout *dialog_layout = new QVBoxLayout(imgDialog);
	dialog_layout->addWidget(selectionList);
	
	// Creates button box with 2 buttons
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
		// Creates new difference image
		// TODO: Simply this unreadable messy assignment.
		FitsPhoto newFitsPhoto = const_cast<const QFitsWindow*>(_currentFitsImage)->getFitsPhoto() - const_cast<const QFitsWindow*>(dynamic_cast<
		QFitsListWidgetItem*>(selectionList->currentItem())->getFitsWindowPtr())->getFitsPhoto();
		
		// Creates new fits image window
		QFitsWindow *newFitsWindow = new QFitsWindow(imageWindowsList, workspace);
		newFitsWindow->createFromFitsPhoto(newFitsPhoto, "SubtractImage");
	}

	delete imgDialog;	// Delete image dialog object from heap
}


void FitsViewer::multiplication()
{
	// Checks whether an image is focused
	if (_currentFitsImage == NULL)
		return;
	
	// Asking user factor value. Variable ok is used to remember user choice.
	// If user confirmed it is set to TRUE, otherwise to FALSE. 
	bool ok;
	double factor = QInputDialog::getDouble(this, "Multiply by:",
									   tr("Factor:"), 1, -2147483647, 2147483647, 3, &ok);
	if (ok)		//Checks if user confirmed operation
	{
		// Creates multiplied new image
		FitsPhoto newFitsPhoto = const_cast<const QFitsWindow*>(_currentFitsImage)->getFitsPhoto() * factor;
		
		// Creates new fits image window
		QFitsWindow *newFitsWindow = new QFitsWindow(imageWindowsList, workspace);
		newFitsWindow->createFromFitsPhoto(newFitsPhoto, "ProductImage");
	}
}


void FitsViewer::division()
{
	// Checks whether an image is focused
	if (_currentFitsImage == NULL)
		return;
	
	// Same as FitsViewer::multiplication method.
	bool ok = 0;
	double divisor = QInputDialog::getDouble(this, "Divide by:",
											tr("Divisor:"), 1, -2147483647, 2147483647, 3, &ok);
	if (ok)
	{
		if (divisor == 0.)	// Makes sure dumb user does not divide by 0
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


void FitsViewer::createMedianFilter()
{	
	// Checks whether an image is focused
	if (_currentFitsImage == NULL)
		return;
	
	// Gets image width and height. Minimum between them is definitely the maximum median matrix size
	int maxN = std::min<int>(const_cast<const QFitsWindow*>(_currentFitsImage)->getFitsPhoto().getWidth(),
						 const_cast<const QFitsWindow*>(_currentFitsImage)->getFitsPhoto().getHeight());
	bool ok = 0;
	int n = QInputDialog::getInt(this, "Insert square matrix dimension (odd integer value)",
								 tr("Value:"), 1, 1, maxN, 3, &ok);
	if (ok)
	{
		if (_currentFitsImage == NULL)
			return;
		
		if (n % 2 == 0)	// Makes sure number is even
		{
			QMessageBox msgBox(QMessageBox::Warning, "Invalid Value",
							   "Odd Integer Required", 0, this);
			msgBox.addButton(tr("&Continue"), QMessageBox::AcceptRole);
			//msgBox.exec();
			return;
		}
		
		// Generates filtered photo
		FitsPhoto newFitsPhoto = (const_cast<const QFitsWindow*>(_currentFitsImage)->getFitsPhoto()).extractMedianFiltered(n);

		// Creates new image window
		QFitsWindow* newFitsWindow = new QFitsWindow(imageWindowsList, workspace);
		
		// Creates and sets Image title
		std::stringstream newTitle;
		newTitle << "MedianFiltered_" << n <<'x' << n;
		newFitsWindow->createFromFitsPhoto(newFitsPhoto, newTitle.str().c_str()); // no std::string temp var?
	}
}


void FitsViewer::createLowPassFilter()
{
	// Checks whether an image is focused
	if (_currentFitsImage == NULL)
		return;

	// Generates filtered photo
	FitsPhoto newFitsPhoto = (const_cast<const QFitsWindow*>(_currentFitsImage)->getFitsPhoto()).extractLowPassFilter3x3();
	
	// Creates new image window
	QFitsWindow* newFitsWindow = new QFitsWindow(imageWindowsList, workspace);
	
	// Creates and sets Image title
	std::stringstream newTitle;
	newTitle << "LowPassFiltered_" << 3 <<'x' << 3;
	newFitsWindow->createFromFitsPhoto(newFitsPhoto, newTitle.str().c_str()); // no std::string temp var?
}


void FitsViewer::findStars()
{
	if (_currentFitsImage == NULL)
		return;
	
	bool ok;
	double treshold = QInputDialog::getDouble(this, "Multiply by:",
											tr("Factor:"), 0.4, 0.001, 1., 3, &ok);
	if (ok)		//Checks if user confirmed operation
	{			
		// Get star vector returned by detectStars function
		std::vector<star> starVector = detectStars(const_cast<const QFitsWindow*>(_currentFitsImage)->getFitsPhoto(), treshold);
		
		// Calls circleStars QFitsWindow's method to circle them
		for (std::vector<star>::iterator it = starVector.begin(); it != starVector.end(); ++it)
			_currentFitsImage->circleStars(starVector, 10);
	}		
}


void FitsViewer::quit()
{
	//TODO: Check for windows with unsaved content
}


void FitsViewer::about()
{
  QMessageBox::about(this, "About Image Viewer",
			"<b>FitsViewer</b> example program. "
			"Test version. "
			"Can open, add and subtract fits images. "
			"Fits image array is displayed in 8 bit grayscale.");
}


// Populates menu bar with menus and functions
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
	viewMenu->addActiMedia(normalSizeAct);
	viewMenu->addSeparator();
	viewMenu->addAction(fitToWindowAct);
*/
	operationsMenu = new QMenu("Operations", this);
	operationsMenu->addAction(addAct);
	operationsMenu->addAction(subtractAct);
	operationsMenu->addAction(multiplyAct);
	operationsMenu->addAction(divideAct);
	
	filtersMenu = new QMenu("Filters", this);
	filtersMenu->addAction(createMedianFilterAct);
	filtersMenu->addAction(createLowPassFilterAct);
	
	analysisMenu = new QMenu("Analysis", this);
	analysisMenu->addAction(findStarsAct);

	helpMenu = new QMenu("Help", this);
	helpMenu->addAction(aboutAct);

	menuBar()->addMenu(fileMenu);
//	menuBar()->addMenu(viewMenu);
	menuBar()->addMenu(operationsMenu);
	menuBar()->addMenu(filtersMenu);
	menuBar()->addMenu(analysisMenu);
	menuBar()->addMenu(helpMenu);
}


// Connects menu functions' signals to relative implemented methods (slots)
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
	
	createMedianFilterAct = new QAction("Extract Median Filtered ...", this);
	connect(createMedianFilterAct, SIGNAL(triggered()), this, SLOT(createMedianFilter()));
	
	createLowPassFilterAct = new QAction("Extract Low Pass 3x3 Filtered ...", this);
	connect(createLowPassFilterAct, SIGNAL(triggered()), this, SLOT(createLowPassFilter()));	
	
	findStarsAct = new QAction("Find Stars ...", this);
	connect(findStarsAct, SIGNAL(triggered()), this, SLOT(findStars()));
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