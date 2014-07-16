SET(TARGET_SRC testapp.cpp
               guicomponent.cpp
               guiscreen.cpp
               inputcomponent.cpp
               terrainactorproxy.cpp
               testappgamestates.cpp
               readme.txt
               config.xml)

SET(TARGET_H guicomponent.h
             guiscreen.h
             inputcomponent.h
             terrainactorproxy.h
             testappgamestates.h)


BUILD_LIB_EXAMPLE( testApp TEST_APP_LIBRARY
                  ${DTCORE_LIBRARY}
                  ${DTABC_LIBRARY}
                  ${DTGAME_LIBRARY}
                  ${DTGUI_LIBRARY}
                  ${DTPHYSICS_LIBRARY}
                 )
