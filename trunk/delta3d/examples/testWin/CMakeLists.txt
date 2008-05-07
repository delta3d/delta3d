SET(TARGET_SRC testwin.cpp readme.txt testWinLayout.xml)
SET(TARGET_H testwin.h)

BUILD_EXE_EXAMPLE( testWin
 											  DTGUI_LIBRARY
											  DTCORE_LIBRARY
											  DTABC_LIBRARY
											  OSGDB_LIBRARY
											  CEGUI_LIBRARY)
