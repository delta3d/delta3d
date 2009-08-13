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

   /////////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(PerspectiveViewport::CameraMode);
   const PerspectiveViewport::CameraMode
      PerspectiveViewport::CameraMode::CAMERA_TRANSLATE("CAMERA_TRANSLATE");
   const PerspectiveViewport::CameraMode
      PerspectiveViewport::CameraMode::CAMERA_LOOK("CAMERA_LOOK");
   const PerspectiveViewport::CameraMode
      PerspectiveViewport::CameraMode::CAMERA_NAVIGATE("CAMERA_NAVIGATE");
   const PerspectiveViewport::CameraMode
      PerspectiveViewport::CameraMode::NOTHING("NOTHING");
   /////////////////////////////////////////////////////////////////////////////


   ///////////////////////////////////////////////////////////////////////////////
   PerspectiveViewport::PerspectiveViewport(const std::string& name, QWidget* parent,
      osg::GraphicsContext* shareWith)
      : EditorViewport(ViewportManager::ViewportType::PERSPECTIVE, name, parent, shareWith)
   {
      mCameraMode = &CameraMode::NOTHING;

      //mCamera = ViewportManager::GetInstance().getWorldViewCamera();
      //mCamera->setFarClipPlane(250000.0f);
      setMoveActorWithCamera(EditorData::GetInstance().getRigidCamera());
   }

   ///////////////////////////////////////////////////////////////////////////////
   //void PerspectiveViewport::setScene(dtCore::Scene* scene)
   //{
   //   EditorViewport::setScene(scene);
   //}

   ///////////////////////////////////////////////////////////////////////////////
   void PerspectiveViewport::resizeGL(int width, int height)
   {
      if (height == 0)
      {
         height = 1;
      }

      getCamera()->setAspectRatio((double)width / (double)height);
      EditorViewport::resizeGL(width, height);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void PerspectiveViewport::initializeGL()
   {
      EditorViewport::initializeGL();
      setRenderStyle(Viewport::RenderStyle::TEXTURED, false);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PerspectiveViewport::keyPressEvent(QKeyEvent* e)
   {
      EditorViewport::keyPressEvent(e);

      if (e->key()==Qt::Key_A)
      {
         // If the 'A' key is pressed, try to create an actor
         EditorEvents::GetInstance().emitCreateActor();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   //void PerspectiveViewport::keyReleaseEvent(QKeyEvent* e)
   //{
   //   EditorViewport::keyReleaseEvent(e);
   //}

   ///////////////////////////////////////////////////////////////////////////////
   //void PerspectiveViewport::mousePressEvent(QMouseEvent* e)
   //{
   //   EditorViewport::mousePressEvent(e);
   //}

   ///////////////////////////////////////////////////////////////////////////////
   //void PerspectiveViewport::mouseReleaseEvent(QMouseEvent* e)
   //{
   //   EditorViewport::mouseReleaseEvent(e);
   //}

   ///////////////////////////////////////////////////////////////////////////////
   //void PerspectiveViewport::onMouseMoveEvent(QMouseEvent* e, float dx, float dy)
   //{
   //   EditorViewport::onMouseMoveEvent(e, dx, dy);
   //}

   ///////////////////////////////////////////////////////////////////////////////
   bool PerspectiveViewport::beginCameraMode(QMouseEvent* e)
   {
      if (!EditorViewport::beginCameraMode(e))
      {
         return false;
      }

      setInteractionMode(Viewport::InteractionMode::CAMERA);

      Qt::MouseButtons bothButtons = Qt::LeftButton | Qt::RightButton;

      if (getMoveActorWithCamera() && getEnableKeyBindings() &&
         e->modifiers() == Qt::ShiftModifier &&
         mCameraMode == &CameraMode::NOTHING)
      {
         attachCurrentSelectionToCamera();
         saveSelectedActorOrigValues(dtDAL::TransformableActorProxy::PROPERTY_TRANSLATION);
         saveSelectedActorOrigValues(dtDAL::TransformableActorProxy::PROPERTY_ROTATION);
         saveSelectedActorOrigValues("Scale");
      }

      if (mMouseButtons == bothButtons || mMouseButtons == Qt::MidButton)
      {
         mCameraMode = &CameraMode::CAMERA_TRANSLATE;
      }
      else if (mMouseButton == Qt::LeftButton)
      {
         mCameraMode = &CameraMode::CAMERA_NAVIGATE;
      }
      else if (mMouseButton == Qt::RightButton)
      {
         mCameraMode = &CameraMode::CAMERA_LOOK;
      }
      else
      {
         mCameraMode = &CameraMode::NOTHING;
         return true;
      }

      trapMouseCursor();

      return true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool PerspectiveViewport::endCameraMode(QMouseEvent* e)
   {
      if (!EditorViewport::endCameraMode(e))
      {
         return false;
      }

      if (mMouseButton == Qt::LeftButton && mMouseButtons == Qt::RightButton)
      {
         mCameraMode = &CameraMode::CAMERA_LOOK;
         EditorViewport::beginCameraMode(e);
      }
      else if (mMouseButton == Qt::RightButton && mMouseButtons == Qt::LeftButton)
      {
         mCameraMode = &CameraMode::CAMERA_NAVIGATE;
         EditorViewport::beginCameraMode(e);
      }
      else
      {
         setInteractionMode(Viewport::InteractionMode::NOTHING);

         mCameraMode = &CameraMode::NOTHING;
         releaseMouseCursor();
         if (getMoveActorWithCamera() && getCamera() != NULL &&
            getEnableKeyBindings() && getCamera()->getNumActorAttachments() != 0)
         {
            // we could send hundreds of translation and rotation events, so make sure
            // we surround it in a change transaction
            EditorEvents::GetInstance().emitBeginChangeTransaction();
            EditorData::GetInstance().getUndoManager().beginMultipleUndo();
            updateActorSelectionProperty(dtDAL::TransformableActorProxy::PROPERTY_TRANSLATION);
            updateActorSelectionProperty(dtDAL::TransformableActorProxy::PROPERTY_ROTATION);
            updateActorSelectionProperty("Scale");
            EditorData::GetInstance().getUndoManager().endMultipleUndo();

            EditorEvents::GetInstance().emitEndChangeTransaction();

            getCamera()->removeAllActorAttachments();
         }
      }

      return true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool PerspectiveViewport::beginActorMode(QMouseEvent* e)
   {
      if (!EditorViewport::beginActorMode(e))
      {
         return false;
      }

      return true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool PerspectiveViewport::endActorMode(QMouseEvent* e)
   {
      if (!EditorViewport::endActorMode(e))
      {
         return false;
      }

      return true;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool PerspectiveViewport::moveCamera(float dx, float dy)
   {
      if (!EditorViewport::moveCamera(dx, dy))
      {
         return false;
      }

      if (*mCameraMode == CameraMode::NOTHING || getCamera() == NULL)
      {
         return true;
      }

      if (*mCameraMode == CameraMode::CAMERA_NAVIGATE)
      {
         getCamera()->yaw(-dx / 10.0f);

         //Move along the view direction, however, ignore the z-axis.  This way
         //we can look at the ground but move parallel to it.
         osg::Vec3 viewDir = getCamera()->getViewDir() * ((float)-dy/getMouseSensitivity());
         viewDir[2] = 0.0f;
         getCamera()->move(viewDir);
      }
      else if (*mCameraMode == CameraMode::CAMERA_LOOK)
      {
         getCamera()->pitch(-dy / 10.0f);
         getCamera()->yaw(-dx / 10.0f);
      }
      else if (*mCameraMode == CameraMode::CAMERA_TRANSLATE)
      {
         getCamera()->move(getCamera()->getUpDir() *
            (-dy / getMouseSensitivity()));
         getCamera()->move(getCamera()->getRightDir() *
            (dx / getMouseSensitivity()));
      }

      return true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void PerspectiveViewport::attachCurrentSelectionToCamera()
   {
      ViewportOverlay::ActorProxyList::iterator itor;
      ViewportOverlay::ActorProxyList& selection =
         ViewportManager::GetInstance().getViewportOverlay()->getCurrentActorSelection();

      if (getCamera() == NULL)
      {
         return;
      }

      for (itor = selection.begin(); itor != selection.end(); ++itor)
      {
         dtDAL::ActorProxy* proxy = const_cast<dtDAL::ActorProxy*>(itor->get());
         dtDAL::TransformableActorProxy* tProxy =
            dynamic_cast<dtDAL::TransformableActorProxy*>(proxy);

         if (tProxy != NULL)
         {
            getCamera()->attachActorProxy(tProxy);
         }
      }
   }
} // namespace dtEditQt
