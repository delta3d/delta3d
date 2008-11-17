
SET(TARGET_SRC testgameenvironment.cpp readme.txt)

BUILD_EXE_EXAMPLE( testGameEnvironment
                   ${DTUTIL_LIBRARY}
                   ${DTCORE_LIBRARY}
                   ${DTGAME_LIBRARY}
                   ${DTABC_LIBRARY}
                   ${DTDAL_LIBRARY}
                   ${DTACTORS_LIBRARY}
                 )
