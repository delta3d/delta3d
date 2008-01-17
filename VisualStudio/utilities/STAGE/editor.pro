# Qt project file for the Delta3D level editor.

TEMPLATE = app
CONFIG += qt debug thread
TARGET = Delta3DLevelEditor

SRC_DIR = ../../../utilities/STAGE/src
INC_DIR = ../../../utilities/STAGE/inc/dtEditQt

SRC_GLOB = $$SRC_DIR/actorbrowser.cpp $$SRC_DIR/actorglobalbrowser.cpp $$SRC_DIR/actorresultstable.cpp $$SRC_DIR/actorsearcher.cpp \
   $$SRC_DIR/actortab.cpp $$SRC_DIR/actortypetreewidget.cpp $$SRC_DIR/camera.cpp $$SRC_DIR/dialoglistselection.cpp \
   $$SRC_DIR/dialogmapproperties.cpp $$SRC_DIR/dynamicabstractcontrol.cpp $$SRC_DIR/dynamicabstractparentcontrol.cpp \
   $$SRC_DIR/dynamicactorcontrol.cpp $$SRC_DIR/dynamicboolcontrol.cpp $$SRC_DIR/dynamiccolorelementcontrol.cpp \
   $$SRC_DIR/dynamiccolorrgbacontrol.cpp $$SRC_DIR/dynamicdoublecontrol.cpp $$SRC_DIR/dynamicenumcontrol.cpp \
   $$SRC_DIR/dynamicfloatcontrol.cpp $$SRC_DIR/dynamicgameeventcontrol.cpp $$SRC_DIR/dynamicgroupcontrol.cpp \
   $$SRC_DIR/dynamicgrouppropertycontrol.cpp $$SRC_DIR/dynamicintcontrol.cpp $$SRC_DIR/dynamiclabelcontrol.cpp \
   $$SRC_DIR/dynamiclongcontrol.cpp $$SRC_DIR/dynamicnamecontrol.cpp $$SRC_DIR/dynamicresourcecontrol.cpp \
   $$SRC_DIR/dynamicstringcontrol.cpp $$SRC_DIR/dynamicvec2control.cpp $$SRC_DIR/dynamicvec3control.cpp \
   $$SRC_DIR/dynamicvec4control.cpp $$SRC_DIR/dynamicvectorelementcontrol.cpp $$SRC_DIR/editoraboutbox.cpp \
   $$SRC_DIR/editoractions.cpp $$SRC_DIR/editordata.cpp $$SRC_DIR/editorevents.cpp $$SRC_DIR/editorsettings.cpp $$SRC_DIR/gameeventdialog.cpp \
   $$SRC_DIR/gameeventsdialog.cpp $$SRC_DIR/groupuiregistry.cpp $$SRC_DIR/libraryeditor.cpp $$SRC_DIR/librarypathseditor.cpp $$SRC_DIR/main.cpp \
   $$SRC_DIR/mainwindow.cpp $$SRC_DIR/mapdialog.cpp $$SRC_DIR/mapsaveasdialog.cpp $$SRC_DIR/orthoviewport.cpp $$SRC_DIR/particlebrowser.cpp \
   $$SRC_DIR/perspectiveviewport.cpp $$SRC_DIR/precomp.cpp $$SRC_DIR/preferencesdialog.cpp $$SRC_DIR/projectbrowser.cpp \
   $$SRC_DIR/projectcontextdialog.cpp $$SRC_DIR/propertyeditor.cpp $$SRC_DIR/propertyeditordelegate.cpp \
   $$SRC_DIR/propertyeditormodel.cpp $$SRC_DIR/propertyeditortreeview.cpp $$SRC_DIR/resourceabstractbrowser.cpp \
   $$SRC_DIR/resourcebrowser.cpp $$SRC_DIR/resourceimportdialog.cpp $$SRC_DIR/resourcetreewidget.cpp $$SRC_DIR/skeletalmeshbrowser.cpp \
   $$SRC_DIR/soundbrowser.cpp $$SRC_DIR/staticmeshbrowser.cpp $$SRC_DIR/tabcontainer.cpp $$SRC_DIR/tabwrapper.cpp $$SRC_DIR/taskeditor.cpp \
   $$SRC_DIR/taskuiplugin.cpp $$SRC_DIR/terrainbrowser.cpp $$SRC_DIR/texturebrowser.cpp $$SRC_DIR/uiresources.cpp $$SRC_DIR/undomanager.cpp \
   $$SRC_DIR/viewport.cpp $$SRC_DIR/viewportcontainer.cpp $$SRC_DIR/viewportmanager.cpp $$SRC_DIR/viewportoverlay.cpp

