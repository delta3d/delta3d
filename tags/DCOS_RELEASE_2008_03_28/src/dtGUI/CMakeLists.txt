
SET(LIB_NAME     dtGUI)


IF (WIN32)
ELSE (WIN32) 
ENDIF (WIN32)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/dtGUI)
SET(LIB_PUBLIC_HEADERS
   ${HEADER_PATH}/basescriptmodule.h
   ${HEADER_PATH}/ceguikeyboardlistener.h
   ${HEADER_PATH}/ceguimouselistener.h
   ${HEADER_PATH}/ceuidrawable.h
   ${HEADER_PATH}/dtgui.h
   ${HEADER_PATH}/export.h
   ${HEADER_PATH}/guiexceptionenum.h
   ${HEADER_PATH}/renderer.h
   ${HEADER_PATH}/resourceprovider.h
   ${HEADER_PATH}/scriptmodule.h
)

ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS} 
   ceguikeyboardlistener.cpp
   ceguimouselistener.cpp
   ceuidrawable.cpp
   guiexceptionenum.cpp
   renderer.cpp
   resourceprovider.cpp
   scriptmodule.cpp
 )
 
 LINK_WITH_VARIABLES(${LIB_NAME}
												DTUTIL_LIBRARY
												DTCORE_LIBRARY
												OSG_LIBRARY
												OSGGA_LIBRARY
												OPENTHREADS_LIBRARY
												CEGUI_LIBRARY
												CEGUIOPENGL_LIBRARY)
												

TARGET_LINK_LIBRARIES(${LIB_NAME}  
													  ${OPENGL_LIBRARY} 
													  ${MATH_LIBRARY})

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_GUI_LIBRARY)


INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
