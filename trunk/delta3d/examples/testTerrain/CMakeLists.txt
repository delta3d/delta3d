SET(TARGET_SRC testterrain.cpp readme.txt)

BUILD_EXE_EXAMPLE( testTerrain
                                         GDAL_LIBRARY
                                         DTCORE_LIBRARY
                                         DTABC_LIBRARY
                                         DTTERRAIN_LIBRARY
                                         ODE_LIBRARY
                                         OSGDB_LIBRARY
                                         OSGPARTICLE_LIBRARY
                                         OSGTEXT_LIBRARY
                                         PLIB_UL_LIBRARY)
