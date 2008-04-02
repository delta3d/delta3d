
SET(PROG_NAME     testStateManager)

SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/examples/${PROG_NAME})
file(GLOB PROG_SOURCES "*.cpp" "*.h")
list(REMOVE_ITEM PROG_SOURCES ${SOURCE_PATH}/precomp.cpp)

ADD_EXECUTABLE(${PROG_NAME}
    ${PROG_SOURCES}
)

LINK_WITH_VARIABLES(${PROG_NAME}
                                         XERCES_LIBRARY                                         
                                         DTABC_LIBRARY
                                         OSGDB_LIBRARY)                                         
                                              
INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${PROG_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${PROG_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
