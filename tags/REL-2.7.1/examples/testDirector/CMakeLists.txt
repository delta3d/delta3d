SET(LIB_NAME testDirector)

SET(LIB_SOURCES
    directorcomponent.cpp
	 directorcomponent.h	 
	 testdirectorentrypoint.h	 
	 testdirectorentrypoint.cpp
	 testdirectorexport.h	
    readme.txt
)


ADD_LIBRARY(${LIB_NAME} SHARED
            ${LIB_SOURCES}
)

SET(LIBS_TO_LINK      ${LIB_NAME}
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

TARGET_LINK_LIBRARIES(${LIBS_TO_LINK})

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL TEST_DIRECTOR_LIBRARY)

IF (QT4_FOUND) 
   ADD_DEFINITIONS(-DUSE_INSPECTOR)
ENDIF (QT4_FOUND)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC_IDE)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC_IDE)