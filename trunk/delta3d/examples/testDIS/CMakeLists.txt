SET(TARGET_SRC testdisapp.h testdisapp.cpp main.cpp
               disactorcomponents.h disactorcomponents.cpp 
               actormap.xml Connection.xml readme.txt)

INCLUDE_DIRECTORIES( ${DTDIS_INCLUDE_DIRECTORIES} )

BUILD_EXE_EXAMPLE( testDIS
                  ${DTCORE_LIBRARY}
                  ${DTABC_LIBRARY}
                  ${DTDIS_LIBRARY}
                 )
