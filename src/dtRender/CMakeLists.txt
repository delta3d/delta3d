SET(LIB_NAME   dtRender)

SET(HEADER_PATH ${CMAKE_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${CMAKE_SOURCE_DIR}/src/${LIB_NAME})

INCLUDE_DIRECTORIES(${OSGPPU_INCLUDE_DIR})

file(GLOB LIB_PUBLIC_HEADERS 
   "${HEADER_PATH}/*.h"
   )

set(LIB_SOURCES 
      atmospherescene.cpp
      cubemapscene.cpp
      dtrenderactorregistry.cpp
      dofscene.cpp
      dynamiclight.cpp
      ephemerisscene.cpp
      graphicsquality.cpp
      guiscene.cpp
      hdrscene.cpp
      lightscene.cpp
      multipassscene.cpp
      oceanscene.cpp
      optimizeractcomp.cpp
      ppuscene.cpp
      parallelsplitshadowmap.cpp
      ssaoscene.cpp
      scenebase.cpp
      sceneenum.cpp
      scenegroup.cpp
      scenemanager.cpp
      scenetype.cpp
      shadowscene.cpp
      simplescene.cpp
      uniformactcomp.cpp
      videoscene.cpp
)

SET(LIB_DEPS 
    ${DTUTIL_LIBRARY}
    ${DTCORE_LIBRARY}
    ${DTABC_LIBRARY}
    ${DTGAME_LIBRARY}
    ${DTACTORS_LIBRARY}
)


SET(LIB_EXTERNAL_DEPS
   OSGPPU_LIBRARY
   OSGSHADOW_LIBRARY
   OSGEPHEMERIS_LIBRARY
   )

DELTA3D_ADD_LIBRARY(${LIB_NAME} DT_RENDER_LIBRARY SHARED)

