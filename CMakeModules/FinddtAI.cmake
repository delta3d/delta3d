# Locate dtAI
#

INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DTAI_LIBRARY         dtAI)
FIND_DELTA3D_LIBRARY(DTAI_DEBUG_LIBRARY   dtAID)


IF (NOT DTAI_DEBUG_LIBRARY)
  SET(DTAI_DEBUG_LIBRARY DTAI_LIBRARY)
  MESSAGE(STATUS "No debug library was found for DTAI_DEBUG_LIBRARY")
ENDIF()

#convienent list of libraries to link with when using dtAI
SET(DTAI_LIBRARIES
    optimized ${DTAI_LIBRARY}      debug ${DTAI_DEBUG_LIBRARY}
    )
    
SET(DTAI_INCLUDE_DIRECTORIES ${DELTA3D_INCLUDE_DIR}
    )


# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dtAI DEFAULT_MSG DELTA3D_INCLUDE_DIR     DTAI_LIBRARY
                                  )
