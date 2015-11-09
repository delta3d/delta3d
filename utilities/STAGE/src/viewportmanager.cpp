/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2008, Alion Science and Technology Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * Matthew W. Campbell
 */
#include <prefix/stageprefix.h>
#include <QtGui/QWidget>
#include <QtOpenGL/QGLWidget>
#include <osg/Texture>
#include <osgDB/DatabasePager>
#include <osgViewer/CompositeViewer>
#include <dtUtil/log.h>
#include <dtCore/map.h>
#include <dtCore/system.h>
#include <dtCore/actorproxyicon.h>
#include <dtEditQt/viewportmanager.h>
#include <dtEditQt/orthoviewport.h>
#include <dtEditQt/perspectiveviewport.h>
#include <dtEditQt/viewportoverlay.h>
#include <dtEditQt/stagecamera.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/editordata.h>
#include <cmath>
namespace dtEditQt
{

   //Singleton global variable for the library manager.
   dtCore::RefPtr<ViewportManager> ViewportManager::sInstance(NULL);

   ///////////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(ViewportManager::ViewportType);
   ViewportManager::ViewportType ViewportManager::ViewportType::ORTHOGRAPHIC("ORTHOGRAPHIC");
   ViewportManager::ViewportType ViewportManager::ViewportType::PERSPECTIVE("PERSPECTIVE");
   ///////////////////////////////////////////////////////////////////////////////

   ///////////////////////////////////////////////////////////////////////////////
   ViewportManager::ViewportManager()
      : mSnapTranslationEnabled()
      , mSnapRotationEnabled()
      , mSnapScaleEnabled()
      , mSnapTranslation()
      , mSnapRotation()
      , mSnapScale()
      , mShareMasterContext(true)
      , mMasterViewport()
      , mMasterScene(new dtCore::Scene())
   {
      mViewportOverlay     = new ViewportOverlay();
      //mWorldCamera         = new StageCamera();
      mInChangeTransaction = false;

      mMasterScene->GetSceneNode()->addChild(mViewportOverlay->getOverlayGroup()); //TODO testing

      //mMasterView->SetCamera(mWorldCamera->getDeltaCamera());

      EditorEvents* editorEvents = &EditorEvents::GetInstance();

      connect(editorEvents, SIGNAL(actorProxyCreated(dtCore::ActorPtr, bool)),
              this, SLOT(onActorProxyCreated(dtCore::ActorPtr, bool)));

      connect(editorEvents,
              SIGNAL(actorPropertyChanged(dtCore::ActorPtr,ActorPropertyRefPtr)),
              this,
              SLOT(onActorPropertyChanged(dtCore::ActorPtr,ActorPropertyRefPtr)));

      connect(editorEvents,SIGNAL(projectChanged()),
              this, SLOT(refreshAllViewports()));
      connect(editorEvents,SIGNAL(currentMapChanged()),
              this, SLOT(onCurrentMapChanged()));
      connect(editorEvents, SIGNAL(beginChangeTransaction()),
              this, SLOT(onBeginChangeTransaction()));
      connect(editorEvents, SIGNAL(endChangeTransaction()),
              this, SLOT(onEndChangeTransaction()));

      connect(&EditorEvents::GetInstance(), SIGNAL(editorCloseEvent()),
              this, SLOT(onEditorShutDown()));
   }

   ///////////////////////////////////////////////////////////////////////////////
   ViewportManager::ViewportManager(const ViewportManager& rhs)
   {

   }

   ///////////////////////////////////////////////////////////////////////////////
   ViewportManager& ViewportManager::operator=(const ViewportManager& rhs)
   {
      return *this;
   }

   ///////////////////////////////////////////////////////////////////////////////
   ViewportManager::~ViewportManager()
   {
      mViewportList.clear();
   }

