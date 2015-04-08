# Locate dtDIS
#

INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DTDIS_LIBRARY         dtDIS)

if (FIND_dtCore_REQUIRED)
   FIND_PACKAGE(DIS REQUIRED)
elseif(FIND_dtCore_QUIETLY)
   FIND_PACKAGE(DIS QUIETLY)
else()
   FIND_PACKAGE(DIS )
endif()

CREATE_LINK_LINES_FOR_TARGETS(DTDIS_LIBRARIES
        DTDIS_LIBRARY
        DIS_INCLUDE_DIR
        DIS_CPPUTILS_INCLUDE_DIR
        )
    
SET(DTDIS_INCLUDE_DIRECTORIES ${DELTA3D_INCLUDE_DIR}
                               ${DIS_INCLUDE_DIR}
                               ${DIS_CPPUTILS_INCLUDE_DIR}
    )


# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dtDIS DEFAULT_MSG DELTA3D_INCLUDE_DIR     DTDIS_LIBRARY
                                                    DIS_INCLUDE_DIR         DIS_LIBRARY
                                                    DIS_CPPUTILS_INCLUDE_DIR
                                  )
