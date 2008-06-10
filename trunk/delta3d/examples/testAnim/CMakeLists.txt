
SET(LIB_NAME     testAnim)

file(GLOB LIB_SOURCES "*.cpp" "*.h")
list(REMOVE_ITEM LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/precomp.cpp)

ADD_LIBRARY(${LIB_NAME} SHARED  
   ${LIB_SOURCES}
   readme.txt
)

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

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL TEST_ANIM_LIBRARY)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
