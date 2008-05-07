SET(TARGET_SRC echomessageprocessor.cpp main.cpp testechoserver.cpp)
SET(TARGET_H echomessageprocessor.h testechoserver.h)

BUILD_EXE_EXAMPLE( testEchoServer
                                         DTUTIL_LIBRARY
                                         DTCORE_LIBRARY                                         
                                         DTABC_LIBRARY
                                         DTDAL_LIBRARY
                                         DTGAME_LIBRARY
                                         DTNETGM_LIBRARY
                                         OSGDB_LIBRARY)                                        
