SET(LIB_NAME     dtDirectorQt)

FIND_PACKAGE(Qt5Multimedia)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/src/${LIB_NAME})
SET(UI_PATH ${DELTA3D_SOURCE_DIR}/ui/${LIB_NAME})

# Define Qt macro to put ui_*.h files into a set place
MACRO (QT5_WRAP_UI_COPY outfiles)
  QT5_EXTRACT_OPTIONS(ui_files ui_options ${ARGN})

  FOREACH (it ${ui_files})
    GET_FILENAME_COMPONENT(outfile ${it} NAME_WE)
    GET_FILENAME_COMPONENT(infile ${it} ABSOLUTE)
    SET(outfile ${UI_PATH}/ui_${outfile}.h) # Here we set output
    ADD_CUSTOM_COMMAND(OUTPUT ${outfile}
      COMMAND ${QT_UIC_EXECUTABLE}
      ARGS ${ui_options} -o ${outfile} ${infile}
      MAIN_DEPENDENCY ${infile})
    SET(${outfiles} ${${outfiles}} ${outfile})
  ENDFOREACH (it)

ENDMACRO (QT5_WRAP_UI_COPY)

SET(LIB_PUBLIC_HEADERS ${HEADER_PATH}/actionitem.h
    ${HEADER_PATH}/clipboard.h
    ${HEADER_PATH}/customeditortool.h
    ${HEADER_PATH}/directoreditor.h
    ${HEADER_PATH}/editornotifier.h
    ${HEADER_PATH}/editorscene.h
    ${HEADER_PATH}/editorview.h
    ${HEADER_PATH}/export.h
    ${HEADER_PATH}/graphbrowser.h
    ${HEADER_PATH}/graphtabs.h
    ${HEADER_PATH}/groupitem.h
    ${HEADER_PATH}/importeditor.h
    ${HEADER_PATH}/libraryeditor.h
    ${HEADER_PATH}/linkitem.h
    ${HEADER_PATH}/lockitem.h
    ${HEADER_PATH}/macroitem.h
    ${HEADER_PATH}/nodeitem.h
    ${HEADER_PATH}/nodescene.h
    ${HEADER_PATH}/nodetabs.h
    ${HEADER_PATH}/plugindialog.h
    ${HEADER_PATH}/plugininterface.h
    ${HEADER_PATH}/pluginmanager.h
    ${HEADER_PATH}/propertyeditor.h
    ${HEADER_PATH}/resizeitem.h
    ${HEADER_PATH}/scriptitem.h
    ${HEADER_PATH}/searchbrowser.h
    ${HEADER_PATH}/threadbrowser.h
    ${HEADER_PATH}/undoaddlibraryevent.h
    ${HEADER_PATH}/undoaddimportevent.h
    ${HEADER_PATH}/undocreateevent.h
    ${HEADER_PATH}/undodeleteevent.h
    ${HEADER_PATH}/undoevent.h
    ${HEADER_PATH}/undolinkevent.h
    ${HEADER_PATH}/undolinkvisibilityevent.h
    ${HEADER_PATH}/undomanager.h
    ${HEADER_PATH}/undomultipleevent.h
    ${HEADER_PATH}/undopropertyevent.h
    ${HEADER_PATH}/undoremovelibraryevent.h
    ${HEADER_PATH}/undoremoveimportevent.h
    ${HEADER_PATH}/valueitem.h
)

SET(LIB_SOURCES ${SOURCE_PATH}/actionitem.cpp
    ${SOURCE_PATH}/clipboard.cpp
    ${SOURCE_PATH}/customeditortool.cpp
    ${SOURCE_PATH}/directoreditor.cpp
    ${SOURCE_PATH}/editornotifier.cpp
    ${SOURCE_PATH}/editorscene.cpp
    ${SOURCE_PATH}/editorview.cpp
    ${SOURCE_PATH}/graphbrowser.cpp
    ${SOURCE_PATH}/graphtabs.cpp
    ${SOURCE_PATH}/groupitem.cpp
    ${SOURCE_PATH}/importeditor.cpp
    ${SOURCE_PATH}/libraryeditor.cpp
    ${SOURCE_PATH}/linkitem.cpp
    ${SOURCE_PATH}/lockitem.cpp
    ${SOURCE_PATH}/macroitem.cpp
    ${SOURCE_PATH}/nodeitem.cpp
    ${SOURCE_PATH}/nodescene.cpp
    ${SOURCE_PATH}/nodetabs.cpp
    ${SOURCE_PATH}/plugindialog.cpp
    ${SOURCE_PATH}/pluginmanager.cpp
    ${SOURCE_PATH}/propertyeditor.cpp
    ${SOURCE_PATH}/resizeitem.cpp
    ${SOURCE_PATH}/scriptitem.cpp
    ${SOURCE_PATH}/searchbrowser.cpp
    ${SOURCE_PATH}/threadbrowser.cpp
    ${SOURCE_PATH}/undoaddlibraryevent.cpp
    ${SOURCE_PATH}/undoaddimportevent.cpp
    ${SOURCE_PATH}/undocreateevent.cpp
    ${SOURCE_PATH}/undodeleteevent.cpp
    ${SOURCE_PATH}/undoevent.cpp
    ${SOURCE_PATH}/undolinkevent.cpp
    ${SOURCE_PATH}/undolinkvisibilityevent.cpp
    ${SOURCE_PATH}/undomanager.cpp
    ${SOURCE_PATH}/undomultipleevent.cpp
    ${SOURCE_PATH}/undopropertyevent.cpp
    ${SOURCE_PATH}/undoremovelibraryevent.cpp
    ${SOURCE_PATH}/undoremoveimportevent.cpp
    ${SOURCE_PATH}/valueitem.cpp
)

qt5_wrap_ui(UI_SOURCES
    ${UI_PATH}/directoreditor.ui
	${UI_PATH}/plugindialog.ui
)

QT5_WRAP_CPP(MOC_SOURCES ${LIB_PUBLIC_HEADERS})
QT5_ADD_RESOURCES(RCC_SOURCES
	directorqt.qrc
	docs/help.qrc
)

############### SOURCE FILE ORGANIZATION #################
SOURCE_GROUP("Auto-Generated" FILES ${MOC_SOURCES})
SOURCE_GROUP("Auto-Generated" FILES ${RCC_SOURCES})
SOURCE_GROUP("Auto-Generated" FILES ${UI_SOURCES})

SOURCE_GROUP("UI Files" FILES ${UI_SOURCES})
##########################################################

INCLUDE_DIRECTORIES(
                    ${QT_INCLUDE_DIR}
                    ${UI_PATH}
		    ${Qt5Multimedia_INCLUDE_DIRS}
                    ${CMAKE_CURRENT_BINARY_DIR} # ui compiled files
                   )

ADD_PRECOMPILED_HEADER(${LIB_NAME} prefix/dtdirectorqtprefix.h prefix/dtdirectorqtprefix.cpp LIB_SOURCES)

set(LIB_SOURCES
    ${LIB_SOURCES}
    ${MOC_SOURCES}
    ${RCC_SOURCES}
    ${UI_SOURCES}
)

SET(LIB_EXTERNAL_DEPS
  ${DT_QT_LIBRARIES}
 )

SET(LIB_DEPS
     ${DTDIRECTOR_LIBRARY}
     dtDirectorNodes # need a macro for this.
     ${DTQT_LIBRARY}
	 ${Qt5Multimedia_LIBRARIES}
   )

DELTA3D_ADD_LIBRARY(${LIB_NAME} DIRECTOR_QT_LIBRARY)

