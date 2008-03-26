
SET(LIB_NAME     dtGame)

ADD_DEFINITIONS(-DNOMINMAX)

IF (WIN32)
ELSE (WIN32) 
ENDIF (WIN32)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/dtGame)
SET(LIB_PUBLIC_HEADERS
   ${HEADER_PATH}/actorupdatemessage.h
   ${HEADER_PATH}/baseinputcomponent.h
   ${HEADER_PATH}/basemessages.h
   ${HEADER_PATH}/binarylogstream.h
   ${HEADER_PATH}/deadreckoningcomponent.h
   ${HEADER_PATH}/deadreckoninghelper.h
   ${HEADER_PATH}/defaultmessageprocessor.h
   ${HEADER_PATH}/defaultnetworkpublishingcomponent.h
   ${HEADER_PATH}/dtgame.h
   ${HEADER_PATH}/environmentactor.h
   ${HEADER_PATH}/exceptionenum.h
   ${HEADER_PATH}/export.h
   ${HEADER_PATH}/gameactor.h
   ${HEADER_PATH}/gameapplication.h
   ${HEADER_PATH}/gameentrypoint.h
   ${HEADER_PATH}/gamemanager.h
   ${HEADER_PATH}/gmcomponent.h
   ${HEADER_PATH}/gmstatistics.h
   ${HEADER_PATH}/groundclamper.h
   ${HEADER_PATH}/invokable.h
   ${HEADER_PATH}/logcontroller.h
   ${HEADER_PATH}/loggermessages.h
   ${HEADER_PATH}/logkeyframe.h
   ${HEADER_PATH}/logkeyframeindex.h
   ${HEADER_PATH}/logstatus.h
   ${HEADER_PATH}/logstream.h
   ${HEADER_PATH}/logtag.h
   ${HEADER_PATH}/logtagindex.h
   ${HEADER_PATH}/machineinfo.h
   ${HEADER_PATH}/mapchangestatedata.h
   ${HEADER_PATH}/message.h
   ${HEADER_PATH}/messagefactory.h
   ${HEADER_PATH}/messageparameter.h
   ${HEADER_PATH}/messagetype.h
   ${HEADER_PATH}/serverloggercomponent.h
   ${HEADER_PATH}/taskcomponent.h
)

ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS}
actorupdatemessage.cpp
baseinputcomponent.cpp
basemessages.cpp
binarylogstream.cpp
deadreckoningcomponent.cpp
deadreckoninghelper.cpp
defaultmessageprocessor.cpp
defaultnetworkpublishingcomponent.cpp
environmentactor.cpp
exceptionenum.cpp
gameactor.cpp
gameapplication.cpp
gamemanager.cpp
gmcomponent.cpp
gmstatistics.cpp
groundclamper.cpp
invokable.cpp
logcontroller.cpp
loggermessages.cpp
logkeyframeindex.cpp
logstatus.cpp
logstream.cpp
logtagindex.cpp
mapchangestatedata.cpp
message.cpp
messagefactory.cpp
messagetype.cpp
serverloggercomponent.cpp
taskcomponent.cpp
)

TARGET_LINK_LIBRARIES(${LIB_NAME}
												optimized dtUtil debug dtUtild
												optimized dtCore debug dtCored
												optimized dtABC debug dtABCd
												optimized osgSim debug osgSimd
                                                 ${CARBON_LIBRARY})

#TARGET_LINK_LIBRARIES(${LIB_NAME} dtUtil dtCore dtAI dtDAL dtABC ${OPENGL_LIBRARY} ${MATH_LIBRARY}
  # ${OSG_LIBRARY} ${OSGDB_LIBRARY} ${OSGGA_LIBRARY} ${OSGPARTICLE_LIBRARY} ${OSGUTIL_LIBRARY}
   #${OSGTEXT_LIBRARY} ${OSGSIM_LIBRARY} ${OSGFX_LIBRARY} ${OSGVIEWER_LIBRARY}
   #${OPENTHREADS_LIBRARY} ${XERCES_LIBRARY} )

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_GAME_LIBRARY)


INCLUDE(ModuleInstall OPTIONAL)
