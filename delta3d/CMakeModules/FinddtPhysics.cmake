# Locate dtPhysics
#

INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DTPHYSICS_LIBRARY         dtPhysics)

if (FIND_dtPhysics_REQUIRED)
   FIND_PACKAGE(Pal REQUIRED)
elseif(FIND_dtPhysics_QUIETLY)
   FIND_PACKAGE(Pal QUIETLY)
else()
   FIND_PACKAGE(Pal )
endif()

#convienent list of libraries to link with when using dtPhysics
CREATE_LINK_LINES_FOR_TARGETS(DTPHYSICS_LIBRARIES
    DTPHYSICS_LIBRARY
    PAL_LIBRARY
    )
    
SET(DTPHYSICS_INCLUDE_DIRECTORIES ${DELTA3D_INCLUDE_DIR} ${PAL_INCLUDE_DIR}
    )


# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dtPhysics DEFAULT_MSG DELTA3D_INCLUDE_DIR   PAL_LIBRARY PAL_INCLUDE_DIR DTPHYSICS_LIBRARY
                                  )
