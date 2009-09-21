# Locate dtDAL
#

INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DTDAL_LIBRARY         dtDAL)
FIND_DELTA3D_LIBRARY(DTDAL_DEBUG_LIBRARY   dtDALD)


IF (NOT DTDAL_DEBUG_LIBRARY)
  SET(DTDAL_DEBUG_LIBRARY DTDAL_LIBRARY)
  MESSAGE(STATUS "No debug library was found for DTDAL_DEBUG_LIBRARY")
ENDIF()

#convienent list of libraries to link with when using dtDAL
SET(DTDAL_LIBRARIES
    optimized ${DTDAL_LIBRARY}      debug ${DTDAL_DEBUG_LIBRARY}
    )
    
SET(DTDAL_INCLUDE_DIRECTORIES ${DELTA3D_INCLUDE_DIR}
    )


# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dtDAL DEFAULT_MSG DELTA3D_INCLUDE_DIR     DTDAL_LIBRARY
                                  )
