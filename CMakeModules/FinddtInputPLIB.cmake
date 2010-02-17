# Locate dtInputPLIB
#

INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DTINPUTPLIB_LIBRARY         dtInputPLIB)
FIND_DELTA3D_LIBRARY(DTINPUTPLIB_DEBUG_LIBRARY   dtInputPLIBD)

DELTA3D_FIND_PATH(PLIB plib/js.h)

SET(JS_NAMES js plibjs)
DELTA3D_FIND_LIBRARY(PLIB_JS "${JS_NAMES}")
 
SET(UL_NAMES ul plibul)
DELTA3D_FIND_LIBRARY(PLIB_UL "${UL_NAMES}")


IF (NOT DTINPUTPLIB_DEBUG_LIBRARY)
  SET(DTINPUTPLIB_DEBUG_LIBRARY DTINPUTPLIB_LIBRARY)
  MESSAGE(STATUS "No debug library was found for DTINPUTPLIB_DEBUG_LIBRARY")
ENDIF()

#convienent list of libraries to link with when using dtInputPLIB
SET(DTINPUTPLIB_LIBRARIES
    optimized ${DTINPUTPLIB_LIBRARY}      debug ${DTINPUTPLIB_DEBUG_LIBRARY}
    optimized ${PLIB_JS_LIBRARY}           debug ${PLIB_JS_LIBRARY_DEBUG}
    optimized ${PLIB_UL_LIBRARY}           debug ${PLIB_UL_LIBRARY_DEBUG}
    )
    
SET(DTINPUTPLIB_INCLUDE_DIRECTORIES ${DELTA3D_INCLUDE_DIR}
                                    ${PLIB_INCLUDE_DIR}
    )


# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dtInputPLIB DEFAULT_MSG DELTA3D_INCLUDE_DIR     DTINPUTPLIB_LIBRARY
                                                          PLIB_INCLUDE_DIR        PLIB_JS_LIBRARY PLIB_JS_LIBRARY_DEBUG 
                                                                                  PLIB_UL_LIBRARY PLIB_UL_LIBRARY_DEBUG
                                  )
