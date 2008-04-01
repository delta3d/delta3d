
SET(LIB_NAME     testAI)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/examples/testAI)

ADD_LIBRARY(${LIB_NAME} SHARED  
     ${HEADER_PATH}/aicharacter.h
     ${HEADER_PATH}/aicharacter.cpp
     ${HEADER_PATH}/testai.h
     ${HEADER_PATH}/testai.cpp
     ${HEADER_PATH}/main.cpp)

LINK_WITH_VARIABLES(${LIB_NAME}
                                         DTUTIL_LIBRARY
                                         DTCORE_LIBRARY
                                         DTABC_LIBRARY
                                         DTANIM_LIBRARY
                                         DTAI_LIBRARY                                          
                                         OSG_LIBRARY
                                         OSGDB_LIBRARY
                                         OSGUTIL_LIBRARY)                                        
                                              

INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