   ///////////////////////////////////////////////////////////////////////////////
   Viewport* ViewportManager::createViewport(const std::string& name, ViewportType& type, QWidget* parent)
   {
      Viewport* vp = NULL;

      // The master viewport for context sharing is the first one created.
      // Therefore, see if we have it and if so grab its context to pass
      // on to the new viewport.
      if (mShareMasterContext)
      {
         if (mMasterViewport == NULL)
         {
            // Must be the first viewport.  Nothing to share with in this case.
            vp = createViewportImpl(name, type, parent, NULL);
            mMasterViewport = vp;
         }
         else
         {
            vp = createViewportImpl(name, type, parent, (osg::GraphicsContext*)(mMasterViewport->GetWindow()->GetOsgViewerGraphicsWindow()));
         }
      }
      else
      {
         vp = createViewportImpl(name, type, parent, NULL);
      }

      // Now make sure the viewport we created has a valid OpenGL context.
      if (!vp || !vp->GetQGLWidget()->isValid())
      {
         LOG_ERROR("Error creating viewport.");
         return NULL;
      }

      vp->setScene(mMasterScene.get());
      mViewportList[vp->getName()] = vp;
      return vp;
   }

   ///////////////////////////////////////////////////////////////////////////////
   ViewportManager& ViewportManager::GetInstance()
   {
      if (ViewportManager::sInstance.get() == NULL)
      {
         ViewportManager::sInstance = new ViewportManager();
      }
      return *ViewportManager::sInstance.get();
   }

