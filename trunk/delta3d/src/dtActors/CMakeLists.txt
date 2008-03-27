
SET(LIB_NAME     dtActors)


IF (WIN32)
ELSE (WIN32) 
ENDIF (WIN32)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/dtActors)
SET(LIB_PUBLIC_HEADERS
   ${HEADER_PATH}/actionactorproxy.h
   ${HEADER_PATH}/animationgameactor.h
   ${HEADER_PATH}/autotriggeractorproxy.h
   ${HEADER_PATH}/baselightactorproxy.h
   ${HEADER_PATH}/basicenvironmentactorproxy.h
   ${HEADER_PATH}/beziercontrolleractorproxy.h
   ${HEADER_PATH}/beziercontrolpointactorproxy.h
   ${HEADER_PATH}/beziernodeactorproxy.h
   ${HEADER_PATH}/cal3dgameactor.h
   ${HEADER_PATH}/cameraactorproxy.h
   ${HEADER_PATH}/characteractorproxy.h
   ${HEADER_PATH}/clouddomeactorproxy.h
   ${HEADER_PATH}/cloudplaneactorproxy.h
   ${HEADER_PATH}/coordinateconfigactor.h
   ${HEADER_PATH}/deltaobjectactorproxy.h
   ${HEADER_PATH}/distancesensoractor.h
   ${HEADER_PATH}/engineactorregistry.h
   ${HEADER_PATH}/envactor.h
   ${HEADER_PATH}/enveffectactorproxy.h
   ${HEADER_PATH}/gamemeshactor.h
   ${HEADER_PATH}/infinitelightactorproxy.h
   ${HEADER_PATH}/infiniteterrainactorproxy.h
   ${HEADER_PATH}/meshterrainactorproxy.h
   ${HEADER_PATH}/motionactionactorproxy.h
   ${HEADER_PATH}/particlesystemactorproxy.h
   ${HEADER_PATH}/playerstartactorproxy.h
   ${HEADER_PATH}/positionallightactorproxy.h
   ${HEADER_PATH}/proximitytriggeractorproxy.h
   ${HEADER_PATH}/skyboxactorproxy.h
   ${HEADER_PATH}/skydomeactorproxy.h
   ${HEADER_PATH}/soundactorproxy.h
   ${HEADER_PATH}/spotlightactorproxy.h
   ${HEADER_PATH}/staticmeshactorproxy.h
   ${HEADER_PATH}/taskactor.h
   ${HEADER_PATH}/taskactorgameevent.h
   ${HEADER_PATH}/taskactorordered.h
   ${HEADER_PATH}/taskactorrollup.h
   ${HEADER_PATH}/triggeractorproxy.h
   ${HEADER_PATH}/tripodactorproxy.h
)

ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS} 
actionactorproxy.cpp
animationgameactor.cpp
autotriggeractorproxy.cpp
baselightactorproxy.cpp
basicenvironmentactorproxy.cpp
beziercontrolleractorproxy.cpp
beziercontrolpointactorproxy.cpp
beziernodeactorproxy.cpp
cal3dgameactor.cpp
cameraactorproxy.cpp
characteractorproxy.cpp
clouddomeactorproxy.cpp
cloudplaneactorproxy.cpp
coordinateconfigactor.cpp
deltaobjectactorproxy.cpp
distancesensoractor.cpp
engineactorregistry.cpp
envactor.cpp
enveffectactorproxy.cpp
gamemeshactor.cpp
infinitelightactorproxy.cpp
infiniteterrainactorproxy.cpp
meshterrainactorproxy.cpp
motionactionactorproxy.cpp
particlesystemactorproxy.cpp
playerstartactorproxy.cpp
positionallightactorproxy.cpp
proximitytriggeractorproxy.cpp
skyboxactorproxy.cpp
skydomeactorproxy.cpp
soundactorproxy.cpp
spotlightactorproxy.cpp
staticmeshactorproxy.cpp
taskactor.cpp
taskactorgameevent.cpp
taskactorordered.cpp
taskactorrollup.cpp
triggeractorproxy.cpp
tripodactorproxy.cpp
)

LINK_WITH_VARIABLES(${LIB_NAME}
											  DTGAME_LIBRARY
											  DTAUDIO_LIBRARY
											  DTANIM_LIBRARY
											  CAL3D_LIBRARY)

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_PLUGIN)


INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
