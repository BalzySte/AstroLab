 HEADERS       = fitsviewer.h QFitsStretchDock.h QFitsZoomDock.h QFitsWindow.h
 SOURCES       = fitsviewer.cxx \
				 main.cxx \
                 QFitsWindow.cxx \
                 FitsPhoto.cxx \
                 FitsToImage.cxx \
                 QFitsStretchDock.cxx \
                 QFitsListWidgetItem.cxx \
                 QFitsZoomDock.cxx \
                 astroAnalizer.cxx
                 
QMAKE_CXXFLAGS += -std=c++11

QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3
QMAKE_LFLAGS_RELEASE -= -O1
QMAKE_LFLAGS_RELEASE += -O3

QMAKE_CXXFLAGS_DEBUG += -pg
QMAKE_LFLAGS_DEBUG += -pg
              
LIBS += -lCCfits -lcfitsio
INCLUDEPATH += "/usr/include/CCfits"
				

# install
target.path = ./install/
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS imageviewer.pro
sources.path = ./install/sources/
INSTALLS += target sources