SET(TARGET_SRC testprerender.cpp readme.txt)
SET(TARGET_H testprerender.h)

BUILD_EXE_EXAMPLE(testPreRender
                   ${DTUTIL_LIBRARY}
                   ${DTCORE_LIBRARY}
                   ${DTABC_LIBRARY}
                 )
