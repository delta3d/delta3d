SET(TARGET_SRC testShaders.cpp readme.txt)

BUILD_EXE_EXAMPLE( testShaders
                                         OSG_LIBRARY
                                         OSGDB_LIBRARY                                              
                                         DTUTIL_LIBRARY
                                         DTDAL_LIBRARY
                                         DTABC_LIBRARY)                                         
