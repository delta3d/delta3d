SET(APP_NAME testGameActorComponents)
PROJECT(${APP_NAME})

SET(EXAMPLE_PATH ../../../${APP_NAME}/)

ADD_LIBRARY(${APP_NAME} SHARED
            ${EXAMPLE_PATH}/componentgameactor.cpp
            ${EXAMPLE_PATH}/componentgameactor.h
            ${EXAMPLE_PATH}/readme.txt 
            ${EXAMPLE_PATH}/testactorlibraryregistry.cpp
            ${EXAMPLE_PATH}/testactorlibraryregistry.h
            ${EXAMPLE_PATH}/testentrypoint.cpp
            ${EXAMPLE_PATH}/testexport.h
            ${EXAMPLE_PATH}/textlabelcomponent.cpp
            ${EXAMPLE_PATH}/textlabelcomponent.h
            )

TARGET_LINK_LIBRARIES(${APP_NAME} ${DTCORE_LIBRARIES}
                                  ${DTABC_LIBRARIES}
                                  ${DTUTIL_LIBRARIES}
                                  ${DTGAME_LIBRARIES}
                                  ${DTDAL_LIBRARIES}
                                  ${DTACTORS_LIBRARIES}
                      )
                      
                      
SET_TARGET_PROPERTIES(${APP_NAME}
                     PROPERTIES DEFINE_SYMBOL TEST_GA_COMPONENTS_LIBRARY)
