
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

LINK_WITH_VARIABLES(${LIB_NAME}
											  DTAI_LIBRARY
											  DTDAL_LIBRARY
											  DTGAME_LIBRARY
											  GNE_LIBRARY
											  HAWKNL_LIBRARY
											  CURSES_LIBRARY)
IF (WIN32)
TARGET_LINK_LIBRARIES(${LIB_NAME} wsock32)
ENDIF (WIN32)

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_NETGM_LIBRARY)


INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
