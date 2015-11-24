SET(LIB_NAME STMapHierarchyEditor)

file(GLOB LIB_PUBLIC_HEADERS "*.h") 
file(GLOB LIB_C_SOURCES "*.cpp")

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/utilities/STAGE/inc
              ${CMAKE_CURRENT_SOURCE_DIR}
              ${QT_INCLUDE_DIRS}
              ${QT_INCLUDE_DIRS}
              ${CMAKE_CURRENT_BINARY_DIR} # for ui*.h files
)

qt5_wrap_cpp(MOC_SOURCES ${LIB_PUBLIC_HEADERS})
QT5_WRAP_UI(UI_SOURCES buttonconnector.ui)
QT5_WRAP_UI(UI_SOURCES maphierarchyeditor.ui)
QT5_WRAP_UI(UI_SOURCES nodebutton.ui)
QT5_WRAP_UI(UI_SOURCES topbottomconnectornogap.ui)

SET(LIB_SOURCES
   ${LIB_C_SOURCES}
   ${UI_SOURCES}
   ${MOC_SOURCES}
)

SET(LIB_DEPS
                       STAGE
                       ${DTUTIL_LIBRARY}
                       ${DTCORE_LIBRARY}
                       ${DTGAME_LIBRARY}
                       ${DTABC_LIBRARY}
                       ${DTACTORS_LIBRARY}
                       ${Qt5Core_LIBRARIES}
                       ${Qt5Gui_LIBRARIES}
                       ${Qt5OpenGL_LIBRARIES}
                       ${Qt5Widgets_LIBRARIES}
                      )


#put the binary into a "STAGE plugins" folder
SETUP_PLUGIN_WITH_OUTPUT_DIRS(${LIB_NAME} STAGE_MAP_HIERARCHY_EDITOR_PLUGIN stplugins)

