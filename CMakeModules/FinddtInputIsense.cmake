# Locate dtInputIsense
#

INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DTINPUTISENSE_LIBRARY         dtInputIsense)

#convienent list of libraries to link with when using dtInputIsense
CREATE_LINK_LINES_FOR_TARGETS(DTINPUTISENSE_LIBRARIES
    DTINPUTISENSE_LIBRARY
    ISENSE_LIBRARY
    )
    
SET(DTINPUTISENSE_INCLUDE_DIRECTORIES ${DELTA3D_INCLUDE_DIR}
    )


# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dtInputIsense DEFAULT_MSG DELTA3D_INCLUDE_DIR     DTINPUTISENSE_LIBRARY
                                  )
