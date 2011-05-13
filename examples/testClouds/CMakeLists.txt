SET(TARGET_SRC testclouds.cpp readme.txt)

BUILD_EXE_EXAMPLE( testClouds                                          
                   ${DTUTIL_LIBRARY}
                   ${DTCORE_LIBRARY}
                   ${DTABC_LIBRARY}
                 )
