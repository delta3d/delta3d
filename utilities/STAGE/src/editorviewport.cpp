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
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/viewportoverlay.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editoractions.h>
#include <dtEditQt/propertyeditor.h>
#include <dtEditQt/stageglwidget.h>
#include <dtEditQt/editorviewport.h>
#include <dtDAL/transformableactorproxy.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/map.h>
#include <dtDAL/librarymanager.h>
#include <dtDAL/actorproxyicon.h>
#include <dtCore/deltadrawable.h>
#include <dtUtil/exception.h>
#include <dtUtil/mathdefines.h>
#include <dtDAL/exceptionenum.h>
#include <QtGui/QDrag>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragLeaveEvent>
#include <dtDAL/mapxml.h>
#include <dtDAL/project.h>


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
      , mEnabled(false)
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

   /////////////////////////////////////////////////////////////////////////////
   bool EditorViewport::moveCamera(float dx, float dy)
   {
      bool overrideDefault = false;
      ViewportManager::GetInstance().emitMoveCamera(this, dx, dy, &overrideDefault);
      if (overrideDefault)
      {
         return false;
      }

      return true;
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
               dtCore::Transformable* target = NULL;
               targetProxy->GetActor(target);
               if (target)
               {
                  mObjectMotionModel->SetTarget(target);
               }


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
         
         //// only enable the MotionModel for active Viewports
         //if (GetEnabled())
         //{
         //   mObjectMotionModel->SetEnabled(true);
         //}
      }
      else
      {
         //mObjectMotionModel->SetEnabled(false);
         mObjectMotionModel->ClearTargets();
         mObjectMotionModel->SetScaleEnabled(false);
      }
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
         mGhostProxy->GetActor(drawable);

         const dtDAL::ActorProxy::RenderMode& renderMode = mGhostProxy->GetRenderMode();
         if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON ||
            renderMode == dtDAL::ActorProxy::RenderMode::DRAW_BILLBOARD_ICON)
         {
            dtDAL::ActorProxyIcon* billBoard = mGhostProxy->GetBillBoardIcon();
            if (billBoard)
            {
               ViewportManager::GetInstance().getMasterScene()->RemoveDrawable(billBoard->GetDrawable());
            }
         }

         ViewportManager::GetInstance().getMasterScene()->RemoveDrawable(drawable);

         mGhostProxy = NULL;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorViewport::dragEnterEvent(QDragEnterEvent* event)
   {
      bool validDrag = false;
      dtCore::DeltaDrawable* drawable = NULL;
      QByteArray ghostData;
      dtDAL::ResourceActorProperty* resourceProp = NULL;

      // Create a ghost of the object being dragged into the view.
      if (event->mimeData()->hasFormat("Prefab"))
      {
         validDrag = true;
         ClearGhostProxy();
         
         mGhostProxy = dtDAL::LibraryManager::GetInstance().CreateActorProxy("dtActors", "Prefab");
         if (mGhostProxy.valid())
         {
            ghostData = event->mimeData()->data("Prefab");
            resourceProp = dynamic_cast<dtDAL::ResourceActorProperty*>(mGhostProxy->GetProperty("PrefabResource"));
            mGhostProxy->GetActor(drawable);
         }
      }
      else if (event->mimeData()->hasFormat("StaticMesh"))
      {
         validDrag = true;
         ClearGhostProxy();

         mGhostProxy = dtDAL::LibraryManager::GetInstance().CreateActorProxy("dtcore.Game.Actors", "Game Mesh Actor");
         if (mGhostProxy.valid())
         {
            ghostData = event->mimeData()->data("StaticMesh");
            resourceProp = dynamic_cast<dtDAL::ResourceActorProperty*>(mGhostProxy->GetProperty("static mesh"));
            mGhostProxy->GetActor(drawable);
         }
      }
      else if (event->mimeData()->hasFormat("SkeletalMesh"))
      {
         //validDrag = true;
         //ClearGhostProxy();

         //mGhostProxy = dtDAL::LibraryManager::GetInstance().CreateActorProxy("dtcore.Game.Actors", "Game Mesh Actor");
         //if (mGhostProxy.valid())
         //{
         //   ghostData = event->mimeData()->data("StaticMesh");
         //   resourceProp = dynamic_cast<dtDAL::ResourceActorProperty*>(mGhostProxy->GetProperty("static mesh"));
         //   mGhostProxy->GetActor(drawable);
         //}
      }
      else if (event->mimeData()->hasFormat("Particle"))
      {
         validDrag = true;
         ClearGhostProxy();

         mGhostProxy = dtDAL::LibraryManager::GetInstance().CreateActorProxy("dtcore", "Particle System");
         if (mGhostProxy.valid())
         {
            ghostData = event->mimeData()->data("Particle");
            resourceProp = dynamic_cast<dtDAL::ResourceActorProperty*>(mGhostProxy->GetProperty("Particle(s) File"));
            mGhostProxy->GetActor(drawable);
         }
      }
      else if (event->mimeData()->hasFormat("Sound"))
      {
         validDrag = true;
         ClearGhostProxy();

         mGhostProxy = dtDAL::LibraryManager::GetInstance().CreateActorProxy("dtcore", "Particle System");
         if (mGhostProxy.valid())
         {
            ghostData = event->mimeData()->data("Particle");
            resourceProp = dynamic_cast<dtDAL::ResourceActorProperty*>(mGhostProxy->GetProperty("Particle(s) File"));
            mGhostProxy->GetActor(drawable);
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

         mGhostProxy = dtDAL::LibraryManager::GetInstance().CreateActorProxy(category.toStdString(), name.toStdString());
         if (mGhostProxy.valid())
         {
            mGhostProxy->GetActor(drawable);
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

            dtDAL::ResourceDescriptor descriptor = dtDAL::ResourceDescriptor(resourceIdentity.toStdString());
            resourceProp->SetValue(&descriptor);
         }

         // Setup the drawable to be visible in the scene.
         if (drawable && mGhostProxy.valid())
         {
            const dtDAL::ActorProxy::RenderMode& renderMode = mGhostProxy->GetRenderMode();
            if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON ||
                renderMode == dtDAL::ActorProxy::RenderMode::DRAW_BILLBOARD_ICON)
            {
               dtDAL::ActorProxyIcon* billBoard = mGhostProxy->GetBillBoardIcon();
               //billBoard->LoadImages();
               if (billBoard)
               {
                  ViewportManager::GetInstance().getMasterScene()->AddDrawable(billBoard->GetDrawable());
               }
            }

            ViewportManager::GetInstance().getMasterScene()->AddDrawable(drawable);
            //ViewportManager::GetInstance().refreshAllViewports();
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

      //ViewportManager::GetInstance().refreshAllViewports();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorViewport::dragMoveEvent(QDragMoveEvent* event)
   {
      if (event->mimeData()->hasFormat("Prefab")      ||
         event->mimeData()->hasFormat("StaticMesh")   ||
         event->mimeData()->hasFormat("SkeletalMesh") ||
         event->mimeData()->hasFormat("Particle")     ||
         event->mimeData()->hasFormat("Sound")        ||
         event->mimeData()->hasFormat("Actor"))
      {
         // Move the position of the ghost.
         if (mGhostProxy.valid())
         {
            dtCore::DeltaDrawable* ghostDrawable = NULL;
            mGhostProxy->GetActor(ghostDrawable);
            std::vector<dtCore::DeltaDrawable*> ignoredDrawables;
            ignoredDrawables.push_back(ghostDrawable);
            osg::Vec3 position;

            if (!getPickPosition(event->pos().x(), event->pos().y(), position, ignoredDrawables))
            {
               // If we get here, it means our mouse position did not collide with an object to attach to.

               // Get the current position and direction the camera is facing.
               osg::Vec3 pos = mCamera->getPosition();
               osg::Vec3 viewDir = mCamera->getViewDir();

               const osg::BoundingSphere& bs = mGhostProxy->GetActor()->GetOSGNode()->getBound();

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

            dtDAL::TransformableActorProxy* tProxy =
               dynamic_cast<dtDAL::TransformableActorProxy*>(mGhostProxy.get());

            if (tProxy)
            {
               tProxy->SetTranslation(position);
            }

            dtDAL::ActorProxyIcon* billBoard = mGhostProxy->GetBillBoardIcon();
            if (billBoard)
            {
               billBoard->SetRotation(osg::Matrix::rotate(getCamera()->getOrientation()));
            }

            //ViewportManager::GetInstance().refreshAllViewports();
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
      if (event->mimeData()->hasFormat("Prefab")      ||
         event->mimeData()->hasFormat("StaticMesh")   ||
         event->mimeData()->hasFormat("SkeletalMesh") ||
         event->mimeData()->hasFormat("Particle")     ||
         event->mimeData()->hasFormat("Sound")        ||
         event->mimeData()->hasFormat("Actor"))
      {
         if (mGhostProxy.valid())
         {
            dtCore::RefPtr<dtDAL::Map> mapPtr = EditorData::GetInstance().getCurrentMap();

            // Unroll prefabs
            if (event->mimeData()->hasFormat("Prefab"))
            {
               dtDAL::TransformableActorProxy* tProxy =
                  dynamic_cast<dtDAL::TransformableActorProxy*>(mGhostProxy.get());
               osg::Vec3 offset;
               if (tProxy) offset = tProxy->GetTranslation();

               QByteArray ghostData = event->mimeData()->data("Prefab");
               QDataStream dataStream(&ghostData, QIODevice::ReadOnly);
               QString resourceIdentity;
               dataStream >> resourceIdentity;

               dtDAL::ResourceDescriptor descriptor = dtDAL::ResourceDescriptor(resourceIdentity.toStdString());

               if (mapPtr.valid())
               {
                  std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > proxies;
                  EditorEvents::GetInstance().emitBeginChangeTransaction();
                  dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
                  fileUtils.PushDirectory(dtDAL::Project::GetInstance().GetContext());
                  try
                  {
                     int groupIndex = mapPtr->GetGroupCount();
                     std::string fullPath = dtDAL::Project::GetInstance().GetResourcePath(descriptor);

                     dtCore::RefPtr<dtDAL::MapParser> parser = new dtDAL::MapParser;
                     parser->ParsePrefab(fullPath, proxies);

                     for (int proxyIndex = 0; proxyIndex < (int)proxies.size(); proxyIndex++)
                     {
                        dtDAL::ActorProxy* proxy = proxies[proxyIndex].get();

                        mapPtr->AddProxy(*proxy, true);
                        mapPtr->AddActorToGroup(groupIndex, proxy);

                        tProxy = dynamic_cast<dtDAL::TransformableActorProxy*>(proxy);
                        if (tProxy)
                        {
                           tProxy->SetTranslation(tProxy->GetTranslation() + offset);
                        }

                        // Notify the creation of the proxies.
                        EditorEvents::GetInstance().emitActorProxyCreated(proxy, false);
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
                  EditorEvents::GetInstance().emitActorsSelected(proxies);
                  //ViewportManager::GetInstance().refreshAllViewports();
               }

               event->setDropAction(Qt::MoveAction);
               event->accept();
               return;
            }

            if (mapPtr.valid())
            {
               mapPtr->AddProxy(*mGhostProxy.get(), true);
            }

            // let the world know that a new proxy exists
            EditorEvents::GetInstance().emitBeginChangeTransaction();
            EditorEvents::GetInstance().emitActorProxyCreated(mGhostProxy.get(), false);
            EditorEvents::GetInstance().emitEndChangeTransaction();

            // Now, let the world that it should select the new actor proxy.
            std::vector< dtCore::RefPtr<dtDAL::ActorProxy> > actors;
            actors.push_back(mGhostProxy.get());
            EditorEvents::GetInstance().emitActorsSelected(actors);

            const dtDAL::ActorProxy::RenderMode& renderMode = mGhostProxy->GetRenderMode();
            if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON ||
               renderMode == dtDAL::ActorProxy::RenderMode::DRAW_BILLBOARD_ICON)
            {
               dtDAL::ActorProxyIcon* billBoard = mGhostProxy->GetBillBoardIcon();
               if (billBoard)
               {
                  ViewportManager::GetInstance().getMasterScene()->RemoveDrawable(billBoard->GetDrawable());
               }
            }

            mGhostProxy = NULL;
         }

         event->setDropAction(Qt::MoveAction);
         event->accept();

         //ViewportManager::GetInstance().refreshAllViewports();
      }
      else
      {
         event->ignore();
      }
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
         selectActors(e);
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
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorViewport::onMouseMoveEvent(QMouseEvent* e, float dx, float dy)
   {
      ViewportManager::GetInstance().emitMouseMoveEvent(this, e);

      // Get the position of the mouse.
      osg::Vec2 pos = convertMousePosition(e->pos());

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
         moveCamera(dx, dy);
      }
      else if (getInteractionMode() == Viewport::InteractionMode::ACTOR)
      {
         // If we are holding ALT during the actor movement,
         // then it should copy the current actor(s) and move
         // them instead.
         if (mKeyMods == Qt::AltModifier)
         {
            DuplicateActors();

            // Make sure we only duplicate the actors once.
            mKeyMods = 0x0;
         }

         //ViewportManager::GetInstance().refreshAllViewports();  //causes gizmo manipulations to be slow
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

      return true;
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

      return true;
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

      if (mObjectMotionModel->Update(position) != dtCore::ObjectMotionModel::MOTION_TYPE_MAX)
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

      saveSelectedActorOrigValues(dtDAL::TransformableActorProxy::PROPERTY_TRANSLATION);
      saveSelectedActorOrigValues(dtDAL::TransformableActorProxy::PROPERTY_ROTATION);
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
            dtDAL::ActorProxy* proxy = selection[selectIndex].get();
            if (proxy)
            {
               dtCore::DeltaDrawable* drawable;
               proxy->GetActor(drawable);

               if (drawable)
               {
                  ignoredDrawables.push_back(drawable);
               }
            }
         }

         if (selection.size() > 0)
         {
            dtDAL::TransformableActorProxy* proxy = dynamic_cast<dtDAL::TransformableActorProxy*>(selection[0].get());
            osg::Vec3 position = proxy->GetTranslation();
            position = ViewportManager::GetInstance().GetSnapPosition(position, true, ignoredDrawables);
            osg::Vec3 offset = position - proxy->GetTranslation();

            for (int selectIndex = 0; selectIndex < (int)selection.size(); selectIndex++)
            {
               dtDAL::TransformableActorProxy* proxy = dynamic_cast<dtDAL::TransformableActorProxy*>(selection[selectIndex].get());
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
      updateActorSelectionProperty(dtDAL::TransformableActorProxy::PROPERTY_TRANSLATION);
      updateActorSelectionProperty(dtDAL::TransformableActorProxy::PROPERTY_ROTATION);
      updateActorSelectionProperty("Scale");
      EditorData::GetInstance().getUndoManager().endMultipleUndo();

      EditorEvents::GetInstance().emitEndChangeTransaction();

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool EditorViewport::selectActors(QMouseEvent* e)
   {
      setInteractionMode(Viewport::InteractionMode::NOTHING);
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
         //ViewportManager::GetInstance().refreshAllViewports();

         // Enable the object motion model if you have a selection.
         if (GetEnabled())
         {
            ViewportOverlay* overlay = ViewportManager::GetInstance().getViewportOverlay();
            ViewportOverlay::ActorProxyList selection = overlay->getCurrentActorSelection();

            if (selection.size() > 0)
            {
               mObjectMotionModel->SetEnabled(true);
            }
         }
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

   //////////////////////////////////////////////////////////////////////////////
   bool EditorViewport::DuplicateActors()
   {
      bool overrideDefault = false;
      ViewportManager::GetInstance().emitDuplicateActors(this, &overrideDefault);
      if (overrideDefault)
      {
         return false;
      }

      LOG_INFO("Duplicating current actor selection.");

      // This commits any changes in the property editor.
      PropertyEditor* propEditor = EditorData::GetInstance().getMainWindow()->GetPropertyEditor();
      if(propEditor != NULL)
      {
         propEditor->CommitCurrentEdits();
      }

      ViewportOverlay::ActorProxyList selection = ViewportManager::GetInstance().getViewportOverlay()->getCurrentActorSelection();
      dtCore::RefPtr<dtDAL::Map> currMap = EditorData::GetInstance().getCurrentMap();
      ViewportOverlay* overlay = ViewportManager::GetInstance().getViewportOverlay();

      // Make sure we have valid data.
      if (!currMap.valid())
      {
         LOG_ERROR("Current map is not valid.");
         return true;
      }

      // We're about to do a LOT of work, especially if lots of things are select
      // so, start a change transaction.
      EditorData::GetInstance().getMainWindow()->startWaitCursor();
      EditorEvents::GetInstance().emitBeginChangeTransaction();

      // Once we have a reference to the current selection and the scene,
      // clone each proxy, add it to the scene, make the newly cloned
      // proxy(s) the current selection.
      ViewportOverlay::ActorProxyList::iterator itor, itorEnd;
      itor    = selection.begin();
      itorEnd = selection.end();

      std::map<int, int> groupMap;

      std::vector< dtCore::RefPtr<dtDAL::ActorProxy> > newSelection;
      for (; itor != itorEnd; ++itor)
      {
         dtDAL::ActorProxy* proxy = const_cast<dtDAL::ActorProxy*>(itor->get());
         dtCore::RefPtr<dtDAL::ActorProxy> copy = proxy->Clone();
         if (!copy.valid())
         {
            LOG_ERROR("Error duplicating proxy: " + proxy->GetName());
            continue;
         }

         // Un-highlight the currently selected proxy.
         if (overlay->isActorSelected(proxy))
         {
            overlay->removeActorFromCurrentSelection(proxy);
         }

         newSelection.push_back(copy);

         osg::Vec3 oldPosition;
         // Store the original location of the proxy so we can position after
         // it has been added to the scene.
         dtDAL::TransformableActorProxy* tProxy =
            dynamic_cast<dtDAL::TransformableActorProxy*>(proxy);
         if (tProxy)
         {
            oldPosition = tProxy->GetTranslation();
         }

         // Add the new proxy to the map and send out a create event.
         currMap->AddProxy(*copy, true);

         EditorEvents::GetInstance().emitActorProxyCreated(copy, false);

         // Preserve the group data for new proxies.
         int groupIndex = currMap->FindGroupForActor(proxy);
         if (groupIndex > -1)
         {
            // If we already have this group index mapped, then we have
            // already created a new group for the copied proxies.
            if (groupMap.find(groupIndex) != groupMap.end())
            {
               int newGroup = groupMap[groupIndex];
               currMap->AddActorToGroup(newGroup, copy.get());
            }
            else
            {
               // Create a new group and map it.
               int newGroup = currMap->GetGroupCount();
               currMap->AddActorToGroup(newGroup, copy.get());
               groupMap[groupIndex] = newGroup;
            }
         }

         // Move the newly duplicated actor to where it is supposed to go.
         if (tProxy)
         {
            tProxy->SetTranslation(oldPosition);
         }
      }

      // Finally set the newly cloned proxies to be the current selection.
      //ViewportManager::GetInstance().getViewportOverlay()->setMultiSelectMode(true);
      EditorEvents::GetInstance().emitActorsSelected(newSelection);
      EditorEvents::GetInstance().emitEndChangeTransaction();

      EditorData::GetInstance().getMainWindow()->endWaitCursor();

      return true;
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

      GetObjectMotionModel()->SetEnabled(mEnabled);
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool EditorViewport::GetEnabled() const
   {
      return mEnabled;
   }
} // namespace dtEditQt
