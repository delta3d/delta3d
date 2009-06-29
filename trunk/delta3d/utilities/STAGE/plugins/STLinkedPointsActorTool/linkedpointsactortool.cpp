#include "linkedpointsactortool.h"
#include "ui_linkedpointsactortool.h"

#include <dtCore/deltadrawable.h>
#include <dtCore/transformable.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/librarymanager.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/pluginmanager.h>
#include <dtEditQt/viewportmanager.h>
#include <dtEditQt/viewportoverlay.h>
#include <dtEditQt/viewport.h>
#include <dtEditQt/editorviewport.h>
#include <dtEditQt/editorviewportcontainer.h>
#include <dtEditQt/perspectiveviewport.h>
#include <dtEditQt/orthoviewport.h>
#include <dtEditQt/stageobjectmotionmodel.h>
#include <dtEditQt/stagecamera.h>
#include <dtUtil/fileutils.h>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QMouseEvent>
#include <QtGui/QToolBar>


const std::string LinkedPointsActorToolPlugin::PLUGIN_NAME = "Linked Points Actor Tool";
const std::string LinkedPointsActorToolPlugin::MODE_ICON   = ":/icons/mode.png";


////////////////////////////////////////////////////////////////////////////////
LinkedPointsActorToolPlugin::LinkedPointsActorToolPlugin(MainWindow* mw)
   : mMainWindow(mw)
   , mActiveProxy(NULL)
   , mActiveActor(NULL)
   , mIsDocked(false)
   , mIsActive(false)
   , mIsInActorMode(false)
   , mIsInCameraMode(false)
   , mShowingPlacementGhost(false)
   , mCurrentPoint(0)
{
   // apply layout made with QT designer
   Ui_LinkedPointsActorTool ui;
   ui.setupUi(this);

   mModeButton = new QAction(QIcon(MODE_ICON.c_str()), "Linked Points Actor Tool", this);
   mMainWindow->AddExclusiveToolMode(mModeButton);

   mCreationModeCheckbox = ui.mCreationModeCheckbox;
   mDeleteLinkButton     = ui.mDeleteLinkButton;
   mFinishedButton       = ui.mFinishedButton;

   // Setup our signal slots.
   connect(&EditorEvents::GetInstance(), SIGNAL(actorProxyCreated(ActorProxyRefPtr, bool)),
      this, SLOT(onActorProxyCreated(ActorProxyRefPtr, bool)));
   connect(&EditorEvents::GetInstance(), SIGNAL(actorProxyDestroyed(ActorProxyRefPtr)),
      this, SLOT(onActorProxyDestroyed(ActorProxyRefPtr)));
   connect(&EditorEvents::GetInstance(), SIGNAL(selectedActors(ActorProxyRefPtrVector &)),
      this, SLOT(onActorsSelected(ActorProxyRefPtrVector &)));

   connect(&ViewportManager::GetInstance(), SIGNAL(mousePressEvent(Viewport*, QMouseEvent*)),
      this, SLOT(onMousePressEvent(Viewport*, QMouseEvent*)));
   connect(&ViewportManager::GetInstance(), SIGNAL(mouseReleaseEvent(Viewport*, QMouseEvent*)),
      this, SLOT(onMouseReleaseEvent(Viewport*, QMouseEvent*)));
   connect(&ViewportManager::GetInstance(), SIGNAL(mouseMoveEvent(Viewport*, QMouseEvent*)),
      this, SLOT(onMouseMoveEvent(Viewport*, QMouseEvent*)));
   connect(&ViewportManager::GetInstance(), SIGNAL(shouldBeginActorMode(Viewport*, osg::Vec2, bool*, bool*)),
      this, SLOT(onShouldBeginActorMode(Viewport*, osg::Vec2, bool*, bool*)));

   connect(&ViewportManager::GetInstance(), SIGNAL(beginCameraMode(Viewport*, QMouseEvent*, bool*)),
      this, SLOT(onBeginCameraMode(Viewport*, QMouseEvent*, bool*)));
   connect(&ViewportManager::GetInstance(), SIGNAL(endCameraMode(Viewport*, QMouseEvent*, bool*)),
      this, SLOT(onEndCameraMode(Viewport*, QMouseEvent*, bool*)));
   connect(&ViewportManager::GetInstance(), SIGNAL(beginActorMode(Viewport*, QMouseEvent*, bool*)),
      this, SLOT(onBeginActorMode(Viewport*, QMouseEvent*, bool*)));
   connect(&ViewportManager::GetInstance(), SIGNAL(endActorMode(Viewport*, QMouseEvent*, bool*)),
      this, SLOT(onEndActorMode(Viewport*, QMouseEvent*, bool*)));
   connect(&ViewportManager::GetInstance(), SIGNAL(selectActors(Viewport*, QMouseEvent*, bool*)),
      this, SLOT(onSelectActors(Viewport*, QMouseEvent*, bool*)));
   connect(&ViewportManager::GetInstance(), SIGNAL(duplicateActors(Viewport*, bool*)),
      this, SLOT(onDuplicateActors(Viewport*, bool*)));

   connect(&EditorEvents::GetInstance(), SIGNAL(editorPreferencesChanged()),
      this, SLOT(onEditorPreferencesChanged()));
   connect(&ViewportManager::GetInstance(), SIGNAL(setSnapTranslation(float)),
      this, SLOT(onSetSnapTranslation(float)));
   connect(&ViewportManager::GetInstance(), SIGNAL(setSnapRotation(float)),
      this, SLOT(onSetSnapRotation(float)));
   connect(&ViewportManager::GetInstance(), SIGNAL(setSnapScale(float)),
      this, SLOT(onSetSnapScale(float)));
   connect(&ViewportManager::GetInstance(), SIGNAL(setSnapEnabled(bool, bool, bool)),
      this, SLOT(onSetSnapEnabled(bool, bool, bool)));

   connect(mModeButton, SIGNAL(changed()), this, SLOT(onModeButtonPressed()));
   connect(mCreationModeCheckbox, SIGNAL(clicked()), this, SLOT(onCreationModePressed()));
   connect(ui.mDeleteLinkButton, SIGNAL(clicked()), this, SLOT(onDeleteLinkPointPressed()));
   connect(ui.mFinishedButton, SIGNAL(clicked()), this, SLOT(onFinishedButtonPressed()));

   InitializeMotionModels();
}

