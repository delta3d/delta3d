
SET(LIB_NAME     dtActors)


SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/src/${LIB_NAME})

file(GLOB LIB_PUBLIC_HEADERS 
   "${HEADER_PATH}/*.h"
   )

SET(LIB_SOURCES
    ${SOURCE_PATH}/actionactorproxy.cpp
    ${SOURCE_PATH}/autotriggeractorproxy.cpp
    ${SOURCE_PATH}/backdropactor.cpp
    ${SOURCE_PATH}/baselightactorproxy.cpp
    ${SOURCE_PATH}/basewateractor.cpp
    ${SOURCE_PATH}/beziercontrolleractor.cpp
    ${SOURCE_PATH}/beziercontrolleractorcomponent.cpp
    ${SOURCE_PATH}/beziercontrolpointactorproxy.cpp
    ${SOURCE_PATH}/beziernodeactorproxy.cpp
    ${SOURCE_PATH}/buildingactor.cpp
    ${SOURCE_PATH}/cameraactorproxy.cpp
    ${SOURCE_PATH}/cameradataactor.cpp
    ${SOURCE_PATH}/clouddomeactorproxy.cpp
    ${SOURCE_PATH}/cloudplaneactorproxy.cpp
    ${SOURCE_PATH}/coordinateconfigactor.cpp
    ${SOURCE_PATH}/directoractor.cpp
    ${SOURCE_PATH}/distancesensoractor.cpp
	${SOURCE_PATH}/dynamicparticlesactorcomponent.cpp
    ${SOURCE_PATH}/dynamicparticlesystemactor.cpp
    ${SOURCE_PATH}/engineactorregistry.cpp
    ${SOURCE_PATH}/enveffectactorproxy.cpp
    ${SOURCE_PATH}/fenceactor.cpp
    ${SOURCE_PATH}/gamemeshactor.cpp
    ${SOURCE_PATH}/infinitelightactorproxy.cpp
    ${SOURCE_PATH}/infiniteterrainactorproxy.cpp
    ${SOURCE_PATH}/labelactorproxy.cpp
    ${SOURCE_PATH}/linkedpointsactorproxy.cpp
    ${SOURCE_PATH}/meshterrainactorproxy.cpp
    ${SOURCE_PATH}/motionactionactorproxy.cpp
    ${SOURCE_PATH}/particlesystemactorproxy.cpp
    ${SOURCE_PATH}/playerstartactorproxy.cpp
    ${SOURCE_PATH}/positionallightactorproxy.cpp
    ${SOURCE_PATH}/positiondataactor.cpp
    ${SOURCE_PATH}/prefabactor.cpp
    ${SOURCE_PATH}/proximitytriggeractorproxy.cpp
    ${SOURCE_PATH}/skyboxactorproxy.cpp
    ${SOURCE_PATH}/skydomeactorproxy.cpp
    ${SOURCE_PATH}/skydomeenvironmentactor.cpp
    ${SOURCE_PATH}/spotlightactorproxy.cpp
    ${SOURCE_PATH}/taskactor.cpp
    ${SOURCE_PATH}/taskactorgameevent.cpp
    ${SOURCE_PATH}/taskactorordered.cpp
    ${SOURCE_PATH}/taskactorrollup.cpp
    ${SOURCE_PATH}/triggeractorproxy.cpp
    ${SOURCE_PATH}/triggervolumeactor.cpp
    ${SOURCE_PATH}/triggervolumeactorproxy.cpp
    ${SOURCE_PATH}/tripodactorproxy.cpp
    ${SOURCE_PATH}/volumeeditactor.cpp
    ${SOURCE_PATH}/watergridactor.cpp
    ${SOURCE_PATH}/watergridbuilder.cpp
    ${SOURCE_PATH}/waypointvolumeactor.cpp
    ${SOURCE_PATH}/weatherenvironmentactor.cpp
)

SET(LIB_DEPS 
     ${DTGAME_LIBRARY}
     ${DTAI_LIBRARY}
     ${DTDIRECTOR_LIBRARY}
)

DELTA3D_ADD_LIBRARY(${LIB_NAME} DT_PLUGIN SHARED)

