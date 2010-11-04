MACRO(_PCH_GET_COMPILE_FLAGS _target _out_compile_flags)

  STRING(TOUPPER "CMAKE_CXX_FLAGS" _flags_var_name)
  SET(${_out_compile_flags} ${${_flags_var_name}} )

  if (CMAKE_BUILD_TYPE)
     STRING(TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" _flags_var_name)
     SET(${_out_compile_flags} ${${_flags_var_name}} )
  endif (CMAKE_BUILD_TYPE)

  GET_TARGET_PROPERTY(_targetType ${_target} TYPE)
  IF(${_targetType} STREQUAL SHARED_LIBRARY)
    LIST(APPEND ${_out_compile_flags} "${${_out_compile_flags}} -fPIC")
  ENDIF(${_targetType} STREQUAL SHARED_LIBRARY)

  GET_TARGET_PROPERTY(_TARGET_DEFINE_SYMBOL ${_target} DEFINE_SYMBOL)
  IF (_TARGET_DEFINE_SYMBOL)
     LIST(APPEND ${_out_compile_flags} "-D${_TARGET_DEFINE_SYMBOL}")
  ENDIF (_TARGET_DEFINE_SYMBOL)

  GET_DIRECTORY_PROPERTY(DIRINC INCLUDE_DIRECTORIES )
  FOREACH(item ${DIRINC})
    LIST(APPEND ${_out_compile_flags} "-I${item}")
  ENDFOREACH(item)

  GET_DIRECTORY_PROPERTY(DIRDEF COMPILE_DEFINITIONS )
  FOREACH(item ${DIRDEF})
    LIST(APPEND ${_out_compile_flags} "-D${item}")
  ENDFOREACH(item)

  if (CMAKE_BUILD_TYPE)
     GET_DIRECTORY_PROPERTY(DIRDEF COMPILE_DEFINITIONS_${CMAKE_BUILD_TYPE} )
     FOREACH(item ${DIRDEF})
        LIST(APPEND ${_out_compile_flags} "-D${item}")
     ENDFOREACH(item)
  endif (CMAKE_BUILD_TYPE)

  GET_DIRECTORY_PROPERTY(_directory_flags DEFINITIONS)
  #MESSAGE("_directory_flags ${_directory_flags}" )
  LIST(APPEND ${_out_compile_flags} ${_directory_flags})
  LIST(APPEND ${_out_compile_flags} ${CMAKE_CXX_FLAGS} )

  SEPARATE_ARGUMENTS(${_out_compile_flags})

ENDMACRO(_PCH_GET_COMPILE_FLAGS)

MACRO(ADD_PRECOMPILED_HEADER Target PrecompiledHeader PrecompiledSource SourcesVar)
if (BUILD_WITH_PCH)
  FIND_PATH(PCH_HEADER_PATH ${PrecompiledHeader}
    PATH_SUFFIXES inc include
    PATHS
    ${CMAKE_SOURCE_DIR}
  )

  IF(MSVC)
    GET_FILENAME_COMPONENT(PrecompiledBasename ${PCH_HEADER_PATH}/${PrecompiledHeader} NAME_WE)
    SET(PrecompiledBinary "${CMAKE_CFG_INTDIR}/${PrecompiledBasename}.pch") #will create .pch file in a folder corresponding with build type (debug, release, etc)
    SET(Sources ${${SourcesVar}})

    SET_SOURCE_FILES_PROPERTIES(${PCH_HEADER_PATH}/${PrecompiledSource}
                                PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_OUTPUTS "${PrecompiledBinary}")
    SET_SOURCE_FILES_PROPERTIES(${Sources}
                                PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /FI\"${PrecompiledBinary}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_DEPENDS "${PrecompiledBinary}")  
    # Add precompiled header to SourcesVar
    LIST(APPEND ${SourcesVar} ${PCH_HEADER_PATH}/${PrecompiledSource})
  ELSE (MSVC)
    IF(CMAKE_COMPILER_IS_GNUCXX)
    GET_FILENAME_COMPONENT(PrecompiledBasename ${PCH_HEADER_PATH}/${PrecompiledHeader} NAME)
    SET(PrecompiledBinary "${CMAKE_CURRENT_BINARY_DIR}/${PrecompiledBasename}.gch")
    SET(Sources ${${SourcesVar}})
    
    _PCH_GET_COMPILE_FLAGS(${Target} FLAGS)

    add_custom_command(OUTPUT ${PrecompiledBinary}
                     MAIN_DEPENDENCY $${PCH_HEADER_PATH}/${PrecompiledSource}
                     Depends  ${PCH_HEADER_PATH}/${PrecompiledHeader}
                     COMMAND ${CMAKE_CXX_COMPILER} ${FLAGS} -Winvalid-pch -x c++-header ${PCH_HEADER_PATH}/${PrecompiledSource} -o ${PrecompiledBinary}
                     WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                     )

    SET_SOURCE_FILES_PROPERTIES(${Sources}
                                PROPERTIES COMPILE_FLAGS "-I${CMAKE_CURRENT_BINARY_DIR} -Winvalid-pch -include ${PrecompiledBasename}"
                                           OBJECT_DEPENDS "${PrecompiledBinary}")  
    # Add precompiled header to SourcesVar
    # LIST(APPEND ${SourcesVar} ${PCH_HEADER_PATH}/${PrecompiledSource})
    ENDIF(CMAKE_COMPILER_IS_GNUCXX)
  ENDIF(MSVC)
endif (BUILD_WITH_PCH)
ENDMACRO(ADD_PRECOMPILED_HEADER)

# ADD -fPIC to g++ because it's hard for the script to figure out if you are build a library or not, and
# -fPIC will work on everything.
IF(CMAKE_COMPILER_IS_GNUCXX)
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)
