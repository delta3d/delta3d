SET(APP_NAME testStateManager)
PROJECT(${APP_NAME})

SET(EXAMPLE_PATH ../../../${APP_NAME}/)

ADD_EXECUTABLE(${APP_NAME} ${EXAMPLE_PATH}/main.cpp 
                           ${EXAMPLE_PATH}/MyEventTypes.cpp
                           ${EXAMPLE_PATH}/MyStateTypes.cpp
                           ${EXAMPLE_PATH}/teststatemanager.cpp
                           ${EXAMPLE_PATH}/test.xml
                           ${EXAMPLE_PATH}/MyEventTypes.h
                           ${EXAMPLE_PATH}/MyStateTypes.h
                           ${EXAMPLE_PATH}/teststatemanager.h
)

TARGET_LINK_LIBRARIES(${APP_NAME} ${DTCORE_LIBRARIES}
                                  ${DTABC_LIBRARIES}  
                                  ${DTUTIL_LIBRARIES}  
                      )
   