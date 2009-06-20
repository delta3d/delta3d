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
#include <dtActors/prefabactorproxy.h>
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/perspectiveviewport.h>
#include <dtEditQt/viewportoverlay.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editoractions.h>
#include <dtEditQt/propertyeditor.h>
#include <dtDAL/transformableactorproxy.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/map.h>
#include <dtDAL/librarymanager.h>
#include <dtDAL/actorproxyicon.h>
#include <dtCore/isector.h>
#include <dtUtil/exception.h>
#include <dtDAL/exceptionenum.h>
#include <QtGui/QDrag>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragLeaveEvent>
#include <dtDAL/transformableactorproxy.h>
#include <dtDAL/mapxml.h>
#include <dtDAL/project.h>

#include <osg/PolygonMode>
#include <osg/BlendFunc>


namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   EditorViewport::EditorViewport(ViewportManager::ViewportType& type,
      const std::string& name, QWidget* parent,
      QGLWidget* shareWith)
      : Viewport(type, name, parent, shareWith)
      , mObjectMotionModel(NULL)
      , mGhostProxy(NULL)
      , mSkipUpdateForCam(false)
   {
      mObjectMotionModel = new STAGEObjectMotionModel(ViewportManager::GetInstance().getMasterView());
      mObjectMotionModel->SetEnabled(false);
      mObjectMotionModel->ClearTargets();
      mObjectMotionModel->SetGetMouseLineFunc(dtDAL::MakeFunctor(*this, &EditorViewport::GetMouseLine));
      mObjectMotionModel->SetObjectToScreenFunc(dtDAL::MakeFunctorRet(*this, &EditorViewport::GetObjectScreenCoordinates));
      mObjectMotionModel->SetScale(1.5f);

      setAcceptDrops(true);
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

      osg::Group* node = getSceneView()->getSceneData()->asGroup();
      if (node)
      {
         if (mObjectMotionModel.valid())
         {
            mObjectMotionModel->SetSceneNode(node);

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
            ViewportManager::GetInstance().refreshAllViewports();
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

      ViewportManager::GetInstance().refreshAllViewports();
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

            osg::Vec3 position;

            if (!getPickPosition(event->pos().x(), event->pos().y(), position, ghostDrawable))
            {
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
               billBoard->SetRotation(osg::Matrix::rotate(getCamera()->getOrientation().inverse()));
            }

            ViewportManager::GetInstance().refreshAllViewports();
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

                        mapPtr->AddProxy(*proxy);
                        mapPtr->AddActorToGroup(groupIndex, proxy);

                        // Notify the creation of the proxies.
                        EditorEvents::GetInstance().emitActorProxyCreated(proxy, false);

                        tProxy = dynamic_cast<dtDAL::TransformableActorProxy*>(proxy);
                        if (tProxy)
                        {
                           tProxy->SetTranslation(tProxy->GetTranslation() + offset);
                        }
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
                  ViewportManager::GetInstance().refreshAllViewports();
               }

               event->setDropAction(Qt::MoveAction);
               event->accept();
               return;
            }

            if (mapPtr.valid())
            {
               mapPtr->AddProxy(*mGhostProxy.get());
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

         ViewportManager::GetInstance().refreshAllViewports();
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

      setFocus();

      //bool isHandled = false;
      //ViewportManager::GetInstance().emitMousePressEvent(this, e, &isHandled);
      //if (isHandled)
      //{
      //   return;
      //}

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

      //bool isHandled = false;
      //ViewportManager::GetInstance().emitMouseReleaseEvent(this, e, &isHandled);
      //if (isHandled)
      //{
      //   return;
      //}

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
      //bool isHandled = false;
      //ViewportManager::GetInstance().emitMouseMoveEvent(this, e, dx, dy, &isHandled);
      //if (isHandled)
      //{
      //   return;
      //}

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

      default:
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

      default:
      break;
      }

      EditorEvents::GetInstance().emitEndChangeTransaction();
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

   //////////////////////////////////////////////////////////////////////////////
   void EditorViewport::DuplicateActors()
   {
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
         return;
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
         currMap->AddProxy(*copy);

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
   }
} // namespace dtEditQt
