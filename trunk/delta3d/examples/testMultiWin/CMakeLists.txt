SET(TARGET_SRC testMultiWin.cpp readme.txt)
SET(TARGET_H testMultiWin.h)

BUILD_EXE_EXAMPLE( testMultiWin
                   ${DTUTIL_LIBRARY}
                   ${DTCORE_LIBRARY}
                   ${DTABC_LIBRARY}                                                                                 
                 )                                         
