SET(TARGET_SRC clientmessageprocessor.cpp testClient.cpp main.cpp)
SET(TARGET_H clientmessageprocessor.h testClient.h)

BUILD_EXE_EXAMPLE( testClient 
                                         DTUTIL_LIBRARY
                                         DTCORE_LIBRARY                                         
                                         DTABC_LIBRARY
                                         DTDAL_LIBRARY
                                         DTGAME_LIBRARY
                                         DTNETGM_LIBRARY
                                         OSG_LIBRARY)                                        
