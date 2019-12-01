# Locate dtCore
#

INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DTCORE_LIBRARY         dtCore)


set(SUB_PACKAGES OpenThreads OSG XercesC)
IF (NOT WIN32 AND NOT APPLE)
   list(APPEND SUB_PACKAGES UUID)
ENDIF ()

if (NOT WIN32)
   list(APPEND SUB_PACKAGES Curses)
endif()

foreach(package ${SUB_PACKAGES})
   if (FIND_dtCore_REQUIRED)
      find_package(${package} REQUIRED)
   elseif(FIND_dtCore_QUIETLY)
      find_package(${package} QUIETLY)
   else()
      find_package(${package})
   endif()
endforeach()

#convienent list of libraries to link with when using dtCore
CREATE_LINK_LINES_FOR_TARGETS(DTCORE_LIBRARIES 
   DTCORE_LIBRARY
   OSG_LIBRARY
   OSGTEXT_LIBRARY
   OSGDB_LIBRARY
   OSGGA_LIBRARY
   OSGVIEWER_LIBRARY
   OSGPARTICLE_LIBRARY
   OSGUTIL_LIBRARY
   #ODE_LIBRARY
   XERCES_LIBRARY
)
    
SET(DTCORE_INCLUDE_DIRECTORIES ${DELTA3D_INCLUDE_DIR}
                               ${OSG_INCLUDE_DIR}
                               #${ODE_INCLUDE_DIR}
                               ${OPENGL_INCLUDE_DIR}
    )


# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dtCore DEFAULT_MSG DELTA3D_INCLUDE_DIR     DTCORE_LIBRARY
                                                     OSGTEXT_LIBRARY
                                                     OSGGA_LIBRARY
                                                     OSGVIEWER_LIBRARY
                                                     OSGPARTICLE_LIBRARY
                                                     #ODE_INCLUDE_DIR         ODE_LIBRARY
                                                     XERCES_INCLUDE_DIR XERCES_LIBRARY
                                  )
