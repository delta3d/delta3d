
SET(LIB_NAME     testWidget)


IF (WIN32)
ELSE (WIN32) 
ENDIF (WIN32)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/examples/testWidget)

ADD_LIBRARY(${LIB_NAME} SHARED
     ${HEADER_PATH}/TestWidget.h
     ${HEADER_PATH}/TestWidget.cxx
     ${HEADER_PATH}/MyWidget.h
     ${HEADER_PATH}/MyWidget.cpp
     ${HEADER_PATH}/MyWindow.h
     ${HEADER_PATH}/MyWindow.cpp
)

#ADD_DEPENDENCIES (${LIB_NAME}  dtGUI dtABC)

LINK_WITH_VARIABLES(${LIB_NAME}
                                         FLTK_LIBRARY
                                         DTCORE_LIBRARY
                                         DTABC_LIBRARY
                                         OSG_LIBRARY)
                                              
TARGET_LINK_LIBRARIES(${LIB_NAME} Ws2_32 Comctl32)



INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
