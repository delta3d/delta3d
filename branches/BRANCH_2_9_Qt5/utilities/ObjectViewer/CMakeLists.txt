
SET(APP_NAME ObjectViewer)

FIND_PACKAGE(Qt5PrintSupport)


INCLUDE_DIRECTORIES(${QT_INCLUDE_DIRS}
					${QT_INCLUDE_DIRS}
					${Qt5PrintSupport_INCLUDE_DIR}
                    ${CAL3D_INCLUDE_DIR}#for dtAnim
                    ${CMAKE_CURRENT_BINARY_DIR} # for ui*.h files
)

SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/utilities/${APP_NAME})

SET(PROG_HEADERS ${SOURCE_PATH}/AnimationControlDock.h
				 ${SOURCE_PATH}/GeometryTree.h
                 ${SOURCE_PATH}/MotionModelToolbar.h
                 ${SOURCE_PATH}/NodeHighlighter.h
                 ${SOURCE_PATH}/ObjectViewer.h
                 ${SOURCE_PATH}/ObjectViewerData.h
                 ${SOURCE_PATH}/ObjectWorkspace.h
                 ${SOURCE_PATH}/ResourceDock.h
                 ${SOURCE_PATH}/ShaderTree.h
                 ${SOURCE_PATH}/TextEdit.h
                 ${SOURCE_PATH}/TextureVisitor.h
                 ${SOURCE_PATH}/Typedefs.h)

SET(PROG_SOURCES ${SOURCE_PATH}/main.cpp
                 ${SOURCE_PATH}/AnimationControlDock.cpp
				 ${SOURCE_PATH}/GeometryTree.cpp
                 ${SOURCE_PATH}/MotionModelToolbar.cpp
                 ${SOURCE_PATH}/NodeHighlighter.cpp
                 ${SOURCE_PATH}/ObjectViewer.cpp
                 ${SOURCE_PATH}/ObjectWorkspace.cpp
                 ${SOURCE_PATH}/ResourceDock.cpp
                 ${SOURCE_PATH}/ShaderTree.cpp
                 ${SOURCE_PATH}/TextEdit.cpp
                 ${SOURCE_PATH}/TextureVisitor.cpp
                 ${SOURCE_PATH}/ObjectViewer.rc)


QT5_ADD_RESOURCES(RCC_SOURCES ObjectViewerResources.qrc)
qt5_wrap_cpp(MOC_SOURCES ${PROG_HEADERS})
qt5_wrap_ui(UI_SOURCES AnimationControlPanel.ui
      MotionModelToolbar.ui)

SOURCE_GROUP("Generated Files" REGULAR_EXPRESSION qrc_.*cxx|moc_.*cxx)

SET(APP_EXTERNAL_DEPS
                     QT_QTCORE_LIBRARY
                     QT_QTGUI_LIBRARY
                     QT_QTOPENGL_LIBRARY
                     OSGANIMATION_LIBRARY
)

SET(APP_DEPS
                       ${DTUTIL_LIBRARY}
                       ${DTCORE_LIBRARY}
                       ${DTANIM_LIBRARY}
                       ${DTQT_LIBRARY}
                       ${QT_LIBRARIES}
                       ${Qt5PrintSupport_LIBRARIES}
)

DELTA3D_ADD_PROGRAM(${APP_NAME} MACOSX_BUNDLE DT_OBJECT_VIEWER)

IF (MSVC)
  SET_TARGET_PROPERTIES(${APP_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
ENDIF (MSVC)
