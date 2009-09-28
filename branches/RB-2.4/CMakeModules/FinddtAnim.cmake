# Locate dtAnim
#

INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DTANIM_LIBRARY         dtAnim)
FIND_DELTA3D_LIBRARY(DTANIM_DEBUG_LIBRARY   dtAnimD)

DELTA3D_FIND_PATH   (CAL3D cal3d/cal3d.h)
DELTA3D_FIND_LIBRARY(CAL3D cal3d)

IF (NOT DTANIM_DEBUG_LIBRARY)
  SET(DTANIM_DEBUG_LIBRARY DTANIM_LIBRARY)
  MESSAGE(STATUS "No debug library was found for DTANIM_DEBUG_LIBRARY")
ENDIF()

#convienent list of libraries to link with when using dtAnim
SET(DTANIM_LIBRARIES
    optimized ${DTANIM_LIBRARY}      debug ${DTANIM_DEBUG_LIBRARY}
    optimized ${CAL3D_LIBRARY}       debug ${CAL3D_LIBRARY_DEBUG}
    )
    
SET(DTANIM_INCLUDE_DIRECTORIES ${DELTA3D_INCLUDE_DIR}
                               ${CAL3D_INCLUDE_DIR}
    )


# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dtAnim DEFAULT_MSG DELTA3D_INCLUDE_DIR     DTANIM_LIBRARY
                                                     CAL3D_INCLUDE_DIR       CAL3D_LIBRARY
                                  )
