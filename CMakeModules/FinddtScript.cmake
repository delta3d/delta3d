# Locate dtScript
# Note: This Find does not include finding Python, which dtScript depends on

INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DTSCRIPT_LIBRARY         dtScript)

#convienent list of libraries to link with when using dtScript
CREATE_LINK_LINES_FOR_TARGETS(DTSCRIPT_LIBRARIES
    DTSCRIPT_LIBRARY
    )
    
SET(DTSCRIPT_INCLUDE_DIRECTORIES ${DELTA3D_INCLUDE_DIR}
    )


# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dtScript DEFAULT_MSG DELTA3D_INCLUDE_DIR DTSCRIPT_LIBRARY DTSCRIPT_DEBUG_LIBRARY
                                  )
