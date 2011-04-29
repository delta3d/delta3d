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

TARGET_LINK_LIBRARIES(${LIB_NAME}
                      ${DTUTIL_LIBRARY}
                      ${DTCORE_LIBRARY}
                      ${DTABC_LIBRARY}
                      ${DTAUDIO_LIBRARY}
                      ${DTGAME_LIBRARY}
                      ${DTACTORS_LIBRARY}
                      ${DTDAL_LIBRARY}
                      ${DTDIRECTOR_LIBRARY}
)

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL TEST_DIRECTOR_LIBRARY)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC_IDE)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC_IDE)