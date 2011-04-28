
SET(LIB_NAME     testActorLibrary)

file(GLOB LIB_SOURCES "*.cpp" "*.h")
list(REMOVE_ITEM LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/precomp.cpp)

ADD_LIBRARY(${LIB_NAME} SHARED  
   ${LIB_SOURCES}
)

TARGET_LINK_LIBRARIES(${LIB_NAME}
                      ${DTUTIL_LIBRARY}
                      ${DTCORE_LIBRARY}
                      ${DTDAL_LIBRARY}
                      ${DTACTORS_LIBRARY}
                      ${DTABC_LIBRARY}
                      )
SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_PLUGIN)


#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC_IDE)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC_IDE)
