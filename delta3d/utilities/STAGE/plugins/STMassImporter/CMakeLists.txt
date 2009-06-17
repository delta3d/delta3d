
SET(LIB_NAME     STMassImporter)

file(GLOB LIB_HEADERS "*.h")
file(GLOB LIB_SOURCES "*.cpp")

INCLUDE_DIRECTORIES(	${CMAKE_SOURCE_DIR}/utilities/STAGE/inc
						${QT_INCLUDE_DIR}
						${CMAKE_CURRENT_BINARY_DIR} # for ui*.h files
)

qt4_wrap_cpp(MOC_SOURCES ${LIB_HEADERS})
QT4_WRAP_UI(UI_SOURCES massimporter.ui)

ADD_LIBRARY(${LIB_NAME} SHARED
   ${LIB_HEADERS}
   ${LIB_SOURCES}
   ${UI_SOURCES}
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
                     PROPERTIES DEFINE_SYMBOL DT_MASS_IMPORTER_PLUGIN)

#put the binary into a "STAGE plugins" folder
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/stplugins)

#little hack to get the .dll into /bin/stplugins and .lib into /lib
IF (MSVC_IDE)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "../../../bin/stplugins/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC_IDE)
