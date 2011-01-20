
SET(LIB_NAME     testAnim)

file(GLOB LIB_SOURCES "*.cpp" "*.h")
list(REMOVE_ITEM LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/precomp.cpp)

INCLUDE_DIRECTORIES( ${CAL3D_INCLUDE_DIR} )#for dtAnim

ADD_LIBRARY(${LIB_NAME} SHARED  
   ${LIB_SOURCES}
   readme.txt
)

TARGET_LINK_LIBRARIES(${LIB_NAME}
                      ${DTUTIL_LIBRARY}
                      ${DTCORE_LIBRARY}
                      ${DTABC_LIBRARY}
                      ${DTANIM_LIBRARY}
                      ${DTAI_LIBRARY}
                      ${DTGAME_LIBRARY}
                      ${DTACTORS_LIBRARY}
                   )


SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL TEST_ANIM_LIBRARY)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC_IDE)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC_IDE)
