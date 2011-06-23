SET(APP_NAME testInput)
PROJECT(${APP_NAME})

SET(EXAMPLE_PATH ../../../${APP_NAME}/)

ADD_EXECUTABLE(${APP_NAME} ${EXAMPLE_PATH}/testinput.cpp 
                           ${EXAMPLE_PATH}/readme.txt
                           ${EXAMPLE_PATH}/gui.xml)

TARGET_LINK_LIBRARIES(${APP_NAME} ${DTCORE_LIBRARIES}
                                  ${DTABC_LIBRARIES}
                                  ${DTUTIL_LIBRARIES}
                                  ${DTGUI_LIBRARIES}
                                  ${DTINPUTPLIB_LIBRARIES}
                      )
