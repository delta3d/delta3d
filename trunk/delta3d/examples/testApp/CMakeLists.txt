
SET(LIB_NAME     testApp)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/examples/testApp)

ADD_LIBRARY(${LIB_NAME} SHARED  
     ${HEADER_PATH}/testapp.cpp)

LINK_WITH_VARIABLES(${LIB_NAME}                                         
                                         DTCORE_LIBRARY
                                         DTABC_LIBRARY     
                                         OSG_LIBRARY)                                        
                                              
INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
