
SET(APP_NAME     ParticleEditor)

SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/utilities/${APP_NAME})
file(GLOB PROG_HEADERS "inc/*.h")
file(GLOB PROG_SOURCES "src/*.cpp" "*.rc")
list(REMOVE_ITEM PROG_SOURCES ${SOURCE_PATH}/precomp.cpp)

INCLUDE_DIRECTORIES(${SOURCE_PATH}/inc ${QT_INCLUDE_DIR}
                                  ${CAL3D_INCLUDE_DIR} #for dtAnim
                                  ${CMAKE_CURRENT_BINARY_DIR} # for ui*.h files
                                  )

qt4_wrap_cpp(MOC_SOURCES ${PROG_HEADERS})
QT4_ADD_RESOURCES(RCC_SOURCES particleeditor.qrc)
QT4_WRAP_UI(UI_SOURCES particleeditor.ui)

ADD_EXECUTABLE(${APP_NAME}
    ${PROG_HEADERS}
    ${PROG_SOURCES}
    ${MOC_SOURCES}
    ${RCC_SOURCES}
    ${UI_SOURCES}
)

LINK_WITH_VARIABLES(${APP_NAME}
                                         QT_QTCORE_LIBRARY
                                         QT_QTGUI_LIBRARY
                                         QT_QTOPENGL_LIBRARY
                                         OSGFX_LIBRARY
                                         AGL_LIBRARY)

TARGET_LINK_LIBRARIES(${APP_NAME} 
                      ${DTUTIL_LIBRARY}
                      ${DTCORE_LIBRARY}
                      ${DTABC_LIBRARY}
                      ${DTQT_LIBRARY}
                     )
IF(WIN32)
  TARGET_LINK_LIBRARIES(${APP_NAME} User32 Gdi32 shell32 Advapi32 ole32 Ws2_32 Comctl32)
ENDIF(WIN32)
                                              
INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC_IDE)
  SET_TARGET_PROPERTIES(${APP_NAME} PROPERTIES PREFIX "../../bin/")
  SET_TARGET_PROPERTIES(${APP_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC_IDE)

IF (MSVC)
  SET_TARGET_PROPERTIES(${APP_NAME} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
ENDIF (MSVC)

IF (PCHSupport_FOUND)
   ADD_PRECOMPILED_HEADER(${APP_NAME} ${SOURCE_PATH}/src/precomp.cpp prefix/dtstageprefix-src.h true)
ENDIF (PCHSupport_FOUND)
