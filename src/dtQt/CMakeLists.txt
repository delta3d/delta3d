SET(LIB_NAME     dtQt)


SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/src/${LIB_NAME})
file(GLOB LIB_PUBLIC_HEADERS "${HEADER_PATH}/*.h")

# These files are listed in alphabetical order.
set(LIB_SOURCES 

actortreepanel.cpp
actortreewidget.cpp
baselibrarylisteditor.cpp
basepropertycontrol.cpp
basepropertyeditor.cpp
clickablelabelcontrol.cpp
constants.cpp
deltastepper.cpp
docbrowser.cpp
docbrowserxmlreader.cpp
dialoglistselection.cpp
dynamicabstractcontrol.cpp
dynamicabstractparentcontrol.cpp
dynamicactorcontrol.cpp
dynamicarraycontrol.cpp
dynamicbitmaskcontrol.cpp
dynamicboolcontrol.cpp
dynamicbyteelementcontrol.cpp
dynamiccolorrgbacontrol.cpp
dynamiccolorelementcontrol.cpp
dynamiccontainercontrol.cpp
dynamiccontainerselectorcontrol.cpp
dynamicenumcontrol.cpp
dynamicgameeventcontrol.cpp
dynamicgroupcontrol.cpp
dynamiclabelcontrol.cpp
dynamicpropertycontainercontrol.cpp
dynamicresourcecontrol.cpp
dynamicstringcontrol.cpp
dynamicvectorelementcontrol.cpp
librarypathseditor.cpp
nodegraphview.cpp
nodetreepanel.cpp
objecttypelistpanel.cpp
objecttypeselectdialog.cpp
osgadapterwidget.cpp
osggraphicswindowqt.cpp
osgoptimizerpanel.cpp
osgwriteroptionsdialog.cpp
osgwriteroptionspanel.cpp
projectcontextdialog.cpp
propertycontrolarray.cpp
propertycontrolbool.cpp
propertycontrolbitmask.cpp
propertycontrolcolor.cpp
propertycontroldouble.cpp
propertycontrolfactory.cpp
propertycontrolenum.cpp
propertycontrolimage.cpp
propertycontrolint.cpp
propertycontrolrange.cpp
propertycontrolresource.cpp
propertycontrolstring.cpp
propertycontrolvec4.cpp
propertyeditordelegate.cpp
propertyeditormodel.cpp
propertyeditortreeview.cpp
propertypanel.cpp
propertypanelbuilder.cpp
resourceselectorwidget.cpp
qtguiwindowsystemwrapper.cpp
viewwindow.cpp

)

INCLUDE_DIRECTORIES(${QT_INCLUDE_DIRS}
					${QT_INCLUDE_DIRS}
                    ${CMAKE_CURRENT_BINARY_DIR} # for ui*.h files
                   )
				   
qt5_wrap_ui(UI_SOURCES actortreepanel.ui
                       basedialog.ui
                       basepropertycontrol.ui
					   nodegraphviewer.ui
                       nodetreepanel.ui
                       objecttypelistpanel.ui
					   osgoptimizerpanel.ui
					   osgwriteroptionsdialog.ui
					   osgwriteroptionspanel.ui
                       propertycontrolarray.ui
                       propertycontrolbool.ui
                       propertycontrolbitmask.ui
                       propertycontrolcolor.ui
                       propertycontroldouble.ui
                       propertycontrolenum.ui
                       propertycontrolimage.ui
                       propertycontrolint.ui
                       propertycontrolrange.ui
                       propertycontrolresource.ui
                       propertycontrolstring.ui
                       propertycontrolvec4.ui
                       propertypanel.ui)
                       
QT5_WRAP_CPP(MOC_SOURCES ${LIB_PUBLIC_HEADERS})

QT5_ADD_RESOURCES(RCC_SOURCES
                  dtqt.qrc
)

ADD_PRECOMPILED_HEADER(${LIB_NAME} prefix/dtqtprefix.h prefix/dtqtprefix.cpp LIB_SOURCES)

SET(LIB_SOURCES 
    ${LIB_SOURCES}
    ${MOC_SOURCES}
    ${RCC_SOURCES}
    ${UI_SOURCES}
    )

set(LIB_DEPS
  ${DTCORE_LIBRARY}
  ${DTGAME_LIBRARY}
  ${QT_LIBRARIES}
)

DELTA3D_ADD_LIBRARY(${LIB_NAME} DT_QT_LIBRARY)

if (X11_FOUND)
   TARGET_LINK_LIBRARIES( ${LIB_NAME}
               ${X11_LIBRARIES})
endif (X11_FOUND)





