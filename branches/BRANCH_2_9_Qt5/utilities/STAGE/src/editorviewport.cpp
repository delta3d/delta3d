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

#include <dtActors/prefabactor.h>
#include <prefix/stageprefix.h>

#include <dtCore/deltadrawable.h>
#include <dtCore/transform.h>

#include <dtEditQt/editoractions.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/editorviewport.h>
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/propertyeditor.h>
#include <dtEditQt/stageglwidget.h>
#include <dtEditQt/viewportoverlay.h>

#include <dtCore/actorproxyicon.h>
#include <dtCore/arrayactorproperty.h>
#include <dtCore/actorfactory.h>
#include <dtCore/map.h>
#include <dtCore/mapxml.h>
#include <dtCore/project.h>
#include <dtCore/resourceactorproperty.h>
#include <dtCore/transformableactorproxy.h>
#include <dtCore/vectoractorproperties.h>
#include <dtCore/compass.h>

#include <dtUtil/exception.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/nodemask.h>

#include <QtCore/QMimeData>
#include <QtGui/QDrag>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragLeaveEvent>
#include <QtGui/QMouseEvent>

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   EditorViewport::EditorViewport(ViewportManager::ViewportType& type,
      const std::string& name, QWidget* parent,
      osg::GraphicsContext* shareWith)
      : Viewport(type, name, parent, shareWith)
      , mObjectMotionModel(NULL)
      , mGhostProxy(NULL)
      , mSkipUpdateForCam(false)
      , mEnabledMask(0xFFFF8FFF)
      , mDisabledMask(0x0000800F)
      , mEnabled(false)
      , mIsRemoved(false)
   {
      mObjectMotionModel = new STAGEObjectMotionModel(GetView());
      mObjectMotionModel->SetEnabled(false);
      mObjectMotionModel->ClearTargets();
      mObjectMotionModel->SetScale(1.5f);

      this->GetQGLWidget()->setAcceptDrops(true);

      STAGEGLWidget* glWidget = dynamic_cast<STAGEGLWidget*>(this->GetQGLWidget());
      if (NULL != glWidget)
      {
         glWidget->SetViewport(this);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorViewport::setCameraMotionModel(STAGECameraMotionModel* motion)
   {
      if (motion)
      {
         mCameraMotionModel = motion;
      }
      else
      {
         mCameraMotionModel = mDefaultCameraMotionModel;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorViewport::setScene(dtCore::Scene* scene)
   {
      Viewport::setScene(scene);

      if (scene->GetSceneNode())
      {
         if (mObjectMotionModel.valid())
         {
            mObjectMotionModel->SetSceneNode(GetRootNode());

            mObjectMotionModel->SetCamera(mCamera->getDeltaCamera());
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorViewport::ShowCompass()
   {
      dtCore::Compass* compass = getCamera()->ShowCompass();
      if (compass)
      {
         GetRootNode()->addChild(compass->GetOSGNode());
         compass->GetOSGNode()->setNodeMask(mDisabledMask & 0x0000F000);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorViewport::refreshActorSelection(const std::vector< dtCore::RefPtr<dtCore::BaseActorObject> >& actors)
   {
      Viewport::refreshActorSelection(actors);

      if (actors.size() > 0)
      {
         mObjectMotionModel->ClearTargets();

         // Now iterate through the additional targets.
         bool canScale = false;
         for (int actorIndex = 0; actorIndex < (int)actors.size(); actorIndex++)
         {
            dtCore::TransformableActorProxy* targetProxy =
               dynamic_cast<dtCore::TransformableActorProxy*>(actors[actorIndex].get());

            if (targetProxy)
            {
               mObjectMotionModel->AddTarget(targetProxy);

               // Only place the widget on the latest selection.
               if (actorIndex == 0)
               {
                  dtCore::Transformable* target = NULL;
                  targetProxy->GetDrawable(target);
                  if (target)
                  {
                     mObjectMotionModel->SetTarget(target);
                  }
               }

               // Once we determine that a target can scale, we don't need to
               // test the others.
               if (!canScale)
               {
                  // Determine if this target can be scaled.
                  dtCore::ActorProperty* prop = targetProxy->GetProperty("Scale");
                  dtCore::Vec3ActorProperty* scaleProp = dynamic_cast<dtCore::Vec3ActorProperty*>(prop);

                  if (scaleProp)
                  {
                     canScale = true;
                  }
               }
            }
         }

         mObjectMotionModel->SetScaleEnabled(canScale);
      }
      else
      {
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


   ////////////////////////////////////////////////////////////////////////////////
   void EditorViewport::keyPressEvent(QKeyEvent* e)
   {
      bool holdingControl = false;
      if (e->modifiers() == Qt::ControlModifier)
      {
         holdingControl = true;
      }

      int index = -1;
      switch(e->key())
      {
      case Qt::Key_1: index = 1; break;
      case Qt::Key_2: index = 2; break;
      case Qt::Key_3: index = 3; break;
      case Qt::Key_4: index = 4; break;
      case Qt::Key_5: index = 5; break;
      case Qt::Key_6: index = 6; break;
      case Qt::Key_7: index = 7; break;
      case Qt::Key_8: index = 8; break;
      case Qt::Key_9: index = 9; break;
      case Qt::Key_0: index = 10; break;
      case Qt::Key_Delete:
         {
            EditorActions::GetInstance().slotEditDeleteActors();
         }
         return;
      }

      if (index > -1)
      {
         // If we are holding control and pressing a number key, save
         // the current camera position to a preset.
         if (holdingControl)
         {
            ViewportManager::GetInstance().SavePresetCamera(index);
         }
         else
         {
            ViewportManager::GetInstance().LoadPresetCamera(index);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorViewport::keyReleaseEvent(QKeyEvent* e)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorViewport::ClearGhostProxy()
   {
      if (mGhostProxy.valid())
      {
         dtCore::DeltaDrawable* drawable = NULL;
         mGhostProxy->GetDrawable(drawable);

         const dtCore::BaseActorObject::RenderMode& renderMode = mGhostProxy->GetRenderMode();
         if (renderMode == dtCore::BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON ||
            renderMode == dtCore::BaseActorObject::RenderMode::DRAW_BILLBOARD_ICON)
         {
            dtCore::ActorProxyIcon* billBoard = mGhostProxy->GetBillBoardIcon();
            if (billBoard)
            {
               ViewportManager::GetInstance().getMasterScene()->RemoveChild(billBoard->GetDrawable());
            }
         }

         ViewportManager::GetInstance().getMasterScene()->RemoveChild(drawable);

         mGhostProxy = NULL;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorViewport::dragEnterEvent(QDragEnterEvent* event)
   {
      bool validDrag = false;
      dtCore::DeltaDrawable* drawable = NULL;
      QByteArray ghostData;
      dtCore::ResourceActorProperty* resourceProp = NULL;

      // Create a ghost of the object being dragged into the view.
      if (event->mimeData()->hasFormat("Prefab"))
      {
         validDrag = true;
         ClearGhostProxy();

         mGhostProxy = dtCore::ActorFactory::GetInstance().CreateActor("dtActors", "Prefab");
         if (mGhostProxy.valid())
         {
            ghostData = event->mimeData()->data("Prefab");
            resourceProp = dynamic_cast<dtCore::ResourceActorProperty*>(mGhostProxy->GetProperty("PrefabResource"));
            mGhostProxy->GetDrawable(drawable);
         }
      }
      else if (event->mimeData()->hasFormat("StaticMesh"))
      {
         validDrag = true;
         ClearGhostProxy();

         mGhostProxy = dtCore::ActorFactory::GetInstance().CreateActor("dtcore", "Static Mesh");
         if (mGhostProxy.valid())
         {
            ghostData = event->mimeData()->data("StaticMesh");
            resourceProp = dynamic_cast<dtCore::ResourceActorProperty*>(mGhostProxy->GetProperty("static mesh"));
            mGhostProxy->GetDrawable(drawable);
         }
      }
      else if (event->mimeData()->hasFormat("SkeletalMesh"))
      {
         validDrag = true;
         ClearGhostProxy();

         mGhostProxy = dtCore::ActorFactory::GetInstance().CreateActor("dtanim", "AnimationGameActor");
         if (mGhostProxy.valid())
         {
            ghostData = event->mimeData()->data("SkeletalMesh");
            resourceProp = dynamic_cast<dtCore::ResourceActorProperty*>(mGhostProxy->GetProperty("Skeletal Mesh"));
            mGhostProxy->GetDrawable(drawable);
         }
      }
      else if (event->mimeData()->hasFormat("Particle"))
      {
         validDrag = true;
         ClearGhostProxy();

         mGhostProxy = dtCore::ActorFactory::GetInstance().CreateActor("dtcore", "Particle System");
         if (mGhostProxy.valid())
         {
            ghostData = event->mimeData()->data("Particle");
            resourceProp = dynamic_cast<dtCore::ResourceActorProperty*>(mGhostProxy->GetProperty("Particle(s) File"));
            mGhostProxy->GetDrawable(drawable);
         }
      }
      else if (event->mimeData()->hasFormat("Sound"))
      {
         validDrag = true;
         ClearGhostProxy();

         mGhostProxy = dtCore::ActorFactory::GetInstance().CreateActor("dtcore.Environment", "Sound Actor");
         if (mGhostProxy.valid())
         {
            ghostData = event->mimeData()->data("Sound");
            resourceProp = dynamic_cast<dtCore::ResourceActorProperty*>(mGhostProxy->GetProperty("The Sound Effect"));
            mGhostProxy->GetDrawable(drawable);
         }
      }
      else if (event->mimeData()->hasFormat("Actor"))
      {
         validDrag = true;
         ClearGhostProxy();

         ghostData = event->mimeData()->data("Actor");
         QDataStream dataStream(&ghostData, QIODevice::ReadOnly);
         QString category, name;
         dataStream >> category >> name;

         mGhostProxy = dtCore::ActorFactory::GetInstance().CreateActor(category.toStdString(), name.toStdString());
         if (mGhostProxy.valid())
         {
            mGhostProxy->GetDrawable(drawable);
         }
      }
      // Create a ghost of the object being dragged into the view.
      if (event->mimeData()->hasFormat("Director"))
      {
         validDrag = true;
         ClearGhostProxy();

         mGhostProxy = dtCore::ActorFactory::GetInstance().CreateActor("dtActors", "Director Actor");
         if (mGhostProxy.valid())
         {
            ghostData = event->mimeData()->data("Director");
            dtCore::ArrayActorProperty<dtCore::ResourceDescriptor>* arrayProp = dynamic_cast<dtCore::ArrayActorProperty<dtCore::ResourceDescriptor>* >(mGhostProxy->GetProperty("DirectorArray"));
            if (arrayProp)
            {
               if (arrayProp->GetArraySize() == 0)
               {
                  arrayProp->Insert(0);
               }

               arrayProp->SetIndex(0);
               resourceProp = dynamic_cast<dtCore::ResourceActorProperty*>(arrayProp->GetArrayProperty());
            }
            mGhostProxy->GetDrawable(drawable);
         }
      }

      if (validDrag)
      {
         // Set the prefab resource of the actor to the current prefab.
         if (resourceProp)
         {
            QDataStream dataStream(&ghostData, QIODevice::ReadOnly);
            QString resourceIdentity;
            dataStream >> resourceIdentity;

            dtCore::ResourceDescriptor descriptor = dtCore::ResourceDescriptor(resourceIdentity.toStdString());
            resourceProp->SetValue(descriptor);
         }

         // Setup the drawable to be visible in the scene.
         if (drawable && mGhostProxy.valid())
         {
            const dtCore::BaseActorObject::RenderMode& renderMode = mGhostProxy->GetRenderMode();
            if (renderMode == dtCore::BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON ||
                renderMode == dtCore::BaseActorObject::RenderMode::DRAW_BILLBOARD_ICON)
            {
               dtCore::ActorProxyIcon* billBoard = mGhostProxy->GetBillBoardIcon();
               //billBoard->LoadImages();
               if (billBoard)
               {
                  ViewportManager::GetInstance().getMasterScene()->AddChild(billBoard->GetDrawable());
               }
            }

            ViewportManager::GetInstance().getMasterScene()->AddChild(drawable);
         }

         event->accept();
         return;
      }

      event->ignore();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorViewport::dragLeaveEvent(QDragLeaveEvent* event)
   {
      // Remove the ghost.
      ClearGhostProxy();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorViewport::dragMoveEvent(QDragMoveEvent* event)
   {
      if (event->mimeData()->hasFormat("Prefab")      ||
         event->mimeData()->hasFormat("StaticMesh")   ||
         event->mimeData()->hasFormat("SkeletalMesh") ||
         event->mimeData()->hasFormat("Particle")     ||
         event->mimeData()->hasFormat("Sound")        ||
         event->mimeData()->hasFormat("Actor")        ||
         event->mimeData()->hasFormat("Director"))
      {
         // Move the position of the ghost.
         if (mGhostProxy.valid())
         {
            dtCore::DeltaDrawable* ghostDrawable = NULL;
            mGhostProxy->GetDrawable(ghostDrawable);
            std::vector<dtCore::DeltaDrawable*> ignoredDrawables;
            ignoredDrawables.push_back(ghostDrawable);
            dtCore::ActorProxyIcon* icon = mGhostProxy->GetBillBoardIcon();
            if (icon)
            {
               ignoredDrawables.push_back(icon->GetDrawable());
            }
            osg::Vec3 position;

            if (!getPickPosition(event->pos().x(), event->pos().y(), position, ignoredDrawables))
            {
               // If we get here, it means our mouse position did not collide with an object to attach to.

               // Get the current position and direction the camera is facing.
               osg::Vec3 pos = mCamera->getPosition();
               osg::Vec3 viewDir = mCamera->getViewDir();

               const osg::BoundingSphere& bs = mGhostProxy->GetDrawable()->GetOSGNode()->getBound();

               float actorCreationOffset = EditorData::GetInstance().GetActorCreationOffset();
               float offset = (bs.radius() < 1000.0f) ? bs.radius() : 1.0f;
               if (offset <= 0.0f)
               {
                  offset = actorCreationOffset;
               }
               position = pos + (viewDir * offset * 2);

               // Clamp the spawn position to the snap grid.
               position = ViewportManager::GetInstance().GetSnapPosition(position, false, ignoredDrawables);
            }
            else
            {
               // Clamp the spawn position to the snap grid and then to the ground.
               position = ViewportManager::GetInstance().GetSnapPosition(
                  position, ViewportManager::GetInstance().GetSnapTranslationEnabled(),
                  ignoredDrawables);
            }

            dtCore::TransformableActorProxy* tProxy =
               dynamic_cast<dtCore::TransformableActorProxy*>(mGhostProxy.get());

            if (tProxy)
            {
               tProxy->SetTranslation(position);
            }

            dtCore::ActorProxyIcon* billBoard = mGhostProxy->GetBillBoardIcon();
            if (billBoard)
            {
               billBoard->SetRotation(osg::Matrix::rotate(getCamera()->getOrientation()));
            }
         }

         event->setDropAction(Qt::MoveAction);
         event->accept();
      }
      else
      {
         event->ignore();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorViewport::dropEvent(QDropEvent* event)
   {
      if (!IsSupportedDragDropFormat(event) ||
          EditorData::GetInstance().getCurrentMap() == NULL)
      {
         event->ignore();

         //wipe out any potential ghost proxy that might be triggering this drop
         ClearGhostProxy();

         return;
      }

      if (mGhostProxy.valid())
      {
         dtCore::RefPtr<dtCore::Map> mapPtr = EditorData::GetInstance().getCurrentMap();

         // Unroll prefabs
         if (event->mimeData()->hasFormat("Prefab"))
         {
            UnrollPrefab(event, mapPtr.get());
            return;
         }

         EditorActions::GetInstance().AddActorToMap(*mGhostProxy, *mapPtr, true);

         // let the world know that a new proxy exists
         EditorEvents::GetInstance().emitBeginChangeTransaction();
         EditorEvents::GetInstance().emitActorProxyCreated(mGhostProxy.get(), false);
         EditorEvents::GetInstance().emitEndChangeTransaction();

         // Now, let the world that it should select the new actor proxy.
         std::vector< dtCore::RefPtr<dtCore::BaseActorObject> > actors;
         actors.push_back(mGhostProxy.get());
         EditorEvents::GetInstance().emitActorsSelected(actors);

         const dtCore::BaseActorObject::RenderMode& renderMode = mGhostProxy->GetRenderMode();
         if (renderMode == dtCore::BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON ||
             renderMode == dtCore::BaseActorObject::RenderMode::DRAW_BILLBOARD_ICON)
         {
            dtCore::ActorProxyIcon* billBoard = mGhostProxy->GetBillBoardIcon();
            if (billBoard)
            {
               ViewportManager::GetInstance().getMasterScene()->RemoveChild(billBoard->GetDrawable());
            }
         }

         mGhostProxy = NULL;
      }

      event->setDropAction(Qt::MoveAction);
      event->accept();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorViewport::mousePressEvent(QMouseEvent* e)
   {
      mMouseButton = e->button();
      mMouseButtons = e->buttons();
      mKeyMods = e->modifiers();

      this->GetQGLWidget()->setFocus();

      ViewportManager::GetInstance().emitMousePressEvent(this, e);

      if (mMouseButton == Qt::LeftButton)
      {
         //mObjectMotionModel->OnLeftMousePressed();

         if (mCameraMotionModel.valid())
         {
            mCameraMotionModel->OnLeftMousePressed();
         }
      }
      else if (mMouseButton == Qt::RightButton)
      {
         //mObjectMotionModel->OnRightMousePressed();

         if (mCameraMotionModel.valid())
         {
            mCameraMotionModel->OnRightMousePressed();
         }
      }
      else if (mMouseButton == Qt::MidButton)
      {
         if (mCameraMotionModel.valid())
         {
            mCameraMotionModel->OnMiddleMousePressed();
         }
      }

      osg::Vec2 pos = convertMousePosition(e->pos());

      // If this is our first click.
      if (getInteractionMode() == Viewport::InteractionMode::NOTHING)
      {
         // If this returns true, it means we are hovering our mouse over a
         // valid motion model widget so we should go into actor mode.
         if (shouldBeginActorMode(pos))
         {
            beginActorMode(e);
         }
         // If we are not hovering over the motion models widget,
         // Then we can select new actors.  If the mouse is moved
         // before it has been released, we will jump into camera
         // mode instead.
         else
         {
            mObjectMotionModel->SetInteractionEnabled(false);
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

      ViewportManager::GetInstance().emitMouseReleaseEvent(this, e);

      if (mMouseButton == Qt::LeftButton)
      {
         //mObjectMotionModel->OnLeftMouseReleased();

         if (mCameraMotionModel.valid())
         {
            mCameraMotionModel->OnLeftMouseReleased();
         }
      }
      else if (mMouseButton == Qt::RightButton)
      {
         //mObjectMotionModel->OnRightMouseReleased();

         if (mCameraMotionModel.valid())
         {
            mCameraMotionModel->OnRightMouseReleased();
         }
      }
      else if (mMouseButton == Qt::MidButton)
      {
         if (mCameraMotionModel.valid())
         {
            mCameraMotionModel->OnMiddleMouseReleased();
         }
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

         // Selecting actors can only be done if we don't have actors attached to the camera.
         if (getCamera()->getNumActorAttachments() == 0)
         {
            selectActors(e);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorViewport::mouseDoubleClickEvent(QMouseEvent* e)
   {
      mMouseButton = e->button();
      mMouseButtons = e->buttons();
      mKeyMods = e->modifiers();

      ViewportManager::GetInstance().emitMouseDoubleClickEvent(this, e);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorViewport::wheelEvent(QWheelEvent* e)
   {
      ViewportManager::GetInstance().emitWheelEvent(this, e);

      if (mCameraMotionModel.valid())
      {
         mCameraMotionModel->WheelEvent(e->delta());

         // The motion model will not update unless the mouse moves,
         // because of this we need to force it to update its' widget
         // geometry.
         mObjectMotionModel->UpdateWidgets();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorViewport::onMouseMoveEvent(QMouseEvent* e, float dx, float dy)
   {
      ViewportManager::GetInstance().emitMouseMoveEvent(this, e);

      // Get the position of the mouse.
      //osg::Vec2 pos = convertMousePosition(e->pos());

      // If we move the mouse while in select actor mode,
      // immediately jump to camera motion mode.
      if (getInteractionMode() == Viewport::InteractionMode::SELECT_ACTOR)
      {
         beginCameraMode(e);
         mSkipUpdateForCam = true;
      }
      // We skip the first camera update so we have time to snap the mouse
      // cursor to the center of the window without having it be recognized
      // as a mouse movement, therefore eliminating the initial camera pop.
      else if (mSkipUpdateForCam)
      {
         mSkipUpdateForCam = false;
      }
      // Update the camera.
      else if (getInteractionMode() == Viewport::InteractionMode::CAMERA)
      {
         if (mCameraMotionModel.valid())
         {
            std::cout << "Mouse Move:" << dx << " " << dy << std::endl;
            mCameraMotionModel->OnMouseMoved(dx, dy);
         }
      }
      else if (getInteractionMode() == Viewport::InteractionMode::ACTOR)
      {
         // If we are holding ALT during the actor movement,
         // then it should copy the current actor(s) and move
         // them instead.
         if (mKeyMods == Qt::AltModifier)
         {
            bool overrideDefault = false;
            ViewportManager::GetInstance().emitDuplicateActors(this, &overrideDefault);
            if (!overrideDefault)
            {
               EditorActions::GetInstance().slotEditDuplicateActors(false);
            }

            // Make sure we only duplicate the actors once.
            mKeyMods = 0x0;
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool EditorViewport::beginCameraMode(QMouseEvent* e)
   {
      bool overrideDefault = false;
      ViewportManager::GetInstance().emitBeginCameraMode(this, e, &overrideDefault);
      if (overrideDefault)
      {
         return false;
      }

      if (mCameraMotionModel.valid())
      {
         if (mCameraMotionModel->BeginCameraMode(e))
         {
            setInteractionMode(Viewport::InteractionMode::CAMERA);
            return true;
         }
      }

      return false;
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool EditorViewport::endCameraMode(QMouseEvent* e)
   {
      bool overrideDefault = false;
      ViewportManager::GetInstance().emitEndCameraMode(this, e, &overrideDefault);
      if (overrideDefault)
      {
         return false;
      }

      if (mCameraMotionModel.valid())
      {
         if (mCameraMotionModel->EndCameraMode(e))
         {
            // Only re-enable the object motion if we don't have actors attached to the camera.
            if (getCamera()->getNumActorAttachments() == 0)
            {
               mObjectMotionModel->SetInteractionEnabled(true);
            }

            setInteractionMode(Viewport::InteractionMode::NOTHING);

            return true;
         }
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool EditorViewport::shouldBeginActorMode(osg::Vec2 position)
   {
      bool overrideDefault = false;
      bool result = false;
      ViewportManager::GetInstance().emitShouldBeginActorMode(this, position, &overrideDefault, &result);
      if (overrideDefault)
      {
         return result;
      }

      if (mObjectMotionModel->IsEnabled() && mObjectMotionModel->GetNumTargets() > 0 &&
         mObjectMotionModel->GetMotionType() != dtCore::ObjectMotionModel::MOTION_TYPE_MAX)
      {
         return true;
      }

      return false;
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool EditorViewport::beginActorMode(QMouseEvent* e)
   {
      setInteractionMode(Viewport::InteractionMode::ACTOR);

      bool overrideDefault = false;
      ViewportManager::GetInstance().emitBeginActorMode(this, e, &overrideDefault);
      if (overrideDefault)
      {
         return false;
      }

      saveSelectedActorOrigValues(dtCore::TransformableActorProxy::PROPERTY_TRANSLATION);
      saveSelectedActorOrigValues(dtCore::TransformableActorProxy::PROPERTY_ROTATION);
      saveSelectedActorOrigValues("Scale");

      return true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool EditorViewport::endActorMode(QMouseEvent* e)
   {
      setInteractionMode(Viewport::InteractionMode::NOTHING);

      bool overrideDefault = false;
      ViewportManager::GetInstance().emitEndActorMode(this, e, &overrideDefault);
      if (overrideDefault)
      {
         return false;
      }

      // If we are holding the Control key, we should clamp the position
      // of the actor to the ground.
      if (mKeyMods == Qt::ControlModifier)
      {
         std::vector<dtCore::DeltaDrawable*> ignoredDrawables;
         ViewportOverlay::ActorProxyList& selection = ViewportManager::GetInstance().getViewportOverlay()->getCurrentActorSelection();
         for (int selectIndex = 0; selectIndex < (int)selection.size(); selectIndex++)
         {
            dtCore::BaseActorObject* proxy = selection[selectIndex].get();
            if (proxy)
            {
               dtCore::DeltaDrawable* drawable;
               proxy->GetDrawable(drawable);

               if (drawable)
               {
                  ignoredDrawables.push_back(drawable);
               }

               dtCore::ActorProxyIcon* icon = proxy->GetBillBoardIcon();
               if (icon)
               {
                  ignoredDrawables.push_back(icon->GetDrawable());
               }
            }
         }

         if (selection.size() > 0)
         {
            dtCore::TransformableActorProxy* proxy = dynamic_cast<dtCore::TransformableActorProxy*>(selection[0].get());
            osg::Vec3 position = proxy->GetTranslation();
            position = ViewportManager::GetInstance().GetSnapPosition(position, true, ignoredDrawables);
            osg::Vec3 offset = position - proxy->GetTranslation();

            for (int selectIndex = 0; selectIndex < (int)selection.size(); selectIndex++)
            {
               dtCore::TransformableActorProxy* proxy = dynamic_cast<dtCore::TransformableActorProxy*>(selection[selectIndex].get());
               if (proxy)
               {
                  proxy->SetTranslation(proxy->GetTranslation() + offset);
               }
            }
         }
      }

      // we could send hundreds of translation and rotation events, so make sure
      // we surround it in a change transaction
      EditorEvents::GetInstance().emitBeginChangeTransaction();
      EditorData::GetInstance().getUndoManager().beginMultipleUndo();
      updateActorSelectionProperty(dtCore::TransformableActorProxy::PROPERTY_TRANSLATION);
      updateActorSelectionProperty(dtCore::TransformableActorProxy::PROPERTY_ROTATION);
      updateActorSelectionProperty("Scale");
      EditorData::GetInstance().getUndoManager().endMultipleUndo();

      EditorEvents::GetInstance().emitEndChangeTransaction();

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool EditorViewport::selectActors(QMouseEvent* e)
   {
      mObjectMotionModel->SetInteractionEnabled(true);

      bool overrideDefault = false;
      ViewportManager::GetInstance().emitSelectActors(this, e, &overrideDefault);
      if (overrideDefault)
      {
         return false;
      }

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
      }

      return true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorViewport::onEditorPreferencesChanged()
   {
      mAttachActorToCamera = EditorData::GetInstance().getRigidCamera();

      bool useGlobalOrientation = EditorData::GetInstance().GetUseGlobalOrientationForViewportWidget();
      setLocalSpace(!useGlobalOrientation);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorViewport::onGotoActor(dtCore::RefPtr<dtCore::BaseActorObject> proxy)
   {
      Viewport::onGotoActor(proxy);

      // Make sure we refresh the camera motion model with the updated camera.
      mCameraMotionModel->SetCamera(getCamera());
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorViewport::onGotoPosition(double x, double y, double z)
   {
      Viewport::onGotoPosition(x, y, z);

      // Make sure we refresh the camera motion model with the updated camera.
      mCameraMotionModel->SetCamera(getCamera());
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorViewport::slotMoveActorOrCamera(QAction* action)
   {
      dtCore::ActorPtrVector selected;
      EditorData::GetInstance().GetSelectedActors(selected);

      if (selected.empty()) {return;}

      //align the selected actors to the camera's position/rotation
      if (action == EditorActions::GetInstance().mAlignActorToCameraAction)
      {

         dtCore::Transform camXform;
         camXform.Set(getCamera()->getPosition(), getCamera()->getOrientation());

         saveSelectedActorOrigValues(dtCore::TransformableActorProxy::PROPERTY_TRANSLATION);
         saveSelectedActorOrigValues(dtCore::TransformableActorProxy::PROPERTY_ROTATION);

         dtCore::ActorPtrVector::iterator itr = selected.begin();
         while (itr != selected.end())
         {
            if ((*itr)->IsPlaceable())
            {
               dtCore::Transformable* t = (*itr)->GetDrawable<dtCore::Transformable>();
               if (t != NULL)
               {
                  t->SetTransform(camXform);
               }
            }
            ++itr;
         }

         EditorEvents::GetInstance().emitBeginChangeTransaction();
         EditorData::GetInstance().getUndoManager().beginMultipleUndo();
         updateActorSelectionProperty(dtCore::TransformableActorProxy::PROPERTY_TRANSLATION);
         updateActorSelectionProperty(dtCore::TransformableActorProxy::PROPERTY_ROTATION);
         EditorData::GetInstance().getUndoManager().endMultipleUndo();
         EditorEvents::GetInstance().emitEndChangeTransaction();
      }

      //align the camera to the selected Actor
      else if (action == EditorActions::GetInstance().mAlignCameraToActorAction)
      {
         EditorEvents::GetInstance().emitBeginChangeTransaction();
         const dtCore::BaseActorObject* firstProxy = (*selected.begin());

         const dtCore::Transformable* t = firstProxy->GetDrawable<dtCore::Transformable>();
         if (t != NULL)
         {
            dtCore::Transform xform;
            t->GetTransform(xform);
            getCamera()->setPosition(xform.GetTranslation());
            osg::Quat rot;
            xform.GetRotation(rot);
            getCamera()->setRotation(rot);
            getCameraMotionModel()->SetCamera(getCamera());//to reset the camera
         }
         EditorEvents::GetInstance().emitEndChangeTransaction();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorViewport::setLocalSpace(bool enabled)
   {
      // sync up our local vs world space setting. Affects the actor movement/rotation widget in the viewports
      bool wasLocal = false;
      if (dtCore::ObjectMotionModel::LOCAL_SPACE == mObjectMotionModel->GetCoordinateSpace())
      {
         wasLocal = true;
      }

      if (enabled)
      {
         mObjectMotionModel->SetCoordinateSpace(dtCore::ObjectMotionModel::LOCAL_SPACE);
      }
      else
      {
         mObjectMotionModel->SetCoordinateSpace(dtCore::ObjectMotionModel::WORLD_SPACE);
      }

      if (wasLocal != enabled)
      {
         refresh();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorViewport::setSnapTranslation(float increment)
   {
      mObjectMotionModel->SetSnapTranslation(increment);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorViewport::setSnapRotation(float increment)
   {
      mObjectMotionModel->SetSnapRotation(increment);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorViewport::setSnapScale(float increment)
   {
      mObjectMotionModel->SetSnapScale(increment);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorViewport::setSnapEnabled(bool translation, bool rotation, bool scale)
   {
      mObjectMotionModel->SetSnapEnabled(translation, rotation, scale);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorViewport::connectInteractionModeSlots()
   {
      Viewport::connectInteractionModeSlots();

      connect(&EditorEvents::GetInstance(), SIGNAL(editorPreferencesChanged()),
         this, SLOT(onEditorPreferencesChanged()));
      connect(&ViewportManager::GetInstance(), SIGNAL(setSnapTranslation(float)),
         this, SLOT(setSnapTranslation(float)));
      connect(&ViewportManager::GetInstance(), SIGNAL(setSnapRotation(float)),
         this, SLOT(setSnapRotation(float)));
      connect(&ViewportManager::GetInstance(), SIGNAL(setSnapScale(float)),
         this, SLOT(setSnapScale(float)));
      connect(&ViewportManager::GetInstance(), SIGNAL(setSnapEnabled(bool, bool, bool)),
         this, SLOT(setSnapEnabled(bool, bool, bool)));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorViewport::disconnectInteractionModeSlots()
   {
      Viewport::disconnectInteractionModeSlots();
      EditorEvents* editorEvents = &EditorEvents::GetInstance();

      disconnect(editorEvents, SIGNAL(editorPreferencesChanged()),
         this, SLOT(onEditorPreferencesChanged()));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorViewport::renderFrame()
   {
      Viewport::renderFrame();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorViewport::SetEnabled(bool enabled)
   {
      mEnabled = enabled;

      bool overrideDefault = false;
      ViewportManager::GetInstance().emitViewportEnabled(this, enabled, &overrideDefault);
      if (overrideDefault)
      {
         return;
      }

//      GetObjectMotionModel()->SetEnabled(true);
//      getCamera()->getDeltaCamera()->GetOSGCamera()->setCullMask(mEnabledMask);
      GetObjectMotionModel()->SetEnabled(enabled);

      if (enabled)
      {
         getCamera()->getDeltaCamera()->GetOSGCamera()->setCullMask(mEnabledMask);
      }
      else
      {
         getCamera()->getDeltaCamera()->GetOSGCamera()->setCullMask(mDisabledMask);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool EditorViewport::GetEnabled() const
   {
      return mEnabled;
   }

   //////////////////////////////////////////////////////////////////////////
   bool EditorViewport::IsSupportedDragDropFormat(const QDropEvent* event) const
   {
      if (event->mimeData()->hasFormat("Prefab")      ||
         event->mimeData()->hasFormat("StaticMesh")   ||
         event->mimeData()->hasFormat("SkeletalMesh") ||
         event->mimeData()->hasFormat("Particle")     ||
         event->mimeData()->hasFormat("Sound")        ||
         event->mimeData()->hasFormat("Actor")        ||
         event->mimeData()->hasFormat("Director"))
      {
         return true;
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorViewport::UnrollPrefab(QDropEvent* event, dtCore::Map* mapPtr)
   {
      dtCore::TransformableActorProxy* tProxy =
         dynamic_cast<dtCore::TransformableActorProxy*>(mGhostProxy.get());
      osg::Vec3 offset;
      if (tProxy) offset = tProxy->GetTranslation();

      QByteArray ghostData = event->mimeData()->data("Prefab");
      QDataStream dataStream(&ghostData, QIODevice::ReadOnly);
      QString resourceIdentity;
      dataStream >> resourceIdentity;

      dtCore::ResourceDescriptor descriptor = dtCore::ResourceDescriptor(resourceIdentity.toStdString());

      if (mapPtr)
      {
         dtCore::ActorRefPtrVector actors;
         EditorEvents::GetInstance().emitBeginChangeTransaction();
         dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
         fileUtils.PushDirectory(dtCore::Project::GetInstance().GetContext());
         try
         {
            int groupIndex = mapPtr->GetGroupCount();

            dtCore::Project::GetInstance().LoadPrefab(descriptor, actors);

            for (auto i = actors.begin(), iend = actors.end(); i != iend; ++i)
            {
               dtCore::BaseActorObject& curActor = **i;

               EditorActions::GetInstance().AddActorToMap(curActor, *mapPtr, true);
               mapPtr->AddActorToGroup(groupIndex, curActor);

               // TODO Only translate it if it doesn't have parents.

               tProxy = dynamic_cast<dtCore::TransformableActorProxy*>(&curActor);
               if (tProxy)
               {
                  tProxy->SetTranslation(tProxy->GetTranslation() + offset);
               }
               // Notify the creation of the proxies.
               EditorEvents::GetInstance().emitActorProxyCreated(&curActor, false);
            }
         }
         catch (const dtUtil::Exception& e)
         {
            LOG_ERROR(e.What());
         }
         fileUtils.PopDirectory();
         EditorEvents::GetInstance().emitEndChangeTransaction();

         ClearGhostProxy();

         ViewportManager::GetInstance().getViewportOverlay()->clearCurrentSelection();
         ViewportManager::GetInstance().getViewportOverlay()->setMultiSelectMode(true);
         EditorEvents::GetInstance().emitActorsSelected(actors);
      }

      event->setDropAction(Qt::MoveAction);
      event->accept();
      return;
   }

} // namespace dtEditQt
