
SET(LIB_NAME     dtScript)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/src/${LIB_NAME})
file(GLOB LIB_PUBLIC_HEADERS "${HEADER_PATH}/*.h")
file(GLOB LIB_SOURCES "*.cpp")
list(REMOVE_ITEM LIB_SOURCES ${SOURCE_PATH}/precomp.cpp)

ADD_LIBRARY(${LIB_NAME} SHARED                        
    ${LIB_PUBLIC_HEADERS}                         
    ${LIB_SOURCES}
)
 
INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_PATH} )

#Use the debug python library, should it exist, otherwise, use the release version
SET (PYTHON_DEBUG ${PYTHON_DEBUG_LIBRARY})

IF (NOT PYTHON_DEBUG)
  SET (PYTHON_DEBUG ${PYTHON_LIBRARY})
ENDIF (NOT PYTHON_DEBUG)

                       
TARGET_LINK_LIBRARIES( ${LIB_NAME}
                       ${DTUTIL_LIBRARY}
                       ${DTCORE_LIBRARY}
                       optimized ${PYTHON_LIBRARY} debug ${PYTHON_DEBUG}
                      )

DELTA3D_ADD_LIBRARY_PROPERTIES(${LIB_NAME} DT_SCRIPT_LIBRARY ${LIB_PUBLIC_HEADERS})
                      

INCLUDE(ModuleInstall OPTIONAL)

IF (MSVC_IDE)
  SET(CMAKE_SHARED_LINKER_FLAGS "/NODEFAULTLIB:python25_d /NODEFAULTLIB:python26_d /NODEFAULTLIB:python27_d") #ignore the auto-link library Python adds in
ENDIF (MSVC_IDE)
