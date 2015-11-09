
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

file(GLOB LIB_PUBLIC_HEADERS
   "${HEADER_PATH}/*.h"
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

