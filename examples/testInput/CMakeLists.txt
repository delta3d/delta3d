SET(TARGET_SRC testinput.cpp readme.txt gui.xml)

INCLUDE_DIRECTORIES( ${CEGUI_INCLUDE_DIR} )

BUILD_EXE_EXAMPLE( testInput
                   ${DTUTIL_LIBRARY}
                   ${DTCORE_LIBRARY}
                   ${DTABC_LIBRARY}
                   ${DTGUI_LIBRARY}
                   ${DTINPUTPLIB_LIBRARY}
                 )
