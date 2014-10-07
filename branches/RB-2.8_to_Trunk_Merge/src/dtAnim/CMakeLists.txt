
SET(LIB_NAME     dtAnim)


SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/src/${LIB_NAME})

SET(LIB_SOURCES
  ${SOURCE_PATH}/animactorregistry.cpp
  ${SOURCE_PATH}/animatable.cpp
  ${SOURCE_PATH}/animationchannel.cpp
  ${SOURCE_PATH}/animationcomponent.cpp
  ${SOURCE_PATH}/animationgameactor.cpp
  ${SOURCE_PATH}/animationhelper.cpp
  ${SOURCE_PATH}/animationinterface.cpp
  ${SOURCE_PATH}/animationsequence.cpp
  ${SOURCE_PATH}/animdriver.cpp
  ${SOURCE_PATH}/animnodebuilder.cpp
  ${SOURCE_PATH}/animclippath.cpp
  ${SOURCE_PATH}/attachmentcontroller.cpp
  ${SOURCE_PATH}/basemodeldata.cpp
  ${SOURCE_PATH}/basemodelloader.cpp
  ${SOURCE_PATH}/basemodelwrapper.cpp
  ${SOURCE_PATH}/cal3danimator.cpp
  ${SOURCE_PATH}/cal3dloader.cpp
  ${SOURCE_PATH}/cal3dmodeldata.cpp
  ${SOURCE_PATH}/cal3dmodelwrapper.cpp
  ${SOURCE_PATH}/cal3dnodebuilder.cpp
  ${SOURCE_PATH}/cal3dobjects.cpp
  ${SOURCE_PATH}/characterfileelements.cpp
  ${SOURCE_PATH}/characterfilehandler.cpp
  ${SOURCE_PATH}/characterfileloader.cpp
  ${SOURCE_PATH}/characterfilewriter.cpp
  ${SOURCE_PATH}/charactershaderbuilder.cpp
  ${SOURCE_PATH}/characterwrapper.cpp
  ${SOURCE_PATH}/chardrawable.cpp
  ${SOURCE_PATH}/constants.cpp
  ${SOURCE_PATH}/geometrybuilder.cpp
  ${SOURCE_PATH}/hardwaresubmesh.cpp
  ${SOURCE_PATH}/hotspotdriver.cpp
  ${SOURCE_PATH}/lodcullcallback.cpp
  ${SOURCE_PATH}/modeldatabase.cpp
  ${SOURCE_PATH}/modelloader.cpp
  ${SOURCE_PATH}/morphdriver.cpp
  ${SOURCE_PATH}/osganimator.cpp
  ${SOURCE_PATH}/osgloader.cpp
  ${SOURCE_PATH}/osgmodeldata.cpp
  ${SOURCE_PATH}/osgmodelresourcefinder.cpp
  ${SOURCE_PATH}/osgmodelwrapper.cpp
  ${SOURCE_PATH}/osgnodebuilder.cpp
  ${SOURCE_PATH}/osgobjects.cpp
  ${SOURCE_PATH}/physiquedriver.cpp
  ${SOURCE_PATH}/posemath.cpp
  ${SOURCE_PATH}/posemesh.cpp
  ${SOURCE_PATH}/posemeshdatabase.cpp
  ${SOURCE_PATH}/posemeshloader.cpp
  ${SOURCE_PATH}/posemeshutility.cpp
  ${SOURCE_PATH}/posemeshxml.cpp
  ${SOURCE_PATH}/posesequence.cpp
  ${SOURCE_PATH}/sequencemixer.cpp
  ${SOURCE_PATH}/skeletaldrawable.cpp
  ${SOURCE_PATH}/skeletondriver.cpp
  ${SOURCE_PATH}/springdriver.cpp
  ${SOURCE_PATH}/animationtransitionplanner.cpp
  ${SOURCE_PATH}/submesh.cpp
  ${SOURCE_PATH}/walkrunblend.cpp
)