   ///////////////////////////////////////////////////////////////////////////////
   Viewport* ViewportManager::createViewportImpl(const std::string& name,
       ViewportType& type, QWidget* parent, osg::GraphicsContext* shareWith)
   {
      if (type == ViewportType::ORTHOGRAPHIC)
      {
         return new OrthoViewport(name, parent, shareWith);
      }
      else if (type == ViewportType::PERSPECTIVE)
      {
         return new PerspectiveViewport(name, parent, shareWith);
      }
      else
      {
         return NULL;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ViewportManager::refreshActorSelection(const std::vector< dtCore::RefPtr<dtCore::BaseActorObject> >& actors)
   {
      std::map<std::string, Viewport*>::iterator itor;
      for (itor = mViewportList.begin(); itor != mViewportList.end(); ++itor)
      {
         if (itor->second->getAutoInteractionMode())
         {
            itor->second->refreshActorSelection(actors);
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportManager::refreshAllViewports()
   {
      std::map<std::string, Viewport*>::iterator itor;
      for (itor = mViewportList.begin(); itor != mViewportList.end(); ++itor)
      {
         //std::cout << itor->first << "  -  " << ((itor->second->getAutoInteractionMode()) ? "on" : "off") << std::endl;

         // MG Always refresh the Preview window to prevent visual lockups when using style sheets
         // I don't know why this occurs but this hack at least keeps things going.
         if (itor->first == "Preview" || itor->second->getAutoInteractionMode())
         {
            itor->second->refresh();
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportManager::refreshScene()
   {
      std::map<std::string, Viewport*>::iterator itor;
      for (itor = mViewportList.begin(); itor != mViewportList.end(); ++itor)
      {
         if (itor->second->getAutoSceneUpdate())
         {
            itor->second->setScene(mMasterScene.get());
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportManager::clearMasterScene(
      const std::map<dtCore::UniqueId, dtCore::RefPtr<dtCore::BaseActorObject> >& proxies)
   {
      std::map< dtCore::UniqueId, dtCore::RefPtr<dtCore::BaseActorObject> >::const_iterator itor;

      for (itor = proxies.begin(); itor != proxies.end(); ++itor)
      {
         dtCore::BaseActorObject* proxy = const_cast<dtCore::BaseActorObject*>(itor->second.get());
         dtCore::ActorProxyIcon* billBoard = proxy->GetBillBoardIcon();
         if (billBoard != NULL)
         {
            mMasterScene->RemoveChild(billBoard->GetDrawable());
         }
         mMasterScene->RemoveChild(proxy->GetDrawable());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportManager::initializeGL()
   {
      static bool hasBeenInitialized = false;
      if (!hasBeenInitialized)
      {
         hasBeenInitialized = true;
      }
   }



   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec3 ViewportManager::GetSnapPosition(osg::Vec3 position, bool groundClamp, std::vector<dtCore::DeltaDrawable*> ignoredDrawables)
   {
      osg::Vec3 snapPos = position;

      // If snapping is not enabled, return the original position.
      if (mSnapTranslationEnabled)
      {
         for (int index = 0; index < 3; index++)
         {
            float trans = position[index];

            int mul = int(std::floor((trans / mSnapTranslation) + 0.5f));
            trans = mSnapTranslation * mul;

            snapPos[index] = trans;
         }
      }

      // Clamp the position to the ground.
      if (groundClamp)
      {
         if (mViewportList.size())
         {
            Viewport* viewport = mViewportList["Perspective View"];
            if (viewport)
            {
               osg::Vec3 nearPos, farPos;
               nearPos = farPos = snapPos;
               nearPos.z() += 10;
               farPos.z() -= 10000;
               viewport->getPickPosition(nearPos, farPos, snapPos, ignoredDrawables);
            }
         }
      }

      return snapPos;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ViewportManager::SavePresetCamera(int index)
   {
      dtCore::Map::PresetCameraData data;
      data.isValid = true;

      Viewport* viewport = mViewportList["Perspective View"];
      if (viewport)
      {
         StageCamera* cam = viewport->getCamera();
         if (cam)
         {
            data.persPosition = cam->getPosition();
            data.persRotation = cam->getOrientation();
         }
      }
      viewport = mViewportList["Top View (XY)"];
      if (viewport)
      {
         StageCamera* cam = viewport->getCamera();
         if (cam)
         {
            data.topPosition = cam->getPosition();
            data.topZoom     = cam->getZoom();
         }
      }
      viewport = mViewportList["Side View (YZ)"];
      if (viewport)
      {
         StageCamera* cam = viewport->getCamera();
         if (cam)
         {
            data.sidePosition = cam->getPosition();
            data.sideZoom     = cam->getZoom();
         }
      }
      viewport = mViewportList["Front View (XZ)"];
      if (viewport)
      {
         StageCamera* cam = viewport->getCamera();
         if (cam)
         {
            data.frontPosition = cam->getPosition();
            data.frontZoom     = cam->getZoom();
         }
      }

      dtCore::Map* map = EditorData::GetInstance().getCurrentMap();
      if (map)
      {
         map->SetPresetCameraData(index, data);
         EditorData::GetInstance().getCurrentMap()->SetModified(true);
         EditorEvents::GetInstance().emitProjectChanged();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ViewportManager::LoadPresetCamera(int index)
   {
      dtCore::Map* map = EditorData::GetInstance().getCurrentMap();
      if (map)
      {
         dtCore::Map::PresetCameraData data = map->GetPresetCameraData(index);

         if (data.isValid || index == 1)
         {
            EditorViewport* viewport = dynamic_cast<EditorViewport*>(mViewportList["Perspective View"]);
            if (viewport)
            {
               StageCamera* cam = viewport->getCamera();
               if (cam)
               {
                  cam->setPosition(data.persPosition);
                  cam->resetRotation();
                  cam->rotate(data.persRotation);

                  // Make sure we refresh the camera motion model with the updated camera.
                  STAGECameraMotionModel* cameraMotion = viewport->getCameraMotionModel();
                  if (cameraMotion) cameraMotion->SetCamera(cam);
               }
            }
            viewport = dynamic_cast<EditorViewport*>(mViewportList["Top View (XY)"]);
            if (viewport)
            {
               StageCamera* cam = viewport->getCamera();
               if (cam)
               {
                  cam->setPosition(data.topPosition);
                  cam->setZoom(data.topZoom);

                  // Make sure we refresh the camera motion model with the updated camera.
                  STAGECameraMotionModel* cameraMotion = viewport->getCameraMotionModel();
                  if (cameraMotion) cameraMotion->SetCamera(cam);
               }
            }
            viewport = dynamic_cast<EditorViewport*>(mViewportList["Side View (YZ)"]);
            if (viewport)
            {
               StageCamera* cam = viewport->getCamera();
               if (cam)
               {
                  cam->setPosition(data.sidePosition);
                  cam->setZoom(data.sideZoom);

                  // Make sure we refresh the camera motion model with the updated camera.
                  STAGECameraMotionModel* cameraMotion = viewport->getCameraMotionModel();
                  if (cameraMotion) cameraMotion->SetCamera(cam);
               }
            }
            viewport = dynamic_cast<EditorViewport*>(mViewportList["Front View (XZ)"]);
            if (viewport)
            {
               StageCamera* cam = viewport->getCamera();
               if (cam)
               {
                  cam->setPosition(data.frontPosition);
                  cam->setZoom(data.frontZoom);

                  // Make sure we refresh the camera motion model with the updated camera.
                  STAGECameraMotionModel* cameraMotion = viewport->getCameraMotionModel();
                  if (cameraMotion) cameraMotion->SetCamera(cam);
               }
            }

            refreshAllViewports();
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ViewportManager::emitViewportEnabled(Viewport* vp, bool enabled, bool* overrideDefault)
   {
      LOG_INFO("Emitting event - [viewportEnabled]");
      emit viewportEnabled(vp, enabled, overrideDefault);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ViewportManager::emitMousePressEvent(Viewport* vp, QMouseEvent* e)
   {
      LOG_INFO("Emitting event - [mousePressEvent]");
      emit mousePressEvent(vp, e);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ViewportManager::emitMouseReleaseEvent(Viewport* vp, QMouseEvent* e)
   {
      LOG_INFO("Emitting event - [mouseReleaseEvent]");
      emit mouseReleaseEvent(vp, e);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ViewportManager::emitMouseDoubleClickEvent(Viewport* vp, QMouseEvent* e)
   {
      LOG_INFO("Emitting event - [mouseDoubleClickEvent]");
      emit mouseDoubleClickEvent(vp, e);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ViewportManager::emitMouseMoveEvent(Viewport* vp, QMouseEvent* e)
   {
      LOG_INFO("Emitting event - [mouseMoveEvent]");
      emit mouseMoveEvent(vp, e);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ViewportManager::emitWheelEvent(Viewport* vp, QWheelEvent* e)
   {
      LOG_INFO("Emitting event - [wheelEvent]");
      emit wheelEvent(vp, e);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ViewportManager::emitShouldBeginActorMode(Viewport* vp, osg::Vec2 position, bool* overrideDefault, bool* result)
   {
      LOG_INFO("Emitting event - [shouldBeginActorMode]");
      emit shouldBeginActorMode(vp, position, overrideDefault, result);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ViewportManager::emitBeginActorMode(Viewport* vp, QMouseEvent* e, bool* overrideDefault)
   {
      LOG_INFO("Emitting event - [beginActorMode]");
      emit beginActorMode(vp, e, overrideDefault);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ViewportManager::emitEndActorMode(Viewport* vp, QMouseEvent* e, bool* overrideDefault)
   {
      LOG_INFO("Emitting event - [endActorMode]");
      emit endActorMode(vp, e, overrideDefault);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ViewportManager::emitBeginCameraMode(Viewport* vp, QMouseEvent* e, bool* overrideDefault)
   {
      LOG_INFO("Emitting event - [beginCameraMode]");
      emit beginCameraMode(vp, e, overrideDefault);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ViewportManager::emitEndCameraMode(Viewport* vp, QMouseEvent* e, bool* overrideDefault)
   {
      LOG_INFO("Emitting event - [endCameraMode]");
      emit endCameraMode(vp, e, overrideDefault);
   }

   //////////////////////////////////////////////////////////////////////////////////
   //void ViewportManager::emitMoveCamera(Viewport* vp, float dx, float dy, bool* overrideDefault)
   //{
   //   LOG_INFO("Emitting event - [moveCamera]");
   //   emit moveCamera(vp, dx, dy, overrideDefault);
   //}

   ////////////////////////////////////////////////////////////////////////////////
   void ViewportManager::emitSelectActors(Viewport* vp, QMouseEvent* e, bool* overrideDefault)
   {
      LOG_INFO("Emitting event - [selectActors]");
      emit selectActors(vp, e, overrideDefault);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ViewportManager::emitDuplicateActors(Viewport* vp, bool* overrideDefault)
   {
      LOG_INFO("Emitting event - [duplicateActors]");
      emit duplicateActors(vp, overrideDefault);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ViewportManager::emitSetSnapTranslation(float increment)
   {
      mSnapTranslation = increment;
      LOG_INFO("Emitting event - [setSnapTranslation]");
      emit setSnapTranslation(increment);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ViewportManager::emitSetSnapRotation(float increment)
   {
      mSnapRotation = increment;
      LOG_INFO("Emitting event - [setSnapRotation]");
      emit setSnapRotation(increment);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ViewportManager::emitSetSnapScale(float increment)
   {
      mSnapScale = increment;
      LOG_INFO("Emitting event - [setSnapScale]");
      emit setSnapScale(increment);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ViewportManager::emitSetSnapEnabled(bool translation, bool rotation, bool scale)
   {
      mSnapTranslationEnabled = translation;
      mSnapRotationEnabled = rotation;
      mSnapScaleEnabled = scale;
      LOG_INFO("Emitting event - [setSnapEnabled]");
      emit setSnapEnabled(translation, rotation, scale);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ViewportManager::emitModifyPropList(dtCore::PropertyContainer& propertyContainer, std::vector<dtCore::ActorProperty*>& propList)
   {
      LOG_INFO("Emitting event - [modifyPropList]");
      emit modifyPropList(propertyContainer, propList);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportManager::onPostTick()
   {
      std::map<std::string,bool>::iterator itor;
      for (itor = mViewportWantsRefresh.begin(); itor != mViewportWantsRefresh.end(); ++itor)
      {
         if (itor->second)
         {
            const std::string& name = itor->first;
            EnableViewport(mViewportList[name], false);
            itor->second = false;
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportManager::onActorPropertyChanged(dtCore::RefPtr<dtCore::BaseActorObject> proxy,
      dtCore::RefPtr<dtCore::ActorProperty> property)
   {
      if (!proxy.valid()) return;

      unsigned int billBoardIndex, actorIndex;
      const dtCore::BaseActorObject::RenderMode& renderMode = proxy->GetRenderMode();
      dtCore::ActorProxyIcon* billBoard = proxy->GetBillBoardIcon();
      bool wasSelected = mViewportOverlay->isActorSelected(proxy);

      if (renderMode == dtCore::BaseActorObject::RenderMode::DRAW_BILLBOARD_ICON)
      {
         mMasterScene->RemoveChild(proxy->GetDrawable());

         mViewportOverlay->unSelect(proxy->GetDrawable());
         if (billBoard == NULL)
         {
            LOG_ERROR("Proxy [" + proxy->GetName() + "] billboard was NULL.");
         }
         else
         {
            billBoard->LoadImages();
            billBoardIndex = mMasterScene->GetChildIndex(billBoard->GetDrawable());
            if (billBoardIndex == (unsigned)mMasterScene->GetNumberOfAddedDrawable())
            {
               mMasterScene->AddChild(billBoard->GetDrawable());
               if (wasSelected) mViewportOverlay->select(billBoard->GetDrawable());
            }
         }
      }
      else if (renderMode == dtCore::BaseActorObject::RenderMode::DRAW_ACTOR)
      {
         if (billBoard == NULL)
         {
            LOG_ERROR("Proxy [" + proxy->GetName() + "] billboard was NULL.");
         }
         else
         {
            mViewportOverlay->unSelect(billBoard->GetDrawable());
            mMasterScene->RemoveChild(billBoard->GetDrawable());
         }

         actorIndex = mMasterScene->GetChildIndex(proxy->GetDrawable());
         if (actorIndex == (unsigned)mMasterScene->GetNumberOfAddedDrawable())
         {
            mMasterScene->AddChild(proxy->GetDrawable());
            if (wasSelected) mViewportOverlay->select(proxy->GetDrawable());
         }
      }
      else if (renderMode == dtCore::BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON)
      {
         if (billBoard == NULL)
         {
            LOG_ERROR("Proxy [" + proxy->GetName() + "] billboard was NULL.");
         }
         else
         {
            billBoard->LoadImages();
            billBoardIndex = mMasterScene->GetChildIndex(billBoard->GetDrawable());
            if (billBoardIndex == (unsigned)mMasterScene->GetNumberOfAddedDrawable())
            {
               mMasterScene->AddChild(billBoard->GetDrawable());
               if (wasSelected) mViewportOverlay->select(billBoard->GetDrawable());
            }
         }

         actorIndex = mMasterScene->GetChildIndex(proxy->GetDrawable());
         if (actorIndex == (unsigned)mMasterScene->GetNumberOfAddedDrawable())
         {
            mMasterScene->AddChild(proxy->GetDrawable());
            if (wasSelected) mViewportOverlay->select(proxy->GetDrawable());
         }
      }
      else
      {
         // If we got here, then the proxy wishes the system to determine how to display
         // the proxy.
      }

      // only redraw if we're doing a single change.  Otherwise, all events will be
      // redrawn in the endChangeTransaction
      if (!mInChangeTransaction)
      {
         refreshAllViewports();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportManager::onEditorShutDown()
   {
      mMasterScene->RemoveAllDrawables();
   }


   ///////////////////////////////////////////////////////////////////////////////
   void ViewportManager::onActorProxyCreated(
      dtCore::RefPtr<dtCore::BaseActorObject> proxy, bool forceNoAdjustments)
   {
      dtCore::Scene* scene = mMasterScene.get();
      dtCore::ActorProxyIcon* billBoard = NULL;

      const dtCore::BaseActorObject::RenderMode& renderMode = proxy->GetRenderMode();
      if (renderMode == dtCore::BaseActorObject::RenderMode::DRAW_BILLBOARD_ICON)
      {
         billBoard = proxy->GetBillBoardIcon();

         // Load the textures for the billboard and orientation arrow.
         // This is only done here so the files will only be loaded when
         // they are actually going to be rendered. Previously this was
         // done inside ActorProxyIcon's constructor, even if billboards
         // will never be displayed. It was broken out and placed here
         // to fix that. -osb
         billBoard->LoadImages();
         if (billBoard == NULL)
         {
            LOG_ERROR("Proxy [" + proxy->GetName() + "] billboard was NULL.");
         }
         else
         {
            scene->AddChild(billBoard->GetDrawable());
         }
      }
      else if (renderMode == dtCore::BaseActorObject::RenderMode::DRAW_ACTOR)
      {
         scene->AddChild(proxy->GetDrawable());
      }
      else if (renderMode == dtCore::BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON)
      {
         scene->AddChild(proxy->GetDrawable());

         billBoard = proxy->GetBillBoardIcon();

         // Load the textures for the billboard and orientation arrow.
         // This is only done here so the files will only be loaded when
         // they are actually going to be rendered. Previously this was
         // done inside ActorProxyIcon's constructor, even if billboards
         // will never be displayed. It was broken out and placed here
         // to fix that. -osb
         billBoard->LoadImages();
         if (billBoard == NULL)
         {
            LOG_ERROR("Proxy [" + proxy->GetName() + "] billboard was NULL.");
         }
         else
         {
            scene->AddChild(billBoard->GetDrawable());
         }
      }
      else
      {
         // If we got here, then the proxy wishes the system to determine how to display
         // the proxy. Currently, not implemented, defaults to DRAW_ACTOR).
         scene->AddChild(proxy->GetDrawable());
      }

      // update the viewports unless we're getting lots of changes back to back, in which
      // case our super class handles that.
      if (!mInChangeTransaction)
      {
         refreshAllViewports();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportManager::placeProxyInFrontOfCamera(dtCore::BaseActorObject* proxy)
   {
      //dont set the position on the proxy if it isnt placeable
      if(proxy->IsPlaceable())
      {
         // Get the current position and direction the camera is facing.
         osg::Vec3 pos = getWorldViewCamera()->getPosition();
         osg::Vec3 viewDir = getWorldViewCamera()->getViewDir();

         // If the object is a transformable (can have a position in the scene)
         // add it to the scene in front of the camera.
         dtCore::TransformableActorProxy* tProxy =
            dynamic_cast<dtCore::TransformableActorProxy*>(proxy);
         dtCore::ActorProperty* prop = proxy->GetProperty(dtCore::TransformableActorProxy::PROPERTY_TRANSLATION);

         if (tProxy != NULL && prop != NULL)
         {
            const osg::BoundingSphere& bs = tProxy->GetDrawable()->GetOSGNode()->getBound();

            // Position it along the camera's view direction.  The distance from
            // the camera is the object's bounding volume so it appears
            // just in front of the camera.  If the object is very large, it is
            // just created at the origin.
            std::string oldValue = prop->ToString();

            float actorCreationOffset = EditorData::GetInstance().GetActorCreationOffset();
            float offset = (bs.radius() < 1000.0f) ? bs.radius() : 1.0f;
            if (offset <= 0.0f)
            {
               offset = actorCreationOffset;
            }
            tProxy->SetTranslation(pos + (viewDir * offset * 2));

            std::string newValue = prop->ToString();
            EditorEvents::GetInstance().emitActorPropertyAboutToChange(proxy, prop, oldValue, newValue);
            EditorEvents::GetInstance().emitActorPropertyChanged(proxy,prop);
         }

         if (!mInChangeTransaction)
         {
            refreshAllViewports();
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportManager::onBeginChangeTransaction()
   {
      mInChangeTransaction = true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportManager::onEndChangeTransaction()
   {
      mInChangeTransaction = false;
      refreshAllViewports();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportManager::onCurrentMapChanged()
   {
      refreshScene();
      refreshAllViewports();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ViewportManager::setWorldViewCamera(StageCamera* camera)
   {
      mWorldCamera = camera;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ViewportManager::SetApplication(dtABC::Application* app)
   {
      mApplication = app;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtABC::Application* ViewportManager::GetApplication() const
   {
      return mApplication.get();
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool ViewportManager::EnableViewport(Viewport* viewport, bool enable, bool disableAfterRender)
   {
      if (GetApplication() == NULL || !viewport)
      {
         return false;
      }

      // Add it the first time
      if (!GetApplication()->ContainsView(*viewport->GetView()))
      {
         GetApplication()->AddView(*viewport->GetView());

         //if OSG thinks we're done, set it straight.
         if (GetApplication()->GetCompositeViewer()->done())
         {
            GetApplication()->GetCompositeViewer()->setDone(false);
         }

         // Now refresh the camera motion model
         EditorViewport* editorView = dynamic_cast<EditorViewport*>(viewport);
         if (editorView)
         {
            editorView->getCameraMotionModel()->SetCamera(editorView->getCamera());
         }
      }

      //printf ("viewport %s has been enabled %d\n", viewport->getName().c_str(), enable);

      bool result = viewport->getCamera()->getDeltaCamera()->GetEnabled() != enable;
      viewport->getCamera()->getDeltaCamera()->SetEnabled(enable);

      if (result)
         mViewportWantsRefresh[viewport->getName()] = disableAfterRender;
      // a subsequent enable before disableAfterRender is cleared, needs to be cleared or it will cause it to be disabled for no reason.
      else if (enable && !disableAfterRender)
         mViewportWantsRefresh[viewport->getName()] = false;

      dtCore::System& sys = dtCore::System::GetInstance();
      if (enable && !sys.IsRunning())
      {
         sys.Start();
      }
      else if (!enable && sys.IsRunning())
      {
         bool oneEnabled = false;
         std::map<std::string, Viewport*>::iterator itor;
         for (itor = mViewportList.begin(); itor != mViewportList.end(); ++itor)
         {
            Viewport* v = itor->second;
            if (v->getCamera()->getDeltaCamera()->GetEnabled())
            {

            }
         }
         if (!oneEnabled)
         {
            sys.Stop();
         }
      }


      return result;

   }
} // namespace dtEditQt
