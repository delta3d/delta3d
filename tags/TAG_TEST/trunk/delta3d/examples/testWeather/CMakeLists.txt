SET(TARGET_SRC testweather.cpp readme.txt)

BUILD_EXE_EXAMPLE( testWeather
                   ${DTCORE_LIBRARY}
                   ${DTABC_LIBRARY}
                 )
