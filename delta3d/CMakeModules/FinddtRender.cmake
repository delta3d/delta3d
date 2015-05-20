# Locate dtRender
#

INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DTRENDER_LIBRARY         dtRender)

SET(DTRENDER_INCLUDE_DIRECTORIES ${DELTA3D_INCLUDE_DIR} )

CREATE_LINK_LINES_FOR_TARGETS(DTRENDER_LIBRARIES
        DTRENDER_LIBRARY
        DTCORE_LIBRARY
        OSG_LIBRARY
        OSGTEXT_LIBRARY
        OSGDB_LIBRARY
        OSGGA_LIBRARY
        OSGVIEWER_LIBRARY
        OSGPARTICLE_LIBRARY
        OSGUTIL_LIBRARY
        OSGPPU_LIBRARY
        XERCES_LIBRARY
        )

# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dtRender DEFAULT_MSG DELTA3D_INCLUDE_DIR     DTRENDER_LIBRARY
                                  )
