
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
    ${HEADER_PATH}/loadlayout.h
    ${HEADER_PATH}/nodelibraryexport.h
    ${HEADER_PATH}/nodelibraryregistry.h
    ${HEADER_PATH}/setguicursor.h
    ${HEADER_PATH}/setlayoutvisibility.h
    ${HEADER_PATH}/setwidgetposition.h
    ${HEADER_PATH}/setwidgetproperty.h
    ${HEADER_PATH}/setwidgettext.h
    ${HEADER_PATH}/setwidgetvisibility.h
    ${HEADER_PATH}/sliderevent.h
    ${HEADER_PATH}/toggleguicursor.h
   )


SET(LIB_SOURCES
    ${SOURCE_PATH}/activatewidget.cpp
    ${SOURCE_PATH}/buttonevent.cpp
    ${SOURCE_PATH}/getwidgetposition.cpp
    ${SOURCE_PATH}/getwidgetproperty.cpp
    ${SOURCE_PATH}/guinodemanager.cpp
    ${SOURCE_PATH}/loadguischeme.cpp
    ${SOURCE_PATH}/loadlayout.cpp
    ${SOURCE_PATH}/nodelibraryregistry.cpp
    ${SOURCE_PATH}/setguicursor.cpp
    ${SOURCE_PATH}/setlayoutvisibility.cpp
    ${SOURCE_PATH}/setwidgetposition.cpp
    ${SOURCE_PATH}/setwidgetproperty.cpp
    ${SOURCE_PATH}/setwidgettext.cpp
    ${SOURCE_PATH}/setwidgetvisibility.cpp
    ${SOURCE_PATH}/sliderevent.cpp
    ${SOURCE_PATH}/toggleguicursor.cpp
   )



INCLUDE_DIRECTORIES( ${CEGUI_INCLUDE_DIR} )

SET(LIB_DEPS
    ${DTDIRECTOR_LIBRARY}
    ${DTUTIL_LIBRARY}
    ${DTCORE_LIBRARY}
    ${DTABC_LIBRARY}
    ${DTGUI_LIBRARY}
   )

DELTA3D_ADD_LIBRARY(${LIB_NAME} DIRECTOR_GUI_NODE_LIBRARY SHARED)
