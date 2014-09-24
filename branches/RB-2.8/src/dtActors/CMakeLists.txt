
SET(LIB_NAME     dtActors)


SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/src/${LIB_NAME})

SET(LIB_PUBLIC_HEADERS
    ${HEADER_PATH}/actionactorproxy.h
    ${HEADER_PATH}/autotriggeractorproxy.h
    ${HEADER_PATH}/backdropactor.h
    ${HEADER_PATH}/baselightactorproxy.h
    ${HEADER_PATH}/basewateractor.h
    ${HEADER_PATH}/beziercontrolleractor.h
    ${HEADER_PATH}/beziercontrolleractorcomponent.h
    ${HEADER_PATH}/beziercontrolpointactorproxy.h
    ${HEADER_PATH}/beziernodeactorproxy.h
    ${HEADER_PATH}/buildingactor.h
    ${HEADER_PATH}/cameraactorproxy.h
    ${HEADER_PATH}/cameradataactor.h    
    ${HEADER_PATH}/clouddomeactorproxy.h
    ${HEADER_PATH}/cloudplaneactorproxy.h
    ${HEADER_PATH}/coordinateconfigactor.h
    ${HEADER_PATH}/deltaobjectactorproxy.h
    ${HEADER_PATH}/directoractor.h
    ${HEADER_PATH}/distancesensoractor.h
    ${HEADER_PATH}/dynamicparticlesystemactor.h
    ${HEADER_PATH}/engineactorregistry.h
    ${HEADER_PATH}/enveffectactorproxy.h
    ${HEADER_PATH}/fenceactor.h
    ${HEADER_PATH}/gamemeshactor.h
    ${HEADER_PATH}/infinitelightactorproxy.h
    ${HEADER_PATH}/infiniteterrainactorproxy.h
    ${HEADER_PATH}/labelactorproxy.h
    ${HEADER_PATH}/linkedpointsactorproxy.h
    ${HEADER_PATH}/meshterrainactorproxy.h
    ${HEADER_PATH}/motionactionactorproxy.h
    ${HEADER_PATH}/particlesystemactorproxy.h
    ${HEADER_PATH}/playerstartactorproxy.h
    ${HEADER_PATH}/positionallightactorproxy.h
    ${HEADER_PATH}/positiondataactor.h
    ${HEADER_PATH}/prefabactorproxy.h
    ${HEADER_PATH}/proximitytriggeractorproxy.h
    ${HEADER_PATH}/skyboxactorproxy.h
    ${HEADER_PATH}/skydomeactorproxy.h
    ${HEADER_PATH}/skydomeenvironmentactor.h
    ${HEADER_PATH}/spotlightactorproxy.h
    ${HEADER_PATH}/staticmeshactorproxy.h
    ${HEADER_PATH}/taskactor.h
    ${HEADER_PATH}/taskactorgameevent.h
    ${HEADER_PATH}/taskactorordered.h
    ${HEADER_PATH}/taskactorrollup.h
    ${HEADER_PATH}/triggeractorproxy.h
    ${HEADER_PATH}/triggervolumeactor.h
    ${HEADER_PATH}/triggervolumeactorproxy.h
    ${HEADER_PATH}/tripodactorproxy.h
    ${HEADER_PATH}/volumeeditactor.h
    ${HEADER_PATH}/watergridactor.h
    ${HEADER_PATH}/watergridbuilder.h
    ${HEADER_PATH}/waypointvolumeactor.h
    ${HEADER_PATH}/weatherenvironmentactor.h
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
    ${SOURCE_PATH}/deltaobjectactorproxy.cpp
    ${SOURCE_PATH}/directoractor.cpp
    ${SOURCE_PATH}/distancesensoractor.cpp
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
    ${SOURCE_PATH}/prefabactorproxy.cpp
    ${SOURCE_PATH}/proximitytriggeractorproxy.cpp
    ${SOURCE_PATH}/skyboxactorproxy.cpp
    ${SOURCE_PATH}/skydomeactorproxy.cpp
    ${SOURCE_PATH}/skydomeenvironmentactor.cpp
    ${SOURCE_PATH}/spotlightactorproxy.cpp
    ${SOURCE_PATH}/staticmeshactorproxy.cpp
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

