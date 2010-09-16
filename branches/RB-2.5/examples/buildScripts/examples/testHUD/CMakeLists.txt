SET(APP_NAME testHUD)
PROJECT(${APP_NAME})

SET(EXAMPLE_PATH ../../../${APP_NAME}/)

ADD_EXECUTABLE(${APP_NAME} ${EXAMPLE_PATH}/TestHUD.cpp 
                           ${EXAMPLE_PATH}/TestHUD.h)

TARGET_LINK_LIBRARIES(${APP_NAME} ${DTCORE_LIBRARIES}
                                  ${DTABC_LIBRARIES}
                                  ${DTUTIL_LIBRARIES}
                                  ${DTGUI_LIBRARIES}
                                  ${DTDAL_LIBRARIES}
                      )
