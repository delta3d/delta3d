SET(TARGET_SRC aicharacter.cpp main.cpp testai.cpp readme.txt)
SET(TARGET_H aicharacter.h testai.h)

BUILD_EXE_EXAMPLE( testAI                                          
                                         DTUTIL_LIBRARY
                                         DTCORE_LIBRARY
                                         DTABC_LIBRARY
                                         DTANIM_LIBRARY
                                         DTAI_LIBRARY                                          
                                         OSG_LIBRARY
                                         OSGDB_LIBRARY
                                         OSGUTIL_LIBRARY)                                        
