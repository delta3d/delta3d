SET(TARGET_SRC aicharacter.cpp main.cpp testai.cpp readme.txt)
SET(TARGET_H aicharacter.h testai.h)

INCLUDE_DIRECTORIES( ${CAL3D_INCLUDE_DIR} )#for dtAnim

BUILD_EXE_EXAMPLE( testAI
                   ${DTUTIL_LIBRARY}
                   ${DTCORE_LIBRARY}
                   ${DTABC_LIBRARY}
                   ${DTANIM_LIBRARY}
                   ${DTAI_LIBRARY}
                  )
