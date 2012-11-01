SET(APP_NAME testDirector)
PROJECT(${APP_NAME})

SET(EXAMPLE_PATH ../../../${APP_NAME}/)

ADD_EXECUTABLE(${APP_NAME} 
                ${EXAMPLE_PATH}/directorcomponent.cpp
                ${EXAMPLE_PATH}/directorcomponent.h
                ${EXAMPLE_PATH}/readme.txt
                ${EXAMPLE_PATH}/testdirectorentrypoint.cpp
                ${EXAMPLE_PATH}/testdirectorentrypoint.h
                ${EXAMPLE_PATH}/testdirectorexport.h)

TARGET_LINK_LIBRARIES(${APP_NAME} ${DTCORE_LIBRARIES}
                                  ${DTABC_LIBRARIES}  
                                  ${DTUTIL_LIBRARIES}
                                  ${DTGAME_LIBRARIES}
                                  ${DTAUDIO_LIBRARIES}
                                  ${DTDIRECTOR_LIBRARIES}
                                  ${DTACTORS_LIBRARIES}
                      )
