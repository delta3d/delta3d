
SET(APP_NAME AnimationViewer)

INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR}
                                            ${CAL3D_INCLUDE_DIR} # for dtAnim
                                            ${CMAKE_CURRENT_BINARY_DIR} # for ui*.h files
                                           )

SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/utilities/${APP_NAME})
SET(PROG_HEADERS ${SOURCE_PATH}/AnimationTableWidget.h
                 ${SOURCE_PATH}/AnimationSliderPanel.h
                 ${SOURCE_PATH}/AttachmentPanel.h
                 ${SOURCE_PATH}/MainWindow.h
                 ${SOURCE_PATH}/NewCharacterDialog.h
                 ${SOURCE_PATH}/PoseMeshItem.h
                 ${SOURCE_PATH}/PoseMeshProperties.h
                 ${SOURCE_PATH}/PoseMeshScene.h
                 ${SOURCE_PATH}/PoseMeshView.h
                 ${SOURCE_PATH}/ResourceDialogs.h
                 ${SOURCE_PATH}/ResourceDelegates.h
                 ${SOURCE_PATH}/Viewer.h  )

SET(PROG_SOURCES ${SOURCE_PATH}/AnimationTableWidget.cpp
                 ${SOURCE_PATH}/AnimationSliderPanel.cpp
                 ${SOURCE_PATH}/AttachmentPanel.cpp
                 ${SOURCE_PATH}/main.cpp
                 ${SOURCE_PATH}/MainWindow.cpp
                 ${SOURCE_PATH}/NewCharacterDialog.cpp
                 ${SOURCE_PATH}/PoseMeshItem.cpp
                 ${SOURCE_PATH}/PoseMeshProperties.cpp
                 ${SOURCE_PATH}/PoseMeshScene.cpp
                 ${SOURCE_PATH}/PoseMeshView.cpp
                 ${SOURCE_PATH}/ResourceDialogs.cpp
                 ${SOURCE_PATH}/ResourceDelegates.cpp
                 ${SOURCE_PATH}/Viewer.cpp
                 ${SOURCE_PATH}/AnimationViewer.rc )


QT4_ADD_RESOURCES(RCC_SOURCES AnimationViewerResources.qrc)
qt4_wrap_cpp(MOC_SOURCES ${PROG_HEADERS})
qt4_wrap_ui(UI_SOURCES AnimationSliderPanel.ui
                       AttachmentPanel.ui
                       NewCharacterDialog.ui
                       PoseMeshProperties.ui
                       ResourceAddDialog.ui)

SOURCE_GROUP("Generated Files" REGULAR_EXPRESSION qrc_.*cxx|moc_.*cxx)

SET(APP_EXTERNAL_DEPS
                    QT_QTCORE_LIBRARY
                    QT_QTGUI_LIBRARY
                    QT_QTOPENGL_LIBRARY
                    OSGFX_LIBRARY
                    AGL_LIBRARY
)

SET(APP_DEPS
                       ${DTUTIL_LIBRARY}
                       ${DTCORE_LIBRARY}
                       ${DTANIM_LIBRARY}
                       ${DTABC_LIBRARY}
                       ${DTQT_LIBRARY}
)

DELTA3D_ADD_PROGRAM(${APP_NAME} MACOSX_BUNDLE DT_ANIMATION_VIEWER)

IF (MSVC)
  SET_TARGET_PROPERTIES(${APP_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
ENDIF (MSVC)
