
SET(LIB_NAME     dtGame)

IF (WIN32)
ADD_DEFINITIONS(-DNOMINMAX)
ENDIF (WIN32)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/src/${LIB_NAME})

SET(LIB_PUBLIC_HEADERS
    ${HEADER_PATH}/actorcomponent.h
    ${HEADER_PATH}/actorcomponentbase.h
    ${HEADER_PATH}/actorcomponentcontainer.h
    ${HEADER_PATH}/actorupdatemessage.h
    ${HEADER_PATH}/basegroundclamper.h
    ${HEADER_PATH}/baseinputcomponent.h
    ${HEADER_PATH}/basemessages.h
    ${HEADER_PATH}/binarylogstream.h
    ${HEADER_PATH}/cascadingdeleteactorcomponent.h
    ${HEADER_PATH}/deadreckoningcomponent.h
    ${HEADER_PATH}/deadreckoninghelper.h
    ${HEADER_PATH}/defaultgroundclamper.h
    ${HEADER_PATH}/defaultmessageprocessor.h
    ${HEADER_PATH}/defaultnetworkpublishingcomponent.h
    ${HEADER_PATH}/drpublishingactcomp.h
    ${HEADER_PATH}/dtgame.h
    ${HEADER_PATH}/environmentactor.h
    ${HEADER_PATH}/exceptionenum.h
    ${HEADER_PATH}/export.h
    ${HEADER_PATH}/gameactor.h
    ${HEADER_PATH}/gameactorproxy.h
    ${HEADER_PATH}/gameapplication.h
    ${HEADER_PATH}/gameentrypoint.h
    ${HEADER_PATH}/gamemanager.h
    ${HEADER_PATH}/gamemanager.inl
    ${HEADER_PATH}/gmcomponent.h
    ${HEADER_PATH}/gmsettings.h
    ${HEADER_PATH}/gmimpl.h
    ${HEADER_PATH}/gmstatistics.h
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
    ${HEADER_PATH}/messagemacros.h
    ${HEADER_PATH}/messageparameter.h
    ${HEADER_PATH}/messagetype.h
    ${HEADER_PATH}/serverloggercomponent.h
    ${HEADER_PATH}/shaderactorcomponent.h
    ${HEADER_PATH}/taskcomponent.h
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
    ${SOURCE_PATH}/deadreckoningcomponent.cpp
    ${SOURCE_PATH}/deadreckoninghelper.cpp
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
