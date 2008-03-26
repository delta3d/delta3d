
# FIXME: For OS X, need flag for Framework or dylib

SET(LIB_NAME     dtDIS)

IF (WIN32)
ELSE (WIN32) 
ENDIF (WIN32)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/dtDIS)
SET(LIB_PUBLIC_HEADERS
   ${HEADER_PATH}/acknowledgeconstants.h
   ${HEADER_PATH}/articulationconstants.h
   ${HEADER_PATH}/connection.h
   ${HEADER_PATH}/containerutils.h
   ${HEADER_PATH}/createdestroypolicy.h
   ${HEADER_PATH}/disxml.h
   ${HEADER_PATH}/dllfinder.h
   ${HEADER_PATH}/dtdisexport.h
   ${HEADER_PATH}/entityidcompare.h
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
   ${HEADER_PATH}/createdestroypolicy.inl
   
   ${HEADER_PATH}/plugins/default/actorupdatetoentitystate.h
   ${HEADER_PATH}/plugins/default/createentityprocessor.h
   ${HEADER_PATH}/plugins/default/defaultplugin.h
   ${HEADER_PATH}/plugins/default/dtdisdefaultpluginexport.h
   ${HEADER_PATH}/plugins/default/espduapplicator.h
   ${HEADER_PATH}/plugins/default/espduprocessor.h
   ${HEADER_PATH}/plugins/default/pluginsymbols.h
   ${HEADER_PATH}/plugins/default/removeentityprocessor.h
)

ADD_LIBRARY(${LIB_NAME} SHARED
   ${LIB_PUBLIC_HEADERS}
   articulationconstants.cpp
   connection.cpp
   disxml.cpp
   entityidcompare.cpp
   hasproperty.cpp
   idisplugin.cpp
   imessagetopacketadapter.cpp
   mastercomponent.cpp
   outgoingmessage.cpp
   pluginmanager.cpp
   propertyname.cpp
   sharedstate.cpp
   valuemaps.cpp
   plugins/default/actorupdatetoentitystate.cpp
   plugins/default/createentityprocessor.cpp
   plugins/default/defaultplugin.cpp
   plugins/default/espduapplicator.cpp
   plugins/default/espduprocessor.cpp
   plugins/default/pluginsymbols.cpp
   plugins/default/removeentityprocessor.cpp
)

TARGET_LINK_LIBRARIES(${LIB_NAME}
												optimized dtAI debug dtAID
												optimized dtDAL debug dtDALD
												optimized NLstatic debug NLstaticD
												optimized DIS debug DIS_debug
												optimized dtGame debug dtGameD
												wsock32
												${CARBON_LIBRARY}
												${FLTK_LIBRARY})																					
																								
#TARGET_LINK_LIBRARIES(${LIB_NAME} dtUtil dtCore dtAI dtDAL ${FLTK_LIBRARY} ${OPENGL_LIBRARY} ${MATH_LIBRARY}
#   ${OSG_LIBRARY} ${OSGDB_LIBRARY} ${OSGGA_LIBRARY} ${OSGPARTICLE_LIBRARY} ${OSGUTIL_LIBRARY}
#   ${OSGTEXT_LIBRARY} ${OSGSIM_LIBRARY} ${OSGFX_LIBRARY} ${OSGVIEWER_LIBRARY}
#   ${OPENTHREADS_LIBRARY} ${XERCES_LIBRARY} ${CARBON_LIBRARY})

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_DIS_LIBRARY COMPILE_FLAGS -DDT_DIS_DEFAULT_PLUGIN)				 



INCLUDE(ModuleInstall OPTIONAL)
