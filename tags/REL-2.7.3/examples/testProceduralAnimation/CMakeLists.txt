
SET(LIB_NAME testProceduralAnimation)

SET(LIB_SOURCES
    proceduralanimationactor.cpp
	proceduralanimationactor.h
	proceduralanimationcomponent.cpp
	proceduralanimationcomponent.h
	readme.txt
	testactorlibraryregistry.cpp
	testactorlibraryregistry.h
	testentrypoint.cpp
	testentrypoint.h
	testexport.h
)

INCLUDE_DIRECTORIES( ${CAL3D_INCLUDE_DIR} )#for dtAnim

ADD_LIBRARY(${LIB_NAME} SHARED
   ${LIB_SOURCES}
)

TARGET_LINK_LIBRARIES(${LIB_NAME}
                      ${DTUTIL_LIBRARY}
                      ${DTCORE_LIBRARY}
                      ${DTABC_LIBRARY}
                      ${DTANIM_LIBRARY}
                      ${DTAI_LIBRARY}
                      ${DTGAME_LIBRARY}
                      ${DTACTORS_LIBRARY}
                   )

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL TEST_PROCEDURAL_ANIM_LIBRARY)
