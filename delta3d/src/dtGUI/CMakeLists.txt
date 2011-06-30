
SET(LIB_NAME     dtGUI)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/src/${LIB_NAME})

IF (NOT BUILD_WITH_OLD_CEGUI)
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
ELSE (NOT BUILD_WITH_OLD_CEGUI)
  SET(LIB_PUBLIC_HEADERS 
      ${HEADER_PATH}/basescriptmodule.h
      ${HEADER_PATH}/ceguiconnectionmanager.h
      ${HEADER_PATH}/ceguiconnectionsignatures.h
      ${HEADER_PATH}/ceguikeyboardlistener.h
      ${HEADER_PATH}/ceguimouselistener.h
      ${HEADER_PATH}/ceguirenderer.h
      ${HEADER_PATH}/ceguitexture.h
      ${HEADER_PATH}/ceuidrawable.h
      ${HEADER_PATH}/dtgui.h
      ${HEADER_PATH}/export.h
      ${HEADER_PATH}/guiexceptionenum.h
      ${HEADER_PATH}/hud.h
      ${HEADER_PATH}/renderer.h
      ${HEADER_PATH}/resourceprovider.h
      ${HEADER_PATH}/scriptmodule.h
      ${HEADER_PATH}/widget.h
  )
  
  SET(LIB_SOURCES
      ${SOURCE_PATH}/ceguiconnectionmanager.cpp
      ${SOURCE_PATH}/ceguikeyboardlistener.cpp
      ${SOURCE_PATH}/ceguimouselistener.cpp
      ${SOURCE_PATH}/ceguirenderer.cpp
      ${SOURCE_PATH}/ceguitexture.cpp
      ${SOURCE_PATH}/ceuidrawable.cpp
      ${SOURCE_PATH}/guiexceptionenum.cpp
      ${SOURCE_PATH}/hud.cpp
      ${SOURCE_PATH}/renderer.cpp
      ${SOURCE_PATH}/resourceprovider.cpp
      ${SOURCE_PATH}/scriptmodule.cpp
  )
ENDIF (NOT BUILD_WITH_OLD_CEGUI)
 
list(REMOVE_ITEM LIB_SOURCES ${SOURCE_PATH}/precomp.cpp)

INCLUDE_DIRECTORIES( ${CEGUI_INCLUDE_DIR} )

ADD_LIBRARY(${LIB_NAME} SHARED                        
    ${LIB_PUBLIC_HEADERS}                         
    ${LIB_SOURCES}
)
 
 LINK_WITH_VARIABLES(${LIB_NAME}
                        OSG_LIBRARY
                        OSGGA_LIBRARY
                        OPENTHREADS_LIBRARY
                        CEGUI_LIBRARY
                        CEGUIOPENGL_LIBRARY
                    )
                        

TARGET_LINK_LIBRARIES(${LIB_NAME}  
                       ${DTUTIL_LIBRARY}
                       ${DTCORE_LIBRARY}
                       ${DTDAL_LIBRARY}
                       ${OPENGL_LIBRARY} 
                       ${MATH_LIBRARY}
                      )

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_GUI_LIBRARY)


INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC_IDE)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC_IDE)
