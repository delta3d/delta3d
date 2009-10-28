# Locate dtTerrain

INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DTTERRAIN_LIBRARY         dtTerrain)
FIND_DELTA3D_LIBRARY(DTTERRAIN_DEBUG_LIBRARY   dtTerrainD)

SET(GDAL_INCLUDES gdal.h gdal/gdal.h)
DELTA3D_FIND_PATH(GDAL "${GDAL_INCLUDES}")
SET(GDAL_NAMES gdal gdal_i gdal1.4.0 gdal1.3.2 gdal1.5.0)
DELTA3D_FIND_LIBRARY(GDAL "${GDAL_NAMES}")


IF (NOT DTTERRAIN_DEBUG_LIBRARY)
  SET(DTTERRAIN_DEBUG_LIBRARY DTTERRAIN_LIBRARY)
  MESSAGE(STATUS "No debug library was found for DTTERRAIN_DEBUG_LIBRARY")
ENDIF()

#convienent list of libraries to link with when using dtTerrain
SET(DTTERRAIN_LIBRARIES
    optimized ${DTTERRAIN_LIBRARY}  debug ${DTTERRAIN_DEBUG_LIBRARY}
    optimized ${GDAL_LIBRARY}       debug ${GDAL_LIBRARY_DEBUG}
    )
    
SET(DTTERRAIN_INCLUDE_DIRECTORIES ${DELTA3D_INCLUDE_DIR}
                                  ${GDAL_INCLUDE_DIR}
    )


# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dtTerrain DEFAULT_MSG DELTA3D_INCLUDE_DIR DTTERRAIN_LIBRARY DTTERRAIN_DEBUG_LIBRARY
                                                        GDAL_INCLUDE_DIR GDAL_LIBRARY GDAL_LIBRARY_DEBUG
                                  )
