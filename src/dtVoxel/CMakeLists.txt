SET(LIB_NAME     dtVoxel)


SET(HEADER_PATH ${CMAKE_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${CMAKE_SOURCE_DIR}/src/${LIB_NAME})

file(GLOB LIB_PUBLIC_HEADERS
   "${HEADER_PATH}/*.h"
   )

SET( LIB_SOURCES
  marchingcubes.cpp
  createmeshtask.cpp
  physicstesselationmode.cpp
  readovdbthreadpooltask.cpp
  volumescene.cpp
  volumeupdatemessage.cpp
  voxelactor.cpp
  voxelactorregistry.cpp
  voxelcell.cpp
  voxelblock.cpp
  voxelgrid.cpp
  voxelgeometry.cpp
  voxelgriddebugdrawable.cpp
  voxelmessagetype.cpp
) 

INCLUDE_DIRECTORIES(${OPENEXR_INCLUDE_DIR} ${TBB_INCLUDE_DIR} ${OPENVDB_INCLUDE_DIR} ${PAL_INCLUDE_DIR}) 

SET(LIB_DEPS 
    ${DTUTIL_LIBRARY}
    ${DTCORE_LIBRARY}
    ${DTRENDER_LIBRARY}
    ${DTPHYSICS_LIBRARY}
    ${TBB_LIBRARIES}
    ${OPENEXR_Half_LIBRARY}
    ${OPENVDB_LIBRARIES}
)


SET(LIB_EXTERNAL_DEPS
   OSGVOLUME_LIBRARY 
)


if (MSVC)
   # PCH doesn't help with dtUtil on g++.
   # no pch on this library yet, though it may eventually help because of all the openvdb templates.
   #ADD_PRECOMPILED_HEADER(${LIB_NAME} prefix/dtutilprefix.h prefix/dtutilprefix.cpp LIB_SOURCES)
endif (MSVC)

DELTA3D_ADD_LIBRARY(${LIB_NAME} DT_VOXEL_LIBRARY SHARED)
