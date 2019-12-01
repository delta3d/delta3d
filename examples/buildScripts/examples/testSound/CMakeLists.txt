SET(APP_NAME testSound)
PROJECT(${APP_NAME})

SET(EXAMPLE_PATH ../../../${APP_NAME}/)

ADD_EXECUTABLE(${APP_NAME} ${EXAMPLE_PATH}/testsound.cpp
                           ${EXAMPLE_PATH}/testsound.h
                           ${EXAMPLE_PATH}/readme.txt)

TARGET_LINK_LIBRARIES(${APP_NAME} ${DTCORE_LIBRARIES}
                                  ${DTABC_LIBRARIES}  
                                  ${DTUTIL_LIBRARIES}
                                  ${DTAUDIO_LIBRARIES}
                      )
