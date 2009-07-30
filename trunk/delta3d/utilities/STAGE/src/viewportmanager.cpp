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
#include <prefix/dtstageprefix-src.h>
#include <QtGui/QWidget>
#include <QtOpenGL/QGLWidget>
#include <osg/Texture>
#include <osgDB/DatabasePager>
#include <osgDB/Registry>
#include <dtUtil/log.h>
#include <dtDAL/actorproxyicon.h>
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
      : mShareMasterContext(true)
      , mMasterViewport(NULL)
      , mMasterScene(new dtCore::Scene())
      , mMasterView(new dtCore::View())
   {
      mMasterView->SetScene(mMasterScene.get());
      mViewportOverlay     = new ViewportOverlay();
      mWorldCamera         = new StageCamera();
      mInChangeTransaction = false;
      mStartTick = 0;

      EditorEvents* editorEvents = &EditorEvents::GetInstance();

      connect(editorEvents, SIGNAL(actorProxyCreated(ActorProxyRefPtr, bool)),
              this, SLOT(onActorProxyCreated(ActorProxyRefPtr, bool)));

      connect(editorEvents,
              SIGNAL(actorPropertyChanged(ActorProxyRefPtr,ActorPropertyRefPtr)),
              this,
              SLOT(onActorPropertyChanged(ActorProxyRefPtr,ActorPropertyRefPtr)));

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
            vp = createViewportImpl(name, type, parent, mMasterViewport);
         }
      }
      else
      {
         vp = createViewportImpl(name, type, parent, NULL);
      }

      // Now make sure the viewport we created has a valid OpenGL context.
      if (!vp || !vp->isValid())
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
       ViewportType& type, QWidget* parent, QGLWidget* shareWith)
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
   void ViewportManager::refreshActorSelection(const std::vector< dtCore::RefPtr<dtDAL::ActorProxy> >& actors)
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
         if (itor->second->getAutoInteractionMode())
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
      const std::map<dtCore::UniqueId, dtCore::RefPtr<dtDAL::ActorProxy> >& proxies)
   {
      std::map< dtCore::UniqueId, dtCore::RefPtr<dtDAL::ActorProxy> >::const_iterator itor;

      for (itor = proxies.begin(); itor != proxies.end(); ++itor)
      {
         dtDAL::ActorProxy* proxy = const_cast<dtDAL::ActorProxy*>(itor->second.get());
         const dtDAL::ActorProxy::RenderMode& renderMode = proxy->GetRenderMode();
         dtDAL::ActorProxyIcon* billBoard;

         if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_BILLBOARD_ICON)
         {
            billBoard = proxy->GetBillBoardIcon();
            if (billBoard != NULL)
            {
               mMasterScene->RemoveDrawable(billBoard->GetDrawable());
            }
         }
         else if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_ACTOR)
         {
            mMasterScene->RemoveDrawable(proxy->GetActor());
         }
         else if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON)
         {
            billBoard = proxy->GetBillBoardIcon();
            if (billBoard != NULL)
            {
               mMasterScene->RemoveDrawable(billBoard->GetDrawable());
            }
            mMasterScene->RemoveDrawable(proxy->GetActor());
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportManager::initializeGL()
   {
      static bool hasBeenInitialized = false;
      if (!hasBeenInitialized)
      {
         hasBeenInitialized = true;

         //Get the texture extensions so we can query for some device information.
         osg::Texture::Extensions* ext = osg::Texture::getExtensions(0, true);
         if (ext != NULL)
         {
            mNumTextureUnits = ext->numTextureUnits();
         }
         else
         {
            mNumTextureUnits = 2;
         }
      }
   }


   ///////////////////////////////////////////////////////////////////////////////
   dtCore::DatabasePager* ViewportManager::GetDatabasePager() const
   {
      return mMasterView->GetDatabasePager();
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

   ////////////////////////////////////////////////////////////////////////////////
   void ViewportManager::emitMoveCamera(Viewport* vp, float dx, float dy, bool* overrideDefault)
   {
      LOG_INFO("Emitting event - [moveCamera]");
      emit moveCamera(vp, dx, dy, overrideDefault);
   }

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

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportManager::onActorPropertyChanged(dtCore::RefPtr<dtDAL::ActorProxy> proxy,
      dtCore::RefPtr<dtDAL::ActorProperty> property)
   {
      unsigned int billBoardIndex, actorIndex;
      const dtDAL::ActorProxy::RenderMode& renderMode = proxy->GetRenderMode();
      dtDAL::ActorProxyIcon* billBoard = proxy->GetBillBoardIcon();

      if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_BILLBOARD_ICON)
      {
         mMasterScene->RemoveDrawable(proxy->GetActor());
         mViewportOverlay->unSelect(proxy->GetActor());
         if (billBoard == NULL)
         {
            LOG_ERROR("Proxy [" + proxy->GetName() + "] billboard was NULL.");
         }
         else
         {
            billBoard->LoadImages();
            billBoardIndex = mMasterScene->GetDrawableIndex(billBoard->GetDrawable());
            if (billBoardIndex == (unsigned)mMasterScene->GetNumberOfAddedDrawable())
            {
               mMasterScene->AddDrawable(billBoard->GetDrawable());
               mViewportOverlay->select(billBoard->GetDrawable());
            }
         }
      }
      else if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_ACTOR)
      {
         if (billBoard == NULL)
         {
            LOG_ERROR("Proxy [" + proxy->GetName() + "] billboard was NULL.");
         }
         else
         {
            mViewportOverlay->unSelect(billBoard->GetDrawable());
            mMasterScene->RemoveDrawable(billBoard->GetDrawable());
         }

         actorIndex = mMasterScene->GetDrawableIndex(proxy->GetActor());
         if (actorIndex == (unsigned)mMasterScene->GetNumberOfAddedDrawable())
         {
            mMasterScene->AddDrawable(proxy->GetActor());
            mViewportOverlay->select(proxy->GetActor());
         }
      }
      else if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON)
      {
         if (billBoard == NULL)
         {
            LOG_ERROR("Proxy [" + proxy->GetName() + "] billboard was NULL.");
         }
         else
         {
            billBoard->LoadImages();
            billBoardIndex = mMasterScene->GetDrawableIndex(billBoard->GetDrawable());
            if (billBoardIndex == (unsigned)mMasterScene->GetNumberOfAddedDrawable())
            {
               mMasterScene->AddDrawable(billBoard->GetDrawable());
               mViewportOverlay->select(billBoard->GetDrawable());
            }
         }

         actorIndex = mMasterScene->GetDrawableIndex(proxy->GetActor());
         if (actorIndex == (unsigned)mMasterScene->GetNumberOfAddedDrawable())
         {
            mMasterScene->AddDrawable(proxy->GetActor());
            mViewportOverlay->select(proxy->GetActor());
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
      dtCore::RefPtr<dtDAL::ActorProxy> proxy, bool forceNoAdjustments)
   {
      dtCore::Scene* scene = mMasterScene.get();
      dtDAL::ActorProxyIcon* billBoard = NULL;

      const dtDAL::ActorProxy::RenderMode& renderMode = proxy->GetRenderMode();
      if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_BILLBOARD_ICON)
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
            scene->AddDrawable(billBoard->GetDrawable());
         }
      }
      else if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_ACTOR)
      {
         scene->AddDrawable(proxy->GetActor());
      }
      else if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON)
      {
         scene->AddDrawable(proxy->GetActor());

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
            scene->AddDrawable(billBoard->GetDrawable());
         }
      }
      else
      {
         // If we got here, then the proxy wishes the system to determine how to display
         // the proxy. Currently, not implemented, defaults to DRAW_ACTOR).
         scene->AddDrawable(proxy->GetActor());
      }

      // update the viewports unless we're getting lots of changes back to back, in which
      // case our super class handles that.
      if (!mInChangeTransaction)
      {
         refreshAllViewports();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportManager::placeProxyInFrontOfCamera(dtDAL::ActorProxy* proxy)
   {
      // Get the current position and direction the camera is facing.
      osg::Vec3 pos = getWorldViewCamera()->getPosition();
      osg::Vec3 viewDir = getWorldViewCamera()->getViewDir();

      // If the object is a transformable (can have a position in the scene)
      // add it to the scene in front of the camera.
      dtDAL::TransformableActorProxy* tProxy =
         dynamic_cast<dtDAL::TransformableActorProxy*>(proxy);
      dtDAL::ActorProperty* prop = proxy->GetProperty(dtDAL::TransformableActorProxy::PROPERTY_TRANSLATION);

      if (tProxy != NULL && prop != NULL)
      {
         const osg::BoundingSphere& bs = tProxy->GetActor()->GetOSGNode()->getBound();

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

   //////////////////////////////////////////////////////////////////////////
   bool ViewportManager::IsPagingEnabled() const
   {
      if (mMasterView->GetDatabasePager() != NULL)
      {
         return true;
      }
      else
      {
         return false;
      }
   }

} // namespace dtEditQt
