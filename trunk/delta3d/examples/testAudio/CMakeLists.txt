
SET(LIB_NAME     testAudio)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/examples/testAudio)

ADD_LIBRARY(${LIB_NAME} SHARED  
     ${HEADER_PATH}/testaudio.h
     ${HEADER_PATH}/testaudio.cpp)

LINK_WITH_VARIABLES(${LIB_NAME}
                                         DTUTIL_LIBRARY
                                         DTCORE_LIBRARY
                                         DTABC_LIBRARY
                                         DTAUDIO_LIBRARY
                                         XERCES_LIBRARY   
                                         OSG_LIBRARY
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
