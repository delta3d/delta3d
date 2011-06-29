
SET(APP_NAME AnimationViewer)

INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR}
                                            ${CAL3D_INCLUDE_DIR} # for dtAnim
                                            ${CMAKE_CURRENT_BINARY_DIR} # for ui*.h files
                                           )

SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/utilities/${APP_NAME})
SET(PROG_HEADERS ${SOURCE_PATH}/AnimationTableWidget.h
                 ${SOURCE_PATH}/MainWindow.h
                 ${SOURCE_PATH}/PoseMeshItem.h
                 ${SOURCE_PATH}/PoseMeshProperties.h
                 ${SOURCE_PATH}/PoseMeshScene.h
                 ${SOURCE_PATH}/PoseMeshView.h
                 ${SOURCE_PATH}/ResourceDelegates.h
                 ${SOURCE_PATH}/Viewer.h  )
                 
SET(PROG_SOURCES ${SOURCE_PATH}/AnimationTableWidget.cpp
                 ${SOURCE_PATH}/main.cpp
                 ${SOURCE_PATH}/MainWindow.cpp
                 ${SOURCE_PATH}/PoseMeshItem.cpp
                 ${SOURCE_PATH}/PoseMeshProperties.cpp
                 ${SOURCE_PATH}/PoseMeshScene.cpp
                 ${SOURCE_PATH}/PoseMeshView.cpp
                 ${SOURCE_PATH}/ResourceDelegates.cpp
                 ${SOURCE_PATH}/Viewer.cpp
                 ${SOURCE_PATH}/AnimationViewer.rc )


QT4_ADD_RESOURCES(RCC_SOURCES AnimationViewerResources.qrc)
qt4_wrap_cpp(MOC_SOURCES ${PROG_HEADERS})
qt4_wrap_ui(UI_SOURCES PoseMeshProperties.ui)

ADD_EXECUTABLE(${APP_NAME}
    ${PROG_HEADERS}
    ${PROG_SOURCES}
    ${MOC_SOURCES}
    ${RCC_SOURCES}
    ${UI_SOURCES}
)

LINK_WITH_VARIABLES( ${APP_NAME}
                     QT_QTCORE_LIBRARY
                     QT_QTGUI_LIBRARY
                     QT_QTOPENGL_LIBRARY
                     OSGFX_LIBRARY
                     AGL_LIBRARY
                    )
                     
TARGET_LINK_LIBRARIES( ${APP_NAME}
                       ${DTUTIL_LIBRARY}
                       ${DTCORE_LIBRARY}
                       ${DTANIM_LIBRARY}
                       ${DTABC_LIBRARY}
                       ${DTQT_LIBRARY}
                     )           



#Not Sure if this is needed
#IF (WIN32)
#TARGET_LINK_LIBRARIES(${APP_NAME} User32 Gdi32 shell32 Advapi32 ole32 )
#ENDIF (WIN32)

INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC_IDE)
  SET_TARGET_PROPERTIES(${APP_NAME} PROPERTIES PREFIX "../../bin/")
  SET_TARGET_PROPERTIES(${APP_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC_IDE)

IF (MSVC)
  SET_TARGET_PROPERTIES(${APP_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
ENDIF (MSVC)