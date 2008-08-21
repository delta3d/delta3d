SET(TARGET_SRC testeffects.cpp readme.txt)

BUILD_EXE_EXAMPLE( testEffects
                   ${DTCORE_LIBRARY}
                   ${DTABC_LIBRARY}
                 )
