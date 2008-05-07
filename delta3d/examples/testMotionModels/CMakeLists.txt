SET(TARGET_SRC testmotionmodels.cpp)

BUILD_EXE_EXAMPLE( testMotionModels
                                         DTUTIL_LIBRARY   
                                         DTGUI_LIBRARY
                                         DTCORE_LIBRARY
                                         DTABC_LIBRARY
                                         CEGUI_LIBRARY)
