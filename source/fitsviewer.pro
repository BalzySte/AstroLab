HEADERS       = fitsviewer.h \
		QFitsWidgets/QFitsWindow.h \
		QFitsWidgets/QFitsStretchDock.h \
		QFitsWidgets/QFitsZoomDock.h \
		QFitsWidgets/QFitsListWidgetItem.h \
		QFitsWidgets/QTextInfoWindow.h \

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
                AstroAnalyzer/AstroAnalyzer.cxx


# COMPILED FILES DESTINATION
DESTDIR = ../build
OBJECTS_DIR = ../build
MOC_DIR = ../build
RCC_DIR = ../build
UI_DIR = ../build
                 

QMAKE_CFLAGS_RELEASE -= -O2
QMAKE_CFLAGS_RELEASE += -O3

QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

                 
QMAKE_CXXFLAGS += -std=c++11

QMAKE_LFLAGS_RELEASE -= -O1
QMAKE_LFLAGS_RELEASE += -O3

QMAKE_CXXFLAGS_DEBUG += -pg
QMAKE_LFLAGS_DEBUG += -pg
              
LIBS += -lCCfits -lcfitsio
INCLUDEPATH += "/usr/include/CCfits"

INCLUDEPATH += "FitsPhoto"
INCLUDEPATH += "QFitsWidgets"
INCLUDEPATH += "AstroAnalyzer"
