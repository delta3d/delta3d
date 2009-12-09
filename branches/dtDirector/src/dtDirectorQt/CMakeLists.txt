
SET(LIB_NAME     dtDirectorQt)


SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/src/${LIB_NAME})
file(GLOB LIB_PUBLIC_HEADERS "${HEADER_PATH}/*.h")
file(GLOB LIB_SOURCES "*.cpp")
list(REMOVE_ITEM LIB_SOURCES ${SOURCE_PATH}/precomp.cpp)

QT4_WRAP_CPP(MOC_SOURCES ${LIB_PUBLIC_HEADERS})
QT4_ADD_RESOURCES(RCC_SOURCES directorqt.qrc)

ADD_LIBRARY(${LIB_NAME} SHARED                        
    ${LIB_PUBLIC_HEADERS}                         
    ${LIB_SOURCES}
	${MOC_SOURCES}
    ${RCC_SOURCES}
)

INCLUDE_DIRECTORIES(${SOURCE_PATH}/inc 
                    ${QT_INCLUDE_DIR}
                   )

LINK_WITH_VARIABLES(${LIB_NAME}
                    QT_QTCORE_LIBRARY
                    QT_QTGUI_LIBRARY
                    QT_QTOPENGL_LIBRARY
                   )

TARGET_LINK_LIBRARIES( ${LIB_NAME}
                       ${DTDAL_LIBRARY}
					   ${DTDIRECTOR_LIBRARY}
					   ${DTQT_LIBRARY}
                     )

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DIRECTOR_QT_LIBRARY)


INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC_IDE)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC_IDE)
