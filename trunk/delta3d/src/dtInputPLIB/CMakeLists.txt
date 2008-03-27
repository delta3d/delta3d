
SET(LIB_NAME dtInputPLIB)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/dtInputPLIB)
SET(LIB_PUBLIC_HEADERS
   ${HEADER_PATH}/export.h
   ${HEADER_PATH}/joystick.h
)

ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS}
	joystick.cpp
)

LINK_WITH_VARIABLES(${LIB_NAME}											  
											  DTCORE_LIBRARY											 
											  OSG_LIBRARY
											  PLIB_JS_LIBRARY
											  PLIB_UL_LIBRARY)
											  
TARGET_LINK_LIBRARIES(${LIB_NAME}
													  winmm)

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_INPUT_PLIB_LIBRARY
					 LINK_FLAGS "/NODEFAULTLIB:libc.lib;libcmt;libcd;libcmtd")		
					 
					 

INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
