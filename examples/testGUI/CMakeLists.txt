SET(TARGET_SRC testgui.cpp readme.txt gui.layout)

INCLUDE_DIRECTORIES( ${CEGUI_INCLUDE_DIR} )

BUILD_EXE_EXAMPLE( testGUI
                   ${DTUTIL_LIBRARY}
                   ${DTCORE_LIBRARY}
                   ${DTABC_LIBRARY}
                   ${DTGUI_LIBRARY}
                 )
