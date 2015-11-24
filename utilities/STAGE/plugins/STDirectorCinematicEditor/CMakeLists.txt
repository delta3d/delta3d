
SET(LIB_NAME     STDirectorCinematicEditor)

file(GLOB LIB_PUBLIC_HEADERS "*.h")
file(GLOB LIB_C_SOURCES "*.cpp")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/utilities/STAGE/inc
                    ${CAL3D_INCLUDE_DIR}
                    ${QT_INCLUDE_DIRS}
                    ${QT_INCLUDE_DIRS}
                    ${CMAKE_CURRENT_BINARY_DIR} # for ui*.h files
                    ${DELTA3D_SOURCE_DIR}/ui/dtDirectorQt # for director editor ui*.h files
)

qt5_wrap_cpp(MOC_SOURCES ${LIB_PUBLIC_HEADERS})
QT5_WRAP_UI(UI_SOURCES cinematiceditor.ui)

SET(LIB_SOURCES
   ${LIB_C_SOURCES}
   ${UI_SOURCES}
   ${RCC_SOURCES}
   ${MOC_SOURCES}
)

SET(LIB_DEPS
                       STAGE
                       ${DTUTIL_LIBRARY}
                       ${DTCORE_LIBRARY}
                       ${DTGAME_LIBRARY}
                       ${DTABC_LIBRARY}
                       ${DTANIM_LIBRARY}
                       ${DTDIRECTOR_LIBRARY}
                       ${DTDIRECTORQT_LIBRARY}
                       dtDirectorAnimNodes
                       ${QT_QTCORE_LIBRARY}
                       ${QT_QTGUI_LIBRARY}
                       ${QT_QTOPENGL_LIBRARY}
                       ${Qt5Core_LIBRARIES}
                       ${Qt5Gui_LIBRARIES}
                       ${Qt5OpenGL_LIBRARIES}
                       ${Qt5Widgets_LIBRARIES}
                      )

#put the binary into a "STAGE plugins" folder
SETUP_PLUGIN_WITH_OUTPUT_DIRS(${LIB_NAME} DT_DIRECTOR_CINEMATIC_EDITOR_PLUGIN stplugins)
