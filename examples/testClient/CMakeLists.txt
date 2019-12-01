SET(TARGET_SRC clientmessageprocessor.cpp testClient.cpp main.cpp)
SET(TARGET_H clientmessageprocessor.h testClient.h)

INCLUDE_DIRECTORIES( ${GNE_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${PYTHON_INCLUDE_DIR} ${HAWKNL_INCLUDE_DIR} )

BUILD_EXE_EXAMPLE( testClient 
                   ${DTUTIL_LIBRARY}
                   ${DTCORE_LIBRARY}
                   ${DTABC_LIBRARY}
                   ${DTGAME_LIBRARY}
                   ${DTNETGM_LIBRARY}
                 )                                        
