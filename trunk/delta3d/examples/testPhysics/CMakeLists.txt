
SET(LIB_NAME     testPhysics)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/examples/testPhysics)

ADD_LIBRARY(${LIB_NAME} SHARED     
     ${HEADER_PATH}/testphysics.cpp)

LINK_WITH_VARIABLES(${LIB_NAME}      
                                         OSG_LIBRARY                                                                                  
                                         DTUTIL_LIBRARY   
                                         DTCORE_LIBRARY
                                         DTABC_LIBRARY)                                         
                                              
SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES 
                     LINK_FLAGS "/NODEFAULTLIB:libc.lib;licmt;libcd;libcmtd")


INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
