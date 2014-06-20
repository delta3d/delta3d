
SET(LIB_NAME     dtDirectorAudioNodes)


SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/src/${LIB_NAME})


SET(LIB_PUBLIC_HEADERS ${HEADER_PATH}/nodelibraryexport.h
    ${HEADER_PATH}/nodelibraryregistry.h
    ${HEADER_PATH}/soundaction.h
   )


SET(LIB_SOURCES ${SOURCE_PATH}/nodelibraryregistry.cpp
    ${SOURCE_PATH}/soundaction.cpp
   )

INCLUDE_DIRECTORIES ( ${ALUT_INCLUDE_DIR} #for dtAudio
                      ${OPENAL_INCLUDE_DIR} #for dtAudio
                     )


SET(LIB_DEPS
 ${DTDIRECTOR_LIBRARY}
 ${DTAUDIO_LIBRARY}
 ${DTACTORS_LIBRARY}
)

DELTA3D_ADD_LIBRARY(${LIB_NAME} DIRECTOR_AUDIO_NODE_LIBRARY SHARED)

