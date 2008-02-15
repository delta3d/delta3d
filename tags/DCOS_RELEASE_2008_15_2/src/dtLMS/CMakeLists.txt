
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

TARGET_LINK_LIBRARIES(${LIB_NAME} dtUtil dtCore dtAI dtDAL dtAudio dtGame 
   ${OSG_LIBRARY} ${OSGDB_LIBRARY} ${OPENTHREADS_LIBRARY})

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_LMS_LIBRARY)


INCLUDE(ModuleInstall OPTIONAL)
