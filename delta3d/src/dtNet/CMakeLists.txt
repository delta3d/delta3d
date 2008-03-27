
SET(LIB_NAME     dtNet)


IF (WIN32)
ELSE (WIN32) 
ENDIF (WIN32)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/dtNet)
SET(LIB_PUBLIC_HEADERS
   ${HEADER_PATH}/export.h
   ${HEADER_PATH}/connectionlistener.h
   ${HEADER_PATH}/connectionserver.h
   ${HEADER_PATH}/dtnet.h
   ${HEADER_PATH}/netmgr.h
)

ADD_LIBRARY(${LIB_NAME} STATIC
   ${LIB_PUBLIC_HEADERS}
   connectionlistener.cpp
   connectionserver.cpp
   netmgr.cpp
)

INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
