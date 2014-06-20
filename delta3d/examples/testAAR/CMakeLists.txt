
SET(LIB_NAME     testAAR)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/examples/testAAR)

INCLUDE_DIRECTORIES( ${CEGUI_INCLUDE_DIR} )


set( TARGET_SRC
     ${HEADER_PATH}/testaar.cpp
     ${HEADER_PATH}/testaarexceptionenum.cpp
     ${HEADER_PATH}/testaargameevent.cpp
     ${HEADER_PATH}/testaarhud.cpp
     ${HEADER_PATH}/testaarinput.cpp
     ${HEADER_PATH}/testaarmessageprocessor.cpp
     ${HEADER_PATH}/testaarmessagetypes.cpp
     ${HEADER_PATH}/readme.txt
     )

set( TARGET_H
     ${HEADER_PATH}/export.h
     ${HEADER_PATH}/testaar.h
     ${HEADER_PATH}/testaarexceptionenum.h
     ${HEADER_PATH}/testaargameevent.h
     ${HEADER_PATH}/testaarhud.h
     ${HEADER_PATH}/testaarinput.h
     ${HEADER_PATH}/testaarmessageprocessor.h
     ${HEADER_PATH}/testaarmessagetypes.h
     )

BUILD_LIB_EXAMPLE(${LIB_NAME} TEST_AAR_LIBRARY
                      ${DTUTIL_LIBRARY}
                      ${DTCORE_LIBRARY}
                      ${DTGAME_LIBRARY}
                      ${DTACTORS_LIBRARY}
                      ${DTABC_LIBRARY}
                      ${DTAI_LIBRARY}
                      ${DTGUI_LIBRARY}
                      ${DTLMS_LIBRARY}
                      )

