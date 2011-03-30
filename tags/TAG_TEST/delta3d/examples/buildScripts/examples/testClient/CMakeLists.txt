SET(APP_NAME testClient)
PROJECT(${APP_NAME})


SET(EXAMPLE_PATH ../../../${APP_NAME}/)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

ADD_EXECUTABLE(${APP_NAME} ${EXAMPLE_PATH}/clientmessageprocessor.cpp
                           ${EXAMPLE_PATH}/testClient.cpp
                           ${EXAMPLE_PATH}/clientmessageprocessor.h
                           ${EXAMPLE_PATH}/testClient.h
                           ${EXAMPLE_PATH}/main.cpp)

TARGET_LINK_LIBRARIES(${APP_NAME} ${DTCORE_LIBRARIES}
                                  ${DTABC_LIBRARIES}  
                                  ${DTUTIL_LIBRARIES}
                                  ${DTDAL_LIBRARIES}
                                  ${DTGAME_LIBRARIES}
                                  ${DTNETGM_LIBRARIES}
                      )
