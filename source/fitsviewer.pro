QT_SELECT = qt4

HEADERS       = fitsviewer.h \
		QFitsWidgets/QFitsWindow.h \
		QFitsWidgets/QFitsStretchDock.h \
		QFitsWidgets/QFitsZoomDock.h \
		QFitsWidgets/QFitsListWidgetItem.h \
		QFitsWidgets/QTextInfoWindow.h \
		QFitsWidgets/QFitsFocalPlanePanel.h \
		QCustomPlot/qcustomplot.h \
		MathFunctions/focalPlaneRegression.h \
		settings.h

SOURCES       = fitsviewer.cxx \
		main.cxx \
		FitsPhoto/FitsPhoto.cxx \
		FitsPhoto/FitsToImage.cxx \
		FitsPhoto/quickselect.c \
		QFitsWidgets/QFitsWindow.cxx \
		QFitsWidgets/QFitsStretchDock.cxx \
		QFitsWidgets/QFitsZoomDock.cxx \
		QFitsWidgets/QMousePointerDock.cxx \
		QFitsWidgets/QFitsListWidgetItem.cxx \
		QFitsWidgets/QTextInfoWindow.cxx \
		QFitsWidgets/QFitsFocalPlanePanel.cxx \
		AstroAnalyzer/AstroAnalyzer.cxx \
		QCustomPlot/qcustomplot.cxx \
		MathFunctions/focalPlaneRegression.cxx

# LIBRARIES
LIBS += -lCCfits -lcfitsio -lgsl -lgslcblas

INCLUDEPATH += "/usr/include/CCfits"

INCLUDEPATH += "FitsPhoto"
INCLUDEPATH += "QFitsWidgets"
INCLUDEPATH += "AstroAnalyzer"
INCLUDEPATH += "QCustomPlot"
INCLUDEPATH += "MathFunctions"


linux {
	# PLATFORM DEFINE
	DEFINES += LINUX_PLATFORM

	
	# COMPILED FILES DESTINATION
	DESTDIR = ../build
	TARGET = fitsviewer 
	OBJECTS_DIR = ../build/linux
	MOC_DIR = ../build/linux
	RCC_DIR = ../build/linux
	UI_DIR = ../buildi/linux
	                 
	
	# FLAGS SETTINGS	
	QMAKE_CFLAGS_RELEASE -= -O2
	QMAKE_CFLAGS_RELEASE += -O3
	
	QMAKE_CXXFLAGS_RELEASE -= -O2
	QMAKE_CXXFLAGS_RELEASE += -O3
	
	                 
	QMAKE_CXXFLAGS += -std=c++11
	
	QMAKE_LFLAGS_RELEASE -= -O1
	QMAKE_LFLAGS_RELEASE += -O3
	
	QMAKE_CXXFLAGS_DEBUG += -pg
	QMAKE_LFLAGS_DEBUG += -pg
}


win32 {
	# PLATFORM DEFINE
	DEFINES += WINDOWS_PLATFORM

	
	# COMPILED FILES DESTINATION
	DESTDIR = ../build
	TARGET = fitsviewer32
	OBJECTS_DIR = ../build/win32
	MOC_DIR = ../build/win32
	RCC_DIR = ../build/win32
	UI_DIR = ../buildi/win32
	                 
	
	# FLAGS SETTINGS	
	QMAKE_CFLAGS_RELEASE -= -O2
	QMAKE_CFLAGS_RELEASE += -O3
	
	QMAKE_CXXFLAGS_RELEASE -= -O2
	QMAKE_CXXFLAGS_RELEASE += -O3
	
	                 
	QMAKE_CXXFLAGS += -std=c++11
	
	QMAKE_LFLAGS_RELEASE -= -O1
	QMAKE_LFLAGS_RELEASE += -O3
	
	QMAKE_CXXFLAGS_DEBUG += -pg
	QMAKE_LFLAGS_DEBUG += -pg

	INCLUDEPATH += "."
	RESOURCES = Resources/win_resources.qrc
}


win64 {
	# PLATFORM DEFINE
	DEFINES += WINDOWS_PLATFORM

	
	# COMPILED FILES DESTINATION
	DESTDIR = ../build
	TARGET = fitsviewer64
	OBJECTS_DIR = ../build/win64
	MOC_DIR = ../build/win64
	RCC_DIR = ../build/win64
	UI_DIR = ../buildi/win64
	                 
	
	# FLAGS SETTINGS	
	QMAKE_CFLAGS_RELEASE -= -O2
	QMAKE_CFLAGS_RELEASE += -O3
	
	QMAKE_CXXFLAGS_RELEASE -= -O2
	QMAKE_CXXFLAGS_RELEASE += -O3
	
	                 
	QMAKE_CXXFLAGS += -std=c++11
	
	QMAKE_LFLAGS_RELEASE -= -O1
	QMAKE_LFLAGS_RELEASE += -O3
	
	QMAKE_CXXFLAGS_DEBUG += -pg
	QMAKE_LFLAGS_DEBUG += -pg

	INCLUDEPATH += "."
	RESOURCES = Resources/win_resources.qrc
}