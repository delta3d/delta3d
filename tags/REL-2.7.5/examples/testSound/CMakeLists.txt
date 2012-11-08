SET(TARGET_SRC testsound.cpp readme.txt)
SET(TARGET_H testsound.h)

INCLUDE_DIRECTORIES ( ${ALUT_INCLUDE_DIR} #for dtAudio
                      ${OPENAL_INCLUDE_DIR} #for dtAudio
                     )

BUILD_EXE_EXAMPLE( testSound
                   ${DTAUDIO_LIBRARY}
                   ${DTABC_LIBRARY}
                 )
 
