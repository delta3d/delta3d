
SET(LIB_NAME     testTrigger)


IF (WIN32)
ELSE (WIN32) 
ENDIF (WIN32)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/examples/testTrigger)

ADD_LIBRARY(${LIB_NAME} SHARED
     ${HEADER_PATH}/testtrigger.h
     ${HEADER_PATH}/testtrigger.cpp
     ${HEADER_PATH}/main.cpp
)

#ADD_DEPENDENCIES (${LIB_NAME}  dtGUI dtABC)

LINK_WITH_VARIABLES(${LIB_NAME}                                         
                                         DTCORE_LIBRARY
                                         DTABC_LIBRARY
                                         OSGDB_LIBRARY)
                                              

INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
