# Locate dtUtil
#


INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DTUTIL_LIBRARY         dtUtil)
FIND_DELTA3D_LIBRARY(DTUTIL_DEBUG_LIBRARY   dtUtilD)

DELTA3D_FIND_PATH(OSG osg/PositionAttitudeTransform)
DELTA3D_FIND_LIBRARY(OSG osg)
DELTA3D_FIND_PATH   (OSGDB osgDB/DatabasePager)
DELTA3D_FIND_LIBRARY(OSGDB osgDB)
DELTA3D_FIND_PATH   (OSGSIM osgSim/ImpostorSprite)
DELTA3D_FIND_LIBRARY(OSGSIM osgSim)
DELTA3D_FIND_PATH   (OSGUTIL osgUtil/SceneView)
DELTA3D_FIND_LIBRARY(OSGUTIL osgUtil)
DELTA3D_FIND_PATH   (OPENTHREADS OpenThreads/Thread)
DELTA3D_FIND_LIBRARY(OPENTHREADS OpenThreads)

SET(XERCES_NAMES xerces-c_2 xerces-c Xerces)
DELTA3D_FIND_PATH   (XERCES xercesc)
DELTA3D_FIND_LIBRARY(XERCES "${XERCES_NAMES}")

IF (NOT DTUTIL_DEBUG_LIBRARY)
  SET(DTUTIL_DEBUG_LIBRARY DTUTIL_LIBRARY)
  MESSAGE(STATUS "No debug library was found for DTUTIL_DEBUG_LIBRARY")
ENDIF()

#convienent list of libraries to link with when using dtUtil
SET(DTUTIL_LIBRARIES
    optimized ${DTUTIL_LIBRARY}  debug ${DTUTIL_DEBUG_LIBRARY}
    optimized ${OSG_LIBRARY}     debug ${OSG_LIBRARY_DEBUG}
    optimized ${OSGDB_LIBRARY}   debug ${OSGDB_LIBRARY_DEBUG}
    optimized ${OSGSIM_LIBRARY}  debug ${OSGSIM_LIBRARY_DEBUG}
    optimized ${OSGUTIL_LIBRARY} debug ${OSGUTIL_LIBRARY_DEBUG}
    optimized ${OPENTHREADS_LIBRARY} debug ${OPENTHREADS_LIBRARY_DEBUG}
    optimized ${XERCES_LIBRARY}  debug ${XERCES_LIBRARY_DEBUG}
    )
    
SET(DTUTIL_INCLUDE_DIRECTORIES ${DELTA3D_INCLUDE_DIR} 
                               ${OSG_INCLUDE_DIR}
                               ${OSGDB_INCLUDE_DIR}
                               ${OSGSIM_INCLUDE_DIR}
                               ${OSGUTIL_INCLUDE_DIR}
                               ${OPENTHREADS_INCLUDE_DIR}
                               ${XERCES_INCLUDE_DIR}
                               )


# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dtUtil DEFAULT_MSG DELTA3D_INCLUDE_DIR DTUTIL_LIBRARY                                                                               
                                                     OSG_INCLUDE_DIR OSG_LIBRARY
                                                     OSGDB_INCLUDE_DIR OSGDB_LIBRARY
                                                     OSGSIM_INCLUDE_DIR OSGSIM_LIBRARY 
                                                     OSGUTIL_INCLUDE_DIR OSGUTIL_LIBRARY 
                                                     OPENTHREADS_INCLUDE_DIR OPENTHREADS_LIBRARY
                                                     XERCES_INCLUDE_DIR XERCES_LIBRARY
                                  )
