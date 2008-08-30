SET(TARGET_SRC testmap.cpp)

BUILD_EXE_EXAMPLE( testMap
                   ${DTUTIL_LIBRARY}
                   ${DTCORE_LIBRARY}
                   ${DTDAL_LIBRARY}
                   ${DTACTORS_LIBRARY}
                   ${DTABC_LIBRARY}
                 )
