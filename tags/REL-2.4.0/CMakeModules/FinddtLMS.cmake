# Locate dtLMS
#

INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DTLMS_LIBRARY         dtLMS)
FIND_DELTA3D_LIBRARY(DTLMS_DEBUG_LIBRARY   dtLMSD)


IF (NOT DTLMS_DEBUG_LIBRARY)
  SET(DTLMS_DEBUG_LIBRARY DTLMS_LIBRARY)
  MESSAGE(STATUS "No debug library was found for DTLMS_DEBUG_LIBRARY")
ENDIF()

#convienent list of libraries to link with when using dtLMS
SET(DTLMS_LIBRARIES
    optimized ${DTLMS_LIBRARY}      debug ${DTLMS_DEBUG_LIBRARY}
    )
    
SET(DTLMS_INCLUDE_DIRECTORIES ${DELTA3D_INCLUDE_DIR}
    )


# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dtLMS DEFAULT_MSG DELTA3D_INCLUDE_DIR     DTLMS_LIBRARY
                                  )
