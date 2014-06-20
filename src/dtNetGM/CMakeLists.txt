
SET(LIB_NAME     dtNetGM)


SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/src/${LIB_NAME})
file(GLOB LIB_PUBLIC_HEADERS "${HEADER_PATH}/*.h")
#file(GLOB LIB_SOURCES "*.cpp")
SET(LIB_SOURCES
   clientconnectionlistener.cpp
   clientnetworkcomponent.cpp
   datastreampacket.cpp
   messagepacket.cpp
   networkbridge.cpp
   networkcomponent.cpp
   serverconnectionlistener.cpp
   servernetworkcomponent.cpp
)

list(REMOVE_ITEM LIB_SOURCES ${SOURCE_PATH}/precomp.cpp)

INCLUDE_DIRECTORIES( ${GNE_INCLUDE_DIR}  
                                    ${HAWKNL_INCLUDE_DIR}
                                    ${Boost_INCLUDE_DIR})

ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS}
    ${LIB_SOURCES}
)

LINK_WITH_VARIABLES(${LIB_NAME}
                        GNE_LIBRARY
                        HAWKNL_LIBRARY
                        CURSES_LIBRARY)
                        
TARGET_LINK_LIBRARIES( ${LIB_NAME}
                       ${DTGAME_LIBRARY}
                     )
                                   
IF (WIN32)
TARGET_LINK_LIBRARIES(${LIB_NAME} wsock32)
ENDIF (WIN32)

DELTA3D_ADD_LIBRARY_PROPERTIES(${LIB_NAME} DT_NETGM_LIBRARY ${LIB_PUBLIC_HEADERS})

INCLUDE(ModuleInstall OPTIONAL)
