
SET(LIB_NAME GameLaunch)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/utilities/LMS/${LIB_NAME}/inc)
SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/utilities/LMS/${LIB_NAME}/src)

SET(PROG_HEADERS ${HEADER_PATH}/Export.h
                 ${HEADER_PATH}/JavaLaunch.h
                 ${HEADER_PATH}/jni.h
                 ${HEADER_PATH}/jni_md.h
                 ${HEADER_PATH}/Launcher.h
                 ${HEADER_PATH}/ParameterParser.h  )
                 
SET(PROG_SOURCES ${SOURCE_PATH}/GameLaunch.cpp
                 ${SOURCE_PATH}/Launcher.cpp
                 ${SOURCE_PATH}/ParameterParser.cpp )

INCLUDE_DIRECTORIES(${HEADER_PATH})

ADD_LIBRARY(${LIB_NAME} SHARED
    ${PROG_HEADERS}
    ${PROG_SOURCES}
)

TARGET_LINK_LIBRARIES( ${LIB_NAME}
                       ${DTABC_LIBRARY}
                       ${DTCORE_LIBRARY}
                       ${DTGAME_LIBRARY}
                       ${DTUTIL_LIBRARY}
                     )           

SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES DEFINE_SYMBOL GAME_LAUNCH_LIBRARY)                     

IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
ENDIF (MSVC)