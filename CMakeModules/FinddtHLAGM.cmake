# Locate dtHLAGM
# NOTE: The RTI External dependencies are not included with this Find
#

INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DTHLAGM_LIBRARY         dtHLAGM)

#convienent list of libraries to link with when using dtHLAGM
CREATE_LINK_LINES_FOR_TARGETS(DTHLAGM_LIBRARIES
   DTHLAGM_LIBRARY
   )
    
SET(DTHLAGM_INCLUDE_DIRECTORIES ${DELTA3D_INCLUDE_DIR}
    )


# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dtHLAGM DEFAULT_MSG DELTA3D_INCLUDE_DIR     DTHLAGM_LIBRARY
                                  )