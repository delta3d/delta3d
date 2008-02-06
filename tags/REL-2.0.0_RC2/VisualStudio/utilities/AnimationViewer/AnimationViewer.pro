CONFIG += qt \
          debug_and_release \
          console 	

TEMPLATE = vcapp

QT += opengl

CONFIG(debug, debug|release) {
  LIBS += dtABCD.lib dtUtilD.lib dtCoreD.lib dtAnimD.lib dtGUID.lib osgD.lib osgGAD.lib osgViewerD.lib cal3d_d.lib xerces-c_2D.lib CEGUIBase_d.lib
  TARGET = AnimationViewerD
} else {
   LIBS += dtABC.lib dtUtil.lib dtCore.lib dtAnim.lib dtGUI.lib osg.lib osgGA.lib osgViewer.lib cal3d.lib xerces-c_2.lib CEGUIBase.lib
 TARGET = AnimationViewer
}

DEFINES -= UNICODE
DESTDIR = ../../../bin

#treat wchar_t as built in type, fixes linker issue with Xerces
QMAKE_CXXFLAGS += /Zc:wchar_t

LIBS += -L"../../../lib;../../../ext/lib"

SOURCES = Delta3DThread.cpp \
	      main.cpp \
	      MainWindow.cpp \
	      Viewer.cpp \
	      AnimationTableWidget.cpp \
          TrackView.cpp \
          TrackScene.cpp \
          TrackItem.cpp \ 
		  OSGAdapterWidget.cpp		  
	    	    
HEADERS = Delta3DThread.h \
	      MainWindow.h \
          Viewer.h \
          AnimationTableWidget.h \
          TrackView.h \
          TrackScene.h \
          TrackItem.h \
		  OSGAdapterWidget.h 

INCLUDEPATH +=  "../../../inc" "../../../ext/inc" "../../../ext/inc/CEGUI"

VPATH = ../../../utilities/AnimationViewer/

RESOURCES = AnimationViewerResources.qrc


