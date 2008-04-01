
SET(LIB_NAME     testStateManager)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/examples/testStateManager)

ADD_LIBRARY(${LIB_NAME} SHARED
     ${HEADER_PATH}/main.cpp
     ${HEADER_PATH}/MyEventTypes.h
     ${HEADER_PATH}/MyEventTypes.cpp
     ${HEADER_PATH}/MyStateTypes.h
     ${HEADER_PATH}/MyStateTypes.cpp
     ${HEADER_PATH}/teststatemanager.h
     ${HEADER_PATH}/teststatemanager.cpp
)

LINK_WITH_VARIABLES(${LIB_NAME}         
                                         XERCES_LIBRARY                                         
                                         DTABC_LIBRARY
                                         OSGDB_LIBRARY)                                         
                                              
INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
