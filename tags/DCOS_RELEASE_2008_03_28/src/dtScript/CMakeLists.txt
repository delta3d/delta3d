
SET(LIB_NAME     dtScript)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/dtScript)
SET(LIB_PUBLIC_HEADERS
   ${HEADER_PATH}/dtscript.h
   ${HEADER_PATH}/export.h
   ${HEADER_PATH}/scriptmanager.h
)

ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS} 
   scriptmanager.cpp
 )
 
 LINK_WITH_VARIABLES(${LIB_NAME}
												DTUTIL_LIBRARY
												DTCORE_LIBRARY
												OSG_LIBRARY
                                                OSGDB_LIBRARY
												OSGGA_LIBRARY
												OPENTHREADS_LIBRARY
                                                PYTHON_LIBRARY)

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_SCRIPT_LIBRARY
                     LINK_FLAGS "/NODEFAULTLIB:python25_d")


INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
