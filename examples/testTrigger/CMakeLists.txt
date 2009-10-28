SET(TARGET_SRC main.cpp testtrigger.cpp readme.txt)
SET(TARGET_H testtrigger.h)

BUILD_EXE_EXAMPLE( testTrigger
                   ${DTCORE_LIBRARY}
                   ${DTABC_LIBRARY}
                 )
