CONFIG += qt \
          debug_and_release \
          thread \
          console 	

TEMPLATE = vcapp

QT += opengl

CONFIG(debug, debug|release) {
  LIBS += dtABCd.lib dtUtild.lib dtCored.lib dtAnimd.lib dtGUId.lib dtDALd.lib osgd.lib osgGAd.lib osgViewerd.lib cal3d_d.lib xerces-c_2D.lib CEGUIBase_d.lib
  TARGET = ObjectViewerD
} else {
   LIBS += dtABC.lib dtUtil.lib dtCore.lib dtAnim.lib dtGUI.lib dtDAL.lib osg.lib osgGA.lib osgViewer.lib cal3d.lib xerces-c_2.lib CEGUIBase.lib
 TARGET = ObjectViewer
}

DEFINES -= UNICODE

#treat wchar_t as built in type, fixes linker issue with Xerces
QMAKE_CXXFLAGS += /Zc:wchar_t

LIBS += -L$(DELTA_LIB) 

SOURCES = Delta3DThread.cpp \
	      main.cpp \
	      ObjectWorkspace.cpp \
	      ObjectViewer.cpp \	
		  OSGAdapterWidget.cpp \
          DialogProjectContext.cpp \ 
          ResourceDock.cpp 
	    	    
HEADERS = Delta3DThread.h \
	      ObjectWorkspace.h \
          ObjectViewer.h \    
		  OSGAdapterWidget.h \
          DialogProjectContext.h \
          ResourceDock.h 
          
INCLUDEPATH += $$(DELTA_ROOT)/inc
INCLUDEPATH += $$(DELTA_ROOT)/ext/inc
INCLUDEPATH += $$(DELTA_ROOT)/ext/inc/CEGUI

RESOURCES = ObjectViewerResources.qrc


