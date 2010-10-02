SET(APP_NAME testGameActorLibrary)
PROJECT(${APP_NAME})

SET(EXAMPLE_PATH ../../../${APP_NAME}/)

ADD_LIBRARY(${APP_NAME} SHARED
            ${EXAMPLE_PATH}/countertaskactor.cpp
            ${EXAMPLE_PATH}/countertaskactor.h
            ${EXAMPLE_PATH}/export.h
            ${EXAMPLE_PATH}/readme.txt
            ${EXAMPLE_PATH}/testgameactor.cpp
            ${EXAMPLE_PATH}/testgameactor.h
            ${EXAMPLE_PATH}/testgameactor2.cpp
            ${EXAMPLE_PATH}/testgameactor2.h
            ${EXAMPLE_PATH}/testgameactorcrash.cpp
            ${EXAMPLE_PATH}/testgameactorcrash.h
            ${EXAMPLE_PATH}/testgameactorlibrary.cpp
            ${EXAMPLE_PATH}/testgameactorlibrary.h
            ${EXAMPLE_PATH}/testgameenvironmentactor.cpp
            ${EXAMPLE_PATH}/testgameenvironmentactor.h
            ${EXAMPLE_PATH}/testgamepropertyproxy.cpp
            ${EXAMPLE_PATH}/testgamepropertyproxy.h
            ${EXAMPLE_PATH}/testhlaobject.cpp
            ${EXAMPLE_PATH}/testhlaobject.h
            ${EXAMPLE_PATH}/testplayer.cpp
            ${EXAMPLE_PATH}/testplayer.h
            )



TARGET_LINK_LIBRARIES(${APP_NAME} ${DTCORE_LIBRARIES}
                                  ${DTABC_LIBRARIES}
                                  ${DTUTIL_LIBRARIES}
                                  ${DTGAME_LIBRARIES}
                                  ${DTDAL_LIBRARIES}
                                  ${DTACTORS_LIBRARIES}
                      )
                      
                      
SET_TARGET_PROPERTIES(${APP_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_EXAMPLE_LIBRARY)
