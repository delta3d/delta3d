SET(TARGET_SRC echomessageprocessor.cpp main.cpp testechoserver.cpp)
SET(TARGET_H echomessageprocessor.h testechoserver.h)

INCLUDE_DIRECTORIES( ${GNE_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${PYTHON_INCLUDE_DIR} ${HAWKNL_INCLUDE_DIR} )

BUILD_EXE_EXAMPLE( testEchoServer
                   ${DTUTIL_LIBRARY}
                   ${DTCORE_LIBRARY}
                   ${DTABC_LIBRARY}
                   ${DTGAME_LIBRARY}
                   ${DTNETGM_LIBRARY}
                 )                                        
