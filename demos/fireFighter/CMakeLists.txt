SET (LIB_NAME fireFighter)
file(GLOB LIB_SOURCES "*.cpp" "*.h")

INCLUDE_DIRECTORIES( ${CEGUI_INCLUDE_DIR}
                     ${ALUT_INCLUDE_DIR} #for dtAudio
                     ${OPENAL_INCLUDE_DIR} #for dtAudio}
                     )

ADD_LIBRARY( ${LIB_NAME} SHARED
  ${LIB_SOURCES}
  readme.txt
)

TARGET_LINK_LIBRARIES( ${LIB_NAME}
                       ${DTACTORS_LIBRARY}
                       ${DTAUDIO_LIBRARY}
                       ${DTCORE_LIBRARY}
                       ${DTGAME_LIBRARY}
                       ${DTGUI_LIBRARY}
                       ${DTLMS_LIBRARY}
                     )

#fireFighter needs to have the "delta3d/demos" folder as an #include search path
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/..")
                                         
#for library export                                  
SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DELTA_FIRE_FIGHTER_LIBRARY)
                                  
INCLUDE(ModuleInstall OPTIONAL)

