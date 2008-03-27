
SET(LIB_NAME     dtTerrain)

INCLUDE_DIRECTORIES(${GDAL_INCLUDE_DIR})

IF (WIN32)
ELSE (WIN32) 
ENDIF (WIN32)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/dtTerrain)
SET(LIB_PUBLIC_HEADERS
   ${HEADER_PATH}/colormapdecorator.h
   ${HEADER_PATH}/dtedterrainreader.h
   ${HEADER_PATH}/fixedpointnoise.h
   ${HEADER_PATH}/geocoordinates.h
   ${HEADER_PATH}/geotiffdecorator.h
   ${HEADER_PATH}/heightfield.h
   ${HEADER_PATH}/imageutils.h
   ${HEADER_PATH}/lccanalyzer.h
   ${HEADER_PATH}/lcctype.h
   ${HEADER_PATH}/mathutils.h
   ${HEADER_PATH}/pagedterraintile.h
   ${HEADER_PATH}/pagedterraintilefactory.h
   ${HEADER_PATH}/soarxdrawable.h
   ${HEADER_PATH}/soarxterrainrenderer.h
   ${HEADER_PATH}/terrain_export.h
   ${HEADER_PATH}/terrain.h
   ${HEADER_PATH}/terraindatareader.h
   ${HEADER_PATH}/terraindatarenderer.h
   ${HEADER_PATH}/terraindatatype.h
   ${HEADER_PATH}/terraindecorationlayer.h
   ${HEADER_PATH}/vegetationdecorator.h
)

ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS} 
   colormapdecorator.cpp
   dtedterrainreader.cpp
   fixedpointnoise.cpp
   geocoordinates.cpp
   geotiffdecorator.cpp
   heightfield.cpp
   imageutils.cpp
   lccanalyzer.cpp
   lcctype.cpp
   mathutils.cpp
   pagedterraintile.cpp
   soarxdrawable.cpp
   soarxterrainrenderer.cpp
   terrain.cpp
   terraindatareader.cpp
   terraindatarenderer.cpp
   terraindatatype.cpp
   terraindecorationlayer.cpp
   vegetationdecorator.cpp
)


TARGET_LINK_LIBRARIES(${LIB_NAME} dtUtil dtCore dtDAL ${OPENGL_LIBRARY} ${MATH_LIBRARY}
   ${OSG_LIBRARY} ${OSGDB_LIBRARY} ${CAL3D_LIBRARY}
   ${GDAL_LIBRARY})

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_TERRAIN_LIBRARY)


INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
