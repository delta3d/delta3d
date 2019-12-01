SET(APP_NAME testMultiWin)
PROJECT(${APP_NAME})

SET(EXAMPLE_PATH ../../../${APP_NAME}/)

ADD_EXECUTABLE(${APP_NAME} ${EXAMPLE_PATH}/testMultiWin.cpp 
                           ${EXAMPLE_PATH}/testMultiWin.h 
                           ${EXAMPLE_PATH}/readme.txt)

TARGET_LINK_LIBRARIES(${APP_NAME} ${DTCORE_LIBRARIES}
                                  ${DTABC_LIBRARIES}  
                                  ${DTUTIL_LIBRARIES}  
                      )
