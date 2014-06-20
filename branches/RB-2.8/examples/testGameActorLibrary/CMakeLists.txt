
SET(LIB_NAME     testGameActorLibrary)

file(GLOB LIB_SOURCES "*.cpp" "*.h")

ADD_LIBRARY(${LIB_NAME} SHARED
   ${LIB_SOURCES}
)

TARGET_LINK_LIBRARIES( ${LIB_NAME}
                       ${DTUTIL_LIBRARY}
                       ${DTCORE_LIBRARY}
                       ${DTGAME_LIBRARY}
                       ${DTABC_LIBRARY}
                       ${DTACTORS_LIBRARY}
                      )


SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_EXAMPLE_LIBRARY)

include(ModuleInstall OPTIONAL)
