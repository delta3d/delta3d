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
#include <osg/StateSet>
#include <osg/ClearNode>
#include <osg/AlphaFunc>


#include <dtCore/scene.h>
#include <dtCore/isector.h>
#include <dtCore/transform.h>

#include <dtCore/exceptionenum.h>
#include <dtCore/map.h>
#include <dtCore/transformableactorproxy.h>
#include <dtCore/actorproxyicon.h>

#include <dtActors/volumeeditactor.h>
#include <dtUtil/mathdefines.h>

#include <dtQt/osggraphicswindowqt.h>

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
      : QObject(parent)
      , mCamera(new StageCamera())
      , mInChangeTransaction(false)
      , mWindow(NULL)
      , mName(name)
      , mViewPortType(type)
      , mUseAutoInteractionMode(false)
      , mAutoSceneUpdate(true)
      , mInitialized(false)
      , mEnableKeyBindings(true)
      , mIsMouseTrapped(false)
      , mScene(new dtCore::Scene())
      , mView(new dtCore::View(name))
      , mIsector(new dtCore::Isector())
   {
      mMouseSensitivity = 1.0f;
      mInteractionMode = &InteractionMode::NOTHING;
      mRenderStyle = &RenderStyle::WIREFRAME;

      dtCore::DeltaWin::DeltaWinTraits winTraits;
      winTraits.contextToShare = shareWith;
      winTraits.realizeUponCreate = false;
      mWindow = new dtCore::DeltaWin(winTraits);

      mRootNodeGroup = new osg::Group();

      if (this->GetQGLWidget() != NULL)
      {
         this->GetQGLWidget()->setMouseTracking(true);
      }

      mCamera->getDeltaCamera()->SetWindow(mWindow.get());
      mView->SetCamera(mCamera->getDeltaCamera());
      mView->SetScene(mScene.get());
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
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::setScene(dtCore::Scene* scene)
   {
      //First, remove the old scene, then add the new one.
      if (mView.valid())
      {
         mScene->GetSceneNode()->removeChild(mRootNodeGroup.get());

         mView->SetScene(scene);
         mScene = scene;
         mScene->GetSceneNode()->addChild(mRootNodeGroup.get());
      }
   }


   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::resizeGL(int width, int height)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::paintGL()
   {
      renderFrame();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Viewport::refreshActorSelection(const std::vector< dtCore::RefPtr<dtCore::BaseActorObject> >& actors)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::refresh()
   {
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
      getCamera()->update();

      // Make sure the billboards of any actor are oriented towards the
      // camera in this viewport.
      if (getAutoSceneUpdate())
      {
         updateActorProxyBillboards();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::setRenderStyle(const RenderStyle& style, bool refreshView)
   {
      int i;
      int numTextureUnits = 8;

      mRenderStyle = &style;

      osg::StateAttribute::GLModeValue turnOn  = osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON;
      osg::StateAttribute::GLModeValue turnOff = osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF;

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
            throw dtCore::BaseException("Cannot refresh the viewport. "
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
      dtCore::RefPtr<dtCore::Map> currMap = EditorData::GetInstance().getCurrentMap();
      if (!currMap.valid() || getCamera()== NULL)
      {
         return;
      }

      dtCore::ActorRefPtrVector toSelect;

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
      dtCore::BaseActorObject* newSelection = currMap->GetProxyById(drawable->GetUniqueId());

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
         const std::map< dtCore::UniqueId, dtCore::RefPtr<dtCore::BaseActorObject> >&
            proxyList = currMap->GetAllProxies();
         std::map< dtCore::UniqueId, dtCore::RefPtr<dtCore::BaseActorObject> >::const_iterator proxyItor;

         // Loop through the proxies searching for the one with billboard geometry
         // matching what was selected.
         for (proxyItor = proxyList.begin(); proxyItor != proxyList.end(); ++proxyItor)
         {
            dtCore::BaseActorObject* proxy =const_cast<dtCore::BaseActorObject*>(proxyItor->second.get());

            if (proxy->GetRenderMode() == dtCore::BaseActorObject::RenderMode::DRAW_ACTOR)
            {
               continue;
            }

            const dtCore::ActorProxyIcon* billBoard = proxy->GetBillBoardIcon();
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

         int groupIndex = currMap->FindGroupForActor(*newSelection);

         if (groupIndex > -1)
         {
            int actorCount = currMap->GetGroupActorCount(groupIndex);
            for (int actorIndex = 0; actorIndex < actorCount; actorIndex++)
            {
               dtCore::BaseActorObject* proxy = currMap->GetActorFromGroup(groupIndex, actorIndex);
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
            toSelect.push_back(const_cast<dtCore::BaseActorObject*>(itor->get()));
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
         pos[0] = dtUtil::MapRangeValue(float(pixelPos.x()), 0.0f, float(viewport->width()), -1.0f, 1.0f);
         pos[1] = dtUtil::MapRangeValue(float(pixelPos.y()), 0.0f, float(viewport->height()), 1.0f, -1.0f);
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
         throw dtCore::BaseException(
            "Scene is invalid.  Cannot pick objects from an invalid scene.", __FILE__, __LINE__);
         return false;
      }

      dtCore::RefPtr<dtCore::Map> currMap = EditorData::GetInstance().getCurrentMap();
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
      dtCore::Isector::HitList& hitList = mIsector->GetHitList();
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
   void Viewport::onGotoActor(dtCore::RefPtr<dtCore::BaseActorObject> actor)
   {
      dtCore::Transformable* tx;
      actor->GetDrawable(tx);

      if (tx != NULL && getCamera()!= NULL)
      {
         osg::Vec3 viewDir = getCamera()->getViewDir();
         dtCore::Transform xform;
         tx->GetTransform(xform, dtCore::Transformable::ABS_CS);

         osg::Vec3 translation = xform.GetTranslation();
         const osg::BoundingSphere& bs = tx->GetOSGNode()->getBound();
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
      // SetPos seems to recurse on mac os, so we drop out early if mMouseTrapped is true.
      if (mIsMouseTrapped)
      {
         return;
      }

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

      mOldMouseLocation = QCursor::pos();

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

      dx = (float)(e->globalX() - mOldMouseLocation.x());
      dy = (float)(e->globalY() - mOldMouseLocation.y());

      onMouseMoveEvent(e, dx, dy);

      if ((mIsMouseTrapped) && (this->GetQGLWidget() != NULL))
      {
         mouseMoving = true;
         QCursor::setPos(mOldMouseLocation);
         mouseMoving = false;
      }

      //refresh();
      renderFrame();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::connectInteractionModeSlots()
   {
      // Connect the global actions we want to track.
      EditorEvents&  ge = EditorEvents::GetInstance();

      connect(&ge, SIGNAL(gotoActor(dtCore::ActorPtr)),          this, SLOT(onGotoActor(dtCore::ActorPtr)));
      connect(&ge, SIGNAL(gotoPosition(double, double, double)), this, SLOT(onGotoPosition(double,double,double)));
      connect(&ge, SIGNAL(beginChangeTransaction()),             this, SLOT(onBeginChangeTransaction()));
      connect(&ge, SIGNAL(endChangeTransaction()),               this, SLOT(onEndChangeTransaction()));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::disconnectInteractionModeSlots()
   {
      //Disconnect from all our global actions we were previously tracking.
      EditorEvents& ge = EditorEvents::GetInstance();

      disconnect(&ge, SIGNAL(gotoActor(dtCore::ActorPtr)), this, SLOT(onGotoActor(dtCore::ActorPtr)));
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
         dtCore::BaseActorObject* proxy = const_cast<dtCore::BaseActorObject*>(itor->get());
         dtCore::ActorProperty* prop = proxy->GetProperty(propName);

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
         dtCore::BaseActorObject*    proxy = const_cast<dtCore::BaseActorObject*>(itor->get());
         dtCore::ActorProperty* prop  = proxy->GetProperty(propName);

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
