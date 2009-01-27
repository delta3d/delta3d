SET(TARGET_SRC main.cpp MyEventTypes.cpp MyStateTypes.cpp teststatemanager.cpp test.xml)
SET(TARGET_H MyEventTypes.h MyStateTypes.h teststatemanager.h)

BUILD_EXE_EXAMPLE( testStateManager
                   ${DTABC_LIBRARY}
                 )   
