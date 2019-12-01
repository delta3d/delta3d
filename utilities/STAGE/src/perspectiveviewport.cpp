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
#include <dtEditQt/perspectiveviewport.h>
#include <dtEditQt/viewportoverlay.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/stagecameramotionmodel3d.h>
#include <dtCore/transformableactorproxy.h>
#include <dtCore/map.h>
#include <dtCore/actorproxyicon.h>

namespace dtEditQt
{
   ///////////////////////////////////////////////////////////////////////////////
   PerspectiveViewport::PerspectiveViewport(const std::string& name, QWidget* parent,
      osg::GraphicsContext* shareWith)
      : EditorViewport(ViewportManager::ViewportType::PERSPECTIVE, name, parent, shareWith)
   {
      setMoveActorWithCamera(EditorData::GetInstance().getRigidCamera());

      ViewportManager::GetInstance().setWorldViewCamera(getCamera());

      mObjectMotionModel->SetScale(1.0f);

      // Create our camera model.
      mDefaultCameraMotionModel = new STAGECameraMotionModel3D();
      mDefaultCameraMotionModel->SetCamera(getCamera());
      mDefaultCameraMotionModel->SetViewport(this);

      mCameraMotionModel = mDefaultCameraMotionModel;

      GetObjectMotionModel()->SetMaxObjectDistanceFromCamera(5000000.0f);
   }

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

      if (e->key() == Qt::Key_A)
      {
         // If the 'A' key is pressed, try to create an actor
         EditorEvents::GetInstance().emitCreateActor();
      }
      else if (e->key() == Qt::Key_Shift)
      {
         attachCurrentSelectionToCamera();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PerspectiveViewport::keyReleaseEvent(QKeyEvent* e)
   {
      EditorViewport::keyReleaseEvent(e);

      if (e->key() == Qt::Key_Shift)
      {
         detachCurrentSelectionFromCamera();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void PerspectiveViewport::attachCurrentSelectionToCamera()
   {
      if (!(getMoveActorWithCamera() && getEnableKeyBindings()))
      {
         return;
      }

      ViewportOverlay::ActorProxyList::iterator itor;
      ViewportOverlay::ActorProxyList& selection =
         ViewportManager::GetInstance().getViewportOverlay()->getCurrentActorSelection();

      if (getCamera() == NULL)
      {
         return;
      }

      for (itor = selection.begin(); itor != selection.end(); ++itor)
      {
         dtCore::BaseActorObject* proxy = const_cast<dtCore::BaseActorObject*>(itor->get());
         dtCore::TransformableActorProxy* tProxy =
            dynamic_cast<dtCore::TransformableActorProxy*>(proxy);

         if (tProxy != NULL)
         {
            getCamera()->attachActorProxy(tProxy);
         }
      }

      mObjectMotionModel->SetInteractionEnabled(false);

      saveSelectedActorOrigValues(dtCore::TransformableActorProxy::PROPERTY_TRANSLATION);
      saveSelectedActorOrigValues(dtCore::TransformableActorProxy::PROPERTY_ROTATION);
      saveSelectedActorOrigValues("Scale");
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PerspectiveViewport::detachCurrentSelectionFromCamera()
   {
      if (!(getMoveActorWithCamera() &&
         getEnableKeyBindings() &&
         getCamera()->getNumActorAttachments() != 0))
      {
         return;
      }

      // we could send hundreds of translation and rotation events, so make sure
      // we surround it in a change transaction
      EditorEvents::GetInstance().emitBeginChangeTransaction();
      EditorData::GetInstance().getUndoManager().beginMultipleUndo();
      updateActorSelectionProperty(dtCore::TransformableActorProxy::PROPERTY_TRANSLATION);
      updateActorSelectionProperty(dtCore::TransformableActorProxy::PROPERTY_ROTATION);
      updateActorSelectionProperty("Scale");
      EditorData::GetInstance().getUndoManager().endMultipleUndo();

      mObjectMotionModel->SetInteractionEnabled(true);
      EditorEvents::GetInstance().emitEndChangeTransaction();

      getCamera()->removeAllActorAttachments();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void PerspectiveViewport::updateActorProxyBillboards()
   {
      dtCore::Map* currentMap = EditorData::GetInstance().getCurrentMap();
      dtCore::ActorRefPtrVector proxies;
      dtCore::ActorRefPtrVector::iterator itor;

      if (currentMap == NULL || getCamera() == NULL)
      {
         return;
      }

      currentMap->GetAllProxies(proxies);
      for (itor = proxies.begin(); itor != proxies.end(); ++itor)
      {
         dtCore::BaseActorObject* proxy = itor->get();
         const dtCore::BaseActorObject::RenderMode& renderMode = proxy->GetRenderMode();

         if (renderMode == dtCore::BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON ||
            renderMode == dtCore::BaseActorObject::RenderMode::DRAW_BILLBOARD_ICON)
         {
            dtCore::ActorProxyIcon* billBoard = proxy->GetBillBoardIcon();
            if (billBoard != NULL)
            {
               billBoard->SetRotation(osg::Matrix::rotate(getCamera()->getOrientation()));
            }
         }
         else if (renderMode == dtCore::BaseActorObject::RenderMode::DRAW_AUTO)
         {
         }
      }
   }
} // namespace dtEditQt
