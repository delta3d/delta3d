# Locate dtUtil
#


INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DTUTIL_LIBRARY         dtUtil)

if (FIND_dtUtil_REQUIRED)
   FIND_PACKAGE(OpenThreads REQUIRED)
   FIND_PACKAGE(OSG REQUIRED)
   FIND_PACKAGE(XercesC REQUIRED)
elseif(FIND_dtUtil_QUIETLY)
   FIND_PACKAGE(OpenThreads QUIETLY)
   FIND_PACKAGE(OSG QUIETLY)
   FIND_PACKAGE(XercesC QUIETLY)
else()
   FIND_PACKAGE(OpenThreads )
   FIND_PACKAGE(OSG )
   FIND_PACKAGE(XercesC )
endif()

#convienent list of libraries to link with when using dtUtil
CREATE_LINK_LINES_FOR_TARGETS(DTUTIL_LIBRARIES
    DTUTIL_LIBRARY
    OSG_LIBRARY
    OSGDB_LIBRARY
    OSGSIM_LIBRARY
    OSGUTIL_LIBRARY
    OPENTHREADS_LIBRARY
    XERCES_LIBRARY
    )
    
SET(DTUTIL_INCLUDE_DIRECTORIES ${DELTA3D_INCLUDE_DIR} 
                               ${OSG_INCLUDE_DIR}
                               ${OPENTHREADS_INCLUDE_DIR}
                               ${XERCES_INCLUDE_DIR}
                               )


# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dtUtil DEFAULT_MSG DELTA3D_INCLUDE_DIR DTUTIL_LIBRARY                                                                               
                                                     OSG_INCLUDE_DIR OSG_LIBRARY
                                                     OSGDB_LIBRARY
                                                     OSGSIM_LIBRARY 
                                                     OSGUTIL_LIBRARY 
                                                     OPENTHREADS_INCLUDE_DIR OPENTHREADS_LIBRARY
                                                     XERCES_INCLUDE_DIR XERCES_LIBRARY
                                  )
