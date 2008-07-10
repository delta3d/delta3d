SET(TARGET_SRC testterrain.cpp readme.txt)

BUILD_EXE_EXAMPLE( testTerrain
                                         DTCORE_LIBRARY
                                         DTABC_LIBRARY
                                         DTTERRAIN_LIBRARY
                                         OSGDB_LIBRARY
                                         OSGPARTICLE_LIBRARY
                                         OSGTEXT_LIBRARY
                                         )
