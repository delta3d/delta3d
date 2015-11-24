SET(LIB_NAME     STDirectorTool)

SET(SOURCE_PATH ${CMAKE_SOURCE_DIR}/utilities/STAGE/plugins/${LIB_NAME})
SET(HEADER_PATH ${CMAKE_SOURCE_DIR}/utilities/STAGE/plugins/${LIB_NAME})

SET(LIB_C_SOURCES
    ${SOURCE_PATH}/director.cpp
    ${SOURCE_PATH}/directoruiplugin.cpp
)

SET(LIB_PUBLIC_HEADERS
    ${HEADER_PATH}/director.h
    ${HEADER_PATH}/directoruiplugin.h
    ${HEADER_PATH}/export.h
)

INCLUDE_DIRECTORIES(  ${CMAKE_SOURCE_DIR}/utilities/STAGE/inc
            ${QT_INCLUDE_DIRS}
            ${QT_INCLUDE_DIRS}
            ${DELTA3D_SOURCE_DIR}/ui/dtDirectorQt # for ui*.h files
)

qt5_wrap_cpp(MOC_SOURCES ${LIB_PUBLIC_HEADERS})
QT5_ADD_RESOURCES(RCC_SOURCES tool.qrc)

#put the binary into a "STAGE plugins" folder

SET(LIB_SOURCES
   ${LIB_C_SOURCES}
   ${RCC_SOURCES}
   ${MOC_SOURCES}
)

SET(LIB_DEPS
                       STAGE
                       ${DTUTIL_LIBRARY}
                       ${DTCORE_LIBRARY}
                       ${DTGAME_LIBRARY}
                       ${DTABC_LIBRARY}
                       ${DTDIRECTOR_LIBRARY}
                       ${DTDIRECTORQT_LIBRARY}
                       ${Qt5Core_LIBRARIES}
                       ${Qt5Gui_LIBRARIES}
                       ${Qt5OpenGL_LIBRARIES}
                       ${Qt5Widgets_LIBRARIES}
                      )

SETUP_PLUGIN_WITH_OUTPUT_DIRS(${LIB_NAME} DT_DIRECTOR_TOOL_PLUGIN stplugins)