////////////////////////////////////////////////////////////////////////////////
LinkedPointsActorToolPlugin::~LinkedPointsActorToolPlugin()
{
}

////////////////////////////////////////////////////////////////////////////////
void LinkedPointsActorToolPlugin::InitializeMotionModels()
{
   mPerspMotionModel = new ToolObjectMotionModel(ViewportManager::GetInstance().getMasterView());
   mPerspMotionModel->SetEnabled(false);
   mPerspMotionModel->SetGetMouseLineFunc(dtDAL::MakeFunctor(*mMainWindow->GetPerspView(), &EditorViewport::GetMouseLine));
   mPerspMotionModel->SetObjectToScreenFunc(dtDAL::MakeFunctorRet(*mMainWindow->GetPerspView(), &EditorViewport::GetObjectScreenCoordinates));
   mPerspMotionModel->SetScale(1.0f);
   osg::Group* node = mMainWindow->GetPerspView()->getSceneView()->getSceneData()->asGroup();
   if (node)
   {
      mPerspMotionModel->SetSceneNode(node);
      mPerspMotionModel->SetCamera(mMainWindow->GetPerspView()->getCamera()->getDeltaCamera());
   }

   mTopMotionModel = new ToolObjectMotionModel(ViewportManager::GetInstance().getMasterView());
   mTopMotionModel->SetEnabled(false);
   mTopMotionModel->SetGetMouseLineFunc(dtDAL::MakeFunctor(*mMainWindow->GetTopView(), &EditorViewport::GetMouseLine));
   mTopMotionModel->SetObjectToScreenFunc(dtDAL::MakeFunctorRet(*mMainWindow->GetTopView(), &EditorViewport::GetObjectScreenCoordinates));
   mTopMotionModel->SetAutoScaleEnabled(false);
   mTopMotionModel->SetScale(300.0f / mMainWindow->GetTopView()->getCamera()->getZoom());
   node = mMainWindow->GetTopView()->getSceneView()->getSceneData()->asGroup();
   if (node)
   {
      mTopMotionModel->SetSceneNode(node);
      mTopMotionModel->SetCamera(mMainWindow->GetTopView()->getCamera()->getDeltaCamera());
   }

   mSideMotionModel = new ToolObjectMotionModel(ViewportManager::GetInstance().getMasterView());
   mSideMotionModel->SetEnabled(false);
   mSideMotionModel->SetGetMouseLineFunc(dtDAL::MakeFunctor(*mMainWindow->GetSideView(), &EditorViewport::GetMouseLine));
   mSideMotionModel->SetObjectToScreenFunc(dtDAL::MakeFunctorRet(*mMainWindow->GetSideView(), &EditorViewport::GetObjectScreenCoordinates));
   mSideMotionModel->SetAutoScaleEnabled(false);
   mSideMotionModel->SetScale(300.0f / mMainWindow->GetSideView()->getCamera()->getZoom());
   node = mMainWindow->GetSideView()->getSceneView()->getSceneData()->asGroup();
   if (node)
   {
      mSideMotionModel->SetSceneNode(node);
      mSideMotionModel->SetCamera(mMainWindow->GetSideView()->getCamera()->getDeltaCamera());
   }

   mFrontMotionModel = new ToolObjectMotionModel(ViewportManager::GetInstance().getMasterView());
   mFrontMotionModel->SetEnabled(false);
   mFrontMotionModel->SetGetMouseLineFunc(dtDAL::MakeFunctor(*mMainWindow->GetFrontView(), &EditorViewport::GetMouseLine));
   mFrontMotionModel->SetObjectToScreenFunc(dtDAL::MakeFunctorRet(*mMainWindow->GetFrontView(), &EditorViewport::GetObjectScreenCoordinates));
   mFrontMotionModel->SetAutoScaleEnabled(false);
   mFrontMotionModel->SetScale(300.0f / mMainWindow->GetFrontView()->getCamera()->getZoom());
   node = mMainWindow->GetFrontView()->getSceneView()->getSceneData()->asGroup();
   if (node)
   {
      mFrontMotionModel->SetSceneNode(node);
      mFrontMotionModel->SetCamera(mMainWindow->GetFrontView()->getCamera()->getDeltaCamera());
   }

   mMainWindow->GetViewContainer()->updateSnaps();
}

