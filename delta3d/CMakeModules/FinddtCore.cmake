# Locate dtCore
#

INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DTCORE_LIBRARY         dtCore)
FIND_DELTA3D_LIBRARY(DTCORE_DEBUG_LIBRARY   dtCoreD)

DELTA3D_FIND_PATH   (OSGTEXT osgText/Text)
DELTA3D_FIND_LIBRARY(OSGTEXT osgText)
DELTA3D_FIND_PATH   (OSGGA osgGA/FlightManipulator)
DELTA3D_FIND_LIBRARY(OSGGA osgGA)
DELTA3D_FIND_PATH   (OSGVIEWER osgViewer/Viewer)
DELTA3D_FIND_LIBRARY(OSGVIEWER osgViewer)
DELTA3D_FIND_PATH   (OSGPARTICLE osgParticle/FireEffect)
DELTA3D_FIND_LIBRARY(OSGPARTICLE osgParticle)

SET(ODE_NAMES ode ode_single)
DELTA3D_FIND_PATH   (ODE ode/ode.h)
DELTA3D_FIND_LIBRARY(ODE "${ODE_NAMES}")

#TODO FIND UUID?

IF (NOT DTCORE_DEBUG_LIBRARY)
  SET(DTCORE_DEBUG_LIBRARY DTCORE_LIBRARY)
  MESSAGE(STATUS "No debug library was found for DTCORE_DEBUG_LIBRARY")
ENDIF()

#convienent list of libraries to link with when using dtCore
SET(DTCORE_LIBRARIES
    optimized ${DTCORE_LIBRARY}      debug ${DTCORE_DEBUG_LIBRARY}
    optimized ${OSGTEXT_LIBRARY}     debug ${OSGTEXT_LIBRARY_DEBUG}
    optimized ${OSGGA_LIBRARY}       debug ${OSGGA_LIBRARY_DEBUG}
    optimized ${OSGVIEWER_LIBRARY}   debug ${OSGVIEWER_LIBRARY_DEBUG}
    optimized ${OSGPARTICLE_LIBRARY} debug ${OSGPARTICLE_LIBRARY_DEBUG}
    optimized ${ODE_LIBRARY}         debug ${ODE_LIBRARY_DEBUG}
    )
    
SET(DTCORE_INCLUDE_DIRECTORIES ${DELTA3D_INCLUDE_DIR}
                               ${OSGTEXT_INCLUDE_DIR}
                               ${OSGGA_INCLUDE_DIR}
                               ${OSGVIEWER_INCLUDE_DIR}
                               ${OSGPARTICLE_INCLUDE_DIR}
                               ${ODE_INCLUDE_DIR}
    )


# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dtCore DEFAULT_MSG DELTA3D_INCLUDE_DIR     DTCORE_LIBRARY
                                                     OSGTEXT_INCLUDE_DIR     OSGTEXT_LIBRARY
                                                     OSGGA_INCLUDE_DIR       OSGGA_LIBRARY
                                                     OSGVIEWER_INCLUDE_DIR   OSGVIEWER_LIBRARY
                                                     OSGPARTICLE_INCLUDE_DIR OSGPARTICLE_LIBRARY
                                                     ODE_INCLUDE_DIR         ODE_LIBRARY
                                  )
