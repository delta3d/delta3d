SET(TARGET_SRC testapp.cpp
               civilianactor.cpp
               civilianaiactorcomponent.cpp
               guicomponent.cpp
               guiscreen.cpp
               inputcomponent.cpp
               terrainactor.cpp
               testappactorregistry.cpp
               testappconstants.cpp
               testappgamestates.cpp
               testappmessages.cpp
               testappmessagetypes.cpp
               readme.txt
               config.xml)

SET(TARGET_H civilianactor.h
             civilianaiactorcomponent.h
             guicomponent.h
             guiscreen.h
             inputcomponent.h
             terrainactor.h
             testappactorregistry.h
             testappconstants.h
             testappgamestates.h
             testappmessages.h
             testappmessagetypes.h)


BUILD_LIB_EXAMPLE( testApp TEST_APP_LIBRARY
                  ${DTCORE_LIBRARY}
                  ${DTABC_LIBRARY}
                  ${DTGAME_LIBRARY}
                  ${DTGUI_LIBRARY}
                  ${DTPHYSICS_LIBRARY}
                  ${DTANIM_LIBRARY}
                  ${DTAI_LIBRARY}
                  ${DTACTORS_LIBRARY}
                  ${DTRENDER_LIBRARY}
                 )