SET(LIB_PUBLIC_HEADERS
  ${HEADER_PATH}/animactorregistry.h
  ${HEADER_PATH}/animatable.h
  ${HEADER_PATH}/animationchannel.h
  ${HEADER_PATH}/animationcomponent.h
  ${HEADER_PATH}/animationgameactor.h
  ${HEADER_PATH}/animationhelper.h
  ${HEADER_PATH}/animationinterface.h
  ${HEADER_PATH}/animationsequence.h
  ${HEADER_PATH}/animationupdaterinterface.h
  ${HEADER_PATH}/animdriver.h
  ${HEADER_PATH}/animnodebuilder.h
  ${HEADER_PATH}/animclippath.h
  ${HEADER_PATH}/attachmentcontroller.h
  ${HEADER_PATH}/baseinterfaceobjectcache.h
  ${HEADER_PATH}/basemodeldata.h
  ${HEADER_PATH}/basemodelloader.h
  ${HEADER_PATH}/basemodelwrapper.h
  ${HEADER_PATH}/boneinterface.h
  ${HEADER_PATH}/cal3danimator.h
  ${HEADER_PATH}/cal3dloader.h
  ${HEADER_PATH}/cal3dmodeldata.h
  ${HEADER_PATH}/cal3dmodelwrapper.h
  ${HEADER_PATH}/cal3dnodebuilder.h
  ${HEADER_PATH}/cal3dobjects.h
  ${HEADER_PATH}/characterfileelements.h
  ${HEADER_PATH}/characterfilehandler.h
  ${HEADER_PATH}/characterfileloader.h
  ${HEADER_PATH}/characterfilewriter.h
  ${HEADER_PATH}/charactershaderbuilder.h
  ${HEADER_PATH}/characterwrapper.h
  ${HEADER_PATH}/chardrawable.h
  ${HEADER_PATH}/constants.h
  ${HEADER_PATH}/dtanim.h
  ${HEADER_PATH}/export.h
  ${HEADER_PATH}/geometrybuilder.h
  ${HEADER_PATH}/hardwaresubmesh.h
  ${HEADER_PATH}/hotspotdriver.h
  ${HEADER_PATH}/ical3ddriver.h
  ${HEADER_PATH}/lodcullcallback.h
  ${HEADER_PATH}/materialinterface.h
  ${HEADER_PATH}/meshinterface.h
  ${HEADER_PATH}/modeldatabase.h
  ${HEADER_PATH}/modelloader.h
  ${HEADER_PATH}/morphdriver.h
  ${HEADER_PATH}/morphinterface.h
  ${HEADER_PATH}/nodebuilderinterface.h
  ${HEADER_PATH}/osganimator.h
  ${HEADER_PATH}/osgloader.h
  ${HEADER_PATH}/osgmodeldata.h
  ${HEADER_PATH}/osgmodelresourcefinder.h
  ${HEADER_PATH}/osgmodelwrapper.h
  ${HEADER_PATH}/osgnodebuilder.h
  ${HEADER_PATH}/osgobjects.h
  ${HEADER_PATH}/physiquedriver.h
  ${HEADER_PATH}/posemath.h
  ${HEADER_PATH}/posemesh.h
  ${HEADER_PATH}/posemeshdatabase.h
  ${HEADER_PATH}/posemeshloader.h
  ${HEADER_PATH}/posemeshutility.h
  ${HEADER_PATH}/posemeshxml.h
  ${HEADER_PATH}/posesequence.h
  ${HEADER_PATH}/sequencemixer.h
  ${HEADER_PATH}/skeletaldrawable.h
  ${HEADER_PATH}/skeletondriver.h
  ${HEADER_PATH}/skeletoninterface.h
  ${HEADER_PATH}/springdriver.h
  ${HEADER_PATH}/animationtransitionplanner.h
  ${HEADER_PATH}/submesh.h
  ${HEADER_PATH}/walkrunblend.h
)

INCLUDE_DIRECTORIES( ${CAL3D_INCLUDE_DIR} )#for dtAnim

SET(LIB_EXTERNAL_DEPS
  CAL3D_LIBRARY
  OSGANIMATION_LIBRARY
)

SET(LIB_DEPS 
    ${OPENGL_LIBRARY} 
    ${MATH_LIBRARY}
    ${DTUTIL_LIBRARY}
    ${DTCORE_LIBRARY}
    ${DTGAME_LIBRARY}
    ${DTAI_LIBRARY}
 )

DELTA3D_ADD_LIBRARY(${LIB_NAME} DT_ANIM_LIBRARY SHARED)

