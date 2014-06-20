
SET(LIB_NAME     dtDirectorAnimNodes)


SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/src/${LIB_NAME})

SET(LIB_PUBLIC_HEADERS
    ${HEADER_PATH}/animateactoraction.h
    ${HEADER_PATH}/nodelibraryexport.h
    ${HEADER_PATH}/nodelibraryregistry.h
)


SET(LIB_SOURCES
    ${SOURCE_PATH}/animateactoraction.cpp
    ${SOURCE_PATH}/nodelibraryregistry.cpp
)

INCLUDE_DIRECTORIES(${CAL3D_INCLUDE_DIR})

ADD_LIBRARY(${LIB_NAME} SHARED
            ${LIB_PUBLIC_HEADERS}
            ${LIB_SOURCES}
)

TARGET_LINK_LIBRARIES(${LIB_NAME}
                      ${DTDIRECTOR_LIBRARY}
                      ${DTACTORS_LIBRARY}
                      ${DTGAME_LIBRARY}
                      ${DTANIM_LIBRARY}
                     )

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DIRECTOR_ANIM_NODE_LIBRARY)


INCLUDE(ModuleInstall OPTIONAL)

