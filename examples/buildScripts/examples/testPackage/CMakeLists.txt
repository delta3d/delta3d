SET(APP_NAME testPackage)
PROJECT(${APP_NAME})

SET(EXAMPLE_PATH ../../../${APP_NAME}/)

ADD_EXECUTABLE(${APP_NAME} ${EXAMPLE_PATH}/testpackage.cpp)

TARGET_LINK_LIBRARIES(${APP_NAME} ${DTCORE_LIBRARIES}  
                                  ${DTUTIL_LIBRARIES}  
                      )
