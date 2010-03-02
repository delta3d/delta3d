MACRO(_PCH_GET_COMPILE_FLAGS BUILD_TYPE _target _out_compile_flags)

  STRING(TOUPPER "CMAKE_CXX_FLAGS_${BUILD_TYPE}" _flags_var_name)
  SET(${_out_compile_flags} ${${_flags_var_name}} )

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

  GET_DIRECTORY_PROPERTY(DIRDEF COMPILE_DEFINITIONS_${BUILD_TYPE} )
  FOREACH(item ${DIRDEF})
    LIST(APPEND ${_out_compile_flags} "-D${item}")
  ENDFOREACH(item)

  GET_DIRECTORY_PROPERTY(_directory_flags DEFINITIONS)
  #MESSAGE("_directory_flags ${_directory_flags}" )
  LIST(APPEND ${_out_compile_flags} ${_directory_flags})
  LIST(APPEND ${_out_compile_flags} ${CMAKE_CXX_FLAGS} )

  SEPARATE_ARGUMENTS(${_out_compile_flags})

ENDMACRO(_PCH_GET_COMPILE_FLAGS)

MACRO(ADD_PRECOMPILED_HEADER Target PrecompiledHeader PrecompiledSource SourcesVar)
if (BUILD_WITH_PCH)
  IF(MSVC)
    GET_FILENAME_COMPONENT(PrecompiledBasename ${CMAKE_SOURCE_DIR}/inc/${PrecompiledHeader} NAME_WE)
    SET(PrecompiledBinary "${CMAKE_CFG_INTDIR}/${PrecompiledBasename}.pch") #will create .pch file in a folder corresponding with build type (debug, release, etc)
    SET(Sources ${${SourcesVar}})

    SET_SOURCE_FILES_PROPERTIES(${CMAKE_SOURCE_DIR}/inc/${PrecompiledSource}
                                PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_OUTPUTS "${PrecompiledBinary}")
    SET_SOURCE_FILES_PROPERTIES(${Sources}
                                PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /FI\"${PrecompiledBinary}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_DEPENDS "${PrecompiledBinary}")  
    # Add precompiled header to SourcesVar
    LIST(APPEND ${SourcesVar} ${CMAKE_SOURCE_DIR}/inc/${PrecompiledSource})
  ELSE (MSVC)
    IF(CMAKE_COMPILER_IS_GNUCXX)
    GET_FILENAME_COMPONENT(PrecompiledBasename ${CMAKE_SOURCE_DIR}/inc/${PrecompiledHeader} NAME)
    SET(PrecompiledBinary "${CMAKE_CURRENT_BINARY_DIR}/${PrecompiledBasename}.gch")
    SET(Sources ${${SourcesVar}})
    
    _PCH_GET_COMPILE_FLAGS(${CMAKE_BUILD_TYPE} ${Target} FLAGS)

    add_custom_command(OUTPUT ${PrecompiledBinary}
                     MAIN_DEPENDENCY ${CMAKE_SOURCE_DIR}/inc/${PrecompiledSource}
                     Depends  ${CMAKE_SOURCE_DIR}/inc/${PrecompiledHeader}
                     COMMAND ${CMAKE_CXX_COMPILER} ${FLAGS} -Winvalid-pch -x c++-header ${CMAKE_SOURCE_DIR}/inc/${PrecompiledSource} -o ${PrecompiledBinary}
                     WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                     )

    SET_SOURCE_FILES_PROPERTIES(${Sources}
                                PROPERTIES COMPILE_FLAGS "-I${CMAKE_CURRENT_BINARY_DIR} -Winvalid-pch -include ${PrecompiledBasename}"
                                           OBJECT_DEPENDS "${PrecompiledBinary}")  
    # Add precompiled header to SourcesVar
    # LIST(APPEND ${SourcesVar} ${CMAKE_SOURCE_DIR}/inc/${PrecompiledSource})
    ENDIF(CMAKE_COMPILER_IS_GNUCXX)
  ENDIF(MSVC)
endif (BUILD_WITH_PCH)
ENDMACRO(ADD_PRECOMPILED_HEADER)
