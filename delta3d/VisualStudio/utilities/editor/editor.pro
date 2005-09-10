# Qt project file for the Delta3D level editor.

TEMPLATE = app
CONFIG += qt debug thread
TARGET = Delta3DLevelEditor

SRC_DIR = ../../../utilities/editor/src
INC_DIR = ../../../utilities/editor/inc/dtEditQt

SRC_GLOB = $$system(python doglob.py $$quote('$$SRC_DIR/*.cpp'))
INC_GLOB = $$system(python doglob.py $$quote('$$INC_DIR/*.h'))

win32 {
   MOC_DIR = $$quote($$SRC_DIR/moc)
   DESTDIR = ../../../bin
}

HEADERS += $$INC_GLOB
SOURCES += $$SRC_GLOB

INCLUDEPATH = ../../../inc
INCLUDEPATH += ../../../utilities/editor/inc
