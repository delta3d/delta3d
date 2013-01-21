# Locate dtPhysics
#

INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DTPHYSICS_LIBRARY         dtPhysics)
FIND_DELTA3D_LIBRARY(DTPHYSICS_DEBUG_LIBRARY   dtPhysicsd)


IF (NOT DTPHYSICS_DEBUG_LIBRARY)
  SET(DTPHYSICS_DEBUG_LIBRARY ${DTPHYSICS_LIBRARY})
  MESSAGE(STATUS "No debug library was found for DTPHYSICS_DEBUG_LIBRARY")
ENDIF()

#convienent list of libraries to link with when using dtPhysics
SET(DTPHYSICS_LIBRARIES
    optimized ${DTPHYSICS_LIBRARY}      debug ${DTPHYSICS_DEBUG_LIBRARY}
    )
    
SET(DTPHYSICS_INCLUDE_DIRECTORIES ${DELTA3D_INCLUDE_DIR}
    )


# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dtPhysics DEFAULT_MSG DELTA3D_INCLUDE_DIR     DTPHYSICS_LIBRARY
                                  )
