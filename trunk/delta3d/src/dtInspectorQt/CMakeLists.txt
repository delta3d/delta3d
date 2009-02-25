#copied from FindQt4.cmake, in CMake 2.4.8.  Used here for compatability with CMake < 2.4.8
MACRO (QT4_EXTRACT_OPTIONS qt4_files qt4_options)
  SET(${qt4_files} ${ARGN})
  SET(${qt4_options})
  LIST(FIND ${qt4_files} OPTIONS _index)
  IF(NOT _index EQUAL -1)
    LIST(REMOVE_ITEM ${qt4_files} OPTIONS)
    LIST(LENGTH ${qt4_files} _length)
    WHILE(_length GREATER ${_index})
      LIST(GET ${qt4_files} ${_index} _opt_value)
      LIST(REMOVE_AT ${qt4_files} ${_index})
      LIST(APPEND ${qt4_options} ${_opt_value})
      LIST(LENGTH ${qt4_files} _length)
    ENDWHILE(_length GREATER ${_index})
  ENDIF(NOT _index EQUAL -1)
ENDMACRO (QT4_EXTRACT_OPTIONS)
  
  # QT4_WRAP_UI(outfiles inputfile ... )
# Custom macro to place the generated .h file into the /inc/dtInspector folder
MACRO (DELTA_QT4_WRAP_UI outfiles )
  QT4_EXTRACT_OPTIONS(ui_files ui_options ${ARGN})

  FOREACH (it ${ui_files})
    GET_FILENAME_COMPONENT(outfile ${it} NAME_WE)
    GET_FILENAME_COMPONENT(infile ${it} ABSOLUTE)
    SET(outfile ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME}/ui_${outfile}.h)
    ADD_CUSTOM_COMMAND(OUTPUT ${outfile}
      COMMAND ${QT_UIC_EXECUTABLE}
      ARGS ${ui_options} -o ${outfile} ${infile}
      MAIN_DEPENDENCY ${infile})
    SET(${outfiles} ${${outfiles}} ${outfile})
  ENDFOREACH (it)

ENDMACRO (DELTA_QT4_WRAP_UI)

SET(LIB_NAME     dtInspectorQt)


SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/src/${LIB_NAME})
file(GLOB LIB_PUBLIC_HEADERS "${HEADER_PATH}/*.h")
file(GLOB LIB_SOURCES "*.cpp")

INCLUDE_DIRECTORIES(${SOURCE_PATH}/inc 
                    ${QT_INCLUDE_DIR}
                   )

QT4_WRAP_CPP(MOC_SOURCES ${LIB_PUBLIC_HEADERS})
DELTA_QT4_WRAP_UI(UI_SOURCES dtinspectorqt.ui)

ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS} 
    ${LIB_SOURCES}
    ${MOC_SOURCES}
    ${UI_SOURCES}
)

LINK_WITH_VARIABLES(${LIB_NAME}
                    QT_QTCORE_LIBRARY
                    QT_QTGUI_LIBRARY
                   )

TARGET_LINK_LIBRARIES( ${LIB_NAME}
            ${DTCORE_LIBRARY}
            )

if (X11_FOUND)
   TARGET_LINK_LIBRARIES( ${LIB_NAME}
               ${X11_LIBRARIES})
endif (X11_FOUND)

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_INSPECTORQT_LIBRARY)


INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)

IF (PCHSupport_FOUND)
   ADD_PRECOMPILED_HEADER(${LIB_NAME} precomp.cpp true)
ENDIF (PCHSupport_FOUND)
