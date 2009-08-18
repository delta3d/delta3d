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
#include <QtGui/QAction>
#include <QtGui/QMouseEvent>

#include <dtEditQt/mainwindow.h>
#include <dtEditQt/viewport.h>
#include <dtEditQt/viewportoverlay.h>
#include <dtEditQt/viewportmanager.h>
#include <dtEditQt/editoractions.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/editordata.h>

#include <osg/StateSet>
#include <osg/PolygonMode>
#include <osg/PolygonOffset>
#include <osg/CullFace>
#include <osg/Viewport>
#include <osg/FrameStamp>
#include <osg/StateSet>
#include <osg/ClearNode>
#include <osg/AlphaFunc>

#include <osgDB/Registry>

//#include <osgUtil/SceneView>

#include <dtCore/scene.h>
#include <dtCore/databasepager.h>
#include <osgDB/DatabasePager>
#include <dtCore/system.h>
#include <dtCore/isector.h>

#include <dtDAL/exceptionenum.h>
#include <dtDAL/map.h>
#include <dtDAL/transformableactorproxy.h>
#include <dtDAL/actorproxyicon.h>

#include <dtActors/prefabactorproxy.h>
#include <dtActors/volumeeditactor.h>
#include <dtUtil/mathdefines.h>

#include <dtQt/osggraphicswindowqt.h>
//#include <cmath>
//#include <sstream>

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(Viewport::RenderStyle);
   const Viewport::RenderStyle Viewport::RenderStyle::WIREFRAME("WIREFRAME");
   const Viewport::RenderStyle Viewport::RenderStyle::LIT("LIT");
   const Viewport::RenderStyle Viewport::RenderStyle::TEXTURED("TEXTURED");
   const Viewport::RenderStyle Viewport::RenderStyle::LIT_AND_TEXTURED("LIT_AND_TEXTURED");
   ///////////////////////////////////////////////////////////////////////////////

   ///////////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(Viewport::InteractionMode);
   const Viewport::InteractionMode Viewport::InteractionMode::NOTHING("NOTHING");
   const Viewport::InteractionMode Viewport::InteractionMode::CAMERA("CAMERA");
   const Viewport::InteractionMode Viewport::InteractionMode::ACTOR("ACTOR");
   const Viewport::InteractionMode Viewport::InteractionMode::SELECT_ACTOR("SELECT_ACTOR");
   ///////////////////////////////////////////////////////////////////////////////


   ///////////////////////////////////////////////////////////////////////////////
   Viewport::Viewport(ViewportManager::ViewportType& type, const std::string& name, QWidget* parent, osg::GraphicsContext* shareWith)
      //: QWidget(parent)
      : QObject(parent)
      , mInChangeTransaction(false)
      , mName(name)
      , mViewPortType(type)
      , mRedrawContinuously(false)
      , mUseAutoInteractionMode(false)
      , mAutoSceneUpdate(true)
      , mInitialized(false)
      , mEnableKeyBindings(true)
      , mIsector(new dtCore::Isector())
      , mIsMouseTrapped(false)
      , mWindow(NULL)
      , mView(new dtCore::View())
      , mCamera(new StageCamera())
      , mScene(new dtCore::Scene())
      , mIsDirty(false)
   {
      //mFrameStamp = new osg::FrameStamp();
      mMouseSensitivity = 10.0f;
      mInteractionMode = &InteractionMode::NOTHING;
      mRenderStyle = &RenderStyle::WIREFRAME;

      dtCore::DeltaWin::DeltaWinTraits winTraits;
      winTraits.contextToShare = shareWith;
      mWindow = new dtCore::DeltaWin(winTraits);

      mRootNodeGroup = new osg::Group();
      //mSceneView = new osgUtil::SceneView();

      //mSceneView->setDefaults();
      //mSceneView->setFrameStamp(mFrameStamp.get());
      //mSceneView->setSceneData(mRootNodeGroup.get());
      //mScene->GetSceneNode()->addChild(mRootNodeGroup.get());
      setOverlay(ViewportManager::GetInstance().getViewportOverlay());

      if (this->GetQGLWidget() != NULL)
      {
         this->GetQGLWidget()->setMouseTracking(true);
      }

      mCacheMouseLocation = true;

      //connect(&mTimer, SIGNAL(timeout()), this, SLOT(updateGL()));
      //mTimer.setInterval(10);
      //mTimer.setSingleShot(true);

      mCamera->getDeltaCamera()->SetWindow(mWindow.get());
      mView->SetCamera(mCamera->getDeltaCamera());
      mView->SetScene(mScene.get());

      //GetQWidget()->setParent(GetQGLWidget());
      //GetQGLWidget()->setParent(GetQWidget());

      initializeGL();
   }

   ///////////////////////////////////////////////////////////////////////////////
   Viewport::~Viewport()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::initializeGL()
   {
      setupInitialRenderState();
      ViewportManager::GetInstance().initializeGL();
      mInitialized = true;
      if (mRedrawContinuously)
      {
         //mTimer.start();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::setScene(dtCore::Scene* scene)
   {
      //First, remove the old scene, then add the new one.
      if (mView.valid())
      {
         mScene->GetSceneNode()->removeChild(mRootNodeGroup.get());

         //mRootNodeGroup->addChild(scene->GetSceneNode());
         mView->SetScene(scene);
         mScene = scene;
         mScene->GetSceneNode()->addChild(mRootNodeGroup.get());

         //scene->GetSceneNode()->setStateSet(mGlobalStateSet.get());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::setOverlay(ViewportOverlay* overlay)
   {
      //return; //TODO
      ////if (mSceneView.valid())
      //{
      //   //If the new overlay is NULL, clear the current overlay.
      //   if (overlay == NULL && mOverlay.valid())
      //   {
      //      mRootNodeGroup->removeChild(mOverlay->getOverlayGroup());
      //      mOverlay = NULL;
      //      return;
      //   }

      //   //Else update the current overlay in both the scene and in the viewport.
      //   if (mOverlay != NULL)
      //   {
      //      mRootNodeGroup->replaceChild(mOverlay->getOverlayGroup(), overlay->getOverlayGroup());
      //   }
      //   else
      //   {
      //      mRootNodeGroup->addChild(overlay->getOverlayGroup());
      //   }

      //   mOverlay = overlay;
      //}
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::SetRedrawContinuously(bool contRedraw)
   {
      if (mRedrawContinuously == contRedraw)
      {
         return;
      }

      mRedrawContinuously = contRedraw;
      if (mRedrawContinuously)
      {
         //mTimer.start();
      }
      else
      {
         //mTimer.stop();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::resizeGL(int width, int height)
   {
      //mSceneView->setViewport(0, 0, width, height); //TODO
      //getCamera()->setViewport(0, 0, width, height);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::paintGL()
   {
      renderFrame();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Viewport::refreshActorSelection(const std::vector< dtCore::RefPtr<dtDAL::ActorProxy> >& actors)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::refresh()
   {
      mIsDirty = true;

      if (GetQGLWidget() != NULL)
      {
         GetQGLWidget()->updateGL(); 
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::setClearColor(const osg::Vec4& color)
   {
      if (mClearNode.valid())
      {
         mClearNode->setClearColor(color);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::renderFrame()
   {
      mIsDirty = false;
      getCamera()->update();

      // Make sure the billboards of any actor proxies are oriented towards the
      // camera in this viewport.
      if (getAutoSceneUpdate())
      {
         updateActorProxyBillboards();
      }

//      if (ViewportManager::GetInstance().IsPagingEnabled())
//      {
//         const dtCore::DatabasePager* dbp = ViewportManager::GetInstance().GetDatabasePager();
//         if (dbp != NULL)
//         {
//            dbp->SignalBeginFrame(mFrameStamp.get());
//#if OPENSCENEGRAPH_MAJOR_VERSION < 2 || (OPENSCENEGRAPH_MAJOR_VERSION == 2 && OPENSCENEGRAPH_MINOR_VERSION <= 6)
//            dbp->UpdateSceneGraph(mFrameStamp->getReferenceTime());
//#else
//            dbp->UpdateSceneGraph(mFrameStamp.get());
//#endif
//         }
//      }
//
//      mFrameStamp->setReferenceTime(osg::Timer::instance()->delta_s(ViewportManager::GetInstance().GetStartTick(), osg::Timer::instance()->tick()));
//      mFrameStamp->setFrameNumber(mFrameStamp->getFrameNumber()+ 1);
//
//      //mSceneView->update();
//      //mSceneView->cull();
//      //mSceneView->draw();
//
//      if (ViewportManager::GetInstance().IsPagingEnabled())
//      {
//         const dtCore::DatabasePager* dbp = ViewportManager::GetInstance().GetDatabasePager();
//         if (dbp != NULL)
//         {
//            dbp->SignalEndFrame();
//            // This magic number is the default amount of time that dtCore Scene USED to use.
//            //double cleanupTime = 0.0025;
//            //dbp->CompileGLObjects(*mSceneView->getState(), cleanupTime);
//
//            //mSceneView->flushDeletedGLObjects(cleanupTime);
//         }
//      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::setRenderStyle(const RenderStyle& style, bool refreshView)
   {
      int i;
      int numTextureUnits = ViewportManager::GetInstance().getNumTextureUnits();

      mRenderStyle = &style;
      //if (!mSceneView.valid())
      //{
      //   throw dtUtil::Exception(dtDAL::ExceptionEnum::BaseException,"Cannot set render style "
      //         "because the current scene view is invalid.", __FILE__, __LINE__);
      //}

      osg::StateAttribute::GLModeValue turnOn  = osg::StateAttribute::OVERRIDE |osg::StateAttribute::ON;
      osg::StateAttribute::GLModeValue turnOff = osg::StateAttribute::OVERRIDE |osg::StateAttribute::OFF;

      osg::PolygonMode* pm = dynamic_cast<osg::PolygonMode*>(
            mGlobalStateSet->getAttribute(osg::StateAttribute::POLYGONMODE));

      if (*mRenderStyle == RenderStyle::WIREFRAME)
      {
         for (i = 0; i < numTextureUnits; ++i)
         {
            mGlobalStateSet->setTextureMode(i, GL_TEXTURE_2D, turnOff);
         }
         mGlobalStateSet->setMode(GL_LIGHTING, turnOff);
         pm->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
      }
      else if (*mRenderStyle == RenderStyle::TEXTURED)
      {
         for (i = 0; i < numTextureUnits; ++i)
         {
            mGlobalStateSet->removeTextureMode(i, GL_TEXTURE_2D);
         }
         mGlobalStateSet->setMode(GL_LIGHTING, turnOff);
         pm->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL);
      }
      else if (*mRenderStyle == RenderStyle::LIT)
      {
         for (i = 0; i < numTextureUnits; ++i)
         {
            mGlobalStateSet->setTextureMode(i, GL_TEXTURE_2D, turnOff);
         }
         mGlobalStateSet->setMode(GL_LIGHTING, turnOn);
         pm->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL);
      }
      else if (*mRenderStyle == RenderStyle::LIT_AND_TEXTURED)
      {
         for (i = 0; i < numTextureUnits; ++i)
         {
            mGlobalStateSet->removeTextureMode(i, GL_TEXTURE_2D);
         }
         mGlobalStateSet->setMode(GL_LIGHTING, turnOn);
         pm->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL);
      }

      if (refreshView)
      {
         if (!isInitialized())
         {
            throw dtUtil::Exception(dtDAL::ExceptionEnum::BaseException,"Cannot refresh the viewport. "
                  "It has not been initialized.", __FILE__, __LINE__);
         }

         if (GetQGLWidget() != NULL)
         {
            GetQGLWidget()->updateGL();
         }
      }

      emit renderStyleChanged();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::pick(int x, int y)
   {
      dtCore::RefPtr<dtDAL::Map> currMap = EditorData::GetInstance().getCurrentMap();
      if (!currMap.valid() || getCamera()== NULL)
      {
         return;
      }

      std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > toSelect;

      //ensure that the Brush outline is on (if it turns out the brush is selected it
      //will get disabled later)
      //dtEditQt::EditorData::GetInstance().getMainWindow()->GetVolumeEditActor()->EnableOutline(true);

      dtCore::DeltaDrawable* drawable = getPickDrawable(x, y);
      if (!drawable)
      {
         EditorEvents::GetInstance().emitActorsSelected(toSelect);
         return;
      }

      ViewportOverlay* overlay = ViewportManager::GetInstance().getViewportOverlay();
      ViewportOverlay::ActorProxyList selection = overlay->getCurrentActorSelection();

      // First see if the selected drawable is an actor.
      dtDAL::ActorProxy* newSelection = currMap->GetProxyById(drawable->GetUniqueId());

      // if its not an actor that is directly part of the map then it may be the main VolumeEditActor
      if (newSelection == NULL)
      {
         dtActors::VolumeEditActor* volEditActTest = dynamic_cast<dtActors::VolumeEditActor*>(drawable);

         if (volEditActTest)
         {
            newSelection = EditorData::GetInstance().getMainWindow()->GetVolumeEditActorProxy();
         }
      }

      // If it's not an actor that is directly part of the map, then it may be a child of one.
      if (newSelection == NULL)
      {
         dtCore::DeltaDrawable* parent = drawable->GetParent();
         while(parent && !newSelection)
         {
            newSelection = currMap->GetProxyById(parent->GetUniqueId());

            parent = parent->GetParent();
         }
      }

      // If its not an actor then it may be a billboard placeholder for an actor.
      if (newSelection == NULL)
      {
         const std::map< dtCore::UniqueId, dtCore::RefPtr<dtDAL::ActorProxy> >&
            proxyList = currMap->GetAllProxies();
         std::map< dtCore::UniqueId, dtCore::RefPtr<dtDAL::ActorProxy> >::const_iterator proxyItor;

         // Loop through the proxies searching for the one with billboard geometry
         // matching what was selected.
         for (proxyItor = proxyList.begin(); proxyItor != proxyList.end(); ++proxyItor)
         {
            dtDAL::ActorProxy* proxy =const_cast<dtDAL::ActorProxy*>(proxyItor->second.get());

            if (proxy->GetRenderMode() == dtDAL::ActorProxy::RenderMode::DRAW_ACTOR)
            {
               continue;
            }

            const dtDAL::ActorProxyIcon* billBoard = proxy->GetBillBoardIcon();
            if (billBoard && billBoard->OwnsDrawable(drawable))
            {
               newSelection = proxy;
               break;
            }
         }
      }

      if (newSelection)
      {
         // Determine if this new selection is part of a group.
         toSelect.push_back(newSelection);

         int groupIndex = currMap->FindGroupForActor(newSelection);

         if (groupIndex > -1)
         {
            int actorCount = currMap->GetGroupActorCount(groupIndex);
            for (int actorIndex = 0; actorIndex < actorCount; actorIndex++)
            {
               dtDAL::ActorProxy* proxy = currMap->GetActorFromGroup(groupIndex, actorIndex);
               if (proxy != newSelection)
               {
                  toSelect.push_back(proxy);
               }
            }
         }
      }

      // Inform the world what objects were selected and refresh all the viewports
      // affected by the change.  If we are in multi-selection mode (i.e. the control
      // key is pressed) add the current selection to the newly selected proxy.
      if (overlay->getMultiSelectMode())
      {
         bool unSelecting = false;
         for (int index = 0; index < (int)toSelect.size(); index++)
         {
            for (int selIndex = 0; selIndex < (int)selection.size(); selIndex++)
            {
               if (toSelect[index] == selection[selIndex])
               {
                  selection.erase(selection.begin() + selIndex);

                  // If we are clicking on the current primary selection, then un-select it instead.
                  if (selIndex == 0 && index == 0)
                  {
                     unSelecting = true;
                  }
                  break;
               }
            }
         }

         // Clearing the toSelect list will un-select the currently selected actor as well as all the actors within its group.
         if (unSelecting)
         {
            toSelect.clear();
         }

         ViewportOverlay::ActorProxyList::iterator itor = selection.begin();
         for (itor = selection.begin(); itor != selection.end(); ++itor)
         {
            toSelect.push_back(const_cast<dtDAL::ActorProxy*>(itor->get()));
         }
      }
      // If we are not multi-selecting, then selecting an actor that is already selected
      // will un-select.
      else
      {
         for (int index = 0; index < (int)toSelect.size(); index++)
         {
            for (int selIndex = 0; selIndex < (int)selection.size(); selIndex++)
            {
               if (toSelect[index] == selection[selIndex])
               {
                  toSelect.clear();
                  break;
               }
            }
         }
      }

      EditorEvents::GetInstance().emitActorsSelected(toSelect);
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec2 Viewport::convertMousePosition(QPoint pixelPos)
   {
      osg::Vec2 pos;
      pos.x() = pixelPos.x();
      pos.y() = 0.0;
      osg::Viewport* viewport = mCamera->getDeltaCamera()->GetOSGCamera()->getViewport();
      if (viewport)
      {
         pos[0] = dtUtil::MapRangeValue(float(pixelPos.x()), 0.f, float(viewport->width()), -1.f, 1.f);
         pos[1] = dtUtil::MapRangeValue(float(pixelPos.y()), 0.f, float(viewport->height()), 1.f, -1.f);
      }
      return pos;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Viewport::GetMouseLine(osg::Vec2 mousePos, osg::Vec3& start, osg::Vec3& end)
   {
      StageCamera* cam = getCamera();
      if (!cam)
      {
         return;
      }

      dtCore::Transform transform;
      cam->getDeltaCamera()->GetTransform(transform);

      const osg::Camera* camera = cam->getDeltaCamera()->GetOSGCamera();

      osg::Vec3 dir;

      double left, right, bottom, top, zNear, zFar;
      if (camera->getProjectionMatrixAsOrtho(left, right, bottom, top, zNear, zFar))
      {
         osg::Vec3 rightAxis, upAxis, forwardAxis;
         transform.GetOrientation(rightAxis, upAxis, forwardAxis);

         osg::Matrix matrix = camera->getProjectionMatrix();
         double xDif = right - left;
         double yDif = top - bottom;

         osg::Vec3 center = transform.GetTranslation();
         center += rightAxis * ((xDif * 0.5f) * mousePos.x());
         center += upAxis * ((yDif * 0.5f) * mousePos.y());

         dir = forwardAxis * (zFar * 1.5f);
         start = center - dir;
         end = center + dir;
      }
      else
      {
         osg::Matrix projMatrix = camera->getProjectionMatrix();
         osg::Matrix viewMatrix = camera->getViewMatrix();
         osg::Matrix matrix = viewMatrix * projMatrix;

         const osg::Matrix inverse = osg::Matrix::inverse(matrix);

         start = transform.GetTranslation();

         dir = (osg::Vec3(mousePos.x(), mousePos.y(), 0.0f) * inverse) - start;
         dir.normalize();
         end = start + (dir * 15000);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Viewport::calculatePickISector(int x, int y)
   {
      if (!mScene.valid())
      {
         throw dtUtil::Exception(dtDAL::ExceptionEnum::BaseException,
            "Scene is invalid.  Cannot pick objects from an invalid scene.", __FILE__, __LINE__);
         return false;
      }

      dtCore::RefPtr<dtDAL::Map> currMap = EditorData::GetInstance().getCurrentMap();
      if (!currMap.valid() || getCamera()== NULL)
      {
         return false;
      }

      // Before we do any intersection tests, make sure the billboards are updated
      // to reflect their orientation in this viewport.
      getCamera()->update();
      if (getAutoSceneUpdate())
      {
         updateActorProxyBillboards();
      }

      osg::Vec3 nearPoint, farPoint;
      osg::Vec2 mousePos = convertMousePosition(QPoint(x, y));
      GetMouseLine(mousePos, nearPoint, farPoint);

      return calculatePickISector(nearPoint, farPoint);
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Viewport::calculatePickISector(osg::Vec3 nearPoint, osg::Vec3 farPoint)
   {
      mIsector->Reset();
      mIsector->SetScene(getScene());
      mIsector->SetStartPosition(nearPoint);
      mIsector->SetDirection(farPoint-nearPoint);

      // If we found no intersections no need to continue so emit an empty selection
      // and return.
      if (!mIsector->Update())
      {
         return false;
      }
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Viewport::getPickPosition(int x, int y, osg::Vec3& position, std::vector<dtCore::DeltaDrawable*> ignoredDrawables)
   {
      if (!calculatePickISector(x, y))
      {
         return false;
      }

      return getPickPosition(position, ignoredDrawables);
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Viewport::getPickPosition(osg::Vec3 nearPoint, osg::Vec3 farPoint, osg::Vec3& position, std::vector<dtCore::DeltaDrawable*> ignoredDrawables)
   {
      if (!calculatePickISector(nearPoint, farPoint))
      {
         return false;
      }

      return getPickPosition(position, ignoredDrawables);
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Viewport::getPickPosition(osg::Vec3& position, std::vector<dtCore::DeltaDrawable*> ignoredDrawables)
   {
      osgUtil::IntersectVisitor::HitList& hitList = mIsector->GetHitList();
      for (int index = 0; index < (int)hitList.size(); index++)
      {
         osg::NodePath &nodePath = hitList[index].getNodePath();
         dtCore::DeltaDrawable* drawable = mIsector->MapNodePathToDrawable(nodePath);
         dtCore::DeltaDrawable* lastDrawable = drawable;

         // Make sure the drawable and none of its parents are the ignored drawable.
         if (drawable)
         {
            bool isIgnored = false;
            while (drawable)
            {
               for (int ignoreIndex = 0; ignoreIndex < (int)ignoredDrawables.size(); ignoreIndex++)
               {
                  if (drawable == ignoredDrawables[ignoreIndex])
                  {
                     isIgnored = true;
                     break;
                  }
               }

               if (isIgnored)
               {
                  break;
               }

               lastDrawable = drawable;
               drawable = drawable->GetParent();
            }

            if (!isIgnored)
            {
               position = hitList[index].getWorldIntersectPoint();

               // Tell the manager the last pick position.
               ViewportManager::GetInstance().setLastDrawable(lastDrawable);
               ViewportManager::GetInstance().setLastPickPosition(position);
               return true;
            }
         }
      }

      ViewportManager::GetInstance().setLastDrawable(NULL);
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::DeltaDrawable* Viewport::getPickDrawable(int x, int y)
   {
      if (!calculatePickISector(x, y))
      {
         return NULL;
      }

      osg::Vec3 position;
      getPickPosition(position);
      dtCore::DeltaDrawable* drawable = getPickDrawable();

      // Tell the manager the last drawable picked.
      ViewportManager::GetInstance().setLastDrawable(drawable);
      ViewportManager::GetInstance().setLastPickPosition(position);

      return drawable;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::DeltaDrawable* Viewport::getPickDrawable()
   {
      if (mIsector->GetClosestDeltaDrawable() == NULL)
      {
         LOG_ERROR("Intersection query reported an intersection but returned an "
            "invalid DeltaDrawable.");
         return NULL;
      }

      return mIsector->GetClosestDeltaDrawable();
   }


   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::onGotoActor(dtCore::RefPtr<dtDAL::ActorProxy> proxy)
   {
      dtDAL::TransformableActorProxy* tProxy = dynamic_cast<dtDAL::TransformableActorProxy*>(proxy.get());

      if (tProxy != NULL && getCamera()!= NULL)
      {
         osg::Vec3 viewDir = getCamera()->getViewDir();

         osg::Vec3 translation = tProxy->GetTranslation();
         const osg::BoundingSphere& bs = tProxy->GetActor()->GetOSGNode()->getBound();
         float actorCreationOffset = EditorData::GetInstance().GetActorCreationOffset();
         float offset = (bs.radius() < 1000.0f) ? bs.radius() : 1.0f;
         if (offset <= 0.0f)
         {
            offset = actorCreationOffset;
         }

         getCamera()->setPosition(translation);
         if (mViewPortType == ViewportManager::ViewportType::PERSPECTIVE)
         {
            getCamera()->move(viewDir*-offset*1.5f);
         }

         refresh();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void Viewport::onGotoPosition(double x, double y, double z)
   {
      StageCamera* cam = getCamera();
      if (cam != NULL)
      {
         cam->setPosition(osg::Vec3(x, y, z));
      }

      refresh(); // manually redraw the viewport to show new position
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::trapMouseCursor()
   {
      if (this->GetQGLWidget() == NULL)
      {
         return;
      }

      // Get the current cursor so we can restore it later.
      if (this->GetQGLWidget()->cursor().shape()!= Qt::BlankCursor)
      {
         mOldMouseCursor = this->GetQGLWidget()->cursor();
      }
      this->GetQGLWidget()->setCursor(QCursor(Qt::BlankCursor));

      // Cache the old mouse location so the cursor doesn't appear to jump when
      // the camera mode operation is complete.
      if (mCacheMouseLocation)
      {
         mOldMouseLocation = QCursor::pos();
         mCacheMouseLocation = false;
      }

      // I disabled this because the mouse move event does this whenever the mouse moves.
      // Commenting this out helps mouse movement work better in Mac OS X.

      // Put the mouse cursor in the center of the viewport.
      QPoint center((this->GetQGLWidget()->x()+this->GetQGLWidget()->width())/2, (this->GetQGLWidget()->y()+this->GetQGLWidget()->height())/2);
      mLastMouseUpdateLocation = center;
      QCursor::setPos(this->GetQGLWidget()->mapToGlobal(center));
      mIsMouseTrapped = true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::releaseMouseCursor(const QPoint& mousePosition)
   {
      mIsMouseTrapped = false;

      if (this->GetQGLWidget() != NULL)
      {
         this->GetQGLWidget()->setCursor(mOldMouseCursor);
      }

      if (mousePosition.x() != -1 && mousePosition.y() != -1)
      {
         QCursor::setPos(mousePosition);
      }
      else
      {
         QCursor::setPos(mOldMouseLocation);
      }

      mCacheMouseLocation = true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::mouseMoveEvent(QMouseEvent* e)
   {
      static bool mouseMoving = false;
      // Moving the mouse back to the center makes the movement recurse
      // so this is a flag to prevent the recursion

      if (mouseMoving)
      {
         return;
      }

      float dx, dy;

      dx = (float)(e->pos().x() - mLastMouseUpdateLocation.x());
      dy = (float)(e->pos().y() - mLastMouseUpdateLocation.y());

      onMouseMoveEvent(e, dx, dy);

      QPoint mousePos = e->pos();

      if ((mIsMouseTrapped) && (this->GetQGLWidget() != NULL))
      {
         QPoint center((this->GetQGLWidget()->x()+this->GetQGLWidget()->width())/2, (this->GetQGLWidget()->y()+this->GetQGLWidget()->height())/2);

         float dxCenter = dtUtil::Abs(float(e->pos().x() - center.x()));
         float dyCenter = dtUtil::Abs(float(e->pos().y() - center.y()));

         if (dxCenter > (this->GetQGLWidget()->width()/2) || dyCenter > (this->GetQGLWidget()->height()/2))
         {
            // Moving the mouse back to the center makes the movement recurse
            // so this is a flag to prevent the recursion
            mouseMoving = true;
            QCursor::setPos(this->GetQGLWidget()->mapToGlobal(center));
            mousePos = center;
            mouseMoving = false;
         }
      }

      mLastMouseUpdateLocation = mousePos;

      refresh();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::focusInEvent(QFocusEvent* event)
   {
      //TODO
      //QGLWidget::focusInEvent(event); 
      //mTimer.setInterval(10);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::focusOutEvent(QFocusEvent* event)
   {
      //TODO
      //QGLWidget::focusOutEvent(event);
      // One half of a second.
      //mTimer.setInterval(500);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::connectInteractionModeSlots()
   {
      // Connect the global actions we want to track.
      EditorEvents&  ge = EditorEvents::GetInstance();

      connect(&ge, SIGNAL(gotoActor(ActorProxyRefPtr)),          this, SLOT(onGotoActor(ActorProxyRefPtr)));
      connect(&ge, SIGNAL(gotoPosition(double, double, double)), this, SLOT(onGotoPosition(double,double,double)));
      connect(&ge, SIGNAL(beginChangeTransaction()),             this, SLOT(onBeginChangeTransaction()));
      connect(&ge, SIGNAL(endChangeTransaction()),               this, SLOT(onEndChangeTransaction()));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::disconnectInteractionModeSlots()
   {
      //Disconnect from all our global actions we were previously tracking.
      EditorEvents& ge = EditorEvents::GetInstance();

      disconnect(&ge, SIGNAL(gotoActor(ActorProxyRefPtr)), this, SLOT(onGotoActor(ActorProxyRefPtr)));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::saveSelectedActorOrigValues(const std::string& propName)
   {
      ViewportOverlay::ActorProxyList& selection = ViewportManager::GetInstance().getViewportOverlay()->getCurrentActorSelection();
      ViewportOverlay::ActorProxyList::iterator itor;

      //Clear the old list first.
      mSelectedActorOrigValues.erase(propName);
      std::vector<std::string> savedValues;

      for (itor = selection.begin(); itor != selection.end(); ++itor)
      {
         dtDAL::ActorProxy* proxy = const_cast<dtDAL::ActorProxy*>(itor->get());
         dtDAL::ActorProperty* prop = proxy->GetProperty(propName);

         if (prop != NULL)
         {
            std::string oldValue = prop->ToString();
            savedValues.push_back(oldValue);
         }
      }

      mSelectedActorOrigValues[propName] = savedValues;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::updateActorSelectionProperty(const std::string& propName)
   {
      ViewportOverlay::ActorProxyList& selection =ViewportManager::GetInstance().getViewportOverlay()->getCurrentActorSelection();
      ViewportOverlay::ActorProxyList::iterator itor;
      std::map< std::string, std::vector<std::string> >::iterator
            saveEntry = mSelectedActorOrigValues.find(propName);
      int oldValueIndex = 0;

      // Make sure we actually saved values for this property.
      if (saveEntry == mSelectedActorOrigValues.end())
      {
         return;
      }

      for (itor = selection.begin(); itor != selection.end(); ++itor)
      {
         dtDAL::ActorProxy*    proxy = const_cast<dtDAL::ActorProxy*>(itor->get());
         dtDAL::ActorProperty* prop  = proxy->GetProperty(propName);

         if (prop != NULL)
         {
            // emit the old value before the change so undo/redo can recover.
            std::string oldValue = saveEntry->second[oldValueIndex];
            std::string newValue = prop->ToString();
            EditorEvents::GetInstance().emitActorPropertyAboutToChange(proxy, prop, oldValue, newValue);
            ++oldValueIndex;

            EditorEvents::GetInstance().emitActorPropertyChanged(proxy, prop);
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::updateActorProxyBillboards()
   {
      dtDAL::Map* currentMap = EditorData::GetInstance().getCurrentMap();
      std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > proxies;
      std::vector<dtCore::RefPtr<dtDAL::ActorProxy> >::iterator itor;

      if (currentMap == NULL || getCamera() == NULL)
      {
         return;
      }

      currentMap->GetAllProxies(proxies);
      for (itor = proxies.begin(); itor != proxies.end(); ++itor)
      {
         dtDAL::ActorProxy* proxy = itor->get();
         const dtDAL::ActorProxy::RenderMode& renderMode = proxy->GetRenderMode();

         if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON ||
            renderMode == dtDAL::ActorProxy::RenderMode::DRAW_BILLBOARD_ICON)
         {
            dtDAL::ActorProxyIcon* billBoard = proxy->GetBillBoardIcon();
            if (billBoard != NULL)
            {
               billBoard->SetRotation(osg::Matrix::rotate(getCamera()->getOrientation()));
            }
         }
         else if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_AUTO)
         {
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::setAutoInteractionMode(bool on)
   {
      if (on)
      {
         if (mUseAutoInteractionMode)
         {
            return; // Already on, so do nothing.
         }
         else
         {
            mUseAutoInteractionMode = true;
            connectInteractionModeSlots();
         }
      }
      else
      {
         if (!mUseAutoInteractionMode)
         {
            return; // Already off, so do nothing.
         }
         else
         {
            mUseAutoInteractionMode = false;
            disconnectInteractionModeSlots();
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::setupInitialRenderState()
   {
      // Some actors in the scene may have a clear node that disables screen clears
      // each frame. (The skybox for example).  Therefore, we add this node to the
      // scene to ensure a clear happens if needed.
      mClearNode = new osg::ClearNode;
      mClearNode->setRequiresClear(true);
      mClearNode->setClearColor(osg::Vec4(0.2f, 0.2f, 0.4f, 1.0f));
      //osg::Group* group = getSceneView()->getSceneData()->asGroup();
      osg::Group* group = mRootNodeGroup.get();
      if (group != NULL)
      {
         group->addChild(mClearNode.get());
      }

      // Sets up the global state set used to render the viewport's contents.
      // This also sets up some default modes which are shared amoung
      // all viewports.
      mGlobalStateSet = new osg::StateSet();

      osg::AlphaFunc* alphafunc = new osg::AlphaFunc;
      alphafunc->setFunction(osg::AlphaFunc::GREATER, 0.0f);
      osg::PolygonMode* pm = new osg::PolygonMode();
      pm->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);

      osg::CullFace* cf = new osg::CullFace();
      cf->setMode(osg::CullFace::BACK);

      mGlobalStateSet->setGlobalDefaults();
      mGlobalStateSet->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
      mGlobalStateSet->setAttributeAndModes(alphafunc, osg::StateAttribute::ON);
      mGlobalStateSet->setAttributeAndModes(pm, osg::StateAttribute::ON);
      mGlobalStateSet->setAttributeAndModes(cf, osg::StateAttribute::ON);

      //mSceneView->setGlobalStateSet(mGlobalStateSet.get());
      //mRootNodeGroup->setStateSet(mGlobalStateSet.get()); //TODO
      mCamera->getDeltaCamera()->GetOSGCamera()->setStateSet(mGlobalStateSet.get());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::onBeginChangeTransaction()
   {
      mInChangeTransaction = true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::onEndChangeTransaction()
   {
      mInChangeTransaction = false;
//      ViewportManager::GetInstance().refreshAllViewports();
   }

   ////////////////////////////////////////////////////////////////////////////////
   QGLWidget* Viewport::GetQGLWidget()
   {
      dtQt::OSGGraphicsWindowQt* osgGraphWindow = dynamic_cast<dtQt::OSGGraphicsWindowQt*>(mWindow->GetOsgViewerGraphicsWindow());
      if (osgGraphWindow)
      {
         return osgGraphWindow->GetQGLWidget();
      }
      else
      {
         return NULL;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   QWidget* Viewport::GetQWidget()
   {
      //return this;
      return GetQGLWidget();
   }

   ////////////////////////////////////////////////////////////////////////////////
   //dtCore::View* Viewport::GetView()
   //{
   //   return mView.get();
   //}

   ////////////////////////////////////////////////////////////////////////////////
   void Viewport::setCamera(StageCamera* cam)
   {
      mCamera = cam;
      mCamera->getDeltaCamera()->SetWindow(mWindow.get());
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::DeltaWin* Viewport::GetWindow()
   {
      return mWindow.get();
   }
} // namespace dtEditQt
