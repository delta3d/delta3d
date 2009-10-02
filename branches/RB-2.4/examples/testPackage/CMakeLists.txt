SET(TARGET_SRC testpackage.cpp)

BUILD_EXE_EXAMPLE( testPackage
                   ${DTUTIL_LIBRARY}
                   ${DTCORE_LIBRARY}
                 )
