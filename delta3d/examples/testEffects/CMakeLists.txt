
SET(PROG_NAME     testEffects)

SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/examples/${PROG_NAME})
file(GLOB PROG_SOURCES "*.cpp" "*.h")
list(REMOVE_ITEM PROG_SOURCES ${SOURCE_PATH}/precomp.cpp)

ADD_EXECUTABLE(${PROG_NAME}       
    ${PROG_SOURCES}
)

LINK_WITH_VARIABLES(${PROG_NAME}
                                         DTCORE_LIBRARY                                         
                                         DTABC_LIBRARY                                         
                                         OSGDB_LIBRARY)

IF (WIN32)                                         
TARGET_LINK_LIBRARIES(${PROG_NAME} odbc32 odbccp32)
ENDIF (WIN32)
                                              
INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${PROG_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${PROG_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
