# Locate dtTerrain

INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DTTERRAIN_LIBRARY         dtTerrain)

if (FIND_dtTerrain_REQUIRED)
   FIND_PACKAGE(GDAL REQUIRED)
elseif(FIND_dtTerrain_QUIETLY)
   FIND_PACKAGE(GDAL QUIETLY)
else()
   FIND_PACKAGE(GDAL )
endif()


#convienent list of libraries to link with when using dtTerrain
CREATE_LINK_LINES_FOR_TARGETS(DTTERRAIN_LIBRARIES
    DTTERRAIN_LIBRARY
    GDAL_LIBRARY
    )
    
SET(DTTERRAIN_INCLUDE_DIRECTORIES ${DELTA3D_INCLUDE_DIR}
                                  ${GDAL_INCLUDE_DIR}
    )


# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dtTerrain DEFAULT_MSG DELTA3D_INCLUDE_DIR DTTERRAIN_LIBRARY
                                                        GDAL_INCLUDE_DIR GDAL_LIBRARY
                                  )
