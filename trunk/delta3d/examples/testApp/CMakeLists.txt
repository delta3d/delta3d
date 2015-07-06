INCLUDE_DIRECTORIES(${PAL_INCLUDE_DIR})

SET(TARGET_SRC testapp.cpp
               civilianaiactorcomponent.cpp
               fireworkactor.cpp
               guicomponent.cpp
               guilistitem.cpp
               guiscreen.cpp
               inputcomponent.cpp
               lightactorcomponent.cpp
               surfacevesselactorcomponent.cpp
               terrainactor.cpp
               testappactorregistry.cpp
               testappconstants.cpp
               testappgamestates.cpp
               testappmessages.cpp
               testappmessagetypes.cpp
               testapputils.cpp
               wateractorheightquery.cpp
               Nodes/ApplicationNode.cpp
               Nodes/ChangeStance.cpp
               Nodes/CivilianAvatar.cpp
               Nodes/DirectorNodePluginRegistry.cpp
               Nodes/GroupNode.cpp
               Nodes/LookAt.cpp
               Nodes/MoveToDestination.cpp
               Nodes/SelectDestination.cpp
               Nodes/SelectTarget.cpp
               Nodes/StateNode.cpp
               Nodes/TurnToFaceDirection.cpp
               readme.txt
               config.xml)

SET(TARGET_H 
             civilianaiactorcomponent.h
             fireworkactor.h
             guicomponent.h
             guilistitem.h
             guiscreen.h
             inputcomponent.h
             lightactorcomponent.h
             surfacevesselactorcomponent.h
             terrainactor.h
             testappactorregistry.h
             testappconstants.h
             testappgamestates.h
             testappmessages.h
             testappmessagetypes.h
             testapputils.h
             wateractorheightquery.h
             Nodes/ApplicationNode.h
             Nodes/ChangeStance.h
             Nodes/CivilianAvatar.h
             Nodes/DirectorNodePluginRegistry.h
             Nodes/GroupNode.h
             Nodes/LookAt.h
             Nodes/MoveToDestination.h
             Nodes/SelectDestination.h
             Nodes/SelectTarget.h
             Nodes/StateNode.h
             Nodes/TurnToFaceDirection.h
             )

IF (QT4_FOUND)
   SET(LIBS_TO_LINK  dtInspectorQt)
   ADD_DEFINITIONS(-DUSE_INSPECTOR)
ENDIF (QT4_FOUND)

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
                  ${DTDIRECTOR_LIBRARY}
                  ${DTDIRECTORNODES_LIBRARY}
                  ${LIBS_TO_LINK}
                 )
