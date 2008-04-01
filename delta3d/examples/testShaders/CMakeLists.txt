
SET(LIB_NAME     testShaders)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/examples/testShaders)

ADD_LIBRARY(${LIB_NAME} SHARED     
     ${HEADER_PATH}/testShaders.cpp
)

LINK_WITH_VARIABLES(${LIB_NAME}      
                                         OSG_LIBRARY
                                         OSGDB_LIBRARY                                              
                                         DTUTIL_LIBRARY
                                         DTDAL_LIBRARY
                                         DTABC_LIBRARY)                                         
                                              
INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
