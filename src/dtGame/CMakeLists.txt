
SET(LIB_NAME     dtGame)

IF (WIN32)
ADD_DEFINITIONS(-DNOMINMAX)
ENDIF (WIN32)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/src/${LIB_NAME})


file(GLOB LIB_PUBLIC_HEADERS
   "${HEADER_PATH}/*.h"
   )

SET(LIB_SOURCES
    ${SOURCE_PATH}/actorcomponent.cpp
    ${SOURCE_PATH}/actorcomponentbase.cpp
    ${SOURCE_PATH}/actorupdatemessage.cpp
    ${SOURCE_PATH}/basegroundclamper.cpp
    ${SOURCE_PATH}/baseinputcomponent.cpp
    ${SOURCE_PATH}/basemessages.cpp
    ${SOURCE_PATH}/binarylogstream.cpp
    ${SOURCE_PATH}/cascadingdeleteactorcomponent.cpp
    ${SOURCE_PATH}/componenttypestatics.cpp
    ${SOURCE_PATH}/deadreckoningcomponent.cpp
    ${SOURCE_PATH}/deadreckoninghelper.cpp
    ${SOURCE_PATH}/defaultgameentrypoint.cpp
    ${SOURCE_PATH}/defaultgroundclamper.cpp
    ${SOURCE_PATH}/defaultmessageprocessor.cpp
    ${SOURCE_PATH}/defaultnetworkpublishingcomponent.cpp
    ${SOURCE_PATH}/drpublishingactcomp.cpp
    ${SOURCE_PATH}/environmentactor.cpp
    ${SOURCE_PATH}/exceptionenum.cpp
    ${SOURCE_PATH}/gameactor.cpp
    ${SOURCE_PATH}/gameactorproxy.cpp
    ${SOURCE_PATH}/gameapplication.cpp
    ${SOURCE_PATH}/gameentrypoint.cpp
    ${SOURCE_PATH}/gamemanager.cpp
    ${SOURCE_PATH}/gamestate.cpp
    ${SOURCE_PATH}/gamestatecomponent.cpp
    ${SOURCE_PATH}/gamestatemessages.cpp
    ${SOURCE_PATH}/gmcomponent.cpp
    ${SOURCE_PATH}/gmsettings.cpp
    ${SOURCE_PATH}/gmimpl.cpp
    ${SOURCE_PATH}/gmstatistics.cpp
    ${SOURCE_PATH}/invokable.cpp
    ${SOURCE_PATH}/logcontroller.cpp
    ${SOURCE_PATH}/loggermessages.cpp
    ${SOURCE_PATH}/logkeyframeindex.cpp
    ${SOURCE_PATH}/logstatus.cpp
    ${SOURCE_PATH}/logstream.cpp
    ${SOURCE_PATH}/logtagindex.cpp
    ${SOURCE_PATH}/machineinfo.cpp
    ${SOURCE_PATH}/mapchangestatedata.cpp
    ${SOURCE_PATH}/message.cpp
    ${SOURCE_PATH}/messagefactory.cpp
    ${SOURCE_PATH}/messagetype.cpp
    ${SOURCE_PATH}/serverloggercomponent.cpp
    ${SOURCE_PATH}/shaderactorcomponent.cpp
    ${SOURCE_PATH}/taskcomponent.cpp
    ${SOURCE_PATH}/transitionxmlhandler.cpp
)

ADD_PRECOMPILED_HEADER(${LIB_NAME} prefix/dtgameprefix.h prefix/dtgameprefix.cpp LIB_SOURCES)


SET(LIB_EXTERNAL_DEPS
     OSGSIM_LIBRARY)

SET(LIB_DEPS
    ${CARBON_LIBRARY}
    ${DTUTIL_LIBRARY}
    ${DTCORE_LIBRARY}
    ${DTABC_LIBRARY}
    )

DELTA3D_ADD_LIBRARY(${LIB_NAME} DT_GAME_LIBRARY)
