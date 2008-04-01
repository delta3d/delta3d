
SET(LIB_NAME     testGameEnvironment)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/examples/testGameEnvironment)

ADD_LIBRARY(${LIB_NAME} SHARED     
     ${HEADER_PATH}/testgameenvironment.cpp)

LINK_WITH_VARIABLES(${LIB_NAME}                                                       
                                         DTUTIL_LIBRARY   
                                         DTCORE_LIBRARY
                                         DTGAME_LIBRARY
                                         DTABC_LIBRARY
                                         DTDAL_LIBRARY
                                         DTACTORS_LIBRARY
                                         OSG_LIBRARY)
                                              
INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
