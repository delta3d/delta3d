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

#include <osgUtil/SceneView>

#include <dtCore/scene.h>
#include <dtCore/databasepager.h>
#include <osgDB/DatabasePager>
#include <dtCore/system.h>
#include <dtCore/isector.h>

#include <dtDAL/exceptionenum.h>
#include <dtDAL/map.h>
#include <dtDAL/transformableactorproxy.h>
#include <dtDAL/actorproxyicon.h>

#include <cmath>
#include <sstream>

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
   const Viewport::InteractionMode Viewport::InteractionMode::CAMERA("CAMERA");
   const Viewport::InteractionMode Viewport::InteractionMode::SELECT_ACTOR("SELECT_ACTOR");
   const Viewport::InteractionMode Viewport::InteractionMode::TRANSLATE_ACTOR("TRANSLATE_ACTOR");
   const Viewport::InteractionMode Viewport::InteractionMode::ROTATE_ACTOR("ROTATE_ACTOR");
   const Viewport::InteractionMode Viewport::InteractionMode::SCALE_ACTOR("SCALE_ACTOR");
   ///////////////////////////////////////////////////////////////////////////////


   ///////////////////////////////////////////////////////////////////////////////
   Viewport::Viewport(ViewportManager::ViewportType &type, const std::string &name, QWidget *parent,
         QGLWidget *shareWith) :
      QGLWidget(parent, shareWith), inChangeTransaction(false), name(name), viewPortType(type),
            mRedrawContinuously(false), useAutoInteractionMode(false), autoSceneUpdate(true),
            initialized(false), enableKeyBindings(true), mIsector(new dtCore::Isector())
   {
      this->frameStamp = new osg::FrameStamp();
      this->mouseSensitivity = 10.0f;
      this->interactionMode = &InteractionMode::CAMERA;
      this->renderStyle = &RenderStyle::WIREFRAME;

      this->rootNodeGroup = new osg::Group();
      this->sceneView = new osgUtil::SceneView();

      this->sceneView->setDefaults();
      this->sceneView->setFrameStamp(this->frameStamp.get());
      this->sceneView->setSceneData(this->rootNodeGroup.get());
      setOverlay(ViewportManager::GetInstance().getViewportOverlay());

      setMouseTracking(false);
      this->cacheMouseLocation = true;

      connect(&mTimer, SIGNAL(timeout()), this, SLOT(updateGL()));
      mTimer.setInterval(10);
      //mTimer.setSingleShot(true);
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
      this->initialized = true;
      if (mRedrawContinuously)
         mTimer.start();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::setScene(dtCore::Scene *scene)
   {
      //First, remove the old scene, then add the new one.
      if (this->sceneView.valid())
      {
         if (this->mScene != NULL)
         {
            this->rootNodeGroup->replaceChild(this->mScene->GetSceneNode(), scene->GetSceneNode());
         }
         else
         {
            this->rootNodeGroup->addChild(scene->GetSceneNode());
         }

         this->mScene = scene;
         //this->scene->GetSceneNode()->setStateSet(this->globalStateSet.get());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::setOverlay(ViewportOverlay *overlay)
   {
      if (this->sceneView.valid())
      {
         //If the new overlay is NULL, clear the current overlay.
         if (overlay == NULL)
         {
            this->rootNodeGroup->removeChild(this->overlay->getOverlayGroup());
            this->overlay = NULL;
            return;
         }

         //Else update the current overlay in both the scene and in the viewport.
         if (this->overlay != NULL)
         {
            this->rootNodeGroup->replaceChild(this->overlay->getOverlayGroup(), overlay->getOverlayGroup());
         }
         else
         {
            this->rootNodeGroup->addChild(overlay->getOverlayGroup());
         }

         this->overlay = overlay;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::SetRedrawContinuously(bool contRedraw)
   {
      if (mRedrawContinuously == contRedraw)
         return;

      mRedrawContinuously = contRedraw;
      if (mRedrawContinuously)
         mTimer.start();
      else
         mTimer.stop();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::resizeGL(int width, int height)
   {
      this->sceneView->setViewport(0, 0, width, height);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::paintGL()
   {
      if (!this->sceneView.valid() || !this->mScene.valid() || !this->camera.valid())
         return;

      renderFrame();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::refresh()
   {
      updateGL();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::setClearColor(const osg::Vec4 &color)
   {
      if (clearNode.valid())
         clearNode->setClearColor(color);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::renderFrame()
   {
      getCamera()->update();
      getSceneView()->setProjectionMatrix(getCamera()->getProjectionMatrix());
      getSceneView()->setViewMatrix(getCamera()->getWorldViewMatrix());

      //Make sure the billboards of any actor proxies are oriented towards the
      //camera in this viewport.
      if (getAutoSceneUpdate())
         updateActorProxyBillboards();

      if (ViewportManager::GetInstance().IsPagingEnabled())
      {
         const dtCore::DatabasePager* dbp = ViewportManager::GetInstance().GetDatabasePager();
         if (dbp != NULL)
         {
            dbp->SignalBeginFrame(frameStamp.get());
#if OPENSCENEGRAPH_MAJOR_VERSION < 2 || (OPENSCENEGRAPH_MAJOR_VERSION == 2 && OPENSCENEGRAPH_MINOR_VERSION <= 6)
            dbp->UpdateSceneGraph(frameStamp->getReferenceTime());
#else
            dbp->UpdateSceneGraph(frameStamp);
#endif
         }
      }

      frameStamp->setReferenceTime(osg::Timer::instance()->delta_s(ViewportManager::GetInstance().GetStartTick(), osg::Timer::instance()->tick()));
      frameStamp->setFrameNumber(frameStamp->getFrameNumber()+ 1);

      sceneView->update();
      sceneView->cull();
      sceneView->draw();

      if (ViewportManager::GetInstance().IsPagingEnabled())
      {
         const dtCore::DatabasePager* dbp = ViewportManager::GetInstance().GetDatabasePager();
         if (dbp != NULL)
         {
            dbp->SignalEndFrame();
            //This magic number is the default amount of time that dtCore Scene USED to use.
            double cleanupTime = 0.0025;
            dbp->CompileGLObjects(*sceneView->getState(), cleanupTime);

            sceneView->flushDeletedGLObjects(cleanupTime);
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::setRenderStyle(const RenderStyle &style, bool refreshView)
   {
      int i;
      int numTextureUnits = ViewportManager::GetInstance().getNumTextureUnits();

      this->renderStyle = &style;
      if (!this->sceneView.valid())
         throw dtUtil::Exception(dtDAL::ExceptionEnum::BaseException,"Cannot set render style "
               "because the current scene view is invalid.", __FILE__, __LINE__);

      osg::StateAttribute::GLModeValue turnOn =osg::StateAttribute::OVERRIDE |osg::StateAttribute::ON;
      osg::StateAttribute::GLModeValue turnOff =osg::StateAttribute::OVERRIDE |osg::StateAttribute::OFF;

      osg::PolygonMode *pm = dynamic_cast<osg::PolygonMode *>(
            this->globalStateSet->getAttribute(osg::StateAttribute::POLYGONMODE));

      if (*this->renderStyle == RenderStyle::WIREFRAME)
      {
         for (i=0; i<numTextureUnits; i++)
            this->globalStateSet->setTextureMode(i, GL_TEXTURE_2D, turnOff);
         this->globalStateSet->setMode(GL_LIGHTING,turnOff);
         pm->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
      }
      else if (*this->renderStyle == RenderStyle::TEXTURED)
      {
         for (i=0; i<numTextureUnits; i++)
            this->globalStateSet->removeTextureMode(i, GL_TEXTURE_2D);
         this->globalStateSet->setMode(GL_LIGHTING,turnOff);
         pm->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL);
      }
      else if (*this->renderStyle == RenderStyle::LIT)
      {
         for (i=0; i<numTextureUnits; i++)
            this->globalStateSet->setTextureMode(i, GL_TEXTURE_2D, turnOff);
         this->globalStateSet->setMode(GL_LIGHTING,turnOn);
         pm->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL);
      }
      else if (*this->renderStyle == RenderStyle::LIT_AND_TEXTURED)
      {
         for (i=0; i<numTextureUnits; i++)
            this->globalStateSet->removeTextureMode(i, GL_TEXTURE_2D);
         this->globalStateSet->setMode(GL_LIGHTING,turnOn);
         pm->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL);
      }

      if (refreshView)
      {
         if (!isInitialized())
            throw dtUtil::Exception(dtDAL::ExceptionEnum::BaseException,"Cannot refresh the viewport. "
                  "It has not been initialized.", __FILE__, __LINE__);
         updateGL();
      }

      emit renderStyleChanged();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::pick(int x, int y)
   {
      if (!this->mScene.valid())
         throw dtUtil::Exception(dtDAL::ExceptionEnum::BaseException,
               "Scene is invalid.  Cannot pick objects from an invalid scene.", __FILE__, __LINE__);

      dtCore::RefPtr<dtDAL::Map> currMap = EditorData::GetInstance().getCurrentMap();
      if (!currMap.valid() || getCamera()== NULL)
         return;

      //Before we do any intersection tests, make sure the billboards are updated
      //to reflect their orientation in this viewport.
      getCamera()->update();
      if (getAutoSceneUpdate())
         updateActorProxyBillboards();

      mIsector->Reset();
      mIsector->SetScene( getScene());
      std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > toSelect;
      osg::Vec3 nearPoint, farPoint;
      int yLoc = int(this->sceneView->getViewport()->height()-y);

      this->sceneView->projectWindowXYIntoObject(x, yLoc, nearPoint, farPoint);
      mIsector->SetStartPosition(nearPoint);
      mIsector->SetDirection(farPoint-nearPoint);

      //If we found no intersections no need to continue so emit an empty selection
      //and return.
      if (!mIsector->Update())
      {
         EditorEvents::GetInstance().emitActorsSelected(toSelect);
         return;
      }

      if (mIsector->GetClosestDeltaDrawable()== NULL)
      {
         LOG_ERROR("Intersection query reported an intersection but returned an "
               "invalid DeltaDrawable.");
         return;
      }

      dtCore::DeltaDrawable *drawable = mIsector->GetClosestDeltaDrawable();
      ViewportOverlay *overlay = ViewportManager::GetInstance().getViewportOverlay();
      ViewportOverlay::ActorProxyList &selection = overlay->getCurrentActorSelection();

      //First see if the selected drawable is an actor.
      dtDAL::ActorProxy *newSelection = currMap->GetProxyById(drawable->GetUniqueId());

      //If its not an actor then it may be a billboard placeholder for an actor.
      if (newSelection == NULL)
      {
         const std::map<dtCore::UniqueId, dtCore::RefPtr<dtDAL::ActorProxy> >
               &proxyList =currMap->GetAllProxies();
         std::map<dtCore::UniqueId, dtCore::RefPtr<dtDAL::ActorProxy> >::const_iterator proxyItor;

         //Loop through the proxies searching for the one with billboard geometry
         //matching what was selected.
         for (proxyItor=proxyList.begin(); proxyItor!=proxyList.end(); ++proxyItor)
         {
            dtDAL::ActorProxy *proxy =const_cast<dtDAL::ActorProxy *>(proxyItor->second.get());

            if (proxy->GetRenderMode()== dtDAL::ActorProxy::RenderMode::DRAW_ACTOR)
               continue;

            const dtDAL::ActorProxyIcon *billBoard = proxy->GetBillBoardIcon();
            if (billBoard && billBoard->OwnsDrawable(drawable))
            {
               newSelection = proxy;
               break;
            }
         }
      }

      if (newSelection)
      {
         if (overlay->isActorSelected(newSelection))
            overlay->removeActorFromCurrentSelection(newSelection);
         else
            toSelect.push_back(newSelection);
      }

      //Inform the world what objects were selected and refresh all the viewports
      //affected by the change.  If we are in multi-selection mode (i.e. the control
      //key is pressed) add the current selection to the newly selected proxy.
      if (overlay->getMultiSelectMode())
      {
         ViewportOverlay::ActorProxyList::iterator itor;
         for (itor=selection.begin(); itor!=selection.end(); ++itor)
         {
            toSelect.push_back(const_cast<dtDAL::ActorProxy *>(itor->get()));
         }
      }

      EditorEvents::GetInstance().emitActorsSelected(toSelect);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::onGotoActor(dtCore::RefPtr<dtDAL::ActorProxy> proxy)
   {
      dtDAL::TransformableActorProxy*tProxy = dynamic_cast<dtDAL::TransformableActorProxy *>(proxy.get());

      if (tProxy != NULL && getCamera()!= NULL)
      {
         osg::Vec3 viewDir = getCamera()->getViewDir();

         osg::Vec3 translation = tProxy->GetTranslation();
         const osg::BoundingSphere &bs = tProxy->GetActor()->GetOSGNode()->getBound();
         float offset = (bs.radius() < 1000.0f) ? bs.radius() : 1.0f;
         if (offset <= 0.0f)
            offset = 10.0f;

         getCamera()->setPosition(translation);
         if (this->viewPortType == ViewportManager::ViewportType::PERSPECTIVE)
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
         cam->setPosition(osg::Vec3(x,y,z));
      }

      refresh(); //manually redraw the viewport to show new position
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::trapMouseCursor()
   {
      //Get the current cursor so we can restore it later.
      if (cursor().shape()!= Qt::BlankCursor)
         this->oldMouseCursor = cursor();
      setCursor(QCursor(Qt::BlankCursor));

      //Cache the old mouse location so the cursor doesn't appear to jump when
      //the camera mode operation is complete.
      if (this->cacheMouseLocation)
      {
         this->oldMouseLocation = QCursor::pos();
         this->cacheMouseLocation = false;
      }

      //I disabled this because the mouse move event does this whenever the mouse moves.
      //Commenting this out helps mouse movement work better in Mac OS X.

      //Put the mouse cursor in the center of the viewport.
      QPoint center((x()+width())/2, (y()+height())/2);
      lastMouseUpdateLocation = center;
      QCursor::setPos(mapToGlobal(center));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::releaseMouseCursor(const QPoint &mousePosition)
   {
      setCursor(this->oldMouseCursor);

      if (mousePosition.x()!= -1 && mousePosition.y()!= -1)
         QCursor::setPos(mousePosition);
      else
         QCursor::setPos(this->oldMouseLocation);

      this->cacheMouseLocation = true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::mouseMoveEvent(QMouseEvent *e)
   {
      static bool mouseMoving = false;
      //Moving the mouse back to the center makes the movement recurse
      //so this is a flag to prevent the recursion

      if (mouseMoving)
         return;

      float dx, dy;

      dx = (float)(e->pos().x() - lastMouseUpdateLocation.x());
      dy = (float)(e->pos().y() - lastMouseUpdateLocation.y());

      onMouseMoveEvent(e, dx, dy);

      QPoint center((x()+width())/2, (y()+height())/2);

      float dxCenter = std::abs(float(e->pos().x() - center.x()));
      float dyCenter = std::abs(float(e->pos().y() - center.y()));

      if (dxCenter > (width()/2) || dyCenter > (height()/2))
      {
         //Moving the mouse back to the center makes the movement recurse
         //so this is a flag to prevent the recursion
         mouseMoving = true;
         QCursor::setPos(mapToGlobal(center));
         lastMouseUpdateLocation = center;
         mouseMoving = false;
      }
      else
      {
         lastMouseUpdateLocation.setX(e->pos().x());
         lastMouseUpdateLocation.setY(e->pos().y());
      }

      refresh();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::focusInEvent(QFocusEvent* event)
   {
      QGLWidget::focusInEvent(event);
      mTimer.setInterval(10);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::focusOutEvent(QFocusEvent* event)
   {
      QGLWidget::focusOutEvent(event);
      //One half of a second.
      mTimer.setInterval(500);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::setCameraMode()
   {
      this->interactionMode = &InteractionMode::CAMERA;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::setActorSelectMode()
   {
      this->interactionMode = &InteractionMode::SELECT_ACTOR;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::setActorTranslateMode()
   {
      this->interactionMode = &InteractionMode::TRANSLATE_ACTOR;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::setActorRotateMode()
   {
      this->interactionMode = &InteractionMode::ROTATE_ACTOR;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::setActorScaleMode()
   {
      this->interactionMode = &InteractionMode::SCALE_ACTOR;
   }


   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::connectInteractionModeSlots()
   {
      //Connect the global actions we want to track.
      EditorActions &ga = EditorActions::GetInstance();
      EditorEvents &ge = EditorEvents::GetInstance();

      connect(ga.actionSelectionCamera, SIGNAL(triggered()), this, SLOT(setCameraMode()));
      connect(ga.actionSelectionSelectActor, SIGNAL(triggered()), this, SLOT(setActorSelectMode()));
      connect(ga.actionSelectionTranslateActor, SIGNAL(triggered()), this, SLOT(setActorTranslateMode()));
      connect(ga.actionSelectionRotateActor, SIGNAL(triggered()), this, SLOT(setActorRotateMode()));
      connect(ga.actionSelectionScaleActor, SIGNAL(triggered()), this, SLOT(setActorScaleMode()));

      connect(&ge, SIGNAL(gotoActor(ActorProxyRefPtr)), this, SLOT(onGotoActor(ActorProxyRefPtr)));
      connect(&ge, SIGNAL(gotoPosition(double,double,double)), this, SLOT(onGotoPosition(double,double,double)));
      connect(&ge, SIGNAL(beginChangeTransaction()), this, SLOT(onBeginChangeTransaction()));
      connect(&ge, SIGNAL(endChangeTransaction()), this, SLOT(onEndChangeTransaction()));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::disconnectInteractionModeSlots()
   {
      //Disconnect from all our global actions we were previously tracking.
      EditorActions &ga = EditorActions::GetInstance();
      EditorEvents &ge = EditorEvents::GetInstance();

      disconnect(ga.actionSelectionCamera, SIGNAL(triggered()), this, SLOT(setCameraMode()));
      disconnect(ga.actionSelectionSelectActor, SIGNAL(triggered()), this, SLOT(setActorSelectMode()));
      disconnect(ga.actionSelectionTranslateActor, SIGNAL(triggered()), this, SLOT(setActorTranslateMode()));
      disconnect(ga.actionSelectionRotateActor, SIGNAL(triggered()), this, SLOT(setActorRotateMode()));
      disconnect(ga.actionSelectionScaleActor, SIGNAL(triggered()), this, SLOT(setActorScaleMode()));

      disconnect(&ge, SIGNAL(gotoActor(ActorProxyRefPtr)), this, SLOT(onGotoActor(ActorProxyRefPtr)));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::syncWithModeActions()
   {
      if (this->useAutoInteractionMode)
      {
         QAction *action = EditorActions::GetInstance().modeToolsGroup->checkedAction();
         if (action == EditorActions::GetInstance().actionSelectionCamera)
            setCameraMode();
         else if (action == EditorActions::GetInstance().actionSelectionSelectActor)
            setActorSelectMode();
         else if (action == EditorActions::GetInstance().actionSelectionTranslateActor)
            setActorTranslateMode();
         else if (action == EditorActions::GetInstance().actionSelectionRotateActor)
            setActorRotateMode();
         else if (action == EditorActions::GetInstance().actionSelectionScaleActor)
            setActorScaleMode();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::saveSelectedActorOrigValues(const std::string &propName)
   {
      ViewportOverlay::ActorProxyList &selection =ViewportManager::GetInstance().getViewportOverlay()->getCurrentActorSelection();
      ViewportOverlay::ActorProxyList::iterator itor;

      //Clear the old list first.
      this->selectedActorOrigValues.erase(propName);
      std::vector<std::string> savedValues;

      for (itor=selection.begin(); itor!=selection.end(); ++itor)
      {
         dtDAL::ActorProxy *proxy = const_cast<dtDAL::ActorProxy *>(itor->get());
         dtDAL::ActorProperty *prop = proxy->GetProperty(propName);

         if (prop != NULL)
         {
            std::string oldValue = prop->ToString();
            savedValues.push_back(oldValue);
         }
      }

      this->selectedActorOrigValues[propName] = savedValues;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::updateActorSelectionProperty(const std::string &propName)
   {
      ViewportOverlay::ActorProxyList &selection =ViewportManager::GetInstance().getViewportOverlay()->getCurrentActorSelection();
      ViewportOverlay::ActorProxyList::iterator itor;
      std::map<std::string,std::vector<std::string> >::iterator
            saveEntry =this->selectedActorOrigValues.find(propName);
      int oldValueIndex = 0;

      //Make sure we actually saved values for this property.
      if (saveEntry == this->selectedActorOrigValues.end())
         return;

      for (itor=selection.begin(); itor!=selection.end(); ++itor)
      {
         dtDAL::ActorProxy *proxy = const_cast<dtDAL::ActorProxy *>(itor->get());
         dtDAL::ActorProperty *prop = proxy->GetProperty(propName);

         if (prop != NULL)
         {
            // emit the old value before the change so undo/redo can recover.
            std::string oldValue = saveEntry->second[oldValueIndex];
            std::string newValue = prop->ToString();
            EditorEvents::GetInstance().emitActorPropertyAboutToChange(proxy, prop, oldValue, newValue);
            oldValueIndex++;

            EditorEvents::GetInstance().emitActorPropertyChanged(proxy, prop);
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::updateActorProxyBillboards()
   {
      dtDAL::Map *currentMap = EditorData::GetInstance().getCurrentMap();
      std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > proxies;
      std::vector<dtCore::RefPtr<dtDAL::ActorProxy> >::iterator itor;

      if (currentMap == NULL || getCamera()== NULL)
         return;

      currentMap->GetAllProxies(proxies);
      for (itor=proxies.begin(); itor!=proxies.end(); ++itor)
      {
         dtDAL::ActorProxy *proxy = itor->get();
         const dtDAL::ActorProxy::RenderMode &renderMode = proxy->GetRenderMode();

         if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON ||renderMode == dtDAL::ActorProxy::RenderMode::DRAW_BILLBOARD_ICON)
         {
            dtDAL::ActorProxyIcon *billBoard = proxy->GetBillBoardIcon();
            if (billBoard != NULL)
               billBoard->SetRotation(osg::Matrix::rotate(getCamera()->getOrientation().inverse()));
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
         if (this->useAutoInteractionMode)
            return; //Already on, so do nothing.
         else
         {
            this->useAutoInteractionMode = true;
            connectInteractionModeSlots();
         }
      }
      else
      {
         if (!this->useAutoInteractionMode)
            return; //Already off, so do nothing.
         else
         {
            this->useAutoInteractionMode = false;
            disconnectInteractionModeSlots();
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::setupInitialRenderState()
   {
      //Some actors in the scene may have a clear node that disables screen clears
      //each frame. (The skybox for example).  Therefore, we add this node to the
      //scene to ensure a clear happens if needed.
      this->clearNode = new osg::ClearNode;
      this->clearNode->setRequiresClear(true);
      this->clearNode->setClearColor(osg::Vec4(0.2f, 0.2f, 0.4f, 1.0f));
      osg::Group *group = getSceneView()->getSceneData()->asGroup();
      if (group != NULL)
         group->addChild(this->clearNode.get());

      //Sets up the global state set used to render the viewport's contents.
      //This also sets up some default modes which are shared amoung
      //all viewports.
      this->globalStateSet = new osg::StateSet();

      osg::AlphaFunc *alphafunc = new osg::AlphaFunc;
      alphafunc->setFunction(osg::AlphaFunc::GREATER, 0.0f);
      osg::PolygonMode *pm = new osg::PolygonMode();
      pm->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);

      osg::CullFace *cf = new osg::CullFace();
      cf->setMode(osg::CullFace::BACK);

      this->globalStateSet->setGlobalDefaults();
      this->globalStateSet->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
      this->globalStateSet->setAttributeAndModes(alphafunc, osg::StateAttribute::ON);
      this->globalStateSet->setAttributeAndModes(pm, osg::StateAttribute::ON);
      this->globalStateSet->setAttributeAndModes(cf, osg::StateAttribute::ON);

      this->sceneView->setGlobalStateSet(this->globalStateSet.get());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::onBeginChangeTransaction()
   {
      inChangeTransaction = true;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Viewport::onEndChangeTransaction()
   {
      inChangeTransaction = false;
      ViewportManager::GetInstance().refreshAllViewports();
   }

}