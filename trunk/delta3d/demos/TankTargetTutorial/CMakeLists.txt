SET(LIB_NAME TutorialLibrary)
SET(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/ProjectComplete )

ADD_LIBRARY( ${LIB_NAME} SHARED
                      ${SOURCE_PATH}/ActorsRegistry.h
                      ${SOURCE_PATH}/export.h
                      ${SOURCE_PATH}/HUDComponent.h
                      ${SOURCE_PATH}/InputComponent.h
                      ${SOURCE_PATH}/KillableTargetActor.h
                      ${SOURCE_PATH}/MyGameEntryPoint.h
                      ${SOURCE_PATH}/TankActor.h
                      ${SOURCE_PATH}/TargetChanged.h
                      ${SOURCE_PATH}/ActorsRegistry.cpp
                      ${SOURCE_PATH}/HUDComponent.cpp
                      ${SOURCE_PATH}/InputComponent.cpp
                      ${SOURCE_PATH}/KillableTargetActor.cpp
                      ${SOURCE_PATH}/MyGameEntryPoint.cpp
                      ${SOURCE_PATH}/TankActor.cpp
                      ${SOURCE_PATH}/TargetChanged.cpp
                      ${SOURCE_PATH}/readme.txt
                    )
                    
INCLUDE_DIRECTORIES( ${CEGUI_INCLUDE_DIR} ) #for dtGUI

TARGET_LINK_LIBRARIES( ${LIB_NAME}
                       ${DTABC_LIBRARY}
                       ${DTACTORS_LIBRARY}
                       ${DTCORE_LIBRARY}
                       ${DTGAME_LIBRARY}
                       ${DTGUI_LIBRARY}
                       ${DTUTIL_LIBRARY}
                     )

SET (PREPROCESSOR_PROPS "")

#Use dtHLAGM if we have access to it
IF (DTHLAGM_AVAILABLE)
  TARGET_LINK_LIBRARIES( ${LIB_NAME}
                         ${DTHLAGM_LIBRARY}
                       )
  
  INCLUDE_DIRECTORIES(${RTI_INCLUDE_DIR}) 
                                          
  TARGET_LINK_LIBRARIES(${LIB_NAME} ${RTI_LIBRARIES} )
  
  ADD_DEFINITIONS( -DHLA )
ENDIF(DTHLAGM_AVAILABLE)

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL TUTORIAL_TANK_LIBRARY)
