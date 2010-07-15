SET(APP_NAME testWin)
PROJECT(${APP_NAME})

SET(EXAMPLE_PATH ../../../${APP_NAME}/)

ADD_EXECUTABLE(${APP_NAME} ${EXAMPLE_PATH}/testwin.cpp 
                           ${EXAMPLE_PATH}/readme.txt
                           ${EXAMPLE_PATH}/testWinLayout.xml)

TARGET_LINK_LIBRARIES(${APP_NAME} ${DTCORE_LIBRARIES}
                                  ${DTABC_LIBRARIES}
                                  ${DTUTIL_LIBRARIES}
                                  ${DTGUI_LIBRARIES}
                      )
