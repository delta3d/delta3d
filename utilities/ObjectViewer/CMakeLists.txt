
SET(PROG_NAME ObjectViewer)

INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR}
                                  ${CAL3D_INCLUDE_DIR}#for dtAnim
                                  )

SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/utilities/${PROG_NAME})
file(GLOB PROG_HEADERS "*.h")
file(GLOB PROG_SOURCES "*.cpp")
list(REMOVE_ITEM PROG_SOURCES ${SOURCE_PATH}/precomp.cpp)

QT4_ADD_RESOURCES(RCC_SOURCES ObjectViewerResources.qrc)
qt4_wrap_cpp(MOC_SOURCES ${PROG_HEADERS})

ADD_EXECUTABLE(${PROG_NAME}
    ${PROG_HEADERS}
    ${PROG_SOURCES}
    ${MOC_SOURCES}
    ${RCC_SOURCES}
)

LINK_WITH_VARIABLES(${PROG_NAME}
                                         QT_QTCORE_LIBRARY
                                         QT_QTGUI_LIBRARY
                                         QT_QTOPENGL_LIBRARY
                                         DTUTIL_LIBRARY
                                         DTCORE_LIBRARY
                                         DTDAL_LIBRARY
                                         DTANIM_LIBRARY
)




INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${PROG_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${PROG_NAME} PROPERTIES IMPORT_PREFIX "../")
  SET_TARGET_PROPERTIES(${PROG_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
ENDIF (MSVC)
