
SET(LIB_NAME     testClient)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/examples/testClient)

ADD_LIBRARY(${LIB_NAME} SHARED  
     ${HEADER_PATH}/testClient.h
     ${HEADER_PATH}/testClient.cpp
     ${HEADER_PATH}/clientmessageprocessor.h
     ${HEADER_PATH}/clientmessageprocessor.cpp
     ${HEADER_PATH}/main.cpp)

LINK_WITH_VARIABLES(${LIB_NAME}
                                         DTUTIL_LIBRARY
                                         DTCORE_LIBRARY                                         
                                         DTABC_LIBRARY
                                         DTDAL_LIBRARY
                                         DTGAME_LIBRARY
                                         DTNETGM_LIBRARY
                                         OSG_LIBRARY)                                        
                                              
INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
