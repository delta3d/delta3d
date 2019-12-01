
SET(LIB_NAME     STPluginDialog)

file(GLOB LIB_PUBLIC_HEADERS "*.h")
file(GLOB LIB_C_SOURCES "*.cpp")

INCLUDE_DIRECTORIES(  ${CMAKE_SOURCE_DIR}/utilities/STAGE/inc 
            ${QT_INCLUDE_DIR}
            ${CMAKE_CURRENT_BINARY_DIR} # for ui*.h files
)

qt4_wrap_cpp(MOC_SOURCES ${LIB_PUBLIC_HEADERS})
QT4_WRAP_UI(UI_SOURCES spplugin.ui)

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
                       ${QT_QTCORE_LIBRARY}
                       ${QT_QTGUI_LIBRARY}
                       ${QT_QTOPENGL_LIBRARY}
                      )


#put the binary into a "STAGE plugins" folder
SETUP_PLUGIN_WITH_OUTPUT_DIRS(${LIB_NAME} DT_PLUGIN_MANAGER_PLUGIN stplugins)
