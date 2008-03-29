
SET(LIB_NAME     testWeather)


IF (WIN32)
ELSE (WIN32) 
ENDIF (WIN32)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/examples/testWeather)

ADD_LIBRARY(${LIB_NAME} SHARED
     ${HEADER_PATH}/TestWeather.cpp
)

ADD_DEPENDENCIES (${LIB_NAME}  dtCore dtABC dtInspector)

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL TEST_WIN
                     LINK_FLAGS "/NODEFAULTLIB:libc.lib;licmt;libcd;libcmtd")


INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
