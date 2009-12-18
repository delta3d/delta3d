SET(TARGET_SRC testdirector.cpp)

BUILD_EXE_EXAMPLE( testDirector
                   ${DTUTIL_LIBRARY}
				   ${DTDAL_LIBRARY}
                   ${DTCORE_LIBRARY}
				   ${DTDIRECTOR_LIBRARY}
                 )
