SET(TARGET_SRC testphysics.cpp readme.txt)

BUILD_EXE_EXAMPLE( testPhysics
                                         OSG_LIBRARY                                                                                  
                                         DTUTIL_LIBRARY   
                                         DTCORE_LIBRARY
                                         DTABC_LIBRARY)                                         
