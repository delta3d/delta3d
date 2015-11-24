SET(LIB_NAME     dtInspectorQt)


SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME})

SET(LIB_PUBLIC_HEADERS
    ${HEADER_PATH}/actorview.h
    ${HEADER_PATH}/baseview.h
    ${HEADER_PATH}/cameraview.h
    ${HEADER_PATH}/clouddomeview.h
    ${HEADER_PATH}/deltawinview.h
    ${HEADER_PATH}/directorview.h
    ${HEADER_PATH}/drawableview.h
    ${HEADER_PATH}/environmentview.h
    ${HEADER_PATH}/export.h
    ${HEADER_PATH}/infiniteterrainview.h
    ${HEADER_PATH}/inspectorqt.h
    ${HEADER_PATH}/inspectorwindow.h
    ${HEADER_PATH}/iview.h
    ${HEADER_PATH}/labelactorview.h
    ${HEADER_PATH}/lightview.h
    ${HEADER_PATH}/objectview.h
    ${HEADER_PATH}/osgview.h
    ${HEADER_PATH}/particlesystemview.h
    ${HEADER_PATH}/propertyeditor.h
    ${HEADER_PATH}/sceneview.h
    ${HEADER_PATH}/skydomeview.h
    ${HEADER_PATH}/systemview.h
    ${HEADER_PATH}/transformableview.h
    ${HEADER_PATH}/viewview.h
    ${HEADER_PATH}/weatherview.h
)

SET(LIB_SOURCES 
    actorview.cpp
    baseview.cpp
    cameraview.cpp
    clouddomeview.cpp
    CMakeLists.txt
    deltawinview.cpp
    directorview.cpp
    drawableview.cpp
    dtinspectorqt.ui
    environmentview.cpp
    infiniteterrainview.cpp
    inspectorqt.cpp
    inspectorwindow.cpp
    labelactorview.cpp
    lightview.cpp
    objectview.cpp
    osgview.cpp
    particlesystemview.cpp
	 propertyeditor.cpp
    sceneview.cpp
    skydomeview.cpp
    systemview.cpp
    transformableview.cpp
    viewview.cpp
    weatherview.cpp
)

INCLUDE_DIRECTORIES(${HEADER_PATH}
                    ${QT_INCLUDE_DIRS}
                    ${CMAKE_CURRENT_BINARY_DIR} # for ui*.h files
                    ${DELTA3D_SOURCE_DIR}/ui/dtDirectorQt)

QT5_WRAP_CPP(MOC_SOURCES ${LIB_PUBLIC_HEADERS})
QT5_WRAP_UI(UI_SOURCES dtinspectorqt.ui)

SET(LIB_SOURCES 
    ${LIB_SOURCES}
    ${MOC_SOURCES}
    ${UI_SOURCES}
    )

SET(LIB_EXTERNAL_DEPS
  QT_QTCORE_LIBRARY
  QT_QTGUI_LIBRARY
 )

SET(LIB_DEPS
   ${DTDIRECTOR_LIBRARY}
   ${DTDIRECTORQT_LIBRARY}
   ${DTCORE_LIBRARY}
   ${DTABC_LIBRARY}
   ${DTQT_LIBRARY}
)

DELTA3D_ADD_LIBRARY(${LIB_NAME} DT_INSPECTORQT_LIBRARY)

if (X11_FOUND)
   TARGET_LINK_LIBRARIES( ${LIB_NAME}
               ${X11_LIBRARIES})
endif (X11_FOUND)
