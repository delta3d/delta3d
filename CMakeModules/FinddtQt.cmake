# Locate dtQt
# Note: This Find does not include finding Qt, which dtQt depends on

INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DTQT_LIBRARY         dtQt)
FIND_DELTA3D_LIBRARY(DTQT_DEBUG_LIBRARY   dtQtD)


IF (NOT DTQT_DEBUG_LIBRARY)
  SET(DTQT_DEBUG_LIBRARY DTQT_LIBRARY)
  MESSAGE(STATUS "No debug library was found for DTQT_DEBUG_LIBRARY")
ENDIF()

#convienent list of libraries to link with when using dtQt
SET(DTQT_LIBRARIES
    optimized ${DTQT_LIBRARY}  debug ${DTQT_DEBUG_LIBRARY}
    )
    
SET(DTQT_INCLUDE_DIRECTORIES ${DELTA3D_INCLUDE_DIR}
    )


# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dtQt DEFAULT_MSG DELTA3D_INCLUDE_DIR DTQT_LIBRARY DTQT_DEBUG_LIBRARY
                                  )
