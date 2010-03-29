SET(TARGET_SRC TestHUD.cpp TestHUD.h)

INCLUDE_DIRECTORIES( ${CEGUI_INCLUDE_DIR} )

BUILD_EXE_EXAMPLE( testHUD
                   ${DTGUI_LIBRARY}
                   ${DTCORE_LIBRARY}
                   ${DTABC_LIBRARY}
                 )
                                         
