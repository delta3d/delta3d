SET(LIB_NAME     dtVR)


SET(HEADER_PATH ${CMAKE_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${CMAKE_SOURCE_DIR}/src/${LIB_NAME})

file(GLOB LIB_PUBLIC_HEADERS
   "${HEADER_PATH}/*.h"
   )

SET( LIB_SOURCES
  openvractorregistry.cpp  
  openvrscene.cpp
  
) 

INCLUDE_DIRECTORIES( ${OSG_OPENVR_VIEWER_INCLUDE_DIR}) 

SET(LIB_DEPS 
    ${DTUTIL_LIBRARY}
    ${DTCORE_LIBRARY}
    ${DTRENDER_LIBRARY}
    ${OPENVDB_LIBRARIES}
)


SET(LIB_EXTERNAL_DEPS
   OSG_OPENVR_VIEWER_LIBRARY 
)


if (MSVC)
   # PCH doesn't help with dtUtil on g++.
   # no pch on this library yet, though it may eventually help because of all the openvdb templates.
   #ADD_PRECOMPILED_HEADER(${LIB_NAME} prefix/dtutilprefix.h prefix/dtutilprefix.cpp LIB_SOURCES)
endif (MSVC)

DELTA3D_ADD_LIBRARY(${LIB_NAME} DT_VR_LIBRARY SHARED)
