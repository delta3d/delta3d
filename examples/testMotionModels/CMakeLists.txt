SET(TARGET_SRC testmotionmodels.cpp readme.txt config.xml)

INCLUDE_DIRECTORIES( ${CEGUI_INCLUDE_DIR} )

BUILD_EXE_EXAMPLE( testMotionModels
                   ${DTUTIL_LIBRARY}
                   ${DTGUI_LIBRARY}
                   ${DTCORE_LIBRARY}
                   ${DTABC_LIBRARY}
                 )
