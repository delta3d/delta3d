SET(APP_NAME testQt)
PROJECT(${APP_NAME})

SET(EXAMPLE_PATH ../../../${APP_NAME}/)

SET(TARGET_SRC ${EXAMPLE_PATH}/App.cpp 
               ${EXAMPLE_PATH}/main.cpp
               ${EXAMPLE_PATH}/MainWindow.cpp 
               ${EXAMPLE_PATH}/readme.txt
    )
    
 SET(TARGET_H ${EXAMPLE_PATH}/App.h 
     ${EXAMPLE_PATH}/MainWindow.h)

QT4_WRAP_CPP(MOC_SOURCES ${TARGET_H})
QT4_WRAP_UI(UI_SOURCES ${EXAMPLE_PATH}/MainWindow.ui)

INCLUDE_DIRECTORIES(
                    ${QT_INCLUDE_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR} # for ui*.h files                    
                   )

SET(TARGET_SRC ${TARGET_SRC} ${MOC_SOURCES} ${UI_SOURCES})
ADD_EXECUTABLE(${APP_NAME} ${TARGET_SRC})

TARGET_LINK_LIBRARIES(${APP_NAME} 
                      ${DTCORE_LIBRARIES}
                      ${DTUTIL_LIBRARIES}
                      ${DTQT_LIBRARIES}
                      ${DTABC_LIBRARIES}
                      ${QT_QTCORE_LIBRARY}
                      ${QT_QTGUI_LIBRARY}
                      ${QT_QTOPENGL_LIBRARY}
                      )
