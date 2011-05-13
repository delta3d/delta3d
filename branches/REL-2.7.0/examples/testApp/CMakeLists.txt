SET(TARGET_SRC testapp.cpp readme.txt config.xml)


BUILD_EXE_EXAMPLE( testApp
                  ${DTCORE_LIBRARY}
                  ${DTABC_LIBRARY}
                 )
