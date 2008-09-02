
# FIXME: For OS X, need flag for Framework or dylib

SET(LIB_NAME     dtDIS)

INCLUDE_DIRECTORIES( ${DIS_INCLUDE_DIR} ${HAWKNL_INCLUDE_DIR} )

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/src/${LIB_NAME})
file(GLOB LIB_PUBLIC_HEADERS "${HEADER_PATH}/*.h" "${HEADER_PATH}/plugins/default/*.h" "${HEADER_PATH}/*.inl")
file(GLOB LIB_SOURCES "*.cpp" "plugins/default/*.cpp"  )
list(REMOVE_ITEM LIB_SOURCES ${SOURCE_PATH}/precomp.cpp)

ADD_LIBRARY(${LIB_NAME} SHARED                        
    ${LIB_PUBLIC_HEADERS}                         
    ${LIB_SOURCES}
)

LINK_WITH_VARIABLES(${LIB_NAME}
                    HAWKNL_LIBRARY
                    DIS_LIBRARY
                   )

IF (WIN32)   
TARGET_LINK_LIBRARIES(${LIB_NAME}
        wsock32)
ENDIF (WIN32)

TARGET_LINK_LIBRARIES(${LIB_NAME}
                      ${DTUTIL_LIBRARY}
                      ${DTCORE_LIBRARY}
                      ${DTGAME_LIBRARY}
                      ${CARBON_LIBRARY}
                      )                                         
                                                
SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_DIS_LIBRARY COMPILE_FLAGS -DDT_DIS_DEFAULT_PLUGIN)         



INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
