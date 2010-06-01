
SET(APP_NAME     MapDump)

SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/utilities/${APP_NAME})

SET(PROG_SOURCES
    ${SOURCE_PATH}/main.cpp
    )

ADD_EXECUTABLE(${APP_NAME}
    ${PROG_SOURCES}
)

TARGET_LINK_LIBRARIES(${APP_NAME}
                      dtUtil
                      dtCore
                      dtDAL
                     )


INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC_IDE)
  SET_TARGET_PROPERTIES(${APP_NAME} PROPERTIES PREFIX "../../bin/")
  SET_TARGET_PROPERTIES(${APP_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC_IDE)

IF (MSVC)
  SET_TARGET_PROPERTIES(${APP_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
ENDIF (MSVC)