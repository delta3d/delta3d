
SET(LIB_NAME     testInput)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/examples/testInput)

ADD_LIBRARY(${LIB_NAME} SHARED     
     ${HEADER_PATH}/testinput.cpp)

LINK_WITH_VARIABLES(${LIB_NAME}                                                       
                                         DTUTIL_LIBRARY   
                                         DTCORE_LIBRARY
                                         DTABC_LIBRARY
                                         DTGUI_LIBRARY
                                         OSG_LIBRARY
                                         OSGDB_LIBRARY
                                         CEGUI_LIBRARY
                                         DTINPUT_PLIB_LIBRARY)
                                              
SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES 
                     LINK_FLAGS "/NODEFAULTLIB:libc.lib;licmt;libcd;libcmtd")


INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
