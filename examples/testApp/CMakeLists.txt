SET(TARGET_SRC testapp.cpp inputcomponent.cpp guicomponent.cpp readme.txt config.xml)
SET(TARGET_H inputcomponent.h guicomponent.h )


BUILD_LIB_EXAMPLE( testApp TEST_APP_LIBRARY
                  ${DTCORE_LIBRARY}
                  ${DTABC_LIBRARY}
                  ${DTGAME_LIBRARY}
                  ${DTGUI_LIBRARY}
                 )
