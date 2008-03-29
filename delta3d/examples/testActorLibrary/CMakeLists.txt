
SET(LIB_NAME     testActorLibrary)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/examples/testActorLibrary)

ADD_LIBRARY(${LIB_NAME} SHARED  
     ${HEADER_PATH}/testactorlib.h
     ${HEADER_PATH}/testactorlib.cpp
     ${HEADER_PATH}/testdalenvironmentactor.h
     ${HEADER_PATH}/testdalenvironmentactor.cpp
     ${HEADER_PATH}/testpropertyproxy.h
     ${HEADER_PATH}/testpropertyproxy.cpp)

LINK_WITH_VARIABLES(${LIB_NAME}
                                         DTUTIL_LIBRARY
                                         DTCORE_LIBRARY
                                         DTDAL_LIBRARY
                                         DTACTORS_LIBRARY
                                         DTABC_LIBRARY         
                                         OSG_LIBRARY)                                        
                                              
SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_PLUGIN)


INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
