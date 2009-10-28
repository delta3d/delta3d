SET(LIB_NAME     dtInspectorQt)


SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME})

SET(LIB_PUBLIC_HEADERS
    ${HEADER_PATH}/baseview.h
    ${HEADER_PATH}/cameraview.h
    ${HEADER_PATH}/clouddomeview.h
    ${HEADER_PATH}/deltawinview.h
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
    ${HEADER_PATH}/particlesystemview.h
    ${HEADER_PATH}/physicalview.h
    ${HEADER_PATH}/sceneview.h
    ${HEADER_PATH}/skydomeview.h
    ${HEADER_PATH}/systemview.h
    ${HEADER_PATH}/transformableview.h
    ${HEADER_PATH}/viewview.h
    ${HEADER_PATH}/weatherview.h
)

SET(LIB_SOURCES 
    baseview.cpp
    cameraview.cpp
    clouddomeview.cpp
    CMakeLists.txt
    deltawinview.cpp
    drawableview.cpp
    dtinspectorqt.ui
    environmentview.cpp
    infiniteterrainview.cpp
    inspectorqt.cpp
    inspectorwindow.cpp
    labelactorview.cpp
    lightview.cpp
    objectview.cpp
    particlesystemview.cpp
    physicalview.cpp
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
                   )

QT4_WRAP_CPP(MOC_SOURCES ${LIB_PUBLIC_HEADERS})
QT4_WRAP_UI(UI_SOURCES dtinspectorqt.ui)

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
            ${DTCORE_LIBRARY}
            ${DTABC_LIBRARY}
            )

if (X11_FOUND)
   TARGET_LINK_LIBRARIES( ${LIB_NAME}
               ${X11_LIBRARIES})
endif (X11_FOUND)

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_INSPECTORQT_LIBRARY)


INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC_IDE)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC_IDE)

IF (PCHSupport_FOUND)
   ADD_PRECOMPILED_HEADER(${LIB_NAME} precomp.cpp true)
ENDIF (PCHSupport_FOUND)
