
# FIXME: For OS X, need flag for Framework or dylib

SET(LIB_NAME     dtDIS)

INCLUDE_DIRECTORIES( ${DIS_INCLUDE_DIR} ${DIS_CPPUTILS_INCLUDE_DIR} ${HAWKNL_INCLUDE_DIR} )

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/src/${LIB_NAME})

SET(LIB_PUBLIC_HEADERS 
		${HEADER_PATH}/acknowledgeconstants.h
		${HEADER_PATH}/activeentitycontrol.h
		${HEADER_PATH}/articulationconstants.h
		${HEADER_PATH}/connection.h
		${HEADER_PATH}/containerutils.h
		${HEADER_PATH}/createdestroypolicy.h
		${HEADER_PATH}/createdestroypolicy.inl
		${HEADER_PATH}/disxml.h
		${HEADER_PATH}/dllfinder.h
		${HEADER_PATH}/dtdisexport.h
		${HEADER_PATH}/entityidcompare.h
		${HEADER_PATH}/entitymap.h
		${HEADER_PATH}/entitytypeconstants.h
		${HEADER_PATH}/hasproperty.h
		${HEADER_PATH}/idisplugin.h
		${HEADER_PATH}/imessagetopacketadapter.h
		${HEADER_PATH}/libraryregistry.h
		${HEADER_PATH}/mastercomponent.h
		${HEADER_PATH}/outgoingmessage.h
		${HEADER_PATH}/pluginmanager.h
		${HEADER_PATH}/propertyname.h
		${HEADER_PATH}/sharedstate.h
		${HEADER_PATH}/valuemaps.h
		${HEADER_PATH}/plugins/default/actorupdatetoentitystate.h
		${HEADER_PATH}/plugins/default/createentityprocessor.h
		${HEADER_PATH}/plugins/default/defaultplugin.h
		${HEADER_PATH}/plugins/default/dtdisdefaultpluginexport.h
		${HEADER_PATH}/plugins/default/espduapplicator.h
		${HEADER_PATH}/plugins/default/espduprocessor.h
		${HEADER_PATH}/plugins/default/pluginsymbols.h
    ${HEADER_PATH}/plugins/default/removeentityprocessor.h
)

SET(LIB_SOURCES 
		${SOURCE_PATH}/activeentitycontrol.cpp
		${SOURCE_PATH}/articulationconstants.cpp
		${SOURCE_PATH}/connection.cpp
		${SOURCE_PATH}/disxml.cpp
		${SOURCE_PATH}/entityidcompare.cpp
		${SOURCE_PATH}/entitymap.cpp
		${SOURCE_PATH}/hasproperty.cpp
		${SOURCE_PATH}/idisplugin.cpp
		${SOURCE_PATH}/imessagetopacketadapter.cpp
		${SOURCE_PATH}/mastercomponent.cpp
		${SOURCE_PATH}/outgoingmessage.cpp
		${SOURCE_PATH}/pluginmanager.cpp
		${SOURCE_PATH}/propertyname.cpp
		${SOURCE_PATH}/sharedstate.cpp
		${SOURCE_PATH}/valuemaps.cpp
		${SOURCE_PATH}/plugins/default/actorupdatetoentitystate.cpp
		${SOURCE_PATH}/plugins/default/createentityprocessor.cpp
		${SOURCE_PATH}/plugins/default/defaultplugin.cpp
		${SOURCE_PATH}/plugins/default/espduapplicator.cpp
		${SOURCE_PATH}/plugins/default/espduprocessor.cpp
		${SOURCE_PATH}/plugins/default/pluginsymbols.cpp
		${SOURCE_PATH}/plugins/default/removeentityprocessor.cpp
)

ADD_LIBRARY(${LIB_NAME} SHARED                        
    ${LIB_PUBLIC_HEADERS}                         
    ${LIB_SOURCES}
)

LINK_WITH_VARIABLES(${LIB_NAME}
                    HAWKNL_LIBRARY
                    DIS_LIBRARY
                   )

IF (WIN32)   
TARGET_LINK_LIBRARIES(${LIB_NAME}
        wsock32)
ENDIF (WIN32)

TARGET_LINK_LIBRARIES(${LIB_NAME}
                      ${DTACTORS_LIBRARY}
                      ${DTUTIL_LIBRARY}
                      ${DTCORE_LIBRARY}
                      ${DTGAME_LIBRARY}
                      ${CARBON_LIBRARY}
                      )                                         
                                                
SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_DIS_LIBRARY COMPILE_FLAGS -DDT_DIS_DEFAULT_PLUGIN)         



INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC_IDE)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC_IDE)
