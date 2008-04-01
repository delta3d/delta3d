
SET(LIB_NAME     dtInputISense)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/dtInputISense)
SET(LIB_PUBLIC_HEADERS
   ${HEADER_PATH}/export.h
   ${HEADER_PATH}/tracker.h
)

ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS}
	tracker.cpp
)

LINK_WITH_VARIABLES(${LIB_NAME}											  
											  DTCORE_LIBRARY											 
											  OSG_LIBRARY
											  ISENSE_LIBRARY)

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_INPUT_ISENSE_LIBRARY)					

INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
