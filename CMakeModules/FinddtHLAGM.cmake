# Locate dtHLAGM
# NOTE: The RTI External dependencies are not included with this Find
#

INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DTHLAGM_LIBRARY         dtHLAGM)
FIND_DELTA3D_LIBRARY(DTHLAGM_DEBUG_LIBRARY   dtHLAGMD)


IF (NOT DTHLAGM_DEBUG_LIBRARY)
  SET(DTHLAGM_DEBUG_LIBRARY DTHLAGM_LIBRARY)
  MESSAGE(STATUS "No debug library was found for DTHLAGM_DEBUG_LIBRARY")
ENDIF()

#convienent list of libraries to link with when using dtHLAGM
SET(DTHLAGM_LIBRARIES
    optimized ${DTHLAGM_LIBRARY}      debug ${DTHLAGM_DEBUG_LIBRARY}
    )
    
SET(DTHLAGM_INCLUDE_DIRECTORIES ${DELTA3D_INCLUDE_DIR}
    )


# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dtHLAGM DEFAULT_MSG DELTA3D_INCLUDE_DIR     DTHLAGM_LIBRARY
                                  )