
SET(LIB_NAME     dtInputISense)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/src/${LIB_NAME})

SET(LIB_SOURCES
      ${SOURCE_PATH}/isense.c
      ${SOURCE_PATH}/tracker.cpp
       )
       
SET(LIB_PUBLIC_HEADERS
       ${HEADER_PATH}/export.h
       ${HEADER_PATH}/isense.h
       ${HEADER_PATH}/tracker.h
       ${HEADER_PATH}/types.h
       )
       
IF (APPLE)
  SET( LIB_SOURCES ${LIB_SOURCES}
         ${SOURCE_PATH}/dlcompat.c )
         
  SET( LIB_PUBLIC_HEADERS ${LIB_PUBLIC_HEADERS}
         ${HEADER_PATH}/dlcompat.h )
ENDIF (APPLE)

#need to add inc/dtInputIsense to the header search path for isence.c can see isense.h
INCLUDE_DIRECTORIES( BEFORE ${HEADER_PATH} )

ADD_LIBRARY(${LIB_NAME} SHARED                        
    ${LIB_PUBLIC_HEADERS}                         
    ${LIB_SOURCES}
)

LINK_WITH_VARIABLES(${LIB_NAME}                       
                     OSG_LIBRARY)

TARGET_LINK_LIBRARIES( ${LIB_NAME}
                       ${DTCORE_LIBRARY}
                     )                        

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_INPUT_ISENSE_LIBRARY)          

INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC_IDE)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC_IDE)
