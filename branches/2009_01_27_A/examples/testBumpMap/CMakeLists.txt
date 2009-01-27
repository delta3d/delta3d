SET(TARGET_SRC testbumpmap.cpp readme.txt)
SET(TARGET_H testbumpmap.h)

BUILD_EXE_EXAMPLE(testBumpMap
                   ${DTUTIL_LIBRARY}
                   ${DTCORE_LIBRARY}
                   ${DTABC_LIBRARY}
                 )                                        
