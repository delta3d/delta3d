
# FIXME: For OS X, need flag for Framework or dylib

SET(LIB_NAME     dtABC)
 



SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/src/${LIB_NAME})
file(GLOB LIB_PUBLIC_HEADERS "${HEADER_PATH}/*.h")
file(GLOB LIB_SOURCES "*.cpp")
list(REMOVE_ITEM LIB_SOURCES ${SOURCE_PATH}/precomp.cpp)

ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS}
    ${LIB_SOURCES}
)


TARGET_LINK_LIBRARIES(${LIB_NAME}
                       ${DTUTIL_LIBRARY}
                       ${DTCORE_LIBRARY}
                       ${OPENGL_LIBRARY}
                       ${MATH_LIBRARY})
                         
DELTA3D_ADD_LIBRARY_PROPERTIES(${LIB_NAME} DT_ABC_LIBRARY ${LIB_PUBLIC_HEADERS})


INCLUDE(ModuleInstall OPTIONAL)
