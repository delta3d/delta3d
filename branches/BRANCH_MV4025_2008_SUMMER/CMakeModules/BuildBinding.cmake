#Build a Delta3D Python binding.
#Expects files to be compiled in TARGET_SRC and TARGET_H.
#Supply the target name as the first parameter.  Additional
#parameters will be passed to LINK_WITH_VARIABLES
MACRO (BUILD_PYTHON_BINDING TGTNAME)
  
  ADD_LIBRARY( ${TGTNAME} SHARED ${TARGET_SRC} ${TARGET_H} )
  INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_PATH} )

  
  FOREACH(varname ${ARGN})
      LINK_WITH_VARIABLES( ${TGTNAME} ${varname} )
  ENDFOREACH(varname)

  IF (WIN32)
    SET_TARGET_PROPERTIES( ${TGTNAME} PROPERTIES SUFFIX ".pyd")
    SET_TARGET_PROPERTIES( ${TGTNAME} PROPERTIES DEBUG_POSTFIX "")
  ENDIF (WIN32)
  
  #little hack to get the .dll into /bin and .lib into /lib
  IF (MSVC)
    SET_TARGET_PROPERTIES( ${TGTNAME} PROPERTIES PREFIX "/../../bin/$(ConfigurationName)/")
  ENDIF (MSVC)

ENDMACRO(BUILD_PYTHON_BINDING)