INC_GLOB = $$INC_DIR/actorbrowser.h $$INC_DIR/actorglobalbrowser.h $$INC_DIR/actorresultstable.h $$INC_DIR/actorsearcher.h $$INC_DIR/actortab.h \
   $$INC_DIR/actortypetreewidget.h $$INC_DIR/camera.h $$INC_DIR/dialoglistselection.h $$INC_DIR/dialogmapproperties.h \
   $$INC_DIR/dynamicabstractcontrol.h $$INC_DIR/dynamicabstractparentcontrol.h $$INC_DIR/dynamicactorcontrol.h \
   $$INC_DIR/dynamicboolcontrol.h $$INC_DIR/dynamiccolorelementcontrol.h $$INC_DIR/dynamiccolorrgbacontrol.h \
   $$INC_DIR/dynamicdoublecontrol.h $$INC_DIR/dynamicenumcontrol.h $$INC_DIR/dynamicfloatcontrol.h $$INC_DIR/dynamicgameeventcontrol.h \
   $$INC_DIR/dynamicgroupcontrol.h $$INC_DIR/dynamicgrouppropertycontrol.h $$INC_DIR/dynamicintcontrol.h $$INC_DIR/dynamiclabelcontrol.h \
   $$INC_DIR/dynamiclongcontrol.h $$INC_DIR/dynamicnamecontrol.h $$INC_DIR/dynamicresourcecontrol.h $$INC_DIR/dynamicstringcontrol.h \
   $$INC_DIR/dynamicsubwidgets.h $$INC_DIR/dynamicvec2control.h $$INC_DIR/dynamicvec3control.h $$INC_DIR/dynamicvec4control.h \
   $$INC_DIR/dynamicvectorelementcontrol.h $$INC_DIR/editoraboutbox.h $$INC_DIR/editoractions.h $$INC_DIR/editordata.h $$INC_DIR/editorevents.h \
   $$INC_DIR/editorsettings.h $$INC_DIR/gameeventdialog.h $$INC_DIR/gameeventsdialog.h $$INC_DIR/groupuiplugin.h $$INC_DIR/groupuiregistry.h \
   $$INC_DIR/libraryeditor.h $$INC_DIR/librarypathseditor.h $$INC_DIR/mainwindow.h $$INC_DIR/mapdialog.h $$INC_DIR/mapsaveasdialog.h $$INC_DIR/orthoviewport.h \
   $$INC_DIR/particlebrowser.h $$INC_DIR/perspectiveviewport.h $$INC_DIR/preferencesdialog.h $$INC_DIR/projectbrowser.h \
   $$INC_DIR/projectcontextdialog.h $$INC_DIR/propertyeditor.h $$INC_DIR/propertyeditordelegate.h $$INC_DIR/propertyeditormodel.h \
   $$INC_DIR/propertyeditortreeview.h $$INC_DIR/resourceabstractbrowser.h $$INC_DIR/resourcebrowser.h $$INC_DIR/resourceimportdialog.h \
   $$INC_DIR/resourcetreewidget.h $$INC_DIR/skeletalmeshbrowser.h $$INC_DIR/soundbrowser.h $$INC_DIR/staticmeshbrowser.h $$INC_DIR/tabcontainer.h \
   $$INC_DIR/tabwrapper.h $$INC_DIR/taskeditor.h $$INC_DIR/taskuiplugin.h $$INC_DIR/terrainbrowser.h $$INC_DIR/texturebrowser.h $$INC_DIR/typedefs.h \
   $$INC_DIR/uiresources.h $$INC_DIR/undomanager.h $$INC_DIR/viewport.h $$INC_DIR/viewportcontainer.h $$INC_DIR/viewportmanager.h $$INC_DIR/viewportoverlay.h

win32 {
   MOC_DIR = $$quote($$SRC_DIR/moc)
   DESTDIR = ../../../bin
}

HEADERS += $$INC_GLOB
SOURCES += $$SRC_GLOB

INCLUDEPATH = ../../../inc
INCLUDEPATH += ../../../utilities/STAGE/inc


