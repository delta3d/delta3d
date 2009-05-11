#Build a Delta3D console app executable.
#Expects files to be compiled in TARGET_SRC and TARGET_H.
#Supply the target name as the first parameter.  Additional
#parameters will be passed to LINK_WITH_VARIABLES
MACRO(BUILD_EXE_EXAMPLE TGTNAME)
  
  ADD_EXECUTABLE(${TGTNAME} ${TARGET_SRC} ${TARGET_H})
  
  FOREACH(varname ${ARGN})
      TARGET_LINK_LIBRARIES( ${TGTNAME} ${varname} )
  ENDFOREACH(varname)
  
  #little hack to get the .dll into /bin and .lib into /lib
  IF (MSVC)
    SET_TARGET_PROPERTIES(${TGTNAME} PROPERTIES PREFIX "/../../bin/")
    SET_TARGET_PROPERTIES(${TGTNAME} PROPERTIES IMPORT_PREFIX "../")
  ENDIF (MSVC)
  
  IF (WIN32)
    SET_TARGET_PROPERTIES(${TGTNAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
  ENDIF (WIN32)
  
 INCLUDE(ModuleInstall OPTIONAL)

ENDMACRO(BUILD_EXE_EXAMPLE)