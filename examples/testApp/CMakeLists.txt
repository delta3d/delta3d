SET(TARGET_SRC testapp.cpp readme.txt)


BUILD_EXE_EXAMPLE( testApp
                                    DTCORE_LIBRARY
                                    DTABC_LIBRARY     
                                    OSG_LIBRARY)                                        
