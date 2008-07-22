SET(TARGET_SRC main.cpp mynetwork.cpp packets.cpp testNetwork.cpp)
SET(TARGET_H mynetwork.h packets.h testNetwork.h)

INCLUDE_DIRECTORIES( ${GNE_INCLUDE_DIR} ${BOOST_INCLUDE_DIR} ${PYTHON_INCLUDE_DIR} ${HAWKNL_INCLUDE_DIR} )

BUILD_EXE_EXAMPLE( testNetwork
                   ${DTNET_LIBRARY}
                   ${DTUTIL_LIBRARY}
                   ${DTCORE_LIBRARY}
                   ${DTABC_LIBRARY}
                  )                                         
