
SET(LIB_NAME     dtHLAGM)

INCLUDE_DIRECTORIES(${RTIS_INCLUDE_DIR})


IF (WIN32)
ELSE (WIN32) 
ENDIF (WIN32)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/dtHLAGM)
SET(LIB_PUBLIC_HEADERS
   ${HEADER_PATH}/attributetoproperty.h
   ${HEADER_PATH}/attributetype.h
   ${HEADER_PATH}/ddmappspacecalculator.h
   ${HEADER_PATH}/ddmcalculatorgeographic.h
   ${HEADER_PATH}/ddmcameracalculatorgeographic.h
   ${HEADER_PATH}/ddmgeographicregiondata.h
   ${HEADER_PATH}/ddmmultienumeratedcalculator.h
   ${HEADER_PATH}/ddmregioncalculator.h
   ${HEADER_PATH}/ddmregioncalculatorgroup.h
   ${HEADER_PATH}/ddmregiondata.h
   ${HEADER_PATH}/ddmutil.h
   ${HEADER_PATH}/distypes.h
   ${HEADER_PATH}/dthlagm.h
   ${HEADER_PATH}/exceptionenum.h
   ${HEADER_PATH}/export.h
   ${HEADER_PATH}/hlacomponent.h
   ${HEADER_PATH}/hlacomponentconfig.h
   ${HEADER_PATH}/hlafomconfigxml.h
   ${HEADER_PATH}/interactiontomessage.h
   ${HEADER_PATH}/objectruntimemappinginfo.h
   ${HEADER_PATH}/objecttoactor.h
   ${HEADER_PATH}/onetomanymapping.h
   ${HEADER_PATH}/onetoonemapping.h
   ${HEADER_PATH}/parametertoparameter.h
   ${HEADER_PATH}/parametertranslator.h
   ${HEADER_PATH}/rprparametertranslator.h
   ${HEADER_PATH}/spatial.h
)

ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS}
   attributetype.cpp
   ddmappspacecalculator.cpp
   ddmcalculatorgeographic.cpp
   ddmcameracalculatorgeographic.cpp
   ddmgeographicregiondata.cpp
   ddmmultienumeratedcalculator.cpp
   ddmregioncalculator.cpp
   ddmregioncalculatorgroup.cpp
   ddmregiondata.cpp
   ddmutil.cpp
   distypes.cpp
   exceptionenum.cpp
   hlacomponent.cpp
   hlacomponentconfig.cpp
   hlafomconfigxml.cpp
   objectruntimemappinginfo.cpp
   objecttoactor.cpp
   onetomanymapping.cpp
   rprparametertranslator.cpp
   spatial.cpp
)

TARGET_LINK_LIBRARIES(${LIB_NAME} dtUtil dtCore dtDAL dtGame
   ${OSGSIM_LIBRARY} ${OPENTHREADS_LIBRARY} 
#${RTIS_NG_LIBRARY}
${RTIS_MEMPOOL_LIBRARY}
${RTIS_13_LIBRARY}
${RTIS_URLREADER_LIBRARY}
${RTIS_BITVECTOR_LIBRARY}
${RTIS_MERSENNE_LIBRARY}
${RTIS_VTERM_LIBRARY}
${RTIS_BUFMGR_LIBRARY}
${RTIS_MSGSTATS_LIBRARY}
${RTIS_RTIFEDTIME_LIBRARY}
${RTIS_XREGEX_LIBRARY}
${RTIS_FEDTIME_LIBRARY}
${RTIS_NETUTIL_LIBRARY}
${RTIS_RTIINTERCEPT_LIBRARY}
${RTIS_Z_LIBRARY}
${RTIS_FEDTIME13_LIBRARY}
${RTIS_PARSER_LIBRARY}
${RTIS_LIBRARY}
${RTIS_IF_LIBRARY}
${RTIS_RID_LIBRARY}
${RTIS_MCAST_LIBRARY}
${RTIS_RTI_LIBRARY}
${RTIS_TIMERS_LIBRARY}
 )

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_HLAGM_LIBRARY)


INCLUDE(ModuleInstall OPTIONAL)
