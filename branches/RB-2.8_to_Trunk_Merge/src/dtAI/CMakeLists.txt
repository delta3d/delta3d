
SET(LIB_NAME     dtAI)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/src/${LIB_NAME})

SET(LIB_PUBLIC_HEADERS
    ${HEADER_PATH}/aiactorregistry.h
    ${HEADER_PATH}/aidebugdrawable.h
    ${HEADER_PATH}/aiinterfaceactor.h
    ${HEADER_PATH}/aiplugininterface.h
    ${HEADER_PATH}/astar.h
    ${HEADER_PATH}/astar.inl
    ${HEADER_PATH}/astarconfig.h
    ${HEADER_PATH}/astarcostfunc.h
    ${HEADER_PATH}/astarnode.h
    ${HEADER_PATH}/astarwaypointutils.h
    ${HEADER_PATH}/baseaicomponent.h
    ${HEADER_PATH}/basenpc.h
    ${HEADER_PATH}/basenpcutils.h
    ${HEADER_PATH}/conditional.h
    ${HEADER_PATH}/controllable.h
    ${HEADER_PATH}/deltaaiinterface.h
    ${HEADER_PATH}/distancesensor.h
    ${HEADER_PATH}/dtai.h
    ${HEADER_PATH}/export.h
    ${HEADER_PATH}/fsm.h
    ${HEADER_PATH}/navmesh.h
    ${HEADER_PATH}/npcparser.h
    ${HEADER_PATH}/npcstate.h
    ${HEADER_PATH}/operator.h
    ${HEADER_PATH}/pathfinding.h
    ${HEADER_PATH}/planner.h
    ${HEADER_PATH}/plannerconfig.h
    ${HEADER_PATH}/plannerhelper.h
    ${HEADER_PATH}/plannernodelink.h
    ${HEADER_PATH}/primitives.h
    ${HEADER_PATH}/sensor.h
    ${HEADER_PATH}/statevariable.h
    ${HEADER_PATH}/steeringbehavior.h
    ${HEADER_PATH}/steeringpipeline.h
    ${HEADER_PATH}/steeringutility.h
    ${HEADER_PATH}/tree.h
    ${HEADER_PATH}/waypoint.h
    ${HEADER_PATH}/waypointcollection.h
    ${HEADER_PATH}/waypointgraph.h
    ${HEADER_PATH}/waypointgraphastar.h
    ${HEADER_PATH}/waypointgraphbuilder.h
    ${HEADER_PATH}/waypointinterface.h
    ${HEADER_PATH}/waypointmanager.h
    ${HEADER_PATH}/waypointpair.h
    ${HEADER_PATH}/waypointpropertycache.h
    ${HEADER_PATH}/waypointpropertycontainer.h
    ${HEADER_PATH}/waypointreaderwriter.h
    ${HEADER_PATH}/waypointrenderinfo.h
    ${HEADER_PATH}/waypointtypes.h
    ${HEADER_PATH}/worldstate.h
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
