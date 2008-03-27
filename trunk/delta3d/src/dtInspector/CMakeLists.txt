
SET(LIB_NAME     dtInspector)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/dtInspector)
SET(LIB_PUBLIC_HEADERS
   ${HEADER_PATH}/export.h
   ${HEADER_PATH}/gui_fltk.h
   ${HEADER_PATH}/inspector.h
)

ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS}
	gui_fltk.cpp
	inspector.cpp
)

LINK_WITH_VARIABLES(${LIB_NAME}			
											  DTUTIL_LIBRARY
											  DTCORE_LIBRARY	
											  DTABC_LIBRARY
											  OSG_LIBRARY
											  OPENTHREADS_LIBRARY)

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_INSPECTOR_LIBRARY
					  LINK_FLAGS "/NODEFAULTLIB:libc.lib;libcmt;libcd;libcmtd;msvcrt")					

INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
