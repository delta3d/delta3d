
SET(APP_NAME ObjectViewer)

INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR}
                                  ${CAL3D_INCLUDE_DIR}#for dtAnim
                                  )

SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/utilities/${APP_NAME})

SET(PROG_HEADERS ${SOURCE_PATH}/GeometryTree.h                 
                 ${SOURCE_PATH}/ObjectViewer.h
                 ${SOURCE_PATH}/ObjectViewerData.h
                 ${SOURCE_PATH}/ObjectWorkspace.h
                 ${SOURCE_PATH}/ResourceDock.h
                 ${SOURCE_PATH}/ShaderTree.h
                 ${SOURCE_PATH}/TextEdit.h)
                 
SET(PROG_SOURCES ${SOURCE_PATH}/GeometryTree.cpp
                 ${SOURCE_PATH}/main.cpp
                 ${SOURCE_PATH}/ObjectViewer.cpp
                 ${SOURCE_PATH}/ObjectWorkspace.cpp
                 ${SOURCE_PATH}/ResourceDock.cpp
                 ${SOURCE_PATH}/ShaderTree.cpp
                 ${SOURCE_PATH}/TextEdit.cpp
                 ${SOURCE_PATH}/ObjectViewer.rc)
                 

QT4_ADD_RESOURCES(RCC_SOURCES ObjectViewerResources.qrc)
qt4_wrap_cpp(MOC_SOURCES ${PROG_HEADERS})

ADD_EXECUTABLE(${APP_NAME}
    ${PROG_HEADERS}
    ${PROG_SOURCES}
    ${MOC_SOURCES}
    ${RCC_SOURCES}
)

LINK_WITH_VARIABLES(${APP_NAME}
                     QT_QTCORE_LIBRARY
                     QT_QTGUI_LIBRARY
                     QT_QTOPENGL_LIBRARY
                    )

TARGET_LINK_LIBRARIES( ${APP_NAME}
                       ${DTUTIL_LIBRARY}
                       ${DTCORE_LIBRARY}
                       ${DTANIM_LIBRARY}
                       ${DTQT_LIBRARY}
                     )
  

INCLUDE(ModuleInstall OPTIONAL)


IF (MSVC)
  SET_TARGET_PROPERTIES(${APP_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
ENDIF (MSVC)
