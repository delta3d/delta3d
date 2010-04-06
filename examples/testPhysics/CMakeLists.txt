SET(TARGET_SRC testphysics.cpp readme.txt)

BUILD_EXE_EXAMPLE( testPhysics
                   ${DTUTIL_LIBRARY}
                   ${DTCORE_LIBRARY}
                   ${DTABC_LIBRARY}
                  )                                         
