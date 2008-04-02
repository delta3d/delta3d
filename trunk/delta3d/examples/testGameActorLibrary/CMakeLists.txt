
SET(LIB_NAME     testGameActorLibrary)

file(GLOB LIB_SOURCES "*.cpp" "*.h")
list(REMOVE_ITEM LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/precomp.cpp)

ADD_LIBRARY(${LIB_NAME} SHARED     
   ${LIB_SOURCES}
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
                                              

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_EXAMPLE_LIBRARY)

INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
