SET(APP_NAME testEchoServer)
PROJECT(${APP_NAME})


SET(EXAMPLE_PATH ../../../${APP_NAME}/)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

ADD_EXECUTABLE(${APP_NAME} ${EXAMPLE_PATH}/echomessageprocessor.cpp
                           ${EXAMPLE_PATH}/main.cpp
                           ${EXAMPLE_PATH}/testechoserver.cpp
                           ${EXAMPLE_PATH}/echomessageprocessor.h
                           ${EXAMPLE_PATH}/testechoserver.h)

TARGET_LINK_LIBRARIES(${APP_NAME} ${DTCORE_LIBRARIES}
                                  ${DTABC_LIBRARIES}  
                                  ${DTUTIL_LIBRARIES}
                                  ${DTDAL_LIBRARIES}
                                  ${DTGAME_LIBRARIES}
                                  ${DTNETGM_LIBRARIES}
                      )
