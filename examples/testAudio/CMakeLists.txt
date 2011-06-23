SET(TARGET_SRC testaudio.cpp readme.txt)
SET(TARGET_H testaudio.h)

BUILD_EXE_EXAMPLE( testAudio
                   ${DTUTIL_LIBRARY}
                   ${DTCORE_LIBRARY}
                   ${DTABC_LIBRARY}
                   ${DTAUDIO_LIBRARY}
                 )                                        
