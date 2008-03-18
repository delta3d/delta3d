
SET(LIB_NAME     dtAI)


IF (WIN32)
ELSE (WIN32) 
ENDIF (WIN32)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/dtAI)
SET(LIB_PUBLIC_HEADERS
   ${HEADER_PATH}/aiparticlemanager.h
   ${HEADER_PATH}/astar.h
   ${HEADER_PATH}/astarconfig.h
   ${HEADER_PATH}/astarcostfunc.h
   ${HEADER_PATH}/astarnode.h
   ${HEADER_PATH}/astarwaypointutils.h
   ${HEADER_PATH}/basenpc.h
   ${HEADER_PATH}/basenpcutils.h
   ${HEADER_PATH}/conditional.h
   ${HEADER_PATH}/distancesensor.h
   ${HEADER_PATH}/export.h
   ${HEADER_PATH}/fsm.h
   ${HEADER_PATH}/navmesh.h
   ${HEADER_PATH}/npcevent.h
   ${HEADER_PATH}/npcparser.h
   ${HEADER_PATH}/npcstate.h
   ${HEADER_PATH}/operator.h
   ${HEADER_PATH}/planner.h
   ${HEADER_PATH}/plannerconfig.h
   ${HEADER_PATH}/plannerhelper.h
   ${HEADER_PATH}/plannernodelink.h
   ${HEADER_PATH}/sensor.h
   ${HEADER_PATH}/statevariable.h
   ${HEADER_PATH}/steeringbehavior.h
   ${HEADER_PATH}/steeringutility.h
   ${HEADER_PATH}/waypoint.h
   ${HEADER_PATH}/waypointactor.h
   ${HEADER_PATH}/waypointmanager.h
   ${HEADER_PATH}/waypointpair.h
   ${HEADER_PATH}/worldstate.h
)

ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS} 
   aiparticlemanager.cpp
   basenpc.cpp
   fsm.cpp
   navmesh.cpp
   npcevent.cpp
   npcparser.cpp
   npcstate.cpp
   operator.cpp
   planner.cpp
   plannerhelper.cpp
   waypoint.cpp
   waypointactor.cpp
   waypointmanager.cpp
   waypointpair.cpp
   worldstate.cpp
)

TARGET_LINK_LIBRARIES(${LIB_NAME} dtUtil dtCore ${MATH_LIBRARY}
   ${OSG_LIBRARY} ${OSGDB_LIBRARY} )

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_AI_LIBRARY)


INCLUDE(ModuleInstall OPTIONAL)
