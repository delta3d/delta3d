SET(TARGET_SRC bumpmapdrawable.cpp testbumpmap.cpp readme.txt)
SET(TARGET_H bumpmapdrawable.h cubevertices.h)

BUILD_EXE_EXAMPLE(testBumpMap
                                        DTUTIL_LIBRARY
                                         DTCORE_LIBRARY                                         
                                         DTABC_LIBRARY
                                         OSG_LIBRARY
                                         OSGDB_LIBRARY)                                        
