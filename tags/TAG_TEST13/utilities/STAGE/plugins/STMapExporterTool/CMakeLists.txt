
SET(LIB_NAME     STMapExporterTool)

file(GLOB LIB_HEADERS "*.h")
file(GLOB LIB_SOURCES "*.cpp")

INCLUDE_DIRECTORIES(  ${CMAKE_SOURCE_DIR}/utilities/STAGE/inc
            ${QT_INCLUDE_DIR}
            ${CMAKE_CURRENT_BINARY_DIR} # for ui*.h files
)

qt4_wrap_cpp(MOC_SOURCES ${LIB_HEADERS})
QT4_WRAP_UI(UI_SOURCES mapexporter.ui)

ADD_LIBRARY(${LIB_NAME} MODULE
   ${LIB_HEADERS}
   ${LIB_SOURCES}
   ${UI_SOURCES}
   ${RCC_SOURCES}
   ${MOC_SOURCES}
)

TARGET_LINK_LIBRARIES( ${LIB_NAME}
                  STAGE
                       ${DTUTIL_LIBRARY}
                       ${DTCORE_LIBRARY}
                       ${DTGAME_LIBRARY}
                       ${DTABC_LIBRARY}
                       ${DTDAL_LIBRARY}
                       ${DTACTORS_LIBRARY}
                       ${QT_QTCORE_LIBRARY}
                       ${QT_QTGUI_LIBRARY}
                       ${QT_QTOPENGL_LIBRARY}
                      )


SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_MAP_EXPORTER_PLUGIN)

#put the binary into a "STAGE plugins" folder
SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin/stplugins)

#make sure that the debug build is the only one that gets built in the debug folder.
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES OUTPUT_NAME ../${LIB_NAME})
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES DEBUG_OUTPUT_NAME ${LIB_NAME})
ENDIF (MSVC)
