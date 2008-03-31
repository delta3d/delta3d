
SET(LIB_NAME     testMultiWin)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/examples/testMultiWin)

ADD_LIBRARY(${LIB_NAME} SHARED     
     ${HEADER_PATH}/testMultiWin.h
     ${HEADER_PATH}/testMultiWin.cpp)

LINK_WITH_VARIABLES(${LIB_NAME}                                                       
                                         DTUTIL_LIBRARY   
                                         DTCORE_LIBRARY
                                         DTABC_LIBRARY
                                         OSG_LIBRARY
                                         OSGUTIL_LIBRARY
                                         OSGVIEWER_LIBRARY)                                         
                                              
SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES 
                     LINK_FLAGS "/NODEFAULTLIB:libc.lib;licmt;libcd;libcmtd")


INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
