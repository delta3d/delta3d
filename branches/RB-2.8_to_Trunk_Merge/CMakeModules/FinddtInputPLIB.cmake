# Locate dtInputPLIB
#

INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DTINPUTPLIB_LIBRARY         dtInputPLIB)

#if (FIND_dtInputPLIB_REQUIRED)
#   FIND_PACKAGE(PLIB REQUIRED)
#elseif(FIND_dtInputPLIB_QUIETLY)
#   FIND_PACKAGE(PLIB QUIETLY)
#else()
#   FIND_PACKAGE(PLIB)
#endif()

#convienent list of libraries to link with when using dtInputPLIB
CREATE_LINK_LINES_FOR_TARGETS(DTINPUTPLIB_LIBRARIES
    DTINPUTPLIB_LIBRARY
#    PLIB_JS_LIBRARY
#    PLIB_UL_LIBRARY
    )
    
SET(DTINPUTPLIB_INCLUDE_DIRECTORIES ${DELTA3D_INCLUDE_DIR}
#                                    ${PLIB_INCLUDE_DIR}
    )


# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dtInputPLIB DEFAULT_MSG DELTA3D_INCLUDE_DIR     DTINPUTPLIB_LIBRARY
#                                                          PLIB_INCLUDE_DIR        PLIB_JS_LIBRARY
#                                                                                  PLIB_UL_LIBRARY
                                  )
