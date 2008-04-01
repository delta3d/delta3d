
SET(LIB_NAME     testBumpMap)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/examples/testBumpMap)

ADD_LIBRARY(${LIB_NAME} SHARED  
     ${HEADER_PATH}/testbumpmap.cpp
     ${HEADER_PATH}/bumpmapdrawable.h
     ${HEADER_PATH}/bumpmapdrawable.cpp
     ${HEADER_PATH}/cubevertices.h)

LINK_WITH_VARIABLES(${LIB_NAME}
                                         DTUTIL_LIBRARY
                                         DTCORE_LIBRARY                                         
                                         DTABC_LIBRARY
                                         OSG_LIBRARY
                                         OSGDB_LIBRARY)                                        
                                              
INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
