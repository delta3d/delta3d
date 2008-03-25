
SET(LIB_NAME     dtNetGM)


IF (WIN32)
ELSE (WIN32) 
ENDIF (WIN32)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/dtNetGM)
SET(LIB_PUBLIC_HEADERS
   ${HEADER_PATH}/clientconnectionlistener.h
   ${HEADER_PATH}/clientnetworkcomponent.h
   ${HEADER_PATH}/datastreampacket.h
   ${HEADER_PATH}/dtnetgm.h
   ${HEADER_PATH}/export.h
   ${HEADER_PATH}/machineinfomessage.h
   ${HEADER_PATH}/messagepacket.h
   ${HEADER_PATH}/networkbridge.h
   ${HEADER_PATH}/networkcomponent.h
   ${HEADER_PATH}/serverconnectionlistener.h
   ${HEADER_PATH}/servernetworkcomponent.h
)

ADD_LIBRARY(${LIB_NAME} SHARED
   ${LIB_PUBLIC_HEADERS}
   clientconnectionlistener.cpp
   clientnetworkcomponent.cpp
   datastreampacket.cpp
   machineinfomessage.cpp
   messagepacket.cpp
   networkbridge.cpp
   networkcomponent.cpp
   serverconnectionlistener.cpp
   servernetworkcomponent.cpp
)

TARGET_LINK_LIBRARIES(${LIB_NAME} dtUtil dtCore dtAI dtDAL dtGame 
   ${GNE_LIBRARY} ${HAWKNL_LIBRARY} ${OSG_LIBRARY} ${OSGDB_LIBRARY} ${OPENTHREADS_LIBRARY})

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_LMS_LIBRARY)


INCLUDE(ModuleInstall OPTIONAL)
