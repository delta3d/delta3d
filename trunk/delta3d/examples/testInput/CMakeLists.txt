SET(TARGET_SRC testinput.cpp readme.txt)

BUILD_EXE_EXAMPLE( testInput
                                         DTUTIL_LIBRARY   
                                         DTCORE_LIBRARY
                                         DTABC_LIBRARY
                                         DTGUI_LIBRARY
                                         OSG_LIBRARY
                                         OSGDB_LIBRARY
                                         CEGUI_LIBRARY
                                         DTINPUT_PLIB_LIBRARY)
