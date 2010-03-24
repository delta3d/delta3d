SET(APP_NAME testRecorder)
PROJECT(${APP_NAME})

SET(EXAMPLE_PATH ../../../${APP_NAME}/)

ADD_EXECUTABLE(${APP_NAME} ${EXAMPLE_PATH}/camerarecorderadaptor.cpp 
                           ${EXAMPLE_PATH}/main.cpp
                           ${EXAMPLE_PATH}/osgserializer.cpp
                           ${EXAMPLE_PATH}/testrecorder.cpp                           
                           ${EXAMPLE_PATH}/camerarecorderadaptor.h
                           ${EXAMPLE_PATH}/osgserializer.h
                           ${EXAMPLE_PATH}/testrecorder.h                          
                           ${EXAMPLE_PATH}/readme.txt)

TARGET_LINK_LIBRARIES(${APP_NAME} ${DTCORE_LIBRARIES}
                                  ${DTABC_LIBRARIES}  
                                  ${DTUTIL_LIBRARIES}  
                      )
