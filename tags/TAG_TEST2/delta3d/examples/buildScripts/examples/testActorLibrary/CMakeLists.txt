SET(APP_NAME testActorLibrary)
PROJECT(${APP_NAME})

SET(EXAMPLE_PATH ../../../${APP_NAME}/)

ADD_LIBRARY(${APP_NAME} SHARED
            ${EXAMPLE_PATH}/testactorlib.cpp 
            ${EXAMPLE_PATH}/readme.txt
            ${EXAMPLE_PATH}/testactorlib.h
            ${EXAMPLE_PATH}/testdalenvironmentactor.cpp
            ${EXAMPLE_PATH}/testdalenvironmentactor.h
            ${EXAMPLE_PATH}/testpropertyproxy.cpp
            ${EXAMPLE_PATH}/testpropertyproxy.h
            ${EXAMPLE_PATH}/readme.txt
)

TARGET_LINK_LIBRARIES(${APP_NAME} ${DTCORE_LIBRARIES}
                                  ${DTABC_LIBRARIES}
                                  ${DTUTIL_LIBRARIES}
                                  ${DTDAL_LIBRARIES}
                                  ${DTACTORS_LIBRARIES}
                      )
                      
                      
SET_TARGET_PROPERTIES(${APP_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_PLUGIN)
