
SET(LIB_NAME     testEchoServer)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/examples/testEchoServer)

ADD_LIBRARY(${LIB_NAME} SHARED     
     ${HEADER_PATH}/testechoserver.h
     ${HEADER_PATH}/testechoserver.cpp
     ${HEADER_PATH}/echomessageprocessor.h
     ${HEADER_PATH}/echomessageprocessor.cpp
     ${HEADER_PATH}/main.cpp
     )

LINK_WITH_VARIABLES(${LIB_NAME}
                                         DTUTIL_LIBRARY
                                         DTCORE_LIBRARY                                         
                                         DTABC_LIBRARY
                                         DTDAL_LIBRARY
                                         DTGAME_LIBRARY
                                         DTNETGM_LIBRARY
                                         OSGDB_LIBRARY)                                        
                                              
SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES 
                     LINK_FLAGS "/NODEFAULTLIB:libc.lib;licmt;libcd;libcmtd")


INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
