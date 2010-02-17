# Locate dtActors
#

INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DTACTORS_LIBRARY         dtActors)
FIND_DELTA3D_LIBRARY(DTACTORS_DEBUG_LIBRARY   dtActorsD)


IF (NOT DTACTORS_DEBUG_LIBRARY)
  SET(DTACTORS_DEBUG_LIBRARY DTACTORS_LIBRARY)
  MESSAGE(STATUS "No debug library was found for DTACTORS_DEBUG_LIBRARY")
ENDIF()

#convienent list of libraries to link with when using dtActors
SET(DTACTORS_LIBRARIES
    optimized ${DTACTORS_LIBRARY}      debug ${DTACTORS_DEBUG_LIBRARY}
    )
    
SET(DTACTORS_INCLUDE_DIRECTORIES ${DELTA3D_INCLUDE_DIR}
    )


# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dtActors DEFAULT_MSG DELTA3D_INCLUDE_DIR     DTACTORS_LIBRARY
                                  )
