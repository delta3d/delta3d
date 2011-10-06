# FIXME: For OS X, need flag for Framework or dylib

SET(LIB_NAME     dtAudio)


SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/src/${LIB_NAME})
file(GLOB LIB_PUBLIC_HEADERS "${HEADER_PATH}/*.h")
file(GLOB LIB_SOURCES "*.cpp")
list(REMOVE_ITEM LIB_SOURCES ${SOURCE_PATH}/precomp.cpp)

INCLUDE_DIRECTORIES( ${OPENAL_INCLUDE_DIR} #for al.h
                     ${ALUT_INCLUDE_DIR}  #for alut.h
                   )

ADD_LIBRARY(${LIB_NAME} SHARED                        
    ${LIB_PUBLIC_HEADERS}                         
    ${LIB_SOURCES}
)

#Use the debug alut library, should it exist, otherwise, use the release version
SET (ALUT_DEBUG ${ALUT_DEBUG_LIBRARY})

IF (NOT ALUT_DEBUG)
  SET (ALUT_DEBUG ${ALUT_LIBRARY})
ENDIF (NOT ALUT_DEBUG)

LINK_WITH_VARIABLES(${LIB_NAME}
                        OSG_LIBRARY
                        OSGDB_LIBRARY
                        OPENTHREADS_LIBRARY
                        XERCES_LIBRARY
                    )

TARGET_LINK_LIBRARIES( ${LIB_NAME}    
                       ${OPENAL_LIBRARY} 
                       ${DTCORE_LIBRARY}
                       ${DTGAME_LIBRARY} #for SoundActorProxy
                       ${DTUTIL_LIBRARY}
                       optimized ${ALUT_LIBRARY} debug ${ALUT_DEBUG}
                       )

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_AUDIO_LIBRARY)


INCLUDE(ModuleInstall OPTIONAL)

