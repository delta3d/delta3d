
SET(LIB_NAME     testGUI)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/examples/testGUI)

ADD_LIBRARY(${LIB_NAME} SHARED     
     ${HEADER_PATH}/testgui.cpp)

LINK_WITH_VARIABLES(${LIB_NAME}                                                       
                                         DTUTIL_LIBRARY   
                                         DTCORE_LIBRARY
                                         DTABC_LIBRARY
                                         DTDAL_LIBRARY
                                         DTGUI_LIBRARY
                                         OSG_LIBRARY                                       
                                         CEGUI_LIBRARY)
                                              
SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES 
                     LINK_FLAGS "/NODEFAULTLIB:libc.lib;licmt;libcd;libcmtd")


INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
