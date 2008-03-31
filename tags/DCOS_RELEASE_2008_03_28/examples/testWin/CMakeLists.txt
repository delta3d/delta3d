
SET(LIB_NAME     testWin)


IF (WIN32)
ELSE (WIN32) 
ENDIF (WIN32)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/examples/testWin)

ADD_LIBRARY(${LIB_NAME} SHARED
    ${HEADER_PATH}/testwin.h
   ${HEADER_PATH}/testwin.cpp
)

ADD_DEPENDENCIES (${LIB_NAME}  dtGUI dtABC)

LINK_WITH_VARIABLES(${LIB_NAME}
											  DTGUI_LIBRARY
											  DTCORE_LIBRARY
											  OSGDB_LIBRARY
											  CEGUI_LIBRARY)
                                              
TARGET_LINK_LIBRARIES(${LIB_NAME} odbccp32)

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES 
                     LINK_FLAGS "/NODEFAULTLIB:libc.lib;licmt;libcd;libcmtd")


INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
