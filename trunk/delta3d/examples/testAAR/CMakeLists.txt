
SET(LIB_NAME     testAAR)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/examples/testAAR)

INCLUDE_DIRECTORIES( ${CEGUI_INCLUDE_DIR} )

ADD_LIBRARY(${LIB_NAME} SHARED 
    ${HEADER_PATH}/readme.txt
     ${HEADER_PATH}/export.h
     ${HEADER_PATH}/testaar.h
     ${HEADER_PATH}/testaar.cpp
     ${HEADER_PATH}/testaarexceptionenum.h
     ${HEADER_PATH}/testaarexceptionenum.cpp
     ${HEADER_PATH}/testaargameevent.h
     ${HEADER_PATH}/testaargameevent.cpp
     ${HEADER_PATH}/testaarhud.h
     ${HEADER_PATH}/testaarhud.cpp
     ${HEADER_PATH}/testaarinput.h
     ${HEADER_PATH}/testaarinput.cpp
     ${HEADER_PATH}/testaarmessageprocessor.h
     ${HEADER_PATH}/testaarmessageprocessor.cpp
     ${HEADER_PATH}/testaarmessagetypes.h
     ${HEADER_PATH}/testaarmessagetypes.cpp)

TARGET_LINK_LIBRARIES(${LIB_NAME}
                      ${DTUTIL_LIBRARY}
                      ${DTCORE_LIBRARY}
                      ${DTGAME_LIBRARY}
                      ${DTACTORS_LIBRARY}
                      ${DTABC_LIBRARY}
                      ${DTAI_LIBRARY}
                      ${DTGUI_LIBRARY}
                      ${DTLMS_LIBRARY}
                     )

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL TEST_AAR_LIBRARY)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC_IDE)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC_IDE)
