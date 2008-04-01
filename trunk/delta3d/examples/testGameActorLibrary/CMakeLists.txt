
SET(LIB_NAME     testGameActorLibrary)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/examples/testGameActorLibrary)

ADD_LIBRARY(${LIB_NAME} SHARED     
     ${HEADER_PATH}/countertaskactor.h
     ${HEADER_PATH}/countertaskactor.cpp
     ${HEADER_PATH}/export.h
     ${HEADER_PATH}/testgameactor2.h
     ${HEADER_PATH}/testgameactor2.cpp
     ${HEADER_PATH}/testgameactor.h
     ${HEADER_PATH}/testgameactor.cpp
     ${HEADER_PATH}/testgameactorcrash.h
     ${HEADER_PATH}/testgameactorcrash.cpp
     ${HEADER_PATH}/testgameactorlibrary.h
     ${HEADER_PATH}/testgameactorlibrary.cpp
     ${HEADER_PATH}/testgameenvironmentactor.h
     ${HEADER_PATH}/testgameenvironmentactor.cpp
     ${HEADER_PATH}/testgamepropertyproxy.h
     ${HEADER_PATH}/testgamepropertyproxy.cpp
     ${HEADER_PATH}/testhlaobject.h
     ${HEADER_PATH}/testhlaobject.cpp
     ${HEADER_PATH}/testplayer.h
     ${HEADER_PATH}/testplayer.cpp
     )

LINK_WITH_VARIABLES(${LIB_NAME}                                                       
                                         DTUTIL_LIBRARY   
                                         DTCORE_LIBRARY
                                         DTGAME_LIBRARY
                                         DTABC_LIBRARY
                                         DTDAL_LIBRARY
                                         DTACTORS_LIBRARY
                                         OSG_LIBRARY
                                         OSGDB_LIBRARY
                                         OSGUTIL_LIBRARY)
                                              

INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
