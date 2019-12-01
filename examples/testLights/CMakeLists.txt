SET(TARGET_SRC testlights.cpp readme.txt)
SET(TARGET_H testlights.h)

BUILD_EXE_EXAMPLE( testLights
                   ${DTUTIL_LIBRARY}
                   ${DTCORE_LIBRARY}
                   ${DTABC_LIBRARY}
                 )
