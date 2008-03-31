#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)

# FIXME: For OS X, need flag for Framework or dylib

SET(LIB_NAME     dtAudio)


IF (WIN32)
ELSE (WIN32) 
ENDIF (WIN32)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/dtAudio)
SET(LIB_PUBLIC_HEADERS
${HEADER_PATH}/audiomanager.h
${HEADER_PATH}/dtaudio.h
${HEADER_PATH}/export.h
${HEADER_PATH}/listener.h
${HEADER_PATH}/sound.h
${HEADER_PATH}/soundeffectbinder.h
)

ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS} 
audiomanager.cpp
listener.cpp
sound.cpp
soundeffectbinder.cpp
)

LINK_WITH_VARIABLES(${LIB_NAME}
											  DTCORE_LIBRARY
											  DTUTIL_LIBRARY
											  OSG_LIBRARY
											  OSGDB_LIBRARY
											  OPENTHREADS_LIBRARY)

TARGET_LINK_LIBRARIES( ${LIB_NAME}		
					                                  ${OPENAL_LIBRARY} 
					                                  ${ALUT_LIBRARY} )

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_AUDIO_LIBRARY)


INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
