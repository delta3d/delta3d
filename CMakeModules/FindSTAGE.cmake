# Locate dtQt
# Note: This Find does not include finding Qt, which dtQt depends on

INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(STAGE_LIBRARY         STAGE)
FIND_DELTA3D_LIBRARY(STAGE_DEBUG_LIBRARY   STAGEd)


IF (NOT STAGE_DEBUG_LIBRARY)
  SET(STAGE_DEBUG_LIBRARY STAGE_LIBRARY)
  MESSAGE(STATUS "No debug library was found for DTQT_DEBUG_LIBRARY")
ENDIF()

#convienent list of libraries to link with when using dtQt
SET(STAGE_LIBRARIES
    optimized ${STAGE_LIBRARY}  debug ${STAGE_DEBUG_LIBRARY}
    )
    
SET(STAGE_INCLUDE_DIRECTORIES ${DELTA3D_ROOT}/utilities/STAGE/inc
    )


# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(STAGE DEFAULT_MSG STAGE_INCLUDE_DIRECTORIES STAGE_LIBRARY STAGE_DEBUG_LIBRARY
                                  )
