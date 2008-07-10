SET(TARGET_SRC camerarecorderadaptor.cpp main.cpp osgserializer.cpp testrecorder.cpp readme.txt)
SET(TARGET_H camerarecorderadaptor.h osgserializer.h testrecorder.h)

BUILD_EXE_EXAMPLE( testRecorder
                                         OSG_LIBRARY
                                         OSGDB_LIBRARY                                              
                                         DTUTIL_LIBRARY   
                                         DTCORE_LIBRARY
                                         DTABC_LIBRARY
                                         XERCES_LIBRARY)                                         
