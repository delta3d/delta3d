CONFIG += qt \
          debug_and_release \
          console

TEMPLATE = vcapp

CONFIG(debug, debug|release) {
  LIBS += dtABCD.lib dtUtilD.lib dtCoreD.lib dtAnimD.lib osgD.lib cal3d_d.lib xerces-c_2D.lib
  TARGET = AnimationViewerD
} else {
   LIBS += dtABC.lib dtUtil.lib dtCore.lib dtAnim.lib osg.lib cal3d.lib xerces-c_2.lib
 TARGET = AnimationViewer
}

DEFINES -= UNICODE
DESTDIR = ../../../bin

LIBS += -L"$$(DELTA_LIB)"

SOURCES = Delta3DThread.cpp \
	    main.cpp \
	    MainWindow.cpp \
	    Viewer.cpp \
	    AnimationTableWidget.cpp \
          TrackView.cpp \
          TrackScene.cpp \
          TrackItem.cpp
	    
	    
HEADERS = Delta3DThread.h \
	    MainWindow.h \
          Viewer.h \
          AnimationTableWidget.h \
          TrackView.h \
          TrackScene.h \
          TrackItem.h

INCLUDEPATH +=  "$$(DELTA_INC)"

VPATH = ../../../utilities/AnimationViewer/

RESOURCES = AnimationViewerResources.qrc
