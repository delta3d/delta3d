#Build a Delta3D Python binding.
#Expects files to be compiled in TARGET_SRC and TARGET_H.
#Supply the target name as the first parameter.  Additional
#parameters will be passed to TARGET_LINK_LIBRARIES()
MACRO (BUILD_PYTHON_BINDING TGTNAME)

    #release goes to the bin folder
    #debug goes to the bin/debug folder
    SET(OUTPUT_BINDIR ${PROJECT_BINARY_DIR}/bin/)
    SET(OUTPUT_LIBDIR ${PROJECT_BINARY_DIR}/lib)
    
    SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY  ${OUTPUT_LIBDIR})
    SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${OUTPUT_BINDIR})
    IF(WIN32)
      SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${OUTPUT_BINDIR})
    ELSE(WIN32)
      SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_LIBDIR}) 
    ENDIF(WIN32)
    
    # For each configuration (Debug, Release, MinSizeRel... and/or anything the user chooses) 
    FOREACH(CONF ${CMAKE_CONFIGURATION_TYPES})
       # Go uppercase (DEBUG, RELEASE...)
       STRING(TOUPPER "${CONF}" CONF)
        
       #Handle the debug differently by putting the output into a debug subfolder
       IF (${CONF} STREQUAL "DEBUG")
         SET(DEBUG_FOLDER "Debug")
       ELSE (${CONF} STREQUAL "DEBUG")
         SET(DEBUG_FOLDER "Release")
       ENDIF (${CONF} STREQUAL "DEBUG")

       SET("CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_LIBDIR}")
       SET("CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_BINDIR}/${DEBUG_FOLDER}")
       IF(WIN32)
         SET("CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_BINDIR}/${DEBUG_FOLDER}")
       ELSE()
         SET("CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_LIBDIR}")
       ENDIF()
    ENDFOREACH()
  
  #With Cmake2.6.x and CMP003 NEW, if user sets BOOST_PYTHON_LIBRARY
  #to be the non-versioned file ("boost_python-vc90-mt.lib"), the compiler
  #will try to link with the versioned file ("boost_python-vc90-mt-1_35.lib")
  #so we need an additional search path to find it.
  LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

  ADD_LIBRARY( ${TGTNAME} SHARED ${TARGET_SRC} ${TARGET_H} )
  INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_PATH} 
                       ${Boost_INCLUDE_DIR} )

  FOREACH(varname ${ARGN})
      TARGET_LINK_LIBRARIES( ${TGTNAME} ${varname} )
  ENDFOREACH(varname)
  
 
  TARGET_LINK_LIBRARIES( ${TGTNAME}
                         ${Boost_PYTHON_LIBRARY}
                        )
                        
  #Use the debug python library, should it exist, otherwise, use the release version
  SET (PYTHON_DEBUG ${PYTHON_DEBUG_LIBRARY})

  IF (NOT PYTHON_DEBUG)
    SET (PYTHON_DEBUG ${PYTHON_LIBRARY})
  ENDIF (NOT PYTHON_DEBUG)

  TARGET_LINK_LIBRARIES( ${TGTNAME}
                         optimized ${PYTHON_LIBRARY} debug ${PYTHON_DEBUG}
                       )

  IF (WIN32)
    SET_TARGET_PROPERTIES( ${TGTNAME} PROPERTIES SUFFIX ".pyd")
    SET_TARGET_PROPERTIES( ${TGTNAME} PROPERTIES DEBUG_POSTFIX "")
  ENDIF (WIN32)


ENDMACRO(BUILD_PYTHON_BINDING)
