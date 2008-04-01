
SET(LIB_NAME     testClouds)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/examples/testClouds)

ADD_LIBRARY(${LIB_NAME} SHARED  
     ${HEADER_PATH}/testclouds.cpp
     )

LINK_WITH_VARIABLES(${LIB_NAME}
                                         DTUTIL_LIBRARY
                                         DTCORE_LIBRARY                                         
                                         DTABC_LIBRARY
                                         DTINSPECTOR_LIBRARY)                                        
                                              

INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
