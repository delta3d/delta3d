
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

DELTA3D_ADD_LIBRARY_PROPERTIES(${LIB_NAME} DT_INPUT_ISENSE_LIBRARY ${LIB_PUBLIC_HEADERS})

INCLUDE(ModuleInstall OPTIONAL)
