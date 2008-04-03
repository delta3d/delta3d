
SET(PROG_NAME AnimationViewer)

INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR})

SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/utilities/${PROG_NAME})
file(GLOB PROG_HEADERS "*.h")
file(GLOB PROG_SOURCES "*.cpp")
list(REMOVE_ITEM PROG_SOURCES ${SOURCE_PATH}/precomp.cpp)

qt4_wrap_cpp(MOC_SOURCES ${PROG_HEADERS})

ADD_EXECUTABLE(${PROG_NAME}
    ${PROG_HEADERS}
    ${PROG_SOURCES}
    ${MOC_SOURCES}
)

LINK_WITH_VARIABLES(${PROG_NAME}
                                         QT_QTCORE_LIBRARY
                                         QT_QTGUI_LIBRARY
                                         QT_QTOPENGL_LIBRARY
                                         DTUTIL_LIBRARY   
                                         DTCORE_LIBRARY
                                         DTDAL_LIBRARY
					 DTANIM_LIBRARY
					 DTAUDIO_LIBRARY
                                         DTABC_LIBRARY
					 DTGAME_LIBRARY 
					 OSGFX_LIBRARY
					 AGL_LIBRARY)

MESSAGE(STATUS ${QT_LIBRARIES})

#Not Sure if this is needed
#IF(WIN32)
#TARGET_LINK_LIBRARIES(${PROG_NAME} User32 Gdi32 shell32 Advapi32 ole32 )
#ENDIF(WIN32)
                                              
INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${PROG_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${PROG_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
