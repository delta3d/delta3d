SET(TARGET_SRC MyWidget.cpp MyWindow.cpp TestWidget.cxx readme.txt)
SET(TARGET_H MyWidget.h MyWindow.h TestWidget.h)

BUILD_EXE_EXAMPLE( testWidget                   
                   ${DTCORE_LIBRARY}
                   ${DTABC_LIBRARY}
                 )
IF (WIN32)                                             
	TARGET_LINK_LIBRARIES(testWidget Ws2_32 Comctl32)
ENDIF (WIN32)
