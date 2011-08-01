
SET(LIB_NAME     dtDirectorGUINodes)


SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/src/${LIB_NAME})


SET(LIB_PUBLIC_HEADERS
    ${HEADER_PATH}/activatewidget.h
    ${HEADER_PATH}/buttonevent.h
    ${HEADER_PATH}/getwidgetposition.h
    ${HEADER_PATH}/getwidgetproperty.h
    ${HEADER_PATH}/guinodemanager.h
    ${HEADER_PATH}/loadguischeme.h
    ${HEADER_PATH}/nodelibraryexport.h
    ${HEADER_PATH}/nodelibraryregistry.h
    ${HEADER_PATH}/setguicursor.h
    ${HEADER_PATH}/setlayoutvisibility.h
    ${HEADER_PATH}/setwidgetposition.h
    ${HEADER_PATH}/setwidgetproperty.h
    ${HEADER_PATH}/setwidgettext.h
    ${HEADER_PATH}/setwidgetvisibility.h
    ${HEADER_PATH}/toggleguicursor.h
   )


SET(LIB_SOURCES
    ${SOURCE_PATH}/activatewidget.cpp
    ${SOURCE_PATH}/buttonevent.cpp
    ${SOURCE_PATH}/getwidgetposition.cpp
    ${SOURCE_PATH}/getwidgetproperty.cpp
    ${SOURCE_PATH}/guinodemanager.cpp
    ${SOURCE_PATH}/loadguischeme.cpp
    ${SOURCE_PATH}/nodelibraryregistry.cpp
    ${SOURCE_PATH}/setguicursor.cpp
    ${SOURCE_PATH}/setlayoutvisibility.cpp
    ${SOURCE_PATH}/setwidgetposition.cpp
    ${SOURCE_PATH}/setwidgetproperty.cpp
    ${SOURCE_PATH}/setwidgettext.cpp
    ${SOURCE_PATH}/setwidgetvisibility.cpp
    ${SOURCE_PATH}/toggleguicursor.cpp
   )



ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS}
    ${LIB_SOURCES}
)

INCLUDE_DIRECTORIES( ${CEGUI_INCLUDE_DIR} )

TARGET_LINK_LIBRARIES(${LIB_NAME}
                      ${DTDIRECTOR_LIBRARY}
                      ${DTUTIL_LIBRARY}
                      ${DTCORE_LIBRARY}
                      ${DTABC_LIBRARY}
                      ${DTGUI_LIBRARY}
                     )

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DIRECTOR_GUI_NODE_LIBRARY)


INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC_IDE)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC_IDE)