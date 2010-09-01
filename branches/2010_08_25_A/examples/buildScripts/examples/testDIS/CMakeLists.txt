SET(APP_NAME testDIS)
PROJECT(${APP_NAME})

SET(EXAMPLE_PATH ../../../${APP_NAME}/)

ADD_EXECUTABLE(${APP_NAME} ${EXAMPLE_PATH}/main.cpp
                           ${EXAMPLE_PATH}/testdisapp.cpp
                           ${EXAMPLE_PATH}/testdisapp.h
                           ${EXAMPLE_PATH}/actormap.xml
                           ${EXAMPLE_PATH}/Connection.xml
                           ${EXAMPLE_PATH}/readme.txt)

TARGET_LINK_LIBRARIES(${APP_NAME} ${DTCORE_LIBRARIES}
                                  ${DTABC_LIBRARIES}  
                                  ${DTUTIL_LIBRARIES}
                                  ${DTGAME_LIBRARIES}
                                  ${DTDAL_LIBRARIES}
                                  ${DTDIS_LIBRARIES}
                      )
