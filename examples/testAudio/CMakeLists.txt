SET(TARGET_SRC testaudio.cpp readme.txt)
SET(TARGET_H testaudio.h)

INCLUDE_DIRECTORIES ( ${ALUT_INCLUDE_DIR} #for dtAudio
                      ${OPENAL_INCLUDE_DIR} #for dtAudio
                     )

BUILD_EXE_EXAMPLE( testAudio
                   ${DTUTIL_LIBRARY}
                   ${DTCORE_LIBRARY}
                   ${DTABC_LIBRARY}
                   ${DTAUDIO_LIBRARY}
                 )                                        
