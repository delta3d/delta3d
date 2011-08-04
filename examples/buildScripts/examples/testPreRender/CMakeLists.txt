SET(APP_NAME testPreRender)
PROJECT(${APP_NAME})

SET(EXAMPLE_PATH ../../../${APP_NAME}/)

ADD_EXECUTABLE(${APP_NAME} ${EXAMPLE_PATH}/testprerender.cpp 
                           ${EXAMPLE_PATH}/testprerender.h
                           ${EXAMPLE_PATH}/readme.txt)

TARGET_LINK_LIBRARIES(${APP_NAME} ${DTCORE_LIBRARIES}
                                  ${DTABC_LIBRARIES}
                                  ${DTUTIL_LIBRARIES}
                                  ${DTDAL_LIBRARIES}
                      )
