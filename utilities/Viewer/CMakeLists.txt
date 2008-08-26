
SET(LIB_NAME     Viewer)

SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/utilities/${LIB_NAME})
file(GLOB PROG_SOURCES "*.cpp" "*.cxx" "*.h" "*.rc")
list(REMOVE_ITEM PROG_SOURCES ${SOURCE_PATH}/precomp.cpp)

ADD_EXECUTABLE(${LIB_NAME}
    ${PROG_SOURCES}
)

LINK_WITH_VARIABLES(${LIB_NAME}
                    OSGFX_LIBRARY)

TARGET_LINK_LIBRARIES(${LIB_NAME} 
                      ${DTUTIL_LIBRARY}
                      ${DTCORE_LIBRARY}
                      ${DTDAL_LIBRARY}
                      ${DTABC_LIBRARY}
                     )

IF(WIN32)
  TARGET_LINK_LIBRARIES(${LIB_NAME} User32 Gdi32 shell32 Advapi32 ole32 Ws2_32 Comctl32)
ENDIF(WIN32)
                               
INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
ENDIF (MSVC)
