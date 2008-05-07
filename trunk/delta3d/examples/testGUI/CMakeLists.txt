SET(TARGET_SRC testgui.cpp readme.txt gui.layout)

BUILD_EXE_EXAMPLE( testGUI
                                         DTUTIL_LIBRARY   
                                         DTCORE_LIBRARY
                                         DTABC_LIBRARY
                                         DTDAL_LIBRARY
                                         DTGUI_LIBRARY
                                         OSG_LIBRARY                                       
                                         CEGUI_LIBRARY)
