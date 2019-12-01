# Locate dtDirector
#

INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DTDIRECTOR_LIBRARY         dtDirector)


CREATE_LINK_LINES_FOR_TARGETS(DTDIRECTOR_LIBRARIES
        DTDIRECTOR_LIBRARY
        )
    
SET(DTDIRECTOR_INCLUDE_DIRECTORIES ${DELTA3D_INCLUDE_DIR}
    )


# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dtDirector DEFAULT_MSG DELTA3D_INCLUDE_DIR     DTDIRECTOR_LIBRARY
                                  )
