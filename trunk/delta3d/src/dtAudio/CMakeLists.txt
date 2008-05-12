# FIXME: For OS X, need flag for Framework or dylib

SET(LIB_NAME     dtAudio)


SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/src/${LIB_NAME})
file(GLOB LIB_PUBLIC_HEADERS "${HEADER_PATH}/*.h")
file(GLOB LIB_SOURCES "*.cpp")
list(REMOVE_ITEM LIB_SOURCES ${SOURCE_PATH}/precomp.cpp)

ADD_LIBRARY(${LIB_NAME} SHARED                        
    ${LIB_PUBLIC_HEADERS}                         
    ${LIB_SOURCES}
)

LINK_WITH_VARIABLES(${LIB_NAME}
											  DTCORE_LIBRARY
											  DTUTIL_LIBRARY
											  OSG_LIBRARY
											  OSGDB_LIBRARY
											  OPENTHREADS_LIBRARY
											  XERCES_LIBRARY)

TARGET_LINK_LIBRARIES( ${LIB_NAME}		
					                                  ${OPENAL_LIBRARY} 
					                                  ${ALUT_LIBRARY} )

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_AUDIO_LIBRARY)


INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
