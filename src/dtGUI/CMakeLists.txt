
SET(LIB_NAME     dtGUI)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/src/${LIB_NAME})

SET(LIB_PUBLIC_HEADERS
   ${HEADER_PATH}/ceguikeyboardlistener.h
   ${HEADER_PATH}/ceguimouselistener.h
   ${HEADER_PATH}/export.h
   ${HEADER_PATH}/gui.h
   ${HEADER_PATH}/resourceprovider.h
   ${HEADER_PATH}/scriptmodule.h
   ${HEADER_PATH}/widget.h
)
  
SET(LIB_SOURCES
   ${SOURCE_PATH}/ceguikeyboardlistener.cpp
   ${SOURCE_PATH}/ceguimouselistener.cpp
   ${SOURCE_PATH}/gui.cpp
   ${SOURCE_PATH}/resourceprovider.cpp
   ${SOURCE_PATH}/scriptmodule.cpp
)
 
INCLUDE_DIRECTORIES( ${CEGUI_INCLUDE_DIR} )
 
SET(LIB_EXTERNAL_DEPS
   OSG_LIBRARY
   OSGGA_LIBRARY
   OPENTHREADS_LIBRARY
   CEGUI_LIBRARY
   CEGUIOPENGL_LIBRARY
)

SET(LIB_DEPS
  ${DTUTIL_LIBRARY}
  ${DTCORE_LIBRARY}
  ${OPENGL_LIBRARY} 
  ${MATH_LIBRARY}
)

DELTA3D_ADD_LIBRARY(${LIB_NAME} DT_GUI_LIBRARY)

