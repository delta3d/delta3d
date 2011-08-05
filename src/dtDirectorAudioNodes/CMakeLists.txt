
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



ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS}
    ${LIB_SOURCES}
)

TARGET_LINK_LIBRARIES(${LIB_NAME}
                      ${DTDIRECTOR_LIBRARY}
                      ${DTAUDIO_LIBRARY}
                      ${DTACTORS_LIBRARY}
                     )

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DIRECTOR_AUDIO_NODE_LIBRARY)


INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC_IDE)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC_IDE)
