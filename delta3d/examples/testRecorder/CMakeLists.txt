
SET(LIB_NAME     testRecorder)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/examples/testRecorder)

ADD_LIBRARY(${LIB_NAME} SHARED     
     ${HEADER_PATH}/camerarecorderadaptor.h
     ${HEADER_PATH}/camerarecorderadaptor.cpp
     ${HEADER_PATH}/osgserializer.h
     ${HEADER_PATH}/osgserializer.cpp
     ${HEADER_PATH}/testrecorder.h
     ${HEADER_PATH}/testrecorder.cpp
     ${HEADER_PATH}/main.cpp)

LINK_WITH_VARIABLES(${LIB_NAME}      
                                         OSG_LIBRARY
                                         OSGDB_LIBRARY                                              
                                         DTUTIL_LIBRARY   
                                         DTCORE_LIBRARY
                                         DTABC_LIBRARY
                                         XERCES_LIBRARY)                                         
                                              

INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
