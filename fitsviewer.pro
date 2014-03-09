 HEADERS       = fitsviewer.h QFitsStretchDock.h QFitsZoomDock.h QFitsWindow.h
 SOURCES       = fitsviewer.cxx \
                 main.cxx \
                 QFitsWindow.cxx \
                 FitsPhoto.cxx \
                 FitsToImage.cxx \
                 QFitsStretchDock.cxx \
                 QFitsListWidgetItem.cxx \
                 QFitsZoomDock.cxx
                 
LIBS += -L"/usr/include/CCfits" -lCCfits -lcfitsio
INCLUDEPATH += -I"/usr/include/CCfits"
                 

 # install
 target.path = $$[QT_INSTALL_EXAMPLES]/widgets/imageviewer/
 sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS imageviewer.pro
 sources.path = $$[QT_INSTALL_EXAMPLES]/widgets/imageviewer
 INSTALLS += target sources

 symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)

 #Symbian has built-in component named imageviewer so we use different target
 symbian: TARGET = imageviewerexample

 wince*: {
    DEPLOYMENT_PLUGIN += qjpeg qmng qgif
 }
 maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)

 symbian: warning(This example might not fully work on Symbian platform)
 maemo5: warning(This example might not fully work on Maemo platform)
 simulator: warning(This example might not fully work on Simulator platform)
