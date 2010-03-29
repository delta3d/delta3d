SET(TARGET_SRC testdisapp.h testdisapp.cpp main.cpp)


BUILD_EXE_EXAMPLE( testDIS
                  ${DTCORE_LIBRARY}
                  ${DTABC_LIBRARY}
                  ${DTDIS_LIBRARY}
                 )
