
SET(LIB_NAME testGameActorComponents)


ADD_LIBRARY(${LIB_NAME} SHARED
            componentgameactor.cpp
            componentgameactor.h
            readme.txt
            testactorlibraryregistry.cpp
            testactorlibraryregistry.h
            testentrypoint.cpp
            testexport.h
            textlabelcomponent.cpp
            textlabelcomponent.h
)

TARGET_LINK_LIBRARIES(${LIB_NAME}
                      ${DTUTIL_LIBRARY}
                      ${DTCORE_LIBRARY}
                      ${DTABC_LIBRARY}
                      ${DTGAME_LIBRARY}
                      ${DTACTORS_LIBRARY}
                   )

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL TEST_GA_COMPONENTS_LIBRARY)
