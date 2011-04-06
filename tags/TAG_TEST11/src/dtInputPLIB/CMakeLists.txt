
SET(LIB_NAME dtInputPLIB)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/src/${LIB_NAME})
file(GLOB LIB_PUBLIC_HEADERS "${HEADER_PATH}/*.h")
file(GLOB LIB_SOURCES "*.cpp")
list(REMOVE_ITEM LIB_SOURCES ${SOURCE_PATH}/precomp.cpp)

ADD_LIBRARY(${LIB_NAME} SHARED                        
    ${LIB_PUBLIC_HEADERS}                         
    ${LIB_SOURCES}
)

LINK_WITH_VARIABLES(${LIB_NAME}                        
                    OSG_LIBRARY
                    PLIB_JS_LIBRARY
                    PLIB_UL_LIBRARY
                    IOKIT_LIBRARY 
                    )
                    
TARGET_LINK_LIBRARIES( ${LIB_NAME}
                       ${DTCORE_LIBRARY}
                     )
                    
                        
IF (WIN32)
   TARGET_LINK_LIBRARIES(${LIB_NAME} winmm)
   SET(CMAKE_SHARED_LINKER_FLAGS "/NODEFAULTLIB:libcmt;libcmtd")
ENDIF (WIN32)

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_INPUT_PLIB_LIBRARY)    
           
           

INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC_IDE)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC_IDE)
