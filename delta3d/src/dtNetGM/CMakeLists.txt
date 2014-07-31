
SET(LIB_NAME     dtNetGM)


SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/src/${LIB_NAME})
file(GLOB LIB_PUBLIC_HEADERS "${HEADER_PATH}/*.h")
#file(GLOB LIB_SOURCES "*.cpp")
SET(LIB_SOURCES
   clientconnectionlistener.cpp
   clientnetworkcomponent.cpp
   componenttypestatics.cpp
   datastreampacket.cpp
   messagepacket.cpp
   networkbridge.cpp
   networkcomponent.cpp
   serverconnectionlistener.cpp
   servernetworkcomponent.cpp
)

INCLUDE_DIRECTORIES( ${GNE_INCLUDE_DIR}
   ${HAWKNL_INCLUDE_DIR}
   ${Boost_INCLUDE_DIR})

SET(LIB_EXTERNAL_DEPS
   GNE_LIBRARY
   HAWKNL_LIBRARY
   CURSES_LIBRARY)

SET(LIB_DEPS
  ${DTGAME_LIBRARY}
)

DELTA3D_ADD_LIBRARY(${LIB_NAME} DT_NETGM_LIBRARY)

IF (WIN32)
TARGET_LINK_LIBRARIES(${LIB_NAME} wsock32)
ENDIF (WIN32)
