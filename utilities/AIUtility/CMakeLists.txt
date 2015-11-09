
SET(APP_NAME AIUtility)

INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR}
                                  ${CAL3D_INCLUDE_DIR}#for dtAnim
                                  ${CMAKE_CURRENT_BINARY_DIR}
                                  )

SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/utilities/${APP_NAME})

SET(PROG_HEADERS
                aicomponent.h
                aipropertyeditor.h
                aiutilityapp.h
                aiutilityinputcomponent.h
                aiutilitypreferencesdialog.h
                mainwindow.h
                objecttypetreewidget.h
                qtglframe.h
                undocommands.h
                waypointbrowser.h
                waypointbrowsertreeitem.h
                waypointmotionmodel.h
                waypointselection.h
)

SET(PROG_SOURCES
                aicomponent.cpp
                aipropertyeditor.cpp
                aiutilityapp.cpp
                aiutilityinputcomponent.cpp
                aiutilitypreferencesdialog.cpp
                main.cpp
                mainwindow.cpp
                objectypettreewidget.cpp
                qtglframe.cpp
                undocommands.cpp
                waypointbrowser.cpp
                waypointbrowsertreeitem.cpp
                waypointmotionmodel.cpp
                waypointselection.cpp
                AIUtility.rc
                AIUtilityResources.qrc
)


QT4_ADD_RESOURCES(RCC_SOURCES ${APP_NAME}Resources.qrc)
qt4_wrap_cpp(MOC_SOURCES ${PROG_HEADERS})
qt4_wrap_ui(UI_SOURCES mainwindow.ui waypointbrowser.ui)


SOURCE_GROUP("Generated Files" REGULAR_EXPRESSION qrc_.*cxx|moc_.*cxx)

SET(APP_EXTERNAL_DEPS
                     QT_QTCORE_LIBRARY
                     QT_QTGUI_LIBRARY
                     QT_QTOPENGL_LIBRARY
)

SET(APP_DEPS
                       ${DTUTIL_LIBRARY}
                       ${DTCORE_LIBRARY}
                       ${DTANIM_LIBRARY}
                       ${DTQT_LIBRARY}
                       ${DTAI_LIBRARY}
                       ${DTGAME_LIBRARY}
                       )

DELTA3D_ADD_PROGRAM(${APP_NAME} MACOSX_BUNDLE DT_AI_UTILITY)


IF (MSVC)
  SET_TARGET_PROPERTIES(${APP_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
ENDIF (MSVC)