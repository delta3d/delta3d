
SET(LIB_NAME     DPDialogEditor)

SET(SOURCE_PATH ${CMAKE_SOURCE_DIR}/utilities/DirectorPlugins/${LIB_NAME})
SET(HEADER_PATH ${CMAKE_SOURCE_DIR}/utilities/DirectorPlugins/${LIB_NAME})

SET(LIB_C_SOURCES
    ${SOURCE_PATH}/dialogconditionline.cpp
    ${SOURCE_PATH}/dialogdelayline.cpp
    ${SOURCE_PATH}/dialogeditor.cpp
    ${SOURCE_PATH}/dialoglineregistry.cpp
    ${SOURCE_PATH}/dialoglinetype.cpp
    ${SOURCE_PATH}/dialoglogline.cpp
    ${SOURCE_PATH}/dialogrefline.cpp
    ${SOURCE_PATH}/dialogswitchline.cpp
    ${SOURCE_PATH}/dialogtreeitems.cpp
)

SET(LIB_PUBLIC_HEADERS
    ${HEADER_PATH}/dialogconditionline.h
    ${HEADER_PATH}/dialogdelayline.h
    ${HEADER_PATH}/dialogeditor.h
    ${HEADER_PATH}/dialoglineregistry.h
    ${HEADER_PATH}/dialoglinetype.h
    ${HEADER_PATH}/dialoglogline.h
    ${HEADER_PATH}/dialogrefline.h
    ${HEADER_PATH}/dialogswitchline.h
    ${HEADER_PATH}/dialogtreeitems.h
    ${HEADER_PATH}/export.h
)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/utilities/DirectorPlugins/${LIB_NAME}
                    ${QT_INCLUDE_DIRS}
                    ${CMAKE_CURRENT_BINARY_DIR} # for ui*.h files
                    ${DELTA3D_SOURCE_DIR}/ui/dtDirectorQt # for director editor ui*.h files
)

qt5_wrap_cpp(MOC_SOURCES ${LIB_PUBLIC_HEADERS})
QT5_WRAP_UI(UI_SOURCES dialogeditor.ui)

SOURCE_GROUP("Generated Files" REGULAR_EXPRESSION qrc_.*cxx|moc_.*cxx)

SET(LIB_SOURCES
   ${LIB_C_SOURCES}
   ${UI_SOURCES}
   ${RCC_SOURCES}
   ${MOC_SOURCES}
)

SET(LIB_DEPS
                       ${DTUTIL_LIBRARY}
                       ${DTCORE_LIBRARY}
                       ${DTDIRECTOR_LIBRARY}
                       ${DTDIRECTORQT_LIBRARY}
                       ${QT_QTCORE_LIBRARY}
                       ${QT_QTGUI_LIBRARY}
                       ${QT_QTOPENGL_LIBRARY}
                      )


#put the binary into a "Director plugins" folder
SETUP_PLUGIN_WITH_OUTPUT_DIRS(${LIB_NAME} DT_DIRECTOR_DIALOG_EDITOR_PLUGIN directorplugins)
