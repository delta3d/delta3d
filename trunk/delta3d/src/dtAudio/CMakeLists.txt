
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

TARGET_LINK_LIBRARIES( ${LIB_NAME}
					  optimized dtCore debug dtCoreD
					  optimized dtUtil  debug dtUtilD
					  optimized osg debug osgd
					  optimized osgDB debug osgDBd
					  optimized OpenThreads debug OpenThreadsd			
					  optimized alut debug alutd
					  ${OPENAL_LIBRARY} 
					  ${ALUT_LIBRARY}
					  )

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_AUDIO_LIBRARY)


INCLUDE(ModuleInstall OPTIONAL)
