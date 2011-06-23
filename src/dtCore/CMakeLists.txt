
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


SET(HEADER_PATH ${DELTA3D_SOURCE_DIR}/inc/${LIB_NAME})
SET(SOURCE_PATH ${DELTA3D_SOURCE_DIR}/src/${LIB_NAME})

SET(LIB_PUBLIC_HEADERS 
                ${HEADER_PATH}/autolodscalecameracallback.h
                ${HEADER_PATH}/axis.h
                ${HEADER_PATH}/axishandler.h
                ${HEADER_PATH}/axislistener.h
                ${HEADER_PATH}/axisobserver.h
                ${HEADER_PATH}/axistoaxistransformation.h
                ${HEADER_PATH}/base.h
                ${HEADER_PATH}/batchisector.h
                ${HEADER_PATH}/button.h
                ${HEADER_PATH}/buttonhandler.h
                ${HEADER_PATH}/buttonlistener.h
                ${HEADER_PATH}/buttonobserver.h
                ${HEADER_PATH}/camera.h
                ${HEADER_PATH}/cameracallbackcontainer.h
                ${HEADER_PATH}/cameradrawcallback.h
                ${HEADER_PATH}/clouddome.h
                ${HEADER_PATH}/cloudplane.h
                ${HEADER_PATH}/collisioncategorydefaults.h
                ${HEADER_PATH}/collisionmotionmodel.h
                ${HEADER_PATH}/compass.h
                ${HEADER_PATH}/databasepager.h
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
                ${HEADER_PATH}/inputdevicefeature.h
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
                ${HEADER_PATH}/object.h
                ${HEADER_PATH}/objectmotionmodel.h
                ${HEADER_PATH}/observerptr.h
                ${HEADER_PATH}/odebodywrap.h
                ${HEADER_PATH}/odecontroller.h
                ${HEADER_PATH}/odegeomwrap.h
                ${HEADER_PATH}/odespacewrap.h
                ${HEADER_PATH}/odeworldwrap.h
                ${HEADER_PATH}/orbitmotionmodel.h
                ${HEADER_PATH}/particlesystem.h
                ${HEADER_PATH}/physical.h
                ${HEADER_PATH}/pointaxis.h
                ${HEADER_PATH}/positionallight.h
                ${HEADER_PATH}/recorder.h
                ${HEADER_PATH}/refptr.h
                ${HEADER_PATH}/rtsmotionmodel.h
                ${HEADER_PATH}/scene.h
                ${HEADER_PATH}/screenshotcallback.h
                ${HEADER_PATH}/shadergroup.h
                ${HEADER_PATH}/shadermanager.h
                ${HEADER_PATH}/shaderparameter.h
                ${HEADER_PATH}/shaderparamfloat.h
                ${HEADER_PATH}/shaderparamint.h
                ${HEADER_PATH}/shaderparamoscillator.h
                ${HEADER_PATH}/shaderparamtexture.h
                ${HEADER_PATH}/shaderparamtexture1d.h
                ${HEADER_PATH}/shaderparamtexture2d.h
                ${HEADER_PATH}/shaderparamtexture3d.h
                ${HEADER_PATH}/shaderparamtexturecubemap.h
                ${HEADER_PATH}/shaderparamvec4.h
                ${HEADER_PATH}/shaderprogram.h
                ${HEADER_PATH}/shaderxml.h
                ${HEADER_PATH}/sigslot.h
                ${HEADER_PATH}/singletonmanager.h
                ${HEADER_PATH}/skybox.h
                ${HEADER_PATH}/skyboxprofiles.h
                ${HEADER_PATH}/skydome.h
                ${HEADER_PATH}/skydomeshader.h
                ${HEADER_PATH}/spotlight.h
                ${HEADER_PATH}/stats.h
                ${HEADER_PATH}/sunlightshader.h
                ${HEADER_PATH}/system.h
                ${HEADER_PATH}/timer.h
                ${HEADER_PATH}/transform.h
                ${HEADER_PATH}/transformable.h
                ${HEADER_PATH}/tripod.h
                ${HEADER_PATH}/ufomotionmodel.h
                ${HEADER_PATH}/uniqueid.h
#                ${HEADER_PATH}/uniqueid_linux.h
#                ${HEADER_PATH}/uniqueid_macosx.h
#                ${HEADER_PATH}/uniqueid_win32.h
                ${HEADER_PATH}/view.h
                ${HEADER_PATH}/walkmotionmodel.h
                ${HEADER_PATH}/windowresizecallback.h
                ${HEADER_PATH}/windowresizecontainer.h)
				
SET(LIB_SOURCES 
                ${SOURCE_PATH}/autolodscalecameracallback.cpp
                ${SOURCE_PATH}/axis.cpp
                ${SOURCE_PATH}/axistoaxistransformation.cpp
                ${SOURCE_PATH}/base.cpp
                ${SOURCE_PATH}/batchisector.cpp
                ${SOURCE_PATH}/button.cpp
                ${SOURCE_PATH}/camera.cpp
                ${SOURCE_PATH}/cameracallbackcontainer.cpp
                ${SOURCE_PATH}/clouddome.cpp
                ${SOURCE_PATH}/cloudplane.cpp
                ${SOURCE_PATH}/collisionmotionmodel.cpp
                ${SOURCE_PATH}/compass.cpp
                ${SOURCE_PATH}/databasepager.cpp
                ${SOURCE_PATH}/deltadrawable.cpp
                ${SOURCE_PATH}/deltawin.cpp
#                ${SOURCE_PATH}/deltawin_carbon.cpp
#                ${SOURCE_PATH}/deltawin_win32.cpp
#                ${SOURCE_PATH}/deltawin_x11.cpp
                ${SOURCE_PATH}/effectmanager.cpp
                ${SOURCE_PATH}/enveffect.cpp
                ${SOURCE_PATH}/environment.cpp
                ${SOURCE_PATH}/ephemeris.cpp
                ${SOURCE_PATH}/exceptionenum.cpp
                ${SOURCE_PATH}/flymotionmodel.cpp
                ${SOURCE_PATH}/fpscollider.cpp
                ${SOURCE_PATH}/fpsmotionmodel.cpp
                ${SOURCE_PATH}/generickeyboardlistener.cpp
                ${SOURCE_PATH}/genericmouselistener.cpp
                ${SOURCE_PATH}/globals.cpp
                ${SOURCE_PATH}/hotspotattachment.cpp
                ${SOURCE_PATH}/infinitelight.cpp
                ${SOURCE_PATH}/infiniteterrain.cpp
                ${SOURCE_PATH}/inputdevice.cpp
                ${SOURCE_PATH}/inputdevicefeature.cpp
                ${SOURCE_PATH}/inputmapper.cpp
                ${SOURCE_PATH}/isector.cpp
                ${SOURCE_PATH}/keyboard.cpp
                ${SOURCE_PATH}/keyboardmousehandler.cpp
                ${SOURCE_PATH}/light.cpp
                ${SOURCE_PATH}/loadable.cpp
                ${SOURCE_PATH}/logicalinputdevice.cpp
                ${SOURCE_PATH}/makeskydome.cpp
                ${SOURCE_PATH}/model.cpp
                ${SOURCE_PATH}/motionmodel.cpp
                ${SOURCE_PATH}/mouse.cpp
                ${SOURCE_PATH}/moveearthtransform.cpp
                ${SOURCE_PATH}/object.cpp
                ${SOURCE_PATH}/objectmotionmodel.cpp
                ${SOURCE_PATH}/odebodywrap.cpp
                ${SOURCE_PATH}/odecontroller.cpp
                ${SOURCE_PATH}/odegeomwrap.cpp
                ${SOURCE_PATH}/odespacewrap.cpp
                ${SOURCE_PATH}/odeworldwrap.cpp
                ${SOURCE_PATH}/orbitmotionmodel.cpp
                ${SOURCE_PATH}/particlesystem.cpp
                ${SOURCE_PATH}/physical.cpp
                ${SOURCE_PATH}/pointaxis.cpp
                ${SOURCE_PATH}/positionallight.cpp
                ${SOURCE_PATH}/rtsmotionmodel.cpp
                ${SOURCE_PATH}/scene.cpp
                ${SOURCE_PATH}/screenshotcallback.cpp
                ${SOURCE_PATH}/shadergroup.cpp
                ${SOURCE_PATH}/shadermanager.cpp
                ${SOURCE_PATH}/shaderparameter.cpp
                ${SOURCE_PATH}/shaderparamfloat.cpp
                ${SOURCE_PATH}/shaderparamint.cpp
                ${SOURCE_PATH}/shaderparamoscillator.cpp
                ${SOURCE_PATH}/shaderparamtexture.cpp
                ${SOURCE_PATH}/shaderparamtexture1d.cpp
                ${SOURCE_PATH}/shaderparamtexture2d.cpp
                ${SOURCE_PATH}/shaderparamtexture3d.cpp
                ${SOURCE_PATH}/shaderparamtexturecubemap.cpp
                ${SOURCE_PATH}/shaderparamvec4.cpp
                ${SOURCE_PATH}/shaderprogram.cpp
                ${SOURCE_PATH}/shaderxml.cpp
                ${SOURCE_PATH}/sigslot.cpp
                ${SOURCE_PATH}/singletonmanager.cpp
                ${SOURCE_PATH}/skybox.cpp
                ${SOURCE_PATH}/skyboxprofiles.cpp
                ${SOURCE_PATH}/skydome.cpp
                ${SOURCE_PATH}/skydomeshader.cpp
                ${SOURCE_PATH}/spotlight.cpp
                ${SOURCE_PATH}/stats.cpp
                ${SOURCE_PATH}/sunlightshader.cpp
                ${SOURCE_PATH}/system.cpp
                ${SOURCE_PATH}/timer.cpp
                ${SOURCE_PATH}/transform.cpp
                ${SOURCE_PATH}/transformable.cpp
                ${SOURCE_PATH}/tripod.cpp
                ${SOURCE_PATH}/ufomotionmodel.cpp
                ${SOURCE_PATH}/uniqueid.cpp
#                ${SOURCE_PATH}/uniqueid_linux.cpp
#                ${SOURCE_PATH}/uniqueid_macosx.cpp
#                ${SOURCE_PATH}/uniqueid_win32.cpp
                ${SOURCE_PATH}/view.cpp
                ${SOURCE_PATH}/walkmotionmodel.cpp
                ${SOURCE_PATH}/windowresizecontainer.cpp)

#LIST(REMOVE_ITEM LIB_SOURCES 
#   ${SOURCE_PATH}/deltawin_win32.cpp 
#   ${SOURCE_PATH}/uniqueid_win32.cpp 
#   ${SOURCE_PATH}/deltawin_carbon.cpp 
#   ${SOURCE_PATH}/uniqueid_macosx.cpp 
#   ${SOURCE_PATH}/deltawin_x11.cpp 
#   ${SOURCE_PATH}/uniqueid_linux.cpp)

ADD_PRECOMPILED_HEADER(${LIB_NAME} prefix/dtcoreprefix.h prefix/dtcoreprefix.cpp LIB_SOURCES)

ADD_LIBRARY(${LIB_NAME} SHARED
    ${LIB_PUBLIC_HEADERS} 
    ${LIB_SOURCES}
    ${PlatformSources}
)

LINK_WITH_VARIABLES(${LIB_NAME}
                    OSG_LIBRARY
                    OSGTEXT_LIBRARY
                    OSGDB_LIBRARY
                    OSGGA_LIBRARY
                    OSGVIEWER_LIBRARY
                    OSGPARTICLE_LIBRARY
                    OSGUTIL_LIBRARY
                    OPENTHREADS_LIBRARY
                    ODE_LIBRARY
                    XERCES_LIBRARY
                    UUID_LIBRARY)

TARGET_LINK_LIBRARIES( ${LIB_NAME}
            ${DTUTIL_LIBRARY}
            ${OPENGL_LIBRARY} 
            ${MATH_LIBRARY}
            ${CARBON_LIBRARY}
            ${APPSERVICES_LIBRARY}
            )

if (X11_FOUND)
   TARGET_LINK_LIBRARIES( ${LIB_NAME}
               ${X11_LIBRARIES})
endif (X11_FOUND)

IF (WIN32)
   TARGET_LINK_LIBRARIES(${LIB_NAME} Rpcrt4)
ENDIF(WIN32)



INCLUDE(ModuleInstall OPTIONAL)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC_IDE)
  SET_TARGET_PROPERTIES(${LIB_NAME}
                     PROPERTIES DEFINE_SYMBOL DT_CORE_LIBRARY)
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "../../bin/")
  SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC_IDE)
