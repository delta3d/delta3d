# Locate dtInspectorQt
# Note: This find does not include finding Qt, which dtInspectorQt is dependant on.
#

INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DTINSPECTORQT_LIBRARY         dtInspectorQt)

#convienent list of libraries to link with when using dtInspectorQt
CREATE_LINK_LINES_FOR_TARGETS(DTINSPECTORQT_LIBRARIES
    DTINSPECTORQT_LIBRARY
    )
    
SET(DTINSPECTORQT_INCLUDE_DIRECTORIES ${DELTA3D_INCLUDE_DIR}
    )


# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dtInspectorQt DEFAULT_MSG DELTA3D_INCLUDE_DIR DTINSPECTORQT_LIBRARY
                                  )
