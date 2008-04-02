
SET(LIB_NAME     testApp)

ADD_LIBRARY(${LIB_NAME} SHARED  
     testapp.cpp)

LINK_WITH_VARIABLES(${LIB_NAME}                                         
                                         DTCORE_LIBRARY
                                         DTABC_LIBRARY     
                                         OSG_LIBRARY)                                        
                                              
INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
