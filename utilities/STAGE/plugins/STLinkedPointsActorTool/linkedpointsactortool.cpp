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
#include <QtGui/QWheelEvent>
#include <QtGui/QToolBar>


const std::string LinkedPointsActorToolPlugin::PLUGIN_NAME = "Linked Points Actor Tool";
const std::string LinkedPointsActorToolPlugin::MODE_ICON   = ":/icons/mode.png";


////////////////////////////////////////////////////////////////////////////////
LinkedPointsActorToolPlugin::LinkedPointsActorToolPlugin(MainWindow* mw)
   : mMainWindow(mw)
   , mIsDocked(false)
   , mIsActive(false)
   , mShowingPlacementGhost(false)
   , mIsInActorMode(false)
   , mIsInCameraMode(false)
   , mCurrentPoint(0)
   , mCanCopy(false)
   , mActiveProxy(NULL)
   , mActiveActor(NULL)
   , mPointsProp(NULL)
{
   // apply layout made with QT designer
   Ui_LinkedPointsActorTool ui;
   ui.setupUi(this);

   mModeButton = new QAction(QIcon(MODE_ICON.c_str()), "Linked Points Actor Tool", this);
   mMainWindow->AddExclusiveToolMode(mModeButton);

   mCreationModeCheckbox      = ui.mCreationModeCheckbox;
   mFavorRightAnglesCheckbox  = ui.mFavorRightAnglesCheckbox;
   mDeleteLinkButton          = ui.mDeleteLinkButton;
   mFinishedButton            = ui.mFinishedButton;

   // Setup our signal slots.
   connect(&EditorEvents::GetInstance(), SIGNAL(actorProxyCreated(ActorProxyRefPtr, bool)),
      this, SLOT(onActorProxyCreated(ActorProxyRefPtr, bool)));
   connect(&EditorEvents::GetInstance(), SIGNAL(actorProxyDestroyed(ActorProxyRefPtr)),
      this, SLOT(onActorProxyDestroyed(ActorProxyRefPtr)));
   connect(&EditorEvents::GetInstance(), SIGNAL(selectedActors(ActorProxyRefPtrVector &)),
      this, SLOT(onActorsSelected(ActorProxyRefPtrVector &)));

   connect(&ViewportManager::GetInstance(), SIGNAL(viewportEnabled(Viewport*, bool, bool*)),
      this, SLOT(onViewportEnabled(Viewport*, bool, bool*)));
   connect(&ViewportManager::GetInstance(), SIGNAL(mousePressEvent(Viewport*, QMouseEvent*)),
      this, SLOT(onMousePressEvent(Viewport*, QMouseEvent*)));
   connect(&ViewportManager::GetInstance(), SIGNAL(mouseReleaseEvent(Viewport*, QMouseEvent*)),
      this, SLOT(onMouseReleaseEvent(Viewport*, QMouseEvent*)));
   connect(&ViewportManager::GetInstance(), SIGNAL(mouseDoubleClickEvent(Viewport*, QMouseEvent*)),
      this, SLOT(onMouseDoubleClickEvent(Viewport*, QMouseEvent*)));
   connect(&ViewportManager::GetInstance(), SIGNAL(mouseMoveEvent(Viewport*, QMouseEvent*)),
      this, SLOT(onMouseMoveEvent(Viewport*, QMouseEvent*)));
   connect(&ViewportManager::GetInstance(), SIGNAL(wheelEvent(Viewport*, QWheelEvent*)),
      this, SLOT(onWheelEvent(Viewport*, QWheelEvent*)));
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
   mPerspMotionModel = new ToolObjectMotionModel(mMainWindow->GetPerspView()->GetView());
   mPerspMotionModel->SetEnabled(false);
   mPerspMotionModel->SetScale(1.0f);
   mPerspMotionModel->SetCamera(mMainWindow->GetPerspView()->getCamera()->getDeltaCamera());
   mPerspMotionModel->SetSceneNode(mMainWindow->GetPerspView()->GetRootNode());

   mTopMotionModel = new ToolObjectMotionModel(mMainWindow->GetTopView()->GetView());
   mTopMotionModel->SetEnabled(false);
   mTopMotionModel->SetAutoScaleEnabled(false);
   mTopMotionModel->SetScale(300.0f / mMainWindow->GetTopView()->getCamera()->getZoom());
   mTopMotionModel->SetCamera(mMainWindow->GetTopView()->getCamera()->getDeltaCamera());
   mTopMotionModel->SetSceneNode(mMainWindow->GetTopView()->GetRootNode());

   mSideMotionModel = new ToolObjectMotionModel(mMainWindow->GetSideView()->GetView());
   mSideMotionModel->SetEnabled(false);
   mSideMotionModel->SetAutoScaleEnabled(false);
   mSideMotionModel->SetScale(300.0f / mMainWindow->GetSideView()->getCamera()->getZoom());
   mSideMotionModel->SetCamera(mMainWindow->GetSideView()->getCamera()->getDeltaCamera());
   mSideMotionModel->SetSceneNode(mMainWindow->GetSideView()->GetRootNode());

   mFrontMotionModel = new ToolObjectMotionModel(mMainWindow->GetFrontView()->GetView());
   mFrontMotionModel->SetEnabled(false);
   mFrontMotionModel->SetAutoScaleEnabled(false);
   mFrontMotionModel->SetScale(300.0f / mMainWindow->GetFrontView()->getCamera()->getZoom());
   mFrontMotionModel->SetCamera(mMainWindow->GetFrontView()->getCamera()->getDeltaCamera());
   mFrontMotionModel->SetSceneNode(mMainWindow->GetFrontView()->GetRootNode());

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
   if (actors.size() > 0)
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
   mPointsProp = NULL;
   mOldPropValue = "";

   if (actors.size() == 0)
   {
      mMainWindow->GetPerspView()->GetObjectMotionModel()->SetEnabled(false);
      mMainWindow->GetTopView()->GetObjectMotionModel()->SetEnabled(false);
      mMainWindow->GetSideView()->GetObjectMotionModel()->SetEnabled(false);
      mMainWindow->GetFrontView()->GetObjectMotionModel()->SetEnabled(false);
   }
}

