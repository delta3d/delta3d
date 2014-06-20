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
    ${HEADER_PATH}/physicalview.h
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
    physicalview.cpp
	 propertyeditor.cpp
    sceneview.cpp
    skydomeview.cpp
    systemview.cpp
    transformableview.cpp
    viewview.cpp
    weatherview.cpp
)

INCLUDE_DIRECTORIES(${HEADER_PATH}
                    ${QT_INCLUDE_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR} # for ui*.h files
                    ${DELTA3D_SOURCE_DIR}/ui/dtDirectorQt)

QT4_WRAP_CPP(MOC_SOURCES ${LIB_PUBLIC_HEADERS})
QT4_WRAP_UI(UI_SOURCES dtinspectorqt.ui)

#ADD_PRECOMPILED_HEADER(${LIB_NAME} prefix/dtcoreprefix.h prefix/dtcoreprefix.cpp LIB_SOURCES)

ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS}
    ${LIB_SOURCES}
    ${MOC_SOURCES}
    ${UI_SOURCES}
)

LINK_WITH_VARIABLES(${LIB_NAME}
                    QT_QTCORE_LIBRARY
                    QT_QTGUI_LIBRARY
                   )

TARGET_LINK_LIBRARIES( ${LIB_NAME}
            ${DTDIRECTOR_LIBRARY}
			${DTDIRECTORQT_LIBRARY}
            ${DTCORE_LIBRARY}
            ${DTABC_LIBRARY}
			${DTQT_LIBRARY}
            )

if (X11_FOUND)
   TARGET_LINK_LIBRARIES( ${LIB_NAME}
               ${X11_LIBRARIES})
endif (X11_FOUND)



INCLUDE(ModuleInstall OPTIONAL)

DELTA3D_ADD_LIBRARY_PROPERTIES(${LIB_NAME} DT_INSPECTORQT_LIBRARY ${LIB_PUBLIC_HEADERS})
