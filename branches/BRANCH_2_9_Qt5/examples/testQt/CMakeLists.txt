SET(TARGET_SRC App.cpp main.cpp MainWindow.cpp readme.txt )
SET(TARGET_H App.h MainWindow.h)

QT5_WRAP_CPP(MOC_SOURCES ${TARGET_H})
QT5_WRAP_UI(UI_SOURCES MainWindow.ui)

SET(TARGET_SRC ${TARGET_SRC} ${MOC_SOURCES} ${UI_SOURCES})

INCLUDE_DIRECTORIES(
                    ${QT_INCLUDE_DIRS}
                    ${CMAKE_CURRENT_BINARY_DIR} # for ui*.h files
                   )
                   
BUILD_EXE_EXAMPLE( testQt
                  ${DTQT_LIBRARY}
                  ${DTCORE_LIBRARY}
                  ${DTABC_LIBRARY}
                 )
