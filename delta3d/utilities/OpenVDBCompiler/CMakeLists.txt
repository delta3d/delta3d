
SET(APP_NAME     OpenVDBCompiler)

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
                      dtPhysics
                      ${TBB_LIBRARIES}
                      ${OPENEXR_Half_LIBRARY}
                      ${OPENVDB_LIBRARIES}
                     )


INCLUDE(ProgramInstall OPTIONAL)

IF (MSVC)
  SET_TARGET_PROPERTIES(${APP_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
ENDIF (MSVC)