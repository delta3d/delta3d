
# FIXME: For OS X, need flag for Framework or dylib

SET(LIB_NAME     dtCore)


IF (WIN32)
   SET(PlatformSources deltawin_win32.cpp uniqueid_win32.cpp)
ELSE (WIN32) 
   IF (APPLE)
      SET(PlatformSources deltawin_carbon.cpp uniqueid_macosx.cpp)
   ELSE (APPLE)
      SET(PlatformSources deltawin_x11.cpp uniqueid_linux.cpp)      
   ENDIF (APPLE)
ENDIF (WIN32)

SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/dtCore)
SET(LIB_PUBLIC_HEADERS
   ${HEADER_PATH}/base.h
   ${HEADER_PATH}/batchisector.h
   ${HEADER_PATH}/boundingboxvisitor.h
   ${HEADER_PATH}/camera.h
   ${HEADER_PATH}/clouddome.h
   ${HEADER_PATH}/cloudplane.h
   ${HEADER_PATH}/collectorutil.h
   ${HEADER_PATH}/collisionmotionmodel.h
   ${HEADER_PATH}/compass.h
   ${HEADER_PATH}/deltadrawable.h
   ${HEADER_PATH}/deltawin.h
   ${HEADER_PATH}/dt.h
   ${HEADER_PATH}/effectmanager.h
   ${HEADER_PATH}/enveffect.h
   ${HEADER_PATH}/environment.h
   ${HEADER_PATH}/ephemeris.h
   ${HEADER_PATH}/exceptionenum.h
   ${HEADER_PATH}/export.h
   ${HEADER_PATH}/flymotionmodel.h
   ${HEADER_PATH}/fpscollider.h
   ${HEADER_PATH}/fpsmotionmodel.h
   ${HEADER_PATH}/generickeyboardlistener.h
   ${HEADER_PATH}/genericmouselistener.h
   ${HEADER_PATH}/globals.h
   ${HEADER_PATH}/hotspotattachment.h
   ${HEADER_PATH}/infinitelight.h
   ${HEADER_PATH}/infiniteterrain.h
   ${HEADER_PATH}/inputdevice.h
   ${HEADER_PATH}/inputmapper.h
   ${HEADER_PATH}/isector.h
   ${HEADER_PATH}/keyboard.h
   ${HEADER_PATH}/keyboardmousehandler.h
   ${HEADER_PATH}/light.h
   ${HEADER_PATH}/loadable.h
   ${HEADER_PATH}/logicalinputdevice.h
   ${HEADER_PATH}/mainpage.h
   ${HEADER_PATH}/makeskydome.h
   ${HEADER_PATH}/model.h
   ${HEADER_PATH}/motionmodel.h
   ${HEADER_PATH}/mouse.h
   ${HEADER_PATH}/moveearthtransform.h
   ${HEADER_PATH}/nodecollector.h
   ${HEADER_PATH}/notify.h
   ${HEADER_PATH}/object.h
   ${HEADER_PATH}/observerptr.h
   ${HEADER_PATH}/orbitmotionmodel.h
   ${HEADER_PATH}/particlesystem.h
   ${HEADER_PATH}/physical.h
   ${HEADER_PATH}/pointaxis.h
   ${HEADER_PATH}/positionallight.h
   ${HEADER_PATH}/recorder.h
   ${HEADER_PATH}/refptr.h
   ${HEADER_PATH}/scene.h
   ${HEADER_PATH}/shadergroup.h
   ${HEADER_PATH}/shadermanager.h
   ${HEADER_PATH}/shaderparameter.h
   ${HEADER_PATH}/shaderparamfloat.h
   ${HEADER_PATH}/shaderparamint.h
   ${HEADER_PATH}/shaderparamoscillator.h
   ${HEADER_PATH}/shaderparamtexture.h
   ${HEADER_PATH}/shaderparamtexture2d.h
   ${HEADER_PATH}/shaderprogram.h
   ${HEADER_PATH}/shaderxml.h
   ${HEADER_PATH}/sigslot.h
   ${HEADER_PATH}/skybox.h
   ${HEADER_PATH}/skydome.h
   ${HEADER_PATH}/skydomeshader.h
   ${HEADER_PATH}/spotlight.h
   ${HEADER_PATH}/stateattributecollector.h
   ${HEADER_PATH}/stats.h
   ${HEADER_PATH}/sunlightshader.h
   ${HEADER_PATH}/system.h
   ${HEADER_PATH}/tangentspacevisitor.h
   ${HEADER_PATH}/timer.h
   ${HEADER_PATH}/transform.h
   ${HEADER_PATH}/transformable.h
   ${HEADER_PATH}/tripod.h
   ${HEADER_PATH}/ufomotionmodel.h
   ${HEADER_PATH}/uniqueid.h
   ${HEADER_PATH}/view.h
   ${HEADER_PATH}/walkmotionmodel.h
)

ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS} 
base.cpp
batchisector.cpp
camera.cpp
clouddome.cpp
cloudplane.cpp
collisionmotionmodel.cpp
compass.cpp
deltadrawable.cpp
deltawin.cpp
effectmanager.cpp
enveffect.cpp
environment.cpp
ephemeris.cpp
exceptionenum.cpp
flymotionmodel.cpp
fpscollider.cpp
fpsmotionmodel.cpp
generickeyboardlistener.cpp
genericmouselistener.cpp
globals.cpp
hotspotattachment.cpp
infinitelight.cpp
infiniteterrain.cpp
inputdevice.cpp
inputmapper.cpp
isector.cpp
keyboard.cpp
keyboardmousehandler.cpp
light.cpp
loadable.cpp
logicalinputdevice.cpp
makeskydome.cpp
model.cpp
motionmodel.cpp
mouse.cpp
moveearthtransform.cpp
nodecollector.cpp
notify.cpp
object.cpp
orbitmotionmodel.cpp
particlesystem.cpp
physical.cpp
pointaxis.cpp
positionallight.cpp
precomp.cpp
scene.cpp
shadergroup.cpp
shadermanager.cpp
shaderparameter.cpp
shaderparamfloat.cpp
shaderparamint.cpp
shaderparamoscillator.cpp
shaderparamtexture.cpp
shaderparamtexture2d.cpp
shaderprogram.cpp
shaderxml.cpp
skybox.cpp
skydome.cpp
skydomeshader.cpp
spotlight.cpp
stateattributecollector.cpp
stats.cpp
sunlightshader.cpp
system.cpp
tangentspacevisitor.cpp
timer.cpp
transform.cpp
transformable.cpp
tripod.cpp
ufomotionmodel.cpp
view.cpp
walkmotionmodel.cpp
${PlatformSources}
)

LINK_WITH_VARIABLES(${LIB_NAME}
											  DTUTIL_LIBRARY
											  OSG_LIBRARY
											  OSGTEXT_LIBRARY											 
											  OSGDB_LIBRARY
											  OSGGA_LIBRARY
											  OSGVIEWER_LIBRARY
											  OSGPARTICLE_LIBRARY
											  OSGUTIL_LIBRARY
											  OPENTHREADS_LIBRARY
											  ODE_LIBRARY
											  XERCES_LIBRARY)

TARGET_LINK_LIBRARIES( ${LIB_NAME}			
					  Rpcrt4.lib
					  ${OPENGL_LIBRARY} 
					  ${MATH_LIBRARY}
					  ${CARBON_LIBRARY}
					  )

SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_CORE_LIBRARY)


INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
