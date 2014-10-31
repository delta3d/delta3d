
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
                ${HEADER_PATH}/abstractenumactorproperty.h
                ${HEADER_PATH}/abstractparameter.h
                ${HEADER_PATH}/actoractorproperty.h
                ${HEADER_PATH}/actoridactorproperty.h
                ${HEADER_PATH}/actorhierarchynode.h
                ${HEADER_PATH}/actorpluginregistry.h
                ${HEADER_PATH}/actorproperty.h
                ${HEADER_PATH}/actorpropertyserializer.h
                ${HEADER_PATH}/actorproxy.h
                ${HEADER_PATH}/actorproxyicon.h
                ${HEADER_PATH}/actortype.h
                ${HEADER_PATH}/arrayactorproperty.h
                ${HEADER_PATH}/arrayactorpropertybase.h
                ${HEADER_PATH}/arrayactorpropertycomplex.h
                ${HEADER_PATH}/autolodscalecameracallback.h
                ${HEADER_PATH}/axis.h
                ${HEADER_PATH}/axishandler.h
                ${HEADER_PATH}/axislistener.h
                ${HEADER_PATH}/axisobserver.h
                ${HEADER_PATH}/axistoaxistransformation.h
                ${HEADER_PATH}/base.h
                ${HEADER_PATH}/baseactorobject.h
                ${HEADER_PATH}/basexml.h
                ${HEADER_PATH}/basexmlhandler.h
                ${HEADER_PATH}/basexmlreaderwriter.h
                ${HEADER_PATH}/batchisector.h
                ${HEADER_PATH}/bitmaskactorproperty.h
                ${HEADER_PATH}/booleanactorproperty.h
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
                ${HEADER_PATH}/collisiongeometrytypeenum.h
                ${HEADER_PATH}/colorrgbaactorproperty.h
                ${HEADER_PATH}/compass.h
                ${HEADER_PATH}/containeractorproperty.h
                ${HEADER_PATH}/containerselectoractorproperty.h
                ${HEADER_PATH}/databasepager.h
                ${HEADER_PATH}/datatype.h
                ${HEADER_PATH}/defaultpropertymanager.h
                ${HEADER_PATH}/deltadrawable.h
                ${HEADER_PATH}/deltawin.h
                ${HEADER_PATH}/directoryresourcetypehandler.h
                ${HEADER_PATH}/doubleactorproperty.h
                ${HEADER_PATH}/effectmanager.h
                ${HEADER_PATH}/enginepropertytypes.h
                ${HEADER_PATH}/enumactorproperty.h
                ${HEADER_PATH}/enveffect.h
                ${HEADER_PATH}/environment.h
                ${HEADER_PATH}/environmentactor.h
                ${HEADER_PATH}/ephemeris.h
                ${HEADER_PATH}/exceptionenum.h
                ${HEADER_PATH}/export.h
                ${HEADER_PATH}/floatactorproperty.h
                ${HEADER_PATH}/flymotionmodel.h
                ${HEADER_PATH}/fpscollider.h
                ${HEADER_PATH}/fpsmotionmodel.h
                ${HEADER_PATH}/functor.h
                ${HEADER_PATH}/gameevent.h
                ${HEADER_PATH}/gameeventactorproperty.h
                ${HEADER_PATH}/gameeventmanager.h
                ${HEADER_PATH}/genericactorproperty.h
                ${HEADER_PATH}/generickeyboardlistener.h
                ${HEADER_PATH}/genericmouselistener.h
                ${HEADER_PATH}/groupactorproperty.h
                ${HEADER_PATH}/hotspotattachment.h
                ${HEADER_PATH}/infinitelight.h
                ${HEADER_PATH}/infiniteterrain.h
                ${HEADER_PATH}/inputdevice.h
                ${HEADER_PATH}/inputdevicefeature.h
                ${HEADER_PATH}/inputmapper.h
                ${HEADER_PATH}/intactorproperty.h
                ${HEADER_PATH}/isector.h
                ${HEADER_PATH}/keyboard.h
                ${HEADER_PATH}/keyboardmousehandler.h
                ${HEADER_PATH}/librarymanager.h
                ${HEADER_PATH}/light.h
                ${HEADER_PATH}/loadable.h
                ${HEADER_PATH}/logicalinputdevice.h
                ${HEADER_PATH}/longactorproperty.h
                ${HEADER_PATH}/mainpage.h
                ${HEADER_PATH}/makeskydome.h
                ${HEADER_PATH}/map.h
                ${HEADER_PATH}/mapcontenthandler.h
                ${HEADER_PATH}/mapheaderdata.h
                ${HEADER_PATH}/mapheaderhandler.h
                ${HEADER_PATH}/mapxml.h
                ${HEADER_PATH}/mapxmlconstants.h
                ${HEADER_PATH}/model.h
                ${HEADER_PATH}/motioninterface.h
                ${HEADER_PATH}/motionmodel.h
                ${HEADER_PATH}/mouse.h
                ${HEADER_PATH}/moveearthtransform.h
                ${HEADER_PATH}/namedactorparameter.h
                ${HEADER_PATH}/namedarrayparameter.h
                ${HEADER_PATH}/namedbitmaskparameter.h
                ${HEADER_PATH}/namedbooleanparameter.h
                ${HEADER_PATH}/namedcontainerparameter.h
                ${HEADER_PATH}/namedcontainerselectorparameter.h
                ${HEADER_PATH}/nameddoubleparameter.h
                ${HEADER_PATH}/namedenumparameter.h
                ${HEADER_PATH}/namedfloatparameter.h
                ${HEADER_PATH}/namedgameeventparameter.h
                ${HEADER_PATH}/namedgenericparameter.h
                ${HEADER_PATH}/namedgroupparameter.h
                ${HEADER_PATH}/namedgroupparameter.inl
                ${HEADER_PATH}/namedintparameter.h
                ${HEADER_PATH}/namedlongintparameter.h
                ${HEADER_PATH}/namedparameter.h
                ${HEADER_PATH}/namedparameters.h
                ${HEADER_PATH}/namedpodparameter.h
                ${HEADER_PATH}/namedpropertycontainerparameter.h
                ${HEADER_PATH}/namedresourceparameter.h
                ${HEADER_PATH}/namedrgbacolorparameter.h
                ${HEADER_PATH}/namedrgbcolorparameter.h
                ${HEADER_PATH}/namedshortintparameter.h
                ${HEADER_PATH}/namedstringparameter.h
                ${HEADER_PATH}/namedunsignedcharparameter.h
                ${HEADER_PATH}/namedunsignedintparameter.h
                ${HEADER_PATH}/namedunsignedlongintparameter.h
                ${HEADER_PATH}/namedunsignedshortintparameter.h
                ${HEADER_PATH}/namedvectorparameters.h
                ${HEADER_PATH}/object.h
                ${HEADER_PATH}/objecttype.h
                ${HEADER_PATH}/objectmotionmodel.h
                ${HEADER_PATH}/observerptr.h
                ${HEADER_PATH}/odebodywrap.h
                ${HEADER_PATH}/odecontroller.h
                ${HEADER_PATH}/odegeomwrap.h
                ${HEADER_PATH}/odespacewrap.h
                ${HEADER_PATH}/odeworldwrap.h
                ${HEADER_PATH}/orbitmotionmodel.h
                ${HEADER_PATH}/particlesystem.h
                ${HEADER_PATH}/particleutil.h
                ${HEADER_PATH}/physical.h
                ${HEADER_PATH}/physicalactorproxy.h
                ${HEADER_PATH}/plugin_export.h
                ${HEADER_PATH}/pointaxis.h
                ${HEADER_PATH}/positionallight.h
                ${HEADER_PATH}/prefabiconhandler.h
                ${HEADER_PATH}/project.h
                ${HEADER_PATH}/projectconfig.h
                ${HEADER_PATH}/projectconfigreaderwriter.h
                ${HEADER_PATH}/projectconfigxmlhandler.h
                ${HEADER_PATH}/propertycontainer.h
                ${HEADER_PATH}/propertycontaineractorproperty.h
                ${HEADER_PATH}/propertymacros.h
                ${HEADER_PATH}/recorder.h
                ${HEADER_PATH}/refptr.h
                ${HEADER_PATH}/resourceactorproperty.h
                ${HEADER_PATH}/resourcedescriptor.h
                ${HEADER_PATH}/resourcehelper.h
                ${HEADER_PATH}/resourcetreenode.h
                ${HEADER_PATH}/rtsmotionmodel.h
                ${HEADER_PATH}/scene.h
                ${HEADER_PATH}/screenshotcallback.h
                ${HEADER_PATH}/serializeable.h
                ${HEADER_PATH}/shadergroup.h
                ${HEADER_PATH}/shadermanager.h
                ${HEADER_PATH}/shaderparambool.h
                ${HEADER_PATH}/shaderparameter.h
                ${HEADER_PATH}/shaderparameteractorproperty.h
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
                ${HEADER_PATH}/stringactorproperty.h
                ${HEADER_PATH}/stringselectoractorproperty.h
                ${HEADER_PATH}/sunlightshader.h
                ${HEADER_PATH}/system.h
                ${HEADER_PATH}/systemcomponenttype.h
                ${HEADER_PATH}/timer.h
                ${HEADER_PATH}/transform.h
                ${HEADER_PATH}/transformable.h
                ${HEADER_PATH}/transformableactorproxy.h
                ${HEADER_PATH}/tripod.h
                ${HEADER_PATH}/typetoactorproperty.h
                ${HEADER_PATH}/ufomotionmodel.h
                ${HEADER_PATH}/uniqueid.h
#                ${HEADER_PATH}/uniqueid_linux.h
#                ${HEADER_PATH}/uniqueid_macosx.h
#                ${HEADER_PATH}/uniqueid_win32.h
                ${HEADER_PATH}/vectoractorproperties.h
                ${HEADER_PATH}/view.h
                ${HEADER_PATH}/walkmotionmodel.h
                ${HEADER_PATH}/windowresizecallback.h
                ${HEADER_PATH}/windowresizecontainer.h
                )
				
SET(LIB_SOURCES 
                ${SOURCE_PATH}/abstractenumactorproperty.cpp
                ${SOURCE_PATH}/actoractorproperty.cpp
                ${SOURCE_PATH}/actoridactorproperty.cpp
                ${SOURCE_PATH}/actorhierarchynode.cpp
                ${SOURCE_PATH}/actorpluginregistry.cpp
                ${SOURCE_PATH}/actorproperty.cpp
                ${SOURCE_PATH}/actorpropertyserializer.cpp
                ${SOURCE_PATH}/actorproxyicon.cpp
                ${SOURCE_PATH}/actortype.cpp
                ${SOURCE_PATH}/arrayactorpropertybase.cpp
                ${SOURCE_PATH}/autolodscalecameracallback.cpp
                ${SOURCE_PATH}/axis.cpp
                ${SOURCE_PATH}/axistoaxistransformation.cpp
                ${SOURCE_PATH}/base.cpp
                ${SOURCE_PATH}/baseactorobject.cpp
                ${SOURCE_PATH}/basexml.cpp
                ${SOURCE_PATH}/basexmlhandler.cpp
                ${SOURCE_PATH}/batchisector.cpp
                ${SOURCE_PATH}/bitmaskactorproperty.cpp
                ${SOURCE_PATH}/booleanactorproperty.cpp
                ${SOURCE_PATH}/button.cpp
                ${SOURCE_PATH}/camera.cpp
                ${SOURCE_PATH}/cameracallbackcontainer.cpp
                ${SOURCE_PATH}/clouddome.cpp
                ${SOURCE_PATH}/cloudplane.cpp
                ${SOURCE_PATH}/collisionmotionmodel.cpp
                ${SOURCE_PATH}/collisiongeometrytypeenum.cpp
                ${SOURCE_PATH}/colorrgbaactorproperty.cpp
                ${SOURCE_PATH}/compass.cpp
                ${SOURCE_PATH}/containeractorproperty.cpp
                ${SOURCE_PATH}/containerselectoractorproperty.cpp
                ${SOURCE_PATH}/databasepager.cpp
                ${SOURCE_PATH}/datatype.cpp
                ${SOURCE_PATH}/defaultpropertymanager.cpp
                ${SOURCE_PATH}/deltadrawable.cpp
                ${SOURCE_PATH}/deltawin.cpp
#                ${SOURCE_PATH}/deltawin_carbon.cpp
#                ${SOURCE_PATH}/deltawin_win32.cpp
#                ${SOURCE_PATH}/deltawin_x11.cpp
                ${SOURCE_PATH}/directoryresourcetypehandler.cpp
                ${SOURCE_PATH}/doubleactorproperty.cpp
                ${SOURCE_PATH}/effectmanager.cpp
                ${SOURCE_PATH}/enumactorproperty.cpp
                ${SOURCE_PATH}/enveffect.cpp
                ${SOURCE_PATH}/environment.cpp
                ${SOURCE_PATH}/environmentactor.cpp
                ${SOURCE_PATH}/ephemeris.cpp
                ${SOURCE_PATH}/exceptionenum.cpp
                ${SOURCE_PATH}/floatactorproperty.cpp
                ${SOURCE_PATH}/flymotionmodel.cpp
                ${SOURCE_PATH}/fpscollider.cpp
                ${SOURCE_PATH}/fpsmotionmodel.cpp
                ${SOURCE_PATH}/gameevent.cpp
                ${SOURCE_PATH}/gameeventactorproperty.cpp
                ${SOURCE_PATH}/gameeventmanager.cpp
                ${SOURCE_PATH}/generickeyboardlistener.cpp
                ${SOURCE_PATH}/genericmouselistener.cpp
                ${SOURCE_PATH}/groupactorproperty.cpp
                ${SOURCE_PATH}/hotspotattachment.cpp
                ${SOURCE_PATH}/infinitelight.cpp
                ${SOURCE_PATH}/infiniteterrain.cpp
                ${SOURCE_PATH}/inputdevice.cpp
                ${SOURCE_PATH}/inputdevicefeature.cpp
                ${SOURCE_PATH}/inputmapper.cpp
                ${SOURCE_PATH}/intactorproperty.cpp
                ${SOURCE_PATH}/isector.cpp
                ${SOURCE_PATH}/keyboard.cpp
                ${SOURCE_PATH}/keyboardmousehandler.cpp
                ${SOURCE_PATH}/librarymanager.cpp
                ${SOURCE_PATH}/light.cpp
                ${SOURCE_PATH}/loadable.cpp
                ${SOURCE_PATH}/logicalinputdevice.cpp
                ${SOURCE_PATH}/longactorproperty.cpp
                ${SOURCE_PATH}/makeskydome.cpp
                ${SOURCE_PATH}/map.cpp
                ${SOURCE_PATH}/mapcontenthandler.cpp
                ${SOURCE_PATH}/mapheaderhandler.cpp
                ${SOURCE_PATH}/mapxml.cpp
                ${SOURCE_PATH}/mapxmlconstants.cpp
                ${SOURCE_PATH}/model.cpp
                ${SOURCE_PATH}/motionmodel.cpp
                ${SOURCE_PATH}/mouse.cpp
                ${SOURCE_PATH}/moveearthtransform.cpp
                ${SOURCE_PATH}/namedactorparameter.cpp
                ${SOURCE_PATH}/namedarrayparameter.cpp
                ${SOURCE_PATH}/namedbitmaskparameter.cpp
                ${SOURCE_PATH}/namedbooleanparameter.cpp
                ${SOURCE_PATH}/namedcontainerparameter.cpp
                ${SOURCE_PATH}/namedcontainerselectorparameter.cpp
                ${SOURCE_PATH}/nameddoubleparameter.cpp
                ${SOURCE_PATH}/namedenumparameter.cpp
                ${SOURCE_PATH}/namedfloatparameter.cpp
                ${SOURCE_PATH}/namedgameeventparameter.cpp
                ${SOURCE_PATH}/namedgroupparameter.cpp
                ${SOURCE_PATH}/namedintparameter.cpp
                ${SOURCE_PATH}/namedlongintparameter.cpp
                ${SOURCE_PATH}/namedparameter.cpp
                ${SOURCE_PATH}/namedpropertycontainerparameter.cpp
                ${SOURCE_PATH}/namedresourceparameter.cpp
                ${SOURCE_PATH}/namedrgbacolorparameter.cpp
                ${SOURCE_PATH}/namedrgbcolorparameter.cpp
                ${SOURCE_PATH}/namedshortintparameter.cpp
                ${SOURCE_PATH}/namedstringparameter.cpp
                ${SOURCE_PATH}/namedunsignedcharparameter.cpp
                ${SOURCE_PATH}/namedunsignedintparameter.cpp
                ${SOURCE_PATH}/namedunsignedlongintparameter.cpp
                ${SOURCE_PATH}/namedunsignedshortintparameter.cpp
                ${SOURCE_PATH}/namedvectorparameters.cpp
                ${SOURCE_PATH}/object.cpp
                ${SOURCE_PATH}/objectmotionmodel.cpp
                ${SOURCE_PATH}/objecttype.cpp
                ${SOURCE_PATH}/odebodywrap.cpp
                ${SOURCE_PATH}/odecontroller.cpp
                ${SOURCE_PATH}/odegeomwrap.cpp
                ${SOURCE_PATH}/odespacewrap.cpp
                ${SOURCE_PATH}/odeworldwrap.cpp
                ${SOURCE_PATH}/orbitmotionmodel.cpp
                ${SOURCE_PATH}/particlesystem.cpp
                ${SOURCE_PATH}/particleutil.cpp
                ${SOURCE_PATH}/physical.cpp
                ${SOURCE_PATH}/physicalactorproxy.cpp
                ${SOURCE_PATH}/pointaxis.cpp
                ${SOURCE_PATH}/positionallight.cpp
                ${SOURCE_PATH}/prefabiconhandler.cpp
                ${SOURCE_PATH}/project.cpp
                ${SOURCE_PATH}/projectconfig.cpp
                ${SOURCE_PATH}/projectconfigreaderwriter.cpp
                ${SOURCE_PATH}/projectconfigxmlhandler.cpp
                ${SOURCE_PATH}/propertycontainer.cpp
                ${SOURCE_PATH}/propertycontaineractorproperty.cpp
                ${SOURCE_PATH}/resourceactorproperty.cpp
                ${SOURCE_PATH}/resourcedescriptor.cpp
                ${SOURCE_PATH}/resourcehelper.cpp
                ${SOURCE_PATH}/rtsmotionmodel.cpp
                ${SOURCE_PATH}/scene.cpp
                ${SOURCE_PATH}/screenshotcallback.cpp
                ${SOURCE_PATH}/shadergroup.cpp
                ${SOURCE_PATH}/shadermanager.cpp
                ${SOURCE_PATH}/shaderparambool.cpp                
                ${SOURCE_PATH}/shaderparameteractorproperty.cpp
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
                ${SOURCE_PATH}/stringactorproperty.cpp
                ${SOURCE_PATH}/stringselectoractorproperty.cpp
                ${SOURCE_PATH}/sunlightshader.cpp
                ${SOURCE_PATH}/system.cpp
                ${SOURCE_PATH}/systemcomponenttype.cpp
                ${SOURCE_PATH}/transformableactorproxy.cpp
                ${SOURCE_PATH}/timer.cpp
                ${SOURCE_PATH}/transform.cpp
                ${SOURCE_PATH}/transformable.cpp
                ${SOURCE_PATH}/tripod.cpp
                ${SOURCE_PATH}/ufomotionmodel.cpp
                ${SOURCE_PATH}/uniqueid.cpp
#                ${SOURCE_PATH}/uniqueid_linux.cpp
#                ${SOURCE_PATH}/uniqueid_macosx.cpp
#                ${SOURCE_PATH}/uniqueid_win32.cpp
                ${SOURCE_PATH}/vectoractorproperties.cpp
                ${SOURCE_PATH}/view.cpp
                ${SOURCE_PATH}/walkmotionmodel.cpp
                ${SOURCE_PATH}/windowresizecontainer.cpp
                ${PlatformSources}
                )

ADD_PRECOMPILED_HEADER(${LIB_NAME} prefix/dtcoreprefix.h prefix/dtcoreprefix.cpp LIB_SOURCES)

SET(LIB_EXTERNAL_DEPS 
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

SET(LIB_DEPS 
   ${DTUTIL_LIBRARY}
   ${OPENGL_LIBRARY} 
   ${MATH_LIBRARY}
   ${APPSERVICES_LIBRARY}
   )



DELTA3D_ADD_LIBRARY(${LIB_NAME} DT_CORE_LIBRARY)

if (X11_FOUND)
   TARGET_LINK_LIBRARIES( ${LIB_NAME}
               ${X11_LIBRARIES})
endif (X11_FOUND)

IF (WIN32)
   TARGET_LINK_LIBRARIES(${LIB_NAME} Rpcrt4)
ENDIF(WIN32)

