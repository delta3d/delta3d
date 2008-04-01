
SET(LIB_NAME     testAnim)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/examples/testAnim)

ADD_LIBRARY(${LIB_NAME} SHARED  
     ${HEADER_PATH}/testanim.h
     ${HEADER_PATH}/testanim.cpp
     ${HEADER_PATH}/testaniminput.h
     ${HEADER_PATH}/testaniminput.cpp
     ${HEADER_PATH}/export.h)

LINK_WITH_VARIABLES(${LIB_NAME}
                                         DTUTIL_LIBRARY
                                         DTCORE_LIBRARY
                                         DTABC_LIBRARY
                                         DTANIM_LIBRARY
                                         DTAI_LIBRARY
                                         DTGAME_LIBRARY
                                         DTACTORS_LIBRARY
                                         CEGUI_LIBRARY  
                                         CEGUIOPENGL_LIBRARY                                         
                                         OSG_LIBRARY
                                         OSGDB_LIBRARY)                                        
                                              

INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
