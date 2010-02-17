# Locate dtNetGM
# Note: Does not find Boost header files, which are required by dtNetGM
#

INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DTNETGM_LIBRARY         dtNetGM)
FIND_DELTA3D_LIBRARY(DTNETGM_DEBUG_LIBRARY   dtNetGMD)

DELTA3D_FIND_PATH(GNE gnelib.h)
DELTA3D_FIND_LIBRARY(GNE gne)

DELTA3D_FIND_PATH(HAWKNL nl.h)
SET(HAWKNL_NAMES NLstatic NL nl)
DELTA3D_FIND_LIBRARY(HAWKNL "${HAWKNL_NAMES}")

IF (NOT DTNETGM_DEBUG_LIBRARY)
  SET(DTNETGM_DEBUG_LIBRARY DTNETGM_LIBRARY)
  MESSAGE(STATUS "No debug library was found for DTNETGM_DEBUG_LIBRARY")
ENDIF()

#convienent list of libraries to link with when using dtNetGM
SET(DTNETGM_LIBRARIES
    optimized ${DTNETGM_LIBRARY}  debug ${DTNETGM_DEBUG_LIBRARY}
    optimized ${GNE_LIBRARY}      debug ${GNE_LIBRARY_DEBUG}
    optimized ${HAWKNL_LIBRARY}   debug ${HAWKNL_LIBRARY_DEBUG}
    )
    
SET(DTNETGM_INCLUDE_DIRECTORIES ${DELTA3D_INCLUDE_DIR}
                                ${GNE_INCLUDE_DIR}
                                ${HAWKNL_INCLUDE_DIR}
    )


# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dtNetGM DEFAULT_MSG DELTA3D_INCLUDE_DIR DTNETGM_LIBRARY DTNETGM_DEBUG_LIBRARY
                                                      GNE_INCLUDE_DIR     GNE_LIBRARY     GNE_LIBRARY_DEBUG
                                                      HAWKNL_INCLUDE_DIR  HAWKNL_LIBRARY  HAWKNL_LIBRARY_DEBUG
                                  )
