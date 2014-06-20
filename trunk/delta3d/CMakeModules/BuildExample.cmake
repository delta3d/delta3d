#Build a Delta3D console app executable.
#Expects files to be compiled in TARGET_SRC and TARGET_H.
#Supply the target name as the first parameter.  Additional
#parameters will be passed to LINK_WITH_VARIABLES
MACRO(BUILD_EXE_EXAMPLE TGTNAME)
  
  ADD_EXECUTABLE(${TGTNAME} ${TARGET_SRC} ${TARGET_H})
  
  FOREACH(varname ${ARGN})
      TARGET_LINK_LIBRARIES( ${TGTNAME} ${varname} )
  ENDFOREACH(varname)
    
  IF (MSVC)
    SET_TARGET_PROPERTIES(${TGTNAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
  ENDIF (MSVC)
  
  # needed for install
  SET(TGT_NAME ${TGTNAME})
  
  INCLUDE(ExampleInstall OPTIONAL)

ENDMACRO(BUILD_EXE_EXAMPLE)

MACRO(BUILD_LIB_EXAMPLE TGTNAME ExportMacro)
  ADD_LIBRARY(${TGTNAME} SHARED ${TARGET_SRC} ${TARGET_H})
  
  FOREACH(varname ${ARGN})
      TARGET_LINK_LIBRARIES( ${TGTNAME} ${varname} )
  ENDFOREACH(varname)
    
  SET_TARGET_PROPERTIES(${TGTNAME}
                     PROPERTIES DEFINE_SYMBOL ${ExportMacro})

  IF (MSVC)
    SET_TARGET_PROPERTIES(${TGTNAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
  ENDIF (MSVC)
  
  # needed for install
  SET(TGT_NAME ${TGTNAME})

  BUILD_GAME_START(${TGTNAME} ON)
  
  INCLUDE(ExampleInstall OPTIONAL)

ENDMACRO(BUILD_LIB_EXAMPLE)
