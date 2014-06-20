
SET(LIB_NAME testGameActorComponents)


set(TARGET_SRC
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

BUILD_LIB_EXAMPLE(${LIB_NAME} TEST_GA_COMPONENTS_LIBRARY
                      ${DTUTIL_LIBRARY}
                      ${DTCORE_LIBRARY}
                      ${DTABC_LIBRARY}
                      ${DTGAME_LIBRARY}
                      ${DTACTORS_LIBRARY}
                      )


