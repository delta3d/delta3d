
SET(LIB_NAME     dtAnim)


IF (WIN32)
ELSE (WIN32) 
ENDIF (WIN32)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/dtAnim)
SET(LIB_PUBLIC_HEADERS
   ${HEADER_PATH}/animatable.h
   ${HEADER_PATH}/animationchannel.h
   ${HEADER_PATH}/animationcomponent.h
   ${HEADER_PATH}/animationhelper.h
   ${HEADER_PATH}/animationsequence.h
   ${HEADER_PATH}/animationwrapper.h
   ${HEADER_PATH}/animdriver.h
   ${HEADER_PATH}/animnodebuilder.h
   ${HEADER_PATH}/attachmentcontroller.h
   ${HEADER_PATH}/cal3danimator.h
   ${HEADER_PATH}/cal3ddatabase.h
   ${HEADER_PATH}/cal3dloader.h
   ${HEADER_PATH}/cal3dmodeldata.h
   ${HEADER_PATH}/cal3dmodelwrapper.h
   ${HEADER_PATH}/characterfilehandler.h
   ${HEADER_PATH}/characterwrapper.h
   ${HEADER_PATH}/chardrawable.h
   ${HEADER_PATH}/dtanim.h
   ${HEADER_PATH}/export.h
   ${HEADER_PATH}/hardwaresubmesh.h
   ${HEADER_PATH}/hotspotdriver.h
   ${HEADER_PATH}/ical3ddriver.h
   ${HEADER_PATH}/morphdriver.h
   ${HEADER_PATH}/physiquedriver.h
   ${HEADER_PATH}/sequencemixer.h
   ${HEADER_PATH}/skeletaldrawable.h
   ${HEADER_PATH}/skeletondriver.h
   ${HEADER_PATH}/springdriver.h
   ${HEADER_PATH}/submesh.h
)

ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS} 
   animatable.cpp
   animationchannel.cpp
   animationcomponent.cpp
   animationhelper.cpp
   animationsequence.cpp
   animationwrapper.cpp
   animdriver.cpp
   animnodebuilder.cpp
   attachmentcontroller.cpp
   cal3danimator.cpp
   cal3ddatabase.cpp
   cal3dloader.cpp
   cal3dmodeldata.cpp
   cal3dmodelwrapper.cpp
   characterfilehandler.cpp
   characterwrapper.cpp
   chardrawable.cpp
   hardwaresubmesh.cpp
   hotspotdriver.cpp
   morphdriver.cpp
   physiquedriver.cpp
   sequencemixer.cpp
   skeletaldrawable.cpp
   skeletondriver.cpp
   springdriver.cpp
   submesh.cpp
)

LINK_WITH_VARIABLES(${LIB_NAME}
											  DTUTIL_LIBRARY
											  DTCORE_LIBRARY
											  DTAI_LIBRARY
											  DTDAL_LIBRARY
											  DTABC_LIBRARY
											  DTGAME_LIBRARY
											  CAL3D_LIBRARY
											  OSG_LIBRARY
											  OSGDB_LIBRARY
											  OPENTHREADS_LIBRARY
											  XERCES_LIBRARY)

TARGET_LINK_LIBRARIES(${LIB_NAME} 
													  ${OPENGL_LIBRARY} 
													  ${MATH_LIBRARY})

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_ANIM_LIBRARY)


INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
