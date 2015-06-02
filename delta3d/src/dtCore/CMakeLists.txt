
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
                ${HEADER_PATH}/actorcomponentcontainer.h
                ${HEADER_PATH}/actoractorproperty.h
                ${HEADER_PATH}/actoridactorproperty.h
                ${HEADER_PATH}/actorhierarchynode.h
                ${HEADER_PATH}/actorpluginregistry.h
                ${HEADER_PATH}/actorproperty.h
                ${HEADER_PATH}/actorpropertyserializer.h
                ${HEADER_PATH}/actorproxy.h
                ${HEADER_PATH}/actorproxyicon.h
                ${HEADER_PATH}/actorfactory.h
                ${HEADER_PATH}/actortype.h
                ${HEADER_PATH}/arrayactorproperty.h
                ${HEADER_PATH}/arrayactorpropertybase.h
                ${HEADER_PATH}/arrayactorpropertycomplex.h
                ${HEADER_PATH}/autolodscalecameracallback.h
                ${HEADER_PATH}/axis.h
                ${HEADER_PATH}/axisenum.h
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
                ${HEADER_PATH}/colorrgbaactorproperty.h
                ${HEADER_PATH}/compass.h
                ${HEADER_PATH}/containeractorproperty.h
                ${HEADER_PATH}/containerselectoractorproperty.h
                ${HEADER_PATH}/databasepager.h
                ${HEADER_PATH}/datatype.h
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
                ${HEADER_PATH}/orbitmotionmodel.h
                ${HEADER_PATH}/particlesystem.h
                ${HEADER_PATH}/particleutil.h
                ${HEADER_PATH}/plugin_export.h
                ${HEADER_PATH}/pointaxis.h
                ${HEADER_PATH}/positionallight.h
                ${HEADER_PATH}/prefabactorregistry.h
                ${HEADER_PATH}/prefabactortype.h
                ${HEADER_PATH}/prefabiconhandler.h
                ${HEADER_PATH}/project.h
                ${HEADER_PATH}/projectconfig.h
                ${HEADER_PATH}/projectconfigreaderwriter.h
                ${HEADER_PATH}/projectconfigxmlhandler.h
                ${HEADER_PATH}/propertycontainer.h
                ${HEADER_PATH}/propertycontaineractorproperty.h
                ${HEADER_PATH}/propertymacros.h
                ${HEADER_PATH}/range.h
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
                ${HEADER_PATH}/transformpropertydelegate.h
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
                abstractenumactorproperty.cpp
                actoractorproperty.cpp
                actoridactorproperty.cpp
                actorhierarchynode.cpp
                actorpluginregistry.cpp
                actorproperty.cpp
                actorpropertyserializer.cpp
                actorproxyicon.cpp
                actortype.cpp
                arrayactorpropertybase.cpp
                autolodscalecameracallback.cpp
                axis.cpp
                axisenum.cpp
                axistoaxistransformation.cpp
                base.cpp
                baseactorobject.cpp
                basexml.cpp
                basexmlhandler.cpp
                batchisector.cpp
                bitmaskactorproperty.cpp
                booleanactorproperty.cpp
                button.cpp
                camera.cpp
                cameracallbackcontainer.cpp
                clouddome.cpp
                cloudplane.cpp
                colorrgbaactorproperty.cpp
                compass.cpp
                containeractorproperty.cpp
                containerselectoractorproperty.cpp
                databasepager.cpp
                datatype.cpp
                deltadrawable.cpp
                deltawin.cpp
#                deltawin_carbon.cpp
#                deltawin_win32.cpp
#                deltawin_x11.cpp
                directoryresourcetypehandler.cpp
                doubleactorproperty.cpp
                effectmanager.cpp
                enumactorproperty.cpp
                enveffect.cpp
                environment.cpp
                environmentactor.cpp
                ephemeris.cpp
                exceptionenum.cpp
                floatactorproperty.cpp
                flymotionmodel.cpp
                fpsmotionmodel.cpp
                gameevent.cpp
                gameeventactorproperty.cpp
                gameeventmanager.cpp
                generickeyboardlistener.cpp
                genericmouselistener.cpp
                groupactorproperty.cpp
                hotspotattachment.cpp
                infinitelight.cpp
                infiniteterrain.cpp
                inputdevice.cpp
                inputdevicefeature.cpp
                inputmapper.cpp
                intactorproperty.cpp
                isector.cpp
                keyboard.cpp
                keyboardmousehandler.cpp
                actorfactory.cpp
                light.cpp
                loadable.cpp
                logicalinputdevice.cpp
                longactorproperty.cpp
                makeskydome.cpp
                map.cpp
                mapcontenthandler.cpp
                mapheaderhandler.cpp
                mapxml.cpp
                mapxmlconstants.cpp
                model.cpp
                motionmodel.cpp
                mouse.cpp
                moveearthtransform.cpp
                namedactorparameter.cpp
                namedarrayparameter.cpp
                namedbitmaskparameter.cpp
                namedbooleanparameter.cpp
                namedcontainerparameter.cpp
                namedcontainerselectorparameter.cpp
                nameddoubleparameter.cpp
                namedenumparameter.cpp
                namedfloatparameter.cpp
                namedgameeventparameter.cpp
                namedgroupparameter.cpp
                namedintparameter.cpp
                namedlongintparameter.cpp
                namedparameter.cpp
                namedpropertycontainerparameter.cpp
                namedresourceparameter.cpp
                namedrgbacolorparameter.cpp
                namedrgbcolorparameter.cpp
                namedshortintparameter.cpp
                namedstringparameter.cpp
                namedunsignedcharparameter.cpp
                namedunsignedintparameter.cpp
                namedunsignedlongintparameter.cpp
                namedunsignedshortintparameter.cpp
                namedvectorparameters.cpp
                object.cpp
                objectmotionmodel.cpp
                objecttype.cpp
                orbitmotionmodel.cpp
                particlesystem.cpp
                particleutil.cpp
                pointaxis.cpp
                positionallight.cpp
                prefabiconhandler.cpp
                prefabactorregistry.cpp
                prefabactortype.cpp
                project.cpp
                projectconfig.cpp
                projectconfigreaderwriter.cpp
                projectconfigxmlhandler.cpp
                propertycontainer.cpp
                propertycontaineractorproperty.cpp
                resourceactorproperty.cpp
                resourcedescriptor.cpp
                resourcehelper.cpp
                rtsmotionmodel.cpp
                scene.cpp
                screenshotcallback.cpp
                shadergroup.cpp
                shadermanager.cpp
                shaderparambool.cpp
                shaderparameteractorproperty.cpp
                shaderparameter.cpp
                shaderparamfloat.cpp
                shaderparamint.cpp
                shaderparamoscillator.cpp
                shaderparamtexture.cpp
                shaderparamtexture1d.cpp
                shaderparamtexture2d.cpp
                shaderparamtexture3d.cpp
                shaderparamtexturecubemap.cpp
                shaderparamvec4.cpp
                shaderprogram.cpp
                shaderxml.cpp
                sigslot.cpp
                singletonmanager.cpp
                skybox.cpp
                skyboxprofiles.cpp
                skydome.cpp
                skydomeshader.cpp
                spotlight.cpp
                stats.cpp
                stringactorproperty.cpp
                stringselectoractorproperty.cpp
                sunlightshader.cpp
                system.cpp
                systemcomponenttype.cpp
                transformableactorproxy.cpp
                transformpropertydelegate.cpp
                timer.cpp
                transform.cpp
                transformable.cpp
                tripod.cpp
                ufomotionmodel.cpp
                uniqueid.cpp
#                uniqueid_linux.cpp
#                uniqueid_macosx.cpp
#                uniqueid_win32.cpp
                vectoractorproperties.cpp
                view.cpp
                walkmotionmodel.cpp
                windowresizecontainer.cpp
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

