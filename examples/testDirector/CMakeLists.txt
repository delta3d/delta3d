SET(LIB_NAME testDirector)

SET(TARGET_SRC
    directorcomponent.cpp
	 testdirectorentrypoint.cpp
    readme.txt
)

set(TARGET_H
    directorcomponent.h  
    testdirectorentrypoint.h
    testdirectorexport.h
    )


SET(LIBS_TO_LINK
                      ${DTUTIL_LIBRARY}
                      ${DTCORE_LIBRARY}
                      ${DTABC_LIBRARY}
                      ${DTGAME_LIBRARY}
                      ${DTDIRECTOR_LIBRARY}
)

IF (QT4_FOUND)
   SET(LIBS_TO_LINK    ${LIBS_TO_LINK} 
                       dtInspectorQt)
ENDIF (QT4_FOUND)

IF (QT4_FOUND) 
   ADD_DEFINITIONS(-DUSE_INSPECTOR)
ENDIF (QT4_FOUND)

BUILD_LIB_EXAMPLE(${LIB_NAME} TEST_DIRECTOR_LIBRARY
                      ${LIBS_TO_LINK}
                      )

