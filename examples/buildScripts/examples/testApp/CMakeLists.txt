PROJECT(testApp)

SET(EXAMPLE_PATH ../../../testApp/)

ADD_EXECUTABLE(testApp ${EXAMPLE_PATH}/testapp.cpp
                       ${EXAMPLE_PATH}/readme.txt)

TARGET_LINK_LIBRARIES(testApp ${DTCORE_LIBRARIES}
                              ${DTABC_LIBRARIES}  
                              ${DTUTIL_LIBRARIES}  
                      )