////////////////////////////////////////////////////////////////////////////////
void LinkedPointsActorToolPlugin::Destroy()
{
   HidePlacementGhost();
   mMainWindow->RemoveExclusiveToolMode(mModeButton);
   mModeButton = NULL;
   mMainWindow->removeDockWidget(this);
}

////////////////////////////////////////////////////////////////////////////////
void LinkedPointsActorToolPlugin::closeEvent(QCloseEvent* event)
{
   mMainWindow->SetNormalToolMode();
   shutdown();
}

////////////////////////////////////////////////////////////////////////////////
void LinkedPointsActorToolPlugin::onActorProxyCreated(ActorProxyRefPtr proxy, bool forceNoAdjustments)
{
   dtActors::LinkedPointsActorProxy* linkedPointsProxy = dynamic_cast<dtActors::LinkedPointsActorProxy*>(proxy.get());

   if (linkedPointsProxy)
   {
      HidePlacementGhost();
      // If the new proxy only has one point, then auto enable the creation tool.
      if (linkedPointsProxy->GetPointArray().size() == 1)
      {
         mCreationModeCheckbox->setChecked(true);
         mModeButton->setChecked(true);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void LinkedPointsActorToolPlugin::onActorProxyDestroyed(ActorProxyRefPtr proxy)
{
   //if (proxy.get() == mActiveProxy.get())
   //{
   //   shutdown();
   //}
}

////////////////////////////////////////////////////////////////////////////////
void LinkedPointsActorToolPlugin::onActorsSelected(ActorProxyRefPtrVector& actors)
{
   // We can only use this editor if the linked points actor is the only one selected.
   if (actors.size() == 1)
   {
      // Only allow linked points actor.
      dtActors::LinkedPointsActorProxy* linkedPointsProxy = dynamic_cast<dtActors::LinkedPointsActorProxy*>(actors[0].get());
      if (linkedPointsProxy)
      {
         initialize(linkedPointsProxy);
         return;
      }
   }

   shutdown();
   mActiveProxy = NULL;
   mActiveActor = NULL;

   if (actors.size() == 0)
   {
      mMainWindow->GetPerspView()->GetObjectMotionModel()->SetEnabled(false);
      mMainWindow->GetTopView()->GetObjectMotionModel()->SetEnabled(false);
      mMainWindow->GetSideView()->GetObjectMotionModel()->SetEnabled(false);
      mMainWindow->GetFrontView()->GetObjectMotionModel()->SetEnabled(false);
   }
}

////////////////////////////////////////////////////////////////////////////////
void LinkedPointsActorToolPlugin::onMousePressEvent(Viewport* vp, QMouseEvent* e)
{
   if (!mIsActive)
   {
      return;
   }

   ToolObjectMotionModel* motion = GetMotionModelForView(vp);

   if (motion)
   {
      if (e->button() == Qt::LeftButton)
      {
         motion->OnLeftMousePressed();
      }
      else if (e->button() == Qt::RightButton)
      {
         motion->OnRightMousePressed();
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void LinkedPointsActorToolPlugin::onMouseReleaseEvent(Viewport* vp, QMouseEvent* e)
{
   if (!mIsActive)
   {
      return;
   }

   ToolObjectMotionModel* motion = GetMotionModelForView(vp);

   if (motion)
   {
      if (e->button() == Qt::LeftButton)
      {
         motion->OnLeftMouseReleased();
      }
      else if (e->button() == Qt::RightButton)
      {
         motion->OnRightMouseReleased();
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void LinkedPointsActorToolPlugin::onMouseMoveEvent(Viewport* vp, QMouseEvent* e)
{
   if (!mIsActive)
   {
      return;
   }

   EditorViewport* editorView = dynamic_cast<EditorViewport*>(vp);
   if (editorView)
   {
      // Get the position of the mouse.
      osg::Vec2 pos = editorView->convertMousePosition(e->pos());

      // Update the object motion model mouse position.
      ToolObjectMotionModel* motion = GetMotionModelForView(vp);
      if (motion)
      {
         bool refresh = false;

         if (motion->Update(pos) != ToolObjectMotionModel::MOTION_TYPE_MAX)
         {
            HidePlacementGhost();
         }
         else
         {
            UpdatePlacementGhost(vp, osg::Vec2(e->pos().x(), e->pos().y()));
            refresh = true;
         }

         // Update ortho view motion model scales.
         if (mIsInCameraMode && motion != mPerspMotionModel.get())
         {
            motion->SetScale(300.0f / editorView->getCamera()->getZoom());
            refresh = true;
         }
         else if (mIsInActorMode)
         {
            // Visualize the updated actor.
            mActiveActor->Visualize(mCurrentPoint);
            mActiveActor->Visualize(mCurrentPoint - 1);
         }

         if (refresh)
         {
            motion->UpdateWidgets();
            //ViewportManager::GetInstance().refreshAllViewports();
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void LinkedPointsActorToolPlugin::onShouldBeginActorMode(Viewport* vp, osg::Vec2 position, bool* overrideDefault, bool* result)
{
   if (mIsActive)
   {
      EditorViewport* editorView = dynamic_cast<EditorViewport*>(vp);

      if (editorView)
      {
         ToolObjectMotionModel* motion = GetMotionModelForView(vp);
         if (motion)
         {
            if (motion->Update(position) != ToolObjectMotionModel::MOTION_TYPE_MAX)
            {
               if (overrideDefault)
               {
                  *overrideDefault = true;
               }

               if (result)
               {
                  *result = true;
               }
            }
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void LinkedPointsActorToolPlugin::onBeginCameraMode(Viewport* vp, QMouseEvent* e, bool* overrideDefault)
{
   HidePlacementGhost();
   mIsInCameraMode = true;
}

////////////////////////////////////////////////////////////////////////////////
void LinkedPointsActorToolPlugin::onEndCameraMode(Viewport* vp, QMouseEvent* e, bool* overrideDefault)
{
   mIsInCameraMode = false;
}

////////////////////////////////////////////////////////////////////////////////
void LinkedPointsActorToolPlugin::onBeginActorMode(Viewport* vp, QMouseEvent* e, bool* overrideDefault)
{
   // We only do something if we have an active proxy.
   if (mIsActive)
   {
      EditorViewport* editorView = dynamic_cast<EditorViewport*>(vp);
      if (!editorView)
      {
         return;
      }

      mIsInActorMode = true;
   }
}

////////////////////////////////////////////////////////////////////////////////
void LinkedPointsActorToolPlugin::onEndActorMode(Viewport* vp, QMouseEvent* e, bool* overrideDefault)
{
   // We only do something if we have an active proxy.
   if (mIsActive)
   {
      EditorViewport* editorView = dynamic_cast<EditorViewport*>(vp);
      if (!editorView)
      {
         return;
      }

      mIsInActorMode = false;
   }
}

////////////////////////////////////////////////////////////////////////////////
void LinkedPointsActorToolPlugin::onSelectActors(Viewport* vp, QMouseEvent* e, bool* overrideDefault)
{
   // We only do something if we have an active proxy.
   if (mIsActive)
   {
      EditorViewport* editorView = dynamic_cast<EditorViewport*>(vp);
      if (!editorView)
      {
         return;
      }

      // You can not select other actors while in this tool, you can only
      // select the points on the current active proxy or create new points
      // in the chain.
      if (editorView->GetMouseButton() == Qt::LeftButton)
      {
         // Always only allow a single selection.
         ViewportManager::GetInstance().getViewportOverlay()->setMultiSelectMode(false);

         // Since we have the ghost object, just let that ghost object be used as the new point
         // and create a new ghost object.
         if (mShowingPlacementGhost)
         {
            mShowingPlacementGhost = false;
            *overrideDefault = true;
         }
         else
         {
            // Only override the selection if we are not selecting a specific point.
            if (selectDrawable(editorView->getPickDrawable(e->pos().x(), e->pos().y())))
            {
               *overrideDefault = true;
            }
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void LinkedPointsActorToolPlugin::onDuplicateActors(Viewport* vp, bool* overrideDefault)
{
   // We only do something if we have an active proxy.
   if (mIsActive)
   {
      EditorViewport* editorView = dynamic_cast<EditorViewport*>(vp);
      if (!editorView)
      {
         return;
      }

      if (overrideDefault)
      {
         *overrideDefault = true;
      }

      // You can only duplicate points in the active proxy.
   }
}

////////////////////////////////////////////////////////////////////////////////
void LinkedPointsActorToolPlugin::onEditorPreferencesChanged()
{
   bool useGlobalOrientation = EditorData::GetInstance().GetUseGlobalOrientationForViewportWidget();

   dtCore::ObjectMotionModel::CoordinateSpace coordinateSpace = dtCore::ObjectMotionModel::LOCAL_SPACE;
   // sync up our local vs world space setting. Affects the actor movement/rotation widget in the viewports
   if (useGlobalOrientation)
   {
      coordinateSpace = dtCore::ObjectMotionModel::WORLD_SPACE;
   }

   mPerspMotionModel->SetCoordinateSpace(coordinateSpace);
   mTopMotionModel->SetCoordinateSpace(coordinateSpace);
   mSideMotionModel->SetCoordinateSpace(coordinateSpace);
   mFrontMotionModel->SetCoordinateSpace(coordinateSpace);

   mPerspMotionModel->UpdateWidgets();
   mTopMotionModel->UpdateWidgets();
   mSideMotionModel->UpdateWidgets();
   mFrontMotionModel->UpdateWidgets();

   //ViewportManager::GetInstance().refreshAllViewports();
}

////////////////////////////////////////////////////////////////////////////////
void LinkedPointsActorToolPlugin::onSetSnapTranslation(float increment)
{
   mPerspMotionModel->SetSnapTranslation(increment);
   mTopMotionModel->SetSnapTranslation(increment);
   mSideMotionModel->SetSnapTranslation(increment);
   mFrontMotionModel->SetSnapTranslation(increment);
}

////////////////////////////////////////////////////////////////////////////////
void LinkedPointsActorToolPlugin::onSetSnapRotation(float increment)
{
   mPerspMotionModel->SetSnapRotation(increment);
   mTopMotionModel->SetSnapRotation(increment);
   mSideMotionModel->SetSnapRotation(increment);
   mFrontMotionModel->SetSnapRotation(increment);
}

////////////////////////////////////////////////////////////////////////////////
void LinkedPointsActorToolPlugin::onSetSnapScale(float increment)
{
   mPerspMotionModel->SetSnapScale(increment);
   mTopMotionModel->SetSnapScale(increment);
   mSideMotionModel->SetSnapScale(increment);
   mFrontMotionModel->SetSnapScale(increment);
}

////////////////////////////////////////////////////////////////////////////////
void LinkedPointsActorToolPlugin::onSetSnapEnabled(bool translation, bool rotation, bool scale)
{
   mPerspMotionModel->SetSnapEnabled(translation, rotation, scale);
   mTopMotionModel->SetSnapEnabled(translation, rotation, scale);
   mPerspMotionModel->SetSnapEnabled(translation, rotation, scale);
   mPerspMotionModel->SetSnapEnabled(translation, rotation, scale);
}

////////////////////////////////////////////////////////////////////////////////
void LinkedPointsActorToolPlugin::onModeButtonPressed()
{
   if (mModeButton->isChecked())
   {
      if (mActiveProxy.valid())
      {
         initialize(mActiveProxy.get());
      }
   }
   else
   {
      onFinishedButtonPressed();
   }
}

////////////////////////////////////////////////////////////////////////////////
void LinkedPointsActorToolPlugin::onCreationModePressed()
{
   if (mCreationModeCheckbox->isChecked())
   {
      mPerspMotionModel->SetEnabled(false);
      mTopMotionModel->SetEnabled(false);
      mSideMotionModel->SetEnabled(false);
      mFrontMotionModel->SetEnabled(false);
      ViewportManager::GetInstance().refreshAllViewports();
   }
   else
   {
      HidePlacementGhost(true);
   }
}

////////////////////////////////////////////////////////////////////////////////
void LinkedPointsActorToolPlugin::onDeleteLinkPointPressed()
{
   HidePlacementGhost();
   if (mIsActive && mActiveActor && mActiveActor->GetPointCount() > 1)
   {
      mActiveActor->RemovePoint(mCurrentPoint);

      if (mCurrentPoint > 0)
      {
         mCurrentPoint--;
      }

      if (!selectDrawable(mActiveActor->GetPointDrawable(mCurrentPoint)))
      {
         ViewportManager::GetInstance().refreshAllViewports();
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void LinkedPointsActorToolPlugin::onFinishedButtonPressed()
{
   mMainWindow->SetNormalToolMode();
   shutdown();
}

////////////////////////////////////////////////////////////////////////////////
void LinkedPointsActorToolPlugin::initialize(dtActors::LinkedPointsActorProxy* activeProxy)
{
   mIsActive = false;
   hide();

   if (mModeButton->isChecked())
   {
      show();
      mIsActive = true;

      if (!mIsDocked)
      {
         mMainWindow->addDockWidget(Qt::RightDockWidgetArea, this);
         mIsDocked = true;
      }

      mMainWindow->GetPerspView()->GetObjectMotionModel()->SetEnabled(false);
      mMainWindow->GetTopView()->GetObjectMotionModel()->SetEnabled(false);
      mMainWindow->GetSideView()->GetObjectMotionModel()->SetEnabled(false);
      mMainWindow->GetFrontView()->GetObjectMotionModel()->SetEnabled(false);

      mPerspMotionModel->SetEnabled(true);
      mTopMotionModel->SetEnabled(true);
      mSideMotionModel->SetEnabled(true);
      mFrontMotionModel->SetEnabled(true);
      //ViewportManager::GetInstance().refreshAllViewports();
   }

   mActiveProxy = activeProxy;
   activeProxy->GetActor(mActiveActor);

   mActiveActor->SetVisualize(true);

   // Determine which point was selected.
   dtCore::DeltaDrawable* drawable = ViewportManager::GetInstance().getLastDrawable();
   if (!selectDrawable(drawable))
   {
      // By default, we always select the first point.
      drawable = mActiveActor->GetPointDrawable(0);
      selectDrawable(drawable);
   }
}

////////////////////////////////////////////////////////////////////////////////
void LinkedPointsActorToolPlugin::shutdown()
{
   mIsActive = false;
   hide();

   if (mActiveActor)
   {
      HidePlacementGhost();
      mActiveActor->SetVisualize(false);
   }

   mMainWindow->GetPerspView()->GetObjectMotionModel()->SetEnabled(true);
   mMainWindow->GetTopView()->GetObjectMotionModel()->SetEnabled(true);
   mMainWindow->GetSideView()->GetObjectMotionModel()->SetEnabled(true);
   mMainWindow->GetFrontView()->GetObjectMotionModel()->SetEnabled(true);

   mPerspMotionModel->SetEnabled(false);
   mTopMotionModel->SetEnabled(false);
   mSideMotionModel->SetEnabled(false);
   mFrontMotionModel->SetEnabled(false);
   //ViewportManager::GetInstance().refreshAllViewports();
}

////////////////////////////////////////////////////////////////////////////////
bool LinkedPointsActorToolPlugin::selectDrawable(dtCore::DeltaDrawable* drawable)
{
   if (drawable && mActiveActor)
   {
      // Check if the drawable is a valid point on the actor.
      int pointIndex = mActiveActor->GetPointIndex(drawable);
      if (pointIndex >= 0)
      {
         dtCore::Transformable* point = mActiveActor->GetPointDrawable(pointIndex);
         mCurrentPoint = pointIndex;

         mPerspMotionModel->SetTarget(point);
         mTopMotionModel->SetTarget(point);
         mSideMotionModel->SetTarget(point);
         mFrontMotionModel->SetTarget(point);

         mPerspMotionModel->UpdateWidgets();
         mTopMotionModel->UpdateWidgets();
         mSideMotionModel->UpdateWidgets();
         mFrontMotionModel->UpdateWidgets();

         ViewportManager::GetInstance().refreshAllViewports();
         return true;
      }
   }

   mCurrentPoint = -1;
   return false;
}

////////////////////////////////////////////////////////////////////////////////
ToolObjectMotionModel* LinkedPointsActorToolPlugin::GetMotionModelForView(Viewport* vp)
{
   if (vp == mMainWindow->GetPerspView())
   {
      return mPerspMotionModel.get();
   }
   else if (vp == mMainWindow->GetTopView())
   {
      return mTopMotionModel.get();
   }
   else if (vp == mMainWindow->GetSideView())
   {
      return mSideMotionModel.get();
   }
   else if (vp == mMainWindow->GetFrontView())
   {
      return mFrontMotionModel.get();
   }

   return NULL;
}

////////////////////////////////////////////////////////////////////////////////
void LinkedPointsActorToolPlugin::ShowPlacementGhost(bool forceRefresh)
{
   if (!mShowingPlacementGhost && mActiveActor)
   {
      mActiveActor->AddPoint(osg::Vec3());
      mShowingPlacementGhost = true;
      forceRefresh = true;
   }

   if (forceRefresh)
   {
      mPerspMotionModel->SetEnabled(false);
      mTopMotionModel->SetEnabled(false);
      mSideMotionModel->SetEnabled(false);
      mFrontMotionModel->SetEnabled(false);
      ViewportManager::GetInstance().refreshAllViewports();
   }
}

////////////////////////////////////////////////////////////////////////////////
void LinkedPointsActorToolPlugin::HidePlacementGhost(bool forceRefresh)
{
   if (mShowingPlacementGhost && mActiveActor)
   {
      int index = mActiveActor->GetPointCount();
      if (index > 1)
      {
         mActiveActor->RemovePoint(index - 1);
      }
      mShowingPlacementGhost = false;
      forceRefresh = true;
   }

   if (forceRefresh)
   {
      mPerspMotionModel->SetEnabled(true);
      mTopMotionModel->SetEnabled(true);
      mSideMotionModel->SetEnabled(true);
      mFrontMotionModel->SetEnabled(true);
      ViewportManager::GetInstance().refreshAllViewports();
   }
}

////////////////////////////////////////////////////////////////////////////////
void LinkedPointsActorToolPlugin::UpdatePlacementGhost(Viewport* vp, osg::Vec2 mousePos)
{
   EditorViewport* editorView = dynamic_cast<EditorViewport*>(vp);
   if (!editorView)
   {
      return;
   }

   // Bail out early if we are not even supposed to see the placement ghost.
   if (mIsInCameraMode || !mCreationModeCheckbox->isChecked())
   {
      HidePlacementGhost();
      return;
   }

   //// If the mouse is hovering over an existing point, we don't need to show the ghost.
   //bool hideGhost = false;
   //dtCore::DeltaDrawable* drawable = editorView->getPickDrawable(mousePos.x(), mousePos.y());
   //dtCore::Transformable* point = dynamic_cast<dtCore::Transformable*>(drawable);
   //int pointIndex = mActiveActor->GetPointIndex(point);
   //// If we have a valid point index, it means we are colliding with a point.
   //if (pointIndex >= 0)
   //{
   //   bool isDirectPoint = (point == mActiveActor->GetPointDrawable(pointIndex));
   //   if (isDirectPoint)
   //   {
   //      hideGhost = true;

   //      // If we are showing the placement ghost, we want to ignore collision with that ghost.
   //      if (mShowingPlacementGhost && pointIndex == mActiveActor->GetPointCount() - 1)
   //      {
   //         hideGhost = false;
   //      }
   //   }
   //}

   //if (hideGhost)
   //{
   //   HidePlacementGhost();
   //}
   //else
   {
      // If the ghost is being shown, update the position of it.
      ShowPlacementGhost();

      osg::Vec3 position;
      if (editorView->getPickPosition(mousePos.x(), mousePos.y(), position, mActiveActor))
      {
         // Convert the pick position to the snap grid if needed.
         position = ViewportManager::GetInstance().GetSnapPosition(position, true, mActiveActor);
         int index = mActiveActor->GetPointCount() - 1;
         if (index > 0)
         {
            mActiveActor->SetPointPosition(index, position);
         }
         ViewportManager::GetInstance().refreshAllViewports();
      }
   }
}

////////////////////////////////////////////////////////////

namespace LinkedPointsActorTool
{
class DT_LINKED_POINTS_ACTOR_EXPORT PluginFactory : public dtEditQt::PluginFactory
{
public:

   PluginFactory() {}
   ~PluginFactory() {}

   /** get the name of the plugin */
   virtual std::string GetName() { return LinkedPointsActorToolPlugin::PLUGIN_NAME; }

   /** get a description of the plugin */
   virtual std::string GetDescription() { return "Creates a list of points."; }

   virtual void GetDependencies(std::list<std::string>& deps) 
   {
   }

    /** construct the plugin and return a pointer to it */
   virtual Plugin* Create(MainWindow* mw) 
   {
      mPlugin = new LinkedPointsActorToolPlugin(mw);
      return mPlugin;
   }

   virtual void Destroy() 
   {
      delete mPlugin;
   }

private:

   Plugin* mPlugin;
}; 
} //namespace LinkedPointsActorToolPlugin

extern "C" DT_LINKED_POINTS_ACTOR_EXPORT dtEditQt::PluginFactory* CreatePluginFactory()
{
   return new LinkedPointsActorTool::PluginFactory;
}
