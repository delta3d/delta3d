SET(TARGET_SRC testsound.cpp readme.txt)
SET(TARGET_H testsound.h)

BUILD_EXE_EXAMPLE( testSound
                   ${DTAUDIO_LIBRARY}
                   ${DTABC_LIBRARY}
                 )
 