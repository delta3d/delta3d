
SET(APP_NAME     GameStart)

SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/utilities/${APP_NAME})
file(GLOB PROG_SOURCES "*.cpp" "*.h")
list(REMOVE_ITEM PROG_SOURCES ${SOURCE_PATH}/precomp.cpp)

ADD_EXECUTABLE(${APP_NAME}
    ${PROG_SOURCES}
)

TARGET_LINK_LIBRARIES(${APP_NAME}
                      dtUtil
                      dtCore
                      dtABC
                      dtGame
                     )


INCLUDE(ModuleInstall OPTIONAL)

IF (MSVC)
  # SET_TARGET_PROPERTIES(${APP_NAME} PROPERTIES LINK_FLAGS "/LARGEADDRESSAWARE")
  SET_TARGET_PROPERTIES(${APP_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
ENDIF (MSVC)
