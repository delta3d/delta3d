
SET(LIB_NAME     testActorLibrary)

file(GLOB LIB_SOURCES "*.cpp" "*.h")
list(REMOVE_ITEM LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/precomp.cpp)

ADD_LIBRARY(${LIB_NAME} SHARED  
   ${LIB_SOURCES}
)

TARGET_LINK_LIBRARIES(${LIB_NAME}
                      ${DTUTIL_LIBRARY}
                      ${DTCORE_LIBRARY}
                      ${DTACTORS_LIBRARY}
                      ${DTABC_LIBRARY}
                      )

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_PLUGIN)

include(ModuleInstall OPTIONAL)
