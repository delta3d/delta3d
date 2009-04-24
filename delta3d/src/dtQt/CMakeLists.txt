SET(LIB_NAME     dtQt)


SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/src/${LIB_NAME})
file(GLOB LIB_PUBLIC_HEADERS "${HEADER_PATH}/*.h")

# These files are listed in alphabetical order.
set(LIB_SOURCES 

baselibrarylisteditor.cpp
deltastepper.cpp
dialoglistselection.cpp
librarypathseditor.cpp
osgadapterwidget.cpp
osggraphicswindowqt
projectcontextdialog.cpp
qtguiwindowsystemwrapper.cpp 
viewwindow.cpp

)

INCLUDE_DIRECTORIES(${SOURCE_PATH}/inc 
                    ${QT_INCLUDE_DIR}
                   )

QT4_WRAP_CPP(MOC_SOURCES ${LIB_PUBLIC_HEADERS})

ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS} 
    ${LIB_SOURCES}
    ${MOC_SOURCES}
)

LINK_WITH_VARIABLES(${LIB_NAME}
                    QT_QTCORE_LIBRARY
                    QT_QTGUI_LIBRARY
                    QT_QTOPENGL_LIBRARY
                   )

TARGET_LINK_LIBRARIES( ${LIB_NAME}
            ${DTCORE_LIBRARY}
            )

if (X11_FOUND)
   TARGET_LINK_LIBRARIES( ${LIB_NAME}
               ${X11_LIBRARIES})
endif (X11_FOUND)

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_QT_LIBRARY)


INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)

IF (PCHSupport_FOUND)
   ADD_PRECOMPILED_HEADER(${LIB_NAME} precomp.cpp true)
ENDIF (PCHSupport_FOUND)
