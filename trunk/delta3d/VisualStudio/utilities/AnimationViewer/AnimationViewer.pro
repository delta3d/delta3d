CONFIG += qt \
          debug_and_release \
          console

TEMPLATE = vcapp

CONFIG(debug, debug|release) {
  LIBS += dtABCD.lib dtUtilD.lib dtCoreD.lib dtCharD.lib osgD.lib
  TARGET = AnimationViewerD
} else {
   LIBS += dtABC.lib dtUtil.lib dtCore.lib dtChar.lib osg.lib
 TARGET = AnimationViewer
}

DEFINES -= UNICODE
DESTDIR = ../../../bin

LIBS += -L"$$(DELTA_LIB)"

SOURCES = Delta3DThread.cpp \
	    main.cpp \
	    MainWindow.cpp \
	    Viewer.cpp
	    
	    
HEADERS = Delta3DThread.h \
	    MainWindow.h \
                    Viewer.h

INCLUDEPATH +=  "$$(DELTA_INC)"

VPATH = ../../../utilities/AnimationViewer/
