
SET(LIB_NAME     testPython)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/examples/testPython)

ADD_LIBRARY(${LIB_NAME} SHARED     
     ${HEADER_PATH}/testPython.cpp)

LINK_WITH_VARIABLES(${LIB_NAME}      
                                         DTSCRIPT_LIBRARY)       

TARGET_LINK_LIBRARIES(${LIB_NAME} odbc32 odbccp32)                                         
                                              
SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES 
                     LINK_FLAGS "/NODEFAULTLIB:python25_d;libc.lib;licmt;libcd;libcmtd")


INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
