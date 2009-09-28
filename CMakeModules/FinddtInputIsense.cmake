# Locate dtInputIsense
#

INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DTINPUTISENSE_LIBRARY         dtInputIsense)
FIND_DELTA3D_LIBRARY(DTINPUTISENSE_DEBUG_LIBRARY   dtInputIsenseD)


IF (NOT DTINPUTISENSE_DEBUG_LIBRARY)
  SET(DTINPUTISENSE_DEBUG_LIBRARY DTINPUTISENSE_LIBRARY)
  MESSAGE(STATUS "No debug library was found for DTINPUTISENSE_DEBUG_LIBRARY")
ENDIF()

#convienent list of libraries to link with when using dtInputIsense
SET(DTINPUTISENSE_LIBRARIES
    optimized ${DTINPUTISENSE_LIBRARY}      debug ${DTINPUTISENSE_DEBUG_LIBRARY}
    )
    
SET(DTINPUTISENSE_INCLUDE_DIRECTORIES ${DELTA3D_INCLUDE_DIR}
    )


# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dtInputIsense DEFAULT_MSG DELTA3D_INCLUDE_DIR     DTINPUTISENSE_LIBRARY
                                  )
