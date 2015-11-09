
SET(LIB_NAME     dtAI)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/src/${LIB_NAME})

file(GLOB LIB_PUBLIC_HEADERS
   "${HEADER_PATH}/*.h"
   )

SET(LIB_SOURCES
    ${SOURCE_PATH}/aiactorregistry.cpp
    ${SOURCE_PATH}/aidebugdrawable.cpp
    ${SOURCE_PATH}/aiinterfaceactor.cpp
    ${SOURCE_PATH}/aiparticlemanager.cpp
    ${SOURCE_PATH}/aiplugininterface.cpp
    ${SOURCE_PATH}/baseaicomponent.cpp
    ${SOURCE_PATH}/basenpc.cpp
    ${SOURCE_PATH}/CMakeLists.txt
    ${SOURCE_PATH}/deltaaiinterface.cpp
    ${SOURCE_PATH}/fsm.cpp
    ${SOURCE_PATH}/navmesh.cpp
    ${SOURCE_PATH}/npcevent.cpp
    ${SOURCE_PATH}/npcparser.cpp
    ${SOURCE_PATH}/npcstate.cpp
    ${SOURCE_PATH}/operator.cpp
    ${SOURCE_PATH}/planner.cpp
    ${SOURCE_PATH}/plannerhelper.cpp
    ${SOURCE_PATH}/waypoint.cpp
    ${SOURCE_PATH}/waypointcollection.cpp
    ${SOURCE_PATH}/waypointgraph.cpp
    ${SOURCE_PATH}/waypointgraphastar.cpp
    ${SOURCE_PATH}/waypointgraphbuilder.cpp
    ${SOURCE_PATH}/waypointinterface.cpp
    ${SOURCE_PATH}/waypointmanager.cpp
    ${SOURCE_PATH}/waypointpair.cpp
    ${SOURCE_PATH}/waypointreaderwriter.cpp
    ${SOURCE_PATH}/waypointrenderinfo.cpp
    ${SOURCE_PATH}/waypointtypes.cpp
    ${SOURCE_PATH}/worldstate.cpp
)

SET(LIB_DEPS
             ${DTUTIL_LIBRARY}
             ${DTCORE_LIBRARY}
             ${DTGAME_LIBRARY}
             ${OPENGL_LIBRARY}
             )

DELTA3D_ADD_LIBRARY(${LIB_NAME} DT_AI_LIBRARY SHARED)
