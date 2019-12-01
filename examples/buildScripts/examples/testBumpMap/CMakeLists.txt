PROJECT(testBumpMap)



SET(EXAMPLE_PATH ../../../testBumpMap/)

ADD_EXECUTABLE(testBumpMap ${EXAMPLE_PATH}/testbumpmap.cpp
                           ${EXAMPLE_PATH}/testbumpmap.h
                           ${EXAMPLE_PATH}/readme.txt
                           )

TARGET_LINK_LIBRARIES(testBumpMap ${DTCORE_LIBRARIES}
                                  ${DTABC_LIBRARIES}  
                                  ${DTUTIL_LIBRARIES}  
                      )
