
SET(LIB_NAME     testNetwork)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/examples/testNetwork)

ADD_LIBRARY(${LIB_NAME} SHARED     
     ${HEADER_PATH}/main.cpp
     ${HEADER_PATH}/mynetwork.h
     ${HEADER_PATH}/mynetwork.cpp
     ${HEADER_PATH}/packets.h
     ${HEADER_PATH}/packets.cpp
     ${HEADER_PATH}/testNetwork.h
     ${HEADER_PATH}/testNetwork.cpp)

LINK_WITH_VARIABLES(${LIB_NAME}      
                                         DTNET_LIBRARY                                                                                  
                                         DTUTIL_LIBRARY   
                                         DTCORE_LIBRARY
                                         DTABC_LIBRARY
                                         GNE_LIBRARY
                                         HAWKNL_LIBRARY
					 CURSES_LIBRARY)                                         
                                              
INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
