SET(TARGET_SRC testNestedView.cpp readme.txt)
SET(TARGET_H testNestedView.h)

BUILD_EXE_EXAMPLE( testNestedView
                   ${DTUTIL_LIBRARY}
                   ${DTCORE_LIBRARY}
                   ${DTABC_LIBRARY}
                 )                                         
