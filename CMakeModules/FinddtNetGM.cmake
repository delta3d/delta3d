INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DTNETGM_LIBRARY         dtNetGM)

if (FIND_dtNetGM_REQUIRED)
   FIND_PACKAGE(HAWKNL REQUIRED)
   FIND_PACKAGE(GNE REQUIRED)
   FIND_PACKAGE(Boost REQUIRED)
elseif(FIND_dtNetGM_QUIETLY)
   FIND_PACKAGE(HAWKNL QUIETLY)
   FIND_PACKAGE(GNE QUIETLY)
   FIND_PACKAGE(Boost QUIETLY)
else()
   FIND_PACKAGE(HAWKNL )
   FIND_PACKAGE(GNE )
   FIND_PACKAGE(Boost )
endif()


#convienent list of libraries to link with when using dtNetGM
CREATE_LINK_LINES_FOR_TARGETS(DTNETGM_LIBRARIES
    DTNETGM_LIBRARY
    GNE_LIBRARY
    HAWKNL_LIBRARY
    )
    
SET(DTNETGM_INCLUDE_DIRECTORIES ${DELTA3D_INCLUDE_DIR}
                                ${GNE_INCLUDE_DIR}
                                ${HAWKNL_INCLUDE_DIR}
                                ${Boost_INCLUDE_DIR}
    )


# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dtNetGM DEFAULT_MSG DELTA3D_INCLUDE_DIR DTNETGM_LIBRARY
                                                      GNE_INCLUDE_DIR     GNE_LIBRARY
                                                      HAWKNL_INCLUDE_DIR  HAWKNL_LIBRARY
                                                      Boost_INCLUDE_DIR
                                  )
