
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

file(GLOB LIB_PUBLIC_HEADERS 
   "${HEADER_PATH}/*.h"
   )

SET(LIB_SOURCES 
                abstractenumactorproperty.cpp
                abstractparameter.cpp
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
                osgpropertycontainers.cpp
                particlesystem.cpp
                particleutil.cpp
                pointaxis.cpp
                positionallight.cpp
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
     XERCES_LIBRARY
     UUID_LIBRARY)


SET(LIB_DEPS 
   ${DTUTIL_LIBRARY}
   ${OPENGL_LIBRARY} 
   ${MATH_LIBRARY}
   ${APPSERVICES_LIBRARY}
  )

if (TBB_FOUND)
   INCLUDE_DIRECTORIES(${TBB_INCLUDE_DIR}) 
   ADD_DEFINITIONS(-DUSE_TBB_MEMORY_POOL)
   set(LIB_DEPS 
      ${LIB_DEPS}
      ${TBB_LIBRARIES}
      ${TBB_MALLOC_LIBRARIES}
      )
endif()


DELTA3D_ADD_LIBRARY(${LIB_NAME} DT_CORE_LIBRARY)

if (X11_FOUND)
   TARGET_LINK_LIBRARIES( ${LIB_NAME}
               ${X11_LIBRARIES})
endif (X11_FOUND)

IF (WIN32)
   TARGET_LINK_LIBRARIES(${LIB_NAME} Rpcrt4)
ENDIF(WIN32)

