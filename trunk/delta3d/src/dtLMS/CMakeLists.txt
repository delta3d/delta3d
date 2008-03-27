
SET(LIB_NAME     dtLMS)


IF (WIN32)
ELSE (WIN32) 
ENDIF (WIN32)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/dtLMS)
SET(LIB_PUBLIC_HEADERS
   ${HEADER_PATH}/export.h
   ${HEADER_PATH}/lmsclientsocket.h
   ${HEADER_PATH}/lmscomponent.h
   ${HEADER_PATH}/lmsconnectionstate.h
   ${HEADER_PATH}/lmsexceptionenum.h
   ${HEADER_PATH}/lmsmessage.h
   ${HEADER_PATH}/lmsmessagetype.h
   ${HEADER_PATH}/lmsmessagevalue.h
   ${HEADER_PATH}/lmstaskstatus.h
)

ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS}
   lmsclientsocket.cpp
   lmscomponent.cpp
   lmsconnectionstate.cpp
   lmsexceptionenum.cpp
   lmsmessage.cpp
   lmsmessagetype.cpp
   lmsmessagevalue.cpp
)

LINK_WITH_VARIABLES(${LIB_NAME}
											  DTGAME_LIBRARY
											  DTAUDIO_LIBRARY)

TARGET_LINK_LIBRARIES(${LIB_NAME} wsock32)

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_LMS_LIBRARY)


INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