////////////////////////////////////////////////////////////////////////////////
void LinkedPointsActorToolPlugin::onViewportEnabled(Viewport* vp, bool enabled, bool* overrideDefault)
{
   if (!mIsActive)
   {
      return;
   }

   if (overrideDefault)
   {
      *overrideDefault = true;
   }

   ToolObjectMotionModel* motion = GetMotionModelForView(vp);
   if (motion)
   {
      motion->SetEnabled(enabled);
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
   mCanCopy = true;

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

      // If we are ALT clicking somewhere on the actor
      // but not on a gizmo, we want to insert a point here.
      EditorViewport* editorView = dynamic_cast<EditorViewport*>(vp);
      if (editorView)
      {
         osg::Vec2 pos = editorView->convertMousePosition(e->pos());
         if (editorView->GetKeyMods() == Qt::AltModifier &&
            motion->Update(pos) == ToolObjectMotionModel::MOTION_TYPE_MAX)
         {
            osg::Vec3 pickPosition;
            if (editorView->getPickPosition(e->pos().x(), e->pos().y(), pickPosition))
            {
               mOldPropValue = mPointsProp->ToString();

               int newPoint = mActiveActor->AddPointOnSegment(pickPosition);
               if (newPoint > -1)
               {
                  EditorEvents::GetInstance().emitBeginChangeTransaction();
                  EditorEvents::GetInstance().emitActorPropertyAboutToChange(mActiveProxy.get(), mPointsProp, mOldPropValue, mPointsProp->ToString());
                  EditorEvents::GetInstance().emitActorPropertyChanged(mActiveProxy.get(), mPointsProp);
                  EditorEvents::GetInstance().emitEndChangeTransaction();

                  // Select this new point.
                  selectPoint(newPoint);
                  mCanCopy = false;
                  //ViewportManager::GetInstance().refreshAllViewports();
               }
            }
         }
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
void LinkedPointsActorToolPlugin::onMouseDoubleClickEvent(Viewport* vp, QMouseEvent* e)
{
   mCreationModeCheckbox->setChecked(!mCreationModeCheckbox->isChecked());
   onCreationModePressed();
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
         // First check if we can copy and we are holding the ALT key.
         if (mCanCopy)
         {
            if (editorView->GetKeyMods() == Qt::AltModifier)
            {
               mActiveActor->AddPoint(mActiveActor->GetPointPosition(mCurrentPoint), mCurrentPoint + 1);
               selectPoint(mCurrentPoint + 1);
            }

            mCanCopy = false;
         }

         if (motion->Update(pos) != ToolObjectMotionModel::MOTION_TYPE_MAX)
         {
            // If we are holding the Control key, we should ground clamp the motion of this point.
            if (editorView->GetKeyMods() == Qt::ControlModifier)
            {
               std::vector<dtCore::DeltaDrawable*> ignoredDrawables;
               ignoredDrawables.push_back(mActiveActor);

               osg::Vec3 position = mActiveActor->GetPointPosition(mCurrentPoint);
               position = ViewportManager::GetInstance().GetSnapPosition(position, true, ignoredDrawables);
               if (position != mActiveActor->GetPointPosition(mCurrentPoint))
               {
                  mActiveActor->SetPointPosition(mCurrentPoint, position);
               }
            }

            HidePlacementGhost();
         }
         else
         {
            UpdatePlacementGhost(vp, osg::Vec2(e->pos().x(), e->pos().y()));
            motion->UpdateWidgets();
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
void LinkedPointsActorToolPlugin::onWheelEvent(Viewport* vp, QWheelEvent* e)
{
   ToolObjectMotionModel* motion = GetMotionModelForView(vp);
   if (motion && motion != mPerspMotionModel.get())
   {
      double zoom = vp->getCamera()->getZoom();

      if (e->delta() > 0)
      {
         zoom *= 1.3;
      }
      else
      {
         zoom *= 0.7;
         if (zoom < 0.0001) zoom = 0.0001;
      }

      motion->SetScale(300.0f / zoom);
      motion->UpdateWidgets();
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

      mOldPropValue = mPointsProp->ToString();

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

      EditorEvents::GetInstance().emitBeginChangeTransaction();
      EditorEvents::GetInstance().emitActorPropertyAboutToChange(mActiveProxy.get(), mPointsProp, mOldPropValue, mPointsProp->ToString());
      EditorEvents::GetInstance().emitActorPropertyChanged(mActiveProxy.get(), mPointsProp);
      EditorEvents::GetInstance().emitEndChangeTransaction();

      mIsInActorMode = false;

      if (overrideDefault)
      {
         *overrideDefault = true;
      }
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
      // create new points in the chain.
      if (editorView->GetMouseButton() == Qt::LeftButton)
      {
         // Always only allow a single selection.
         ViewportManager::GetInstance().getViewportOverlay()->setMultiSelectMode(false);

         // Since we have the ghost object, just let that ghost object be used as the new point
         // and create a new ghost object.
         if (mShowingPlacementGhost)
         {
            mShowingPlacementGhost = false;
            //selectPoint(mActiveActor->GetPointCount() - 1);
            mCurrentPoint = mActiveActor->GetPointCount() - 1;

            EditorEvents::GetInstance().emitBeginChangeTransaction();
            EditorEvents::GetInstance().emitActorPropertyAboutToChange(mActiveProxy.get(), mPointsProp, mOldPropValue, mPointsProp->ToString());
            EditorEvents::GetInstance().emitActorPropertyChanged(mActiveProxy.get(), mPointsProp);
            EditorEvents::GetInstance().emitEndChangeTransaction();

            *overrideDefault = true;
         }
         else
         {
            // Only override the selection if we are not selecting a specific point or we are holding the ALT key.
            if (editorView->GetKeyMods() == Qt::AltModifier)
            {
               *overrideDefault = true;
            }
            else
            {
               dtCore::DeltaDrawable* drawable = editorView->getPickDrawable(e->pos().x(), e->pos().y());
               osg::Vec3 position;
               editorView->getPickPosition(position);
               if (selectDrawable(drawable, position))
               {
                  *overrideDefault = true;
               }
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
   mSideMotionModel->SetSnapEnabled(translation, rotation, scale);
   mFrontMotionModel->SetSnapEnabled(translation, rotation, scale);
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
      mPerspMotionModel->SetTarget(NULL);
      mTopMotionModel->SetTarget(NULL);
      mSideMotionModel->SetTarget(NULL);
      mFrontMotionModel->SetTarget(NULL);
      //ViewportManager::GetInstance().refreshAllViewports();
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
      mOldPropValue = mPointsProp->ToString();

      mActiveActor->RemovePoint(mCurrentPoint);

      EditorEvents::GetInstance().emitBeginChangeTransaction();
      EditorEvents::GetInstance().emitActorPropertyAboutToChange(mActiveProxy.get(), mPointsProp, mOldPropValue, mPointsProp->ToString());
      EditorEvents::GetInstance().emitActorPropertyChanged(mActiveProxy.get(), mPointsProp);
      EditorEvents::GetInstance().emitEndChangeTransaction();

      if (mCurrentPoint > 0) mCurrentPoint--;

      if (!selectPoint(mCurrentPoint))
      {
         //ViewportManager::GetInstance().refreshAllViewports();
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
         //setFloating(true);
         mIsDocked = true;
      }

      mMainWindow->GetPerspView()->GetObjectMotionModel()->SetEnabled(false);
      mMainWindow->GetTopView()->GetObjectMotionModel()->SetEnabled(false);
      mMainWindow->GetSideView()->GetObjectMotionModel()->SetEnabled(false);
      mMainWindow->GetFrontView()->GetObjectMotionModel()->SetEnabled(false);

      //mPerspMotionModel->SetEnabled(true);
      //mTopMotionModel->SetEnabled(true);
      //mSideMotionModel->SetEnabled(true);
      //mFrontMotionModel->SetEnabled(true);
      //ViewportManager::GetInstance().refreshAllViewports();
   }

   mActiveProxy = activeProxy;
   activeProxy->GetActor(mActiveActor);
   mPointsProp = mActiveProxy->GetProperty("PointList");

   mActiveActor->SetVisualize(true);

   // Determine which point was selected.
   dtCore::DeltaDrawable* drawable = ViewportManager::GetInstance().getLastDrawable();
   osg::Vec3 position = ViewportManager::GetInstance().getLastPickPosition();
   if (!selectDrawable(drawable, position))
   {
      // By default, we always select the first point.
      selectPoint(0);
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

   //mMainWindow->GetPerspView()->GetObjectMotionModel()->SetEnabled(true);
   //mMainWindow->GetTopView()->GetObjectMotionModel()->SetEnabled(true);
   //mMainWindow->GetSideView()->GetObjectMotionModel()->SetEnabled(true);
   //mMainWindow->GetFrontView()->GetObjectMotionModel()->SetEnabled(true);

   mPerspMotionModel->SetEnabled(false);
   mTopMotionModel->SetEnabled(false);
   mSideMotionModel->SetEnabled(false);
   mFrontMotionModel->SetEnabled(false);
   mPerspMotionModel->SetTarget(NULL);
   mTopMotionModel->SetTarget(NULL);
   mSideMotionModel->SetTarget(NULL);
   mFrontMotionModel->SetTarget(NULL);
   //ViewportManager::GetInstance().refreshAllViewports();
}

////////////////////////////////////////////////////////////////////////////////
bool LinkedPointsActorToolPlugin::selectPoint(int pointIndex)
{
   if (pointIndex >= 0 && pointIndex < mActiveActor->GetPointCount())
   {
      dtCore::Transformable* point = mActiveActor->GetPointDrawable(pointIndex);
      mCurrentPoint = pointIndex;

      mPerspMotionModel->SetTarget(point);
      mTopMotionModel->SetTarget(point);
      mSideMotionModel->SetTarget(point);
      mFrontMotionModel->SetTarget(point);

      mPerspMotionModel->SetPointIndex(mCurrentPoint);
      mTopMotionModel->SetPointIndex(mCurrentPoint);
      mSideMotionModel->SetPointIndex(mCurrentPoint);
      mFrontMotionModel->SetPointIndex(mCurrentPoint);

      mPerspMotionModel->SetActiveActor(mActiveActor);
      mTopMotionModel->SetActiveActor(mActiveActor);
      mSideMotionModel->SetActiveActor(mActiveActor);
      mFrontMotionModel->SetActiveActor(mActiveActor);

      mPerspMotionModel->UpdateWidgets();
      mTopMotionModel->UpdateWidgets();
      mSideMotionModel->UpdateWidgets();
      mFrontMotionModel->UpdateWidgets();

      //ViewportManager::GetInstance().refreshAllViewports();
      return true;
   }

   return false;
}

////////////////////////////////////////////////////////////////////////////////
bool LinkedPointsActorToolPlugin::selectDrawable(dtCore::DeltaDrawable* drawable, osg::Vec3 pickPosition)
{
   if (drawable && mActiveActor)
   {
      // Check if the drawable is a valid point on the actor.
      int pointIndex = mActiveActor->GetPointIndex(drawable, pickPosition);

      // now select our point.
      return selectPoint(pointIndex);
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
void LinkedPointsActorToolPlugin::ShowPlacementGhost(osg::Vec3 position, bool forceRefresh)
{
   if (!mShowingPlacementGhost && mActiveActor)
   {
      mOldPropValue = mPointsProp->ToString();

      mActiveActor->AddPoint(position);
      mShowingPlacementGhost = true;
      forceRefresh = true;
      mPlacementGhostIndex = mActiveActor->GetPointCount();
   }

   if (forceRefresh)
   {
      mPerspMotionModel->SetTarget(NULL);
      mTopMotionModel->SetTarget(NULL);
      mSideMotionModel->SetTarget(NULL);
      mFrontMotionModel->SetTarget(NULL);
      //ViewportManager::GetInstance().refreshAllViewports();
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
      selectPoint(mCurrentPoint);
      //ViewportManager::GetInstance().refreshAllViewports();
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

   // If our placement ghost index is not the size of our point list,
   // then it may have changed from an undo command.
   if (mShowingPlacementGhost &&
      mPlacementGhostIndex != mActiveActor->GetPointCount())
   {
      mShowingPlacementGhost = false;
   }

   // If the ghost is being shown, update the position of it.
   std::vector<dtCore::DeltaDrawable*> ignoredDrawables;
   ignoredDrawables.push_back(mActiveActor);
   osg::Vec3 position;
   if (editorView->getPickPosition(mousePos.x(), mousePos.y(), position, ignoredDrawables))
   {
      // Snap to right angles.
      if (mShowingPlacementGhost && mFavorRightAnglesCheckbox->isChecked())
      {
         int prevPoint = mActiveActor->GetPointCount() - 2;
         if (prevPoint >= 0)
         {
            osg::Vec3 angle1, angle2;
            osg::Vec3 prevPos = mActiveActor->GetPointPosition(prevPoint);

            // If we only have a single point placed so far, align
            // the favored angles with the world axes.
            if (prevPoint == 0)
            {
               angle1.set(0.0f, 1.0f, 0.0f);
               angle2.set(1.0f, 0.0f, 0.0f);
            }
            else
            {
               osg::Vec3 prevPos2 = mActiveActor->GetPointPosition(prevPoint - 1);
               angle1 = prevPos - prevPos2;
               angle1.normalize();
               angle2 = angle1 ^ osg::Vec3(0.0f, 0.0f, 1.0f);
            }

            osg::Vec3 vec = position - prevPos;
            float len = vec.length2();
            vec.z() = 0.0f;
            vec.normalize();
            float dot = vec * angle1;
            if (abs(dot) >= 0.99f)
            {
               osg::Vec3 start = prevPos - (angle1 * len);
               osg::Vec3 end = prevPos + (angle1 * len);
               position = mActiveActor->FindNearestPointOnLine(start, end, position);
            }
            else
            {
               dot = vec * angle2;
               if (abs(dot) >= 0.99f)
               {
                  osg::Vec3 start = prevPos - (angle2 * len);
                  osg::Vec3 end = prevPos + (angle2 * len);
                  position = mActiveActor->FindNearestPointOnLine(start, end, position);
               }
            }

            osg::Vec3 nextPos = mActiveActor->GetPointPosition(0);

            // If we only have a single point placed so far, align
            // the favored angles with the world axes.
            if (prevPoint == 0)
            {
               angle1.set(0.0f, 1.0f, 0.0f);
               angle2.set(1.0f, 0.0f, 0.0f);
            }
            else
            {
               osg::Vec3 nextPos2 = mActiveActor->GetPointPosition(1);
               angle1 = nextPos2 - nextPos;
               angle1.normalize();
               angle2 = angle1 ^ osg::Vec3(0.0f, 0.0f, 1.0f);
            }

            vec = position - nextPos;
            len = vec.length();

            if (len <= 1.0f)
            {
               position = nextPos;
            }
            else
            {
               vec.z() = 0.0f;
               vec.normalize();

               // Test for a lock with the next point.
               dot = vec * angle1;
               if (abs(dot) >= 0.99f)
               {
                  osg::Vec3 start = nextPos - (angle1 * len * 2);
                  osg::Vec3 end = nextPos + (angle1 * len * 2);
                  position = mActiveActor->FindNearestPointOnLine(start, end, position);
               }
               else
               {
                  dot = vec * angle2;
                  if (abs(dot) >= 0.99f)
                  {
                     osg::Vec3 start = nextPos - (angle2 * len);
                     osg::Vec3 end = nextPos + (angle2 * len);
                     position = mActiveActor->FindNearestPointOnLine(start, end, position);
                  }
               }
            }
         }
      }

      // Convert the pick position to the snap grid if needed.
      position = ViewportManager::GetInstance().GetSnapPosition(position, true, ignoredDrawables);
      ShowPlacementGhost(position);

      int index = mActiveActor->GetPointCount() - 1;
      if (index > 0)
      {
         mActiveActor->SetPointPosition(index, position);
      }
      //ViewportManager::GetInstance().refreshAllViewports();
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
