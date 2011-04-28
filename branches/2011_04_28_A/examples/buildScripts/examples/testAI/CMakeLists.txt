SET(APP_NAME testAI)
PROJECT(${APP_NAME})

SET(EXAMPLE_PATH ../../../${APP_NAME}/)

ADD_EXECUTABLE(${APP_NAME}
               ${EXAMPLE_PATH}/aicharacter.cpp 
               ${EXAMPLE_PATH}/main.cpp
               ${EXAMPLE_PATH}/testai.cpp
               ${EXAMPLE_PATH}/readme.txt
)

TARGET_LINK_LIBRARIES(${APP_NAME} ${DTCORE_LIBRARIES}
                                  ${DTABC_LIBRARIES} 
                                  ${DTUTIL_LIBRARIES}
                                  ${DTDAL_LIBRARIES}
                                  ${DTANIM_LIBRARIES}
                                  ${DTAI_LIBRARIES}
                      )