# Locate dtDIS
#

INCLUDE(delta3d_common)
  
#variable names of the individual Delta3D libraries.  Can be used in application cmakelist.txt files.
FIND_DELTA3D_LIBRARY(DTDIS_LIBRARY         dtDIS)
FIND_DELTA3D_LIBRARY(DTDIS_DEBUG_LIBRARY   dtDISD)

DELTA3D_FIND_PATH   (DIS DIS/Pdu.h)
DELTA3D_FIND_PATH   (DIS_CPPUTILS DIS/DataStream.h)
DELTA3D_FIND_LIBRARY(DIS DIS) #TODO DIS debug has a funky filename that screws this up



IF (NOT DTDIS_DEBUG_LIBRARY AND NOT MSVC)
  SET(DTDIS_DEBUG_LIBRARY DTDIS_LIBRARY)
  MESSAGE(STATUS "No debug library was found for DTDIS_DEBUG_LIBRARY")
ENDIF(NOT DTDIS_DEBUG_LIBRARY AND NOT MSVC)

#convienent list of libraries to link with when using dtDIS
SET(DTDIS_LIBRARIES
    optimized ${DTDIS_LIBRARY}      debug ${DTDIS_DEBUG_LIBRARY}
    optimized ${DIS_LIBRARY}        debug ${DIS_LIBRARY_DEBUG}
    )
    
SET(DTDIS_INCLUDE_DIRECTORIES ${DELTA3D_INCLUDE_DIR}
                               ${DIS_INCLUDE_DIR}
                               ${DIS_CPPUTILS_INCLUDE_DIR}
    )


# handle the QUIETLY and REQUIRED arguments and set DELTA3D_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dtDIS DEFAULT_MSG DELTA3D_INCLUDE_DIR     DTDIS_LIBRARY DTDIS_DEBUG_LIBRARY
                                                    DIS_INCLUDE_DIR         DIS_LIBRARY
                                                    DIS_CPPUTILS_INCLUDE_DIR
                                  )
