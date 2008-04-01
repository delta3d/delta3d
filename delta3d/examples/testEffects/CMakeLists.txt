
SET(LIB_NAME     testEffects)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/examples/testEffects)

ADD_LIBRARY(${LIB_NAME} SHARED     
     ${HEADER_PATH}/testeffects.cpp
     )

LINK_WITH_VARIABLES(${LIB_NAME}
                                         DTCORE_LIBRARY                                         
                                         DTABC_LIBRARY                                         
                                         OSGDB_LIBRARY)

IF (WIN32)                                         
TARGET_LINK_LIBRARIES(${LIB_NAME} odbc32 odbccp32)
ENDIF (WIN32)
                                              
INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
