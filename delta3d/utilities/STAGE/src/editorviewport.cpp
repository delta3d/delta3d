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
 * Jeffrey P Houde
 */

#include <prefix/dtstageprefix-src.h>
#include <QtGui/QMouseEvent>
#include <QtGui/QAction>
#include <dtEditQt/perspectiveviewport.h>
#include <dtEditQt/viewportoverlay.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editoractions.h>
#include <dtDAL/transformableactorproxy.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtCore/isector.h>
#include <dtUtil/exception.h>
#include <dtDAL/exceptionenum.h>


namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   EditorViewport::EditorViewport(ViewportManager::ViewportType& type,
      const std::string& name, QWidget* parent,
      QGLWidget* shareWith)
      : Viewport(type, name, parent, shareWith)
      , mObjectMotionModel(NULL)
      , mSkipUpdateForCam(false)
   {
      mObjectMotionModel = new STAGEObjectMotionModel(ViewportManager::GetInstance().getMasterView());
      mObjectMotionModel->SetEnabled(false);
      mObjectMotionModel->ClearTargets();
      mObjectMotionModel->SetGetMouseLineFunc(dtDAL::MakeFunctor(*this, &EditorViewport::GetMouseLine));
      mObjectMotionModel->SetObjectToScreenFunc(dtDAL::MakeFunctorRet(*this, &EditorViewport::GetObjectScreenCoordinates));
      mObjectMotionModel->SetScale(1.5f);
   }

   /////////////////////////////////////////////////////////////////////////////
   void EditorViewport::moveCamera(float dx, float dy)
   {
      // This should be overloaded for proper camera control.
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorViewport::setScene(dtCore::Scene* scene)
   {
      Viewport::setScene(scene);

      if (mObjectMotionModel.valid())
      {
         osg::Group* node = getSceneView()->getSceneData()->asGroup();
         if (node)
         {
            mObjectMotionModel->SetSceneNode(node);
         }

         mObjectMotionModel->SetCamera(mCamera->getDeltaCamera());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorViewport::refreshActorSelection(const std::vector< dtCore::RefPtr<dtDAL::ActorProxy> >& actors)
   {
      Viewport::refreshActorSelection(actors);

      if (actors.size() > 0)
      {
         mObjectMotionModel->ClearTargets();

         // Now iterate through the additional targets.
         bool canScale = false;
         for (int actorIndex = 0; actorIndex < (int)actors.size(); actorIndex++)
         {
            dtDAL::TransformableActorProxy* targetProxy =
               dynamic_cast<dtDAL::TransformableActorProxy*>(actors[actorIndex].get());

            if (targetProxy)
            {
               mObjectMotionModel->AddTarget(targetProxy);

               // Once we determine that a target can scale, we don't need to
               // test the others.
               if (!canScale)
               {
                  // Determine if this target can be scaled.
                  dtDAL::ActorProperty* prop = targetProxy->GetProperty("Scale");
                  dtDAL::Vec3ActorProperty* scaleProp = dynamic_cast<dtDAL::Vec3ActorProperty*>(prop);

                  if (scaleProp)
                  {
                     canScale = true;
                  }
               }
            }
         }

         mObjectMotionModel->SetScaleEnabled(canScale);
         mObjectMotionModel->SetEnabled(true);
      }
      else
      {
         mObjectMotionModel->SetEnabled(false);
         mObjectMotionModel->ClearTargets();
         mObjectMotionModel->SetScaleEnabled(false);
      }

      mObjectMotionModel->UpdateWidgets();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorViewport::refresh()
   {
      mObjectMotionModel->UpdateWidgets();

      Viewport::refresh();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorViewport::resizeGL(int width, int height)
   {
      Viewport::resizeGL(width, height);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorViewport::initializeGL()
   {
      Viewport::initializeGL();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorViewport::keyPressEvent(QKeyEvent* e)
   {
      Viewport::keyPressEvent(e);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorViewport::keyReleaseEvent(QKeyEvent* e)
   {
      Viewport::keyReleaseEvent(e);
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec2 EditorViewport::convertMousePosition(QPoint pixelPos)
   {
      osg::Vec2 pos;
      pos.x() = pixelPos.x();
      pos.y() = getSceneView()->getViewport()->height() - pixelPos.y();
      return pos;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorViewport::GetMouseLine(osg::Vec2 mousePos, osg::Vec3& start, osg::Vec3& end)
   {
      int xLoc = mousePos.x();
      int yLoc = mousePos.y();

      if (getSceneView()->getViewport()->height() > 0 ||
          getSceneView()->getViewport()->width()  > 0)
      {
         getSceneView()->projectWindowXYIntoObject(xLoc, yLoc, start, end);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec2 EditorViewport::GetObjectScreenCoordinates(osg::Vec3 objectPos)
   {
      osg::Vec3 screenPos;
      getSceneView()->projectObjectIntoWindow(objectPos, screenPos);

      return osg::Vec2(screenPos.x(), screenPos.y());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorViewport::mousePressEvent(QMouseEvent* e)
   {
      mMouseButton = e->button();
      mMouseButtons = e->buttons();
      mKeyMods = e->modifiers();

      setFocus();

      if (mMouseButton == Qt::LeftButton)
      {
         mObjectMotionModel->OnLeftMousePressed();
      }
      else if (mMouseButton == Qt::RightButton)
      {
         mObjectMotionModel->OnRightMousePressed();
      }

      osg::Vec2 pos = convertMousePosition(e->pos());

      // If this is our first click.
      if (getInteractionMode() == Viewport::InteractionMode::NOTHING)
      {
         // If this returns true, it means we are hovering our mouse over a
         // valid motion model widget so we should go into actor mode.
         mMotionType = mObjectMotionModel->Update(pos);
         if (mMotionType != STAGEObjectMotionModel::MOTION_TYPE_MAX)
         {
            beginActorMode(e);
         }
         // If we are not hovering over the motion models widget,
         // Then we can select new actors.  If the mouse is moved
         // before it has been released, we will jump into camera
         // mode instead.
         else
         {
            setInteractionMode(Viewport::InteractionMode::SELECT_ACTOR);
         }
      }
      // If the camera is already active but we are changing buttons, reactivate the camera mode.
      else if (getInteractionMode() == Viewport::InteractionMode::CAMERA)
      {
         beginCameraMode(e);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorViewport::mouseReleaseEvent(QMouseEvent* e)
   {
      mMouseButton = e->button();
      mMouseButtons = e->buttons();
      mKeyMods = e->modifiers();

      if (mMouseButton == Qt::LeftButton)
      {
         mObjectMotionModel->OnLeftMouseReleased();
      }
      else if (mMouseButton == Qt::RightButton)
      {
         mObjectMotionModel->OnRightMouseReleased();
      }

      // End camera mode.
      if (getInteractionMode() == Viewport::InteractionMode::CAMERA)
      {
         endCameraMode(e);
      }
      // End actor mode.
      else if (getInteractionMode() == Viewport::InteractionMode::ACTOR)
      {
         endActorMode(e);
      }
      // If we are still in select actor mode when the mouse button is
      // released, it means we want to select actors.
      else if (getInteractionMode() == Viewport::InteractionMode::SELECT_ACTOR)
      {
         setInteractionMode(Viewport::InteractionMode::NOTHING);

         if (mMouseButton == Qt::LeftButton)
         {
            if (mKeyMods == Qt::ControlModifier)
            {
               ViewportManager::GetInstance().getViewportOverlay()->setMultiSelectMode(true);
            }
            else
            {
               ViewportManager::GetInstance().getViewportOverlay()->setMultiSelectMode(false);
            }

            pick(e->pos().x(), e->pos().y());
            ViewportManager::GetInstance().refreshAllViewports();
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorViewport::onMouseMoveEvent(QMouseEvent* e, float dx, float dy)
   {
      // Get the position of the mouse.
      osg::Vec2 pos = convertMousePosition(e->pos());

      // Update the object motion model mouse position.
      mObjectMotionModel->Update(pos);

      // If we move the mouse while in select actor mode,
      // immediately jump to camera motion mode.
      if (getInteractionMode() == Viewport::InteractionMode::SELECT_ACTOR)
      {
         beginCameraMode(e);
         mSkipUpdateForCam = true;
      }
      else if (mSkipUpdateForCam)
      {
         mSkipUpdateForCam = false;
      }
      // Update the camera.
      else if (getInteractionMode() == Viewport::InteractionMode::CAMERA)
      {
         moveCamera(dx,dy);
      }
      else if (getInteractionMode() == Viewport::InteractionMode::ACTOR)
      {
         ViewportManager::GetInstance().refreshAllViewports();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorViewport::beginCameraMode(QMouseEvent* e)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorViewport::endCameraMode(QMouseEvent* e)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorViewport::beginActorMode(QMouseEvent* e)
   {
      setInteractionMode(Viewport::InteractionMode::ACTOR);

      switch (mMotionType)
      {
      case STAGEObjectMotionModel::MOTION_TYPE_TRANSLATION:
         saveSelectedActorOrigValues(dtDAL::TransformableActorProxy::PROPERTY_TRANSLATION);
         break;
      case STAGEObjectMotionModel::MOTION_TYPE_ROTATION:
         saveSelectedActorOrigValues(dtDAL::TransformableActorProxy::PROPERTY_ROTATION);
         break;
      case STAGEObjectMotionModel::MOTION_TYPE_SCALE:
         saveSelectedActorOrigValues("Scale");
         break;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorViewport::endActorMode(QMouseEvent* e)
   {
      setInteractionMode(Viewport::InteractionMode::NOTHING);

      // we could send hundreds of translation and rotation events, so make sure
      // we surround it in a change transaction
      EditorEvents::GetInstance().emitBeginChangeTransaction();

      // Update the selected actor proxies with their new values.
      switch (mMotionType)
      {
      case STAGEObjectMotionModel::MOTION_TYPE_TRANSLATION:
         updateActorSelectionProperty(dtDAL::TransformableActorProxy::PROPERTY_TRANSLATION);
         break;
      case STAGEObjectMotionModel::MOTION_TYPE_ROTATION:
         updateActorSelectionProperty(dtDAL::TransformableActorProxy::PROPERTY_ROTATION);
         break;
      case STAGEObjectMotionModel::MOTION_TYPE_SCALE:
         updateActorSelectionProperty("Scale");
         break;
      }

      EditorEvents::GetInstance().emitEndChangeTransaction();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorViewport::onEditorPreferencesChanged()
   {
      mAttachActorToCamera = EditorData::GetInstance().getRigidCamera();

      // sync up our local vs world space setting. Affects the actor movement/rotation widget in the viewports
      bool wasGlobal = true;
      if (dtCore::ObjectMotionModel::LOCAL_SPACE == mObjectMotionModel->GetCoordinateSpace())
      {
         wasGlobal = false;
      }

      bool useGlobalOrientation = EditorData::GetInstance().GetUseGlobalOrientationForViewportWidget();
      if (useGlobalOrientation)
      {
         mObjectMotionModel->SetCoordinateSpace(dtCore::ObjectMotionModel::WORLD_SPACE);
      }
      else
      {
         mObjectMotionModel->SetCoordinateSpace(dtCore::ObjectMotionModel::LOCAL_SPACE);
      }

      if (wasGlobal != useGlobalOrientation)
      {
         refresh();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorViewport::connectInteractionModeSlots()
   {
      Viewport::connectInteractionModeSlots();
      EditorEvents* editorEvents = &EditorEvents::GetInstance();

      connect(editorEvents, SIGNAL(editorPreferencesChanged()),
         this, SLOT(onEditorPreferencesChanged()));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorViewport::disconnectInteractionModeSlots()
   {
      Viewport::disconnectInteractionModeSlots();
      EditorEvents* editorEvents = &EditorEvents::GetInstance();

      disconnect(editorEvents, SIGNAL(editorPreferencesChanged()),
         this, SLOT(onEditorPreferencesChanged()));
   }

} // namespace dtEditQt
