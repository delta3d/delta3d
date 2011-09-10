
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
  ${SOURCE_PATH}/animationsequence.cpp
  ${SOURCE_PATH}/animationwrapper.cpp
  ${SOURCE_PATH}/animdriver.cpp
  ${SOURCE_PATH}/animnodebuilder.cpp
  ${SOURCE_PATH}/attachmentcontroller.cpp
  ${SOURCE_PATH}/cal3danimator.cpp
  ${SOURCE_PATH}/cal3ddatabase.cpp
  ${SOURCE_PATH}/cal3dgameactor.cpp
  ${SOURCE_PATH}/cal3dloader.cpp
  ${SOURCE_PATH}/cal3dmodeldata.cpp
  ${SOURCE_PATH}/cal3dmodelwrapper.cpp
  ${SOURCE_PATH}/characterfileelements.cpp
  ${SOURCE_PATH}/characterfilehandler.cpp
  ${SOURCE_PATH}/characterfilewriter.cpp
  ${SOURCE_PATH}/characterwrapper.cpp
  ${SOURCE_PATH}/chardrawable.cpp
  ${SOURCE_PATH}/hardwaresubmesh.cpp
  ${SOURCE_PATH}/hotspotdriver.cpp
  ${SOURCE_PATH}/lodcullcallback.cpp
  ${SOURCE_PATH}/morphdriver.cpp
  ${SOURCE_PATH}/physiquedriver.cpp
  ${SOURCE_PATH}/posemath.cpp
  ${SOURCE_PATH}/posemesh.cpp
  ${SOURCE_PATH}/posemeshdatabase.cpp
  ${SOURCE_PATH}/posemeshloader.cpp
  ${SOURCE_PATH}/posemeshutility.cpp
  ${SOURCE_PATH}/posemeshxml.cpp
  ${SOURCE_PATH}/sequencemixer.cpp
  ${SOURCE_PATH}/skeletaldrawable.cpp
  ${SOURCE_PATH}/skeletondriver.cpp
  ${SOURCE_PATH}/springdriver.cpp
  ${SOURCE_PATH}/submesh.cpp
)

SET(LIB_PUBLIC_HEADERS
  ${HEADER_PATH}/animactorregistry.h
  ${HEADER_PATH}/animatable.h
  ${HEADER_PATH}/animationchannel.h
  ${HEADER_PATH}/animationcomponent.h
  ${HEADER_PATH}/animationgameactor.h
  ${HEADER_PATH}/animationhelper.h
  ${HEADER_PATH}/animationsequence.h
  ${HEADER_PATH}/animationwrapper.h
  ${HEADER_PATH}/animdriver.h
  ${HEADER_PATH}/animnodebuilder.h
  ${HEADER_PATH}/attachmentcontroller.h
  ${HEADER_PATH}/cal3danimator.h
  ${HEADER_PATH}/cal3ddatabase.h
  ${HEADER_PATH}/cal3dgameactor.h
  ${HEADER_PATH}/cal3dloader.h
  ${HEADER_PATH}/cal3dmodeldata.h
  ${HEADER_PATH}/cal3dmodelwrapper.h
  ${HEADER_PATH}/characterfileelements.h
  ${HEADER_PATH}/characterfilehandler.h
  ${HEADER_PATH}/characterfilewriter.h
  ${HEADER_PATH}/characterwrapper.h
  ${HEADER_PATH}/chardrawable.h
  ${HEADER_PATH}/dtanim.h
  ${HEADER_PATH}/export.h
  ${HEADER_PATH}/hardwaresubmesh.h
  ${HEADER_PATH}/hotspotdriver.h
  ${HEADER_PATH}/ical3ddriver.h
  ${HEADER_PATH}/lodcullcallback.h
  ${HEADER_PATH}/morphdriver.h
  ${HEADER_PATH}/physiquedriver.h
  ${HEADER_PATH}/posemath.h
  ${HEADER_PATH}/posemesh.h
  ${HEADER_PATH}/posemeshdatabase.h
  ${HEADER_PATH}/posemeshloader.h
  ${HEADER_PATH}/posemeshutility.h
  ${HEADER_PATH}/posemeshxml.h
  ${HEADER_PATH}/sequencemixer.h
  ${HEADER_PATH}/skeletaldrawable.h
  ${HEADER_PATH}/skeletondriver.h
  ${HEADER_PATH}/springdriver.h
  ${HEADER_PATH}/submesh.h
)

INCLUDE_DIRECTORIES( ${CAL3D_INCLUDE_DIR} )#for dtAnim

ADD_LIBRARY(${LIB_NAME} SHARED                        
    ${LIB_PUBLIC_HEADERS}                         
    ${LIB_SOURCES}
)

LINK_WITH_VARIABLES(${LIB_NAME}
                    CAL3D_LIBRARY
                   )

TARGET_LINK_LIBRARIES(${LIB_NAME} 
                      ${OPENGL_LIBRARY} 
                      ${MATH_LIBRARY}
                      ${DTUTIL_LIBRARY}
                      ${DTCORE_LIBRARY}
                      ${DTDAL_LIBRARAY}
                      ${DTGAME_LIBRARY}
                      )

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_ANIM_LIBRARY)


INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC_IDE)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC_IDE)
