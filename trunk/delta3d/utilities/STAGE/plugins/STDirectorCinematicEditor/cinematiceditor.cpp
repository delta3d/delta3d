/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Author: Jeff P. Houde
 */

#include <cinematiceditor.h>
#include <SelectionEvent.h>

#include <dtEditQt/mainwindow.h>
#include <dtEditQt/pluginmanager.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/viewportmanager.h>

#include <dtDirector/nodemanager.h>

#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/propertyeditor.h>
#include <dtDirectorQt/editorscene.h>
#include <dtDirectorQt/undomanager.h>

#include <dtCore/object.h>
#include <dtDAL/arrayactorpropertybase.h>
#include <dtDAL/containeractorproperty.h>
#include <dtDAL/stringactorproperty.h>
#include <dtDAL/floatactorproperty.h>

#include <QtCore/QTimeLine>

const std::string DirectorCinematicEditorPlugin::PLUGIN_NAME = "Director Cinematic Editor";


////////////////////////////////////////////////////////////////////////////////
DirectorCinematicEditorPlugin::DirectorCinematicEditorPlugin(MainWindow* mw)
   : dtDirector::CustomEditorTool("Cinematic")
   , mMainWindow(mw)
   , mSelectedActor(-1)
   , mTransformEvent(NULL)
   , mAnimationEvent(NULL)
   , mEventEvent(NULL)
   , mPlaying(false)
   , mTotalTime(1.0f)
{
   mUI.setupUi(this);

   setWindowTitle("Director Cinematic Editor");

   dtEditQt::PluginManager* pluginManager = mMainWindow->GetPluginManager();
   if (!pluginManager) return;

   dtEditQt::Plugin* plugin = pluginManager->GetPlugin("Director Tool");
   if (!plugin) return;

   dtDirector::DirectorEditor* directorEditor = dynamic_cast<dtDirector::DirectorEditor*>(plugin);
   SetEditor(directorEditor);

   Initialize();
}

////////////////////////////////////////////////////////////////////////////////
DirectorCinematicEditorPlugin::~DirectorCinematicEditorPlugin()
{
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::Initialize()
{
   connect(&EditorEvents::GetInstance(), SIGNAL(selectedActors(ActorProxyRefPtrVector &)),
      this, SLOT(onActorsSelected(ActorProxyRefPtrVector &)));
   connect(&ViewportManager::GetInstance(), SIGNAL(endActorMode(Viewport*, QMouseEvent*, bool*)),
      this, SLOT(onEndActorMode(Viewport*, QMouseEvent*, bool*)));

   // Actor listing
   connect(mUI.mActorCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(OnActorComboChanged(int)));
   connect(mUI.mAddActorButton, SIGNAL(clicked()), this, SLOT(OnAddActor()));
   connect(mUI.mRemoveActorButton, SIGNAL(clicked()), this, SLOT(OnRemoveActor()));

   // Transform Event Track.
   connect(mUI.mTransformTrack, SIGNAL(EventSelected(BaseEvent*)), this, SLOT(OnTransformEventSelected(BaseEvent*)));
   connect(mUI.mTransformTrack, SIGNAL(EventTimesChanged(int, int)), this, SLOT(OnTransformEventTimesChanged(int, int)));
   connect(mUI.mTransformTrack, SIGNAL(EventDeleted(BaseEvent*)), this, SLOT(OnTransformEventRemoved(BaseEvent*)));
   connect(mUI.mAddTransformButton, SIGNAL(clicked()), this, SLOT(OnAddTransform()));
   connect(mUI.mRemoveTransformButton, SIGNAL(clicked()), this, SLOT(OnRemoveTransform()));

   connect(mUI.mPosXEdit, SIGNAL(textEdited(QString)), this, SLOT(OnPosXChanged(QString)));
   connect(mUI.mPosYEdit, SIGNAL(textEdited(QString)), this, SLOT(OnPosYChanged(QString)));
   connect(mUI.mPosZEdit, SIGNAL(textEdited(QString)), this, SLOT(OnPosZChanged(QString)));
   connect(mUI.mRotXEdit, SIGNAL(textEdited(QString)), this, SLOT(OnRotXChanged(QString)));
   connect(mUI.mRotYEdit, SIGNAL(textEdited(QString)), this, SLOT(OnRotYChanged(QString)));
   connect(mUI.mRotZEdit, SIGNAL(textEdited(QString)), this, SLOT(OnRotZChanged(QString)));
   connect(mUI.mScaleXEdit, SIGNAL(textEdited(QString)), this, SLOT(OnScaleXChanged(QString)));
   connect(mUI.mScaleYEdit, SIGNAL(textEdited(QString)), this, SLOT(OnScaleYChanged(QString)));
   connect(mUI.mScaleZEdit, SIGNAL(textEdited(QString)), this, SLOT(OnScaleZChanged(QString)));

   // Create a time line.
   mTimeLine = new QTimeLine(1000, this);
   mTimeLine->setCurveShape(QTimeLine::LinearCurve);

   connect(mUI.mCurrentTimeEdit, SIGNAL(valueChanged(double)), this, SLOT(OnCurrentTimeChanged(double)));
   connect(mUI.mTotalTimeEdit, SIGNAL(valueChanged(double)), this, SLOT(OnTotalTimeChanged(double)));
   connect(mTimeLine, SIGNAL(frameChanged(int)), mUI.mTimeSlider, SLOT(setValue(int)));
   connect(mTimeLine, SIGNAL(finished()), this, SLOT(OnTimeLineFinished()));

   connect(mUI.mTimeSlider, SIGNAL(valueChanged(int)), this, SLOT(OnTimeSliderValueChanged(int)));

   // Add a selection event to the selection track
   SelectionEvent* selectionEvent = new SelectionEvent(0, 0);
   mUI.mSelectionTrack->AddEvent(selectionEvent);
   mUI.mSelectionTrack->update();

   connect(mUI.mSelectionTrack, SIGNAL(EditingFinished()), this, SLOT(OnSelectionEdited()));

   connect(mUI.mPlayButton, SIGNAL(clicked()), this, SLOT(OnPlay()));

   // Save and close buttons
   connect(mUI.mSaveButton, SIGNAL(clicked()), this, SLOT(OnSave()));
   connect(mUI.mSaveAndCloseButton, SIGNAL(clicked()), this, SLOT(OnSaveAndClose()));
   connect(mUI.mCancelButton, SIGNAL(clicked()), this, SLOT(OnCancel()));
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::ResetUI()
{
   // Actors
   mUI.mActorCombo->blockSignals(true);
   mUI.mActorCombo->clear();
   bool foundSelected = false;
   int count = (int)mActorData.size();
   for (int index = 0; index < count; ++index)
   {
      dtDAL::ActorProxy* proxy = mActorData[index].mActor.get();
      if (proxy)
      {
         QString name = proxy->GetName().c_str();
         mUI.mActorCombo->addItem(name);

         if (mSelectedActor == index)
         {
            mUI.mActorCombo->setCurrentIndex(index);
            mUI.mRemoveActorButton->setEnabled(true);
            foundSelected = true;
         }
      }
      // If the actor was removed, remove it from this list as well.
      else
      {
         mActorData.erase(mActorData.begin() + index);
         index--;
         continue;
      }
   }
   if (!foundSelected)
   {
      mSelectedActor = -1;
      mUI.mActorCombo->setCurrentIndex(-1);
      mUI.mRemoveActorButton->setEnabled(false);
   }
   mUI.mActorCombo->blockSignals(false);

   mUI.mTransformTrack->ClearEvents();
   if (mSelectedActor > -1)
   {
      mUI.mTransformTrack->setEnabled(true);
      mUI.mAnimationTrack->setEnabled(true);
      mUI.mEventTrack->setEnabled(true);

      // Set up the transform track.
      std::vector<TransformData>& transforms = mActorData[mSelectedActor].mTransformData;
      int count = (int)transforms.size();
      for (int index = 0; index < count; ++index)
      {
         TransformData& data = transforms[index];
         data.mEvent = new KeyFrameEvent(data.mTime);
         if (index == 0) data.mEvent->SetMovable(false);
         mUI.mTransformTrack->AddEvent(data.mEvent);
      }

      OnTransformEventSelected(NULL);
      OnTimeSliderValueChanged(mUI.mTimeSlider->value());
   }
   else
   {
      mUI.mTransformTrack->setEnabled(false);
      mUI.mAnimationTrack->setEnabled(false);
      mUI.mEventTrack->setEnabled(false);

      mUI.mAddTransformButton->setEnabled(false);
      mUI.mRemoveTransformButton->setEnabled(false);
      mUI.mAddAnimationButton->setEnabled(false);
      mUI.mRemoveAnimationButton->setEnabled(false);
      mUI.mAddEventButton->setEnabled(false);
      mUI.mRemoveEventButton->setEnabled(false);
   }

   mUI.mTransformTrack->SetMaximum(mTotalTime * 1000);
   mUI.mAnimationTrack->SetMaximum(mTotalTime * 1000);
   mUI.mEventTrack->SetMaximum(mTotalTime * 1000);
   mUI.mSelectionTrack->SetMaximum(mTotalTime * 1000);
   mUI.mTimeSlider->setMaximum(mTotalTime * 1000);
   mUI.mTotalTimeEdit->setValue(mTotalTime);

   std::vector<dtDAL::ActorProxy*> selection;
   EditorData::GetInstance().GetSelectedActors(selection);

   mUI.mAddActorButton->setEnabled(!selection.empty());

}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::Open(dtDirector::DirectorGraph* graph)
{
   CustomEditorTool::Open(graph);

   // Set all cinematic values to defaults.
   mActorData.clear();
   mTotalTime = 1.0f;
   if (mPlaying) OnPlay();

   OnLoad();

   show();
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::Close()
{
   // Reset the preview time slider back to the start so all actors reset.
   mUI.mTimeSlider->setValue(0);

   CustomEditorTool::Close();
   hide();
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::Destroy()
{
   Close();
}

//////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::closeEvent(QCloseEvent* event)
{
   Close();
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::onActorsSelected(ActorProxyRefPtrVector& actors)
{
   mSelectedActor = -1;

   // If we select an actor in STAGE, try and find that actor in our cinematic actor list.
   if (actors.size() == 1)
   {
      int count = (int)mActorData.size();
      for (int index = 0; index < count; ++index)
      {
         dtDAL::ActorProxy* proxy = mActorData[index].mActor.get();
         if (proxy == actors[0].get())
         {
            mSelectedActor = index;
            break;
         }
      }
   }

   ResetUI();
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::onEndActorMode(Viewport* vp, QMouseEvent* e, bool* overrideDefault)
{
   if (mSelectedActor == -1 || !mTransformEvent) return;

   // Update the transform data if we have one selected.
   TransformData* data = GetTransformData(mTransformEvent);
   if (data)
   {
      dtCore::Transformable* actor = NULL;
      mActorData[mSelectedActor].mActor->GetActor(actor);
      if (actor)
      {
         dtCore::Transform transform;
         actor->GetTransform(transform);
         data->mTransform = transform;

         dtCore::Object* obj = NULL;
         mActorData[mSelectedActor].mActor->GetActor(obj);
         if (obj)
         {
            data->mScale = obj->GetScale();
         }
      }

      // Refresh the UI edit windows to show new transform data.
      OnTransformEventSelected(mTransformEvent);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnActorComboChanged(int index)
{
   if (index < (int)mActorData.size())
   {
      mSelectedActor = index;
   }

   // Now select this actor in STAGE and move the camera to it.
   GotoSelectedActor();

   ResetUI();
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnAddActor()
{
   std::vector<dtDAL::ActorProxy*> selection;
   EditorData::GetInstance().GetSelectedActors(selection);

   int addCount = (int)selection.size();
   for (int addIndex = 0; addIndex < addCount; ++addIndex)
   {
      bool canAdd = true;
      int count = (int)mActorData.size();
      for (int index = 0; index < count; ++index)
      {
         dtDAL::ActorProxy* proxy = mActorData[index].mActor.get();
         if (proxy == selection[addIndex])
         {
            canAdd = false;
            mSelectedActor = index;
            break;
         }
      }

      if (canAdd)
      {
         ActorData data;
         data.mActor = selection[addIndex];
         if (data.mActor.valid())
         {
            mSelectedActor = (int)mActorData.size();
            mActorData.push_back(data);

            // Add our root transform event.
            dtCore::Transformable* actor = NULL;
            data.mActor->GetActor(actor);
            if (actor)
            {
               dtCore::Transform transform;
               osg::Vec3 scale = osg::Vec3(1, 1, 1);

               actor->GetTransform(transform);

               dtCore::Object* obj = NULL;
               data.mActor->GetActor(obj);
               if (obj) scale = obj->GetScale();

               InsertTransform(0, transform, scale, false);
            }
         }
      }
   }

   // Now select this actor in STAGE and move the camera to it.
   GotoSelectedActor();

   ResetUI();
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnRemoveActor()
{
   if (mSelectedActor == -1) return;

   dtDAL::ActorProxy* proxy = mActorData[mSelectedActor].mActor.get();
   mActorData.erase(mActorData.begin() + mSelectedActor);
   if (mSelectedActor >= (int)mActorData.size())
   {
      mSelectedActor--;
   }

   // Now select this actor in STAGE and move the camera to it.
   GotoSelectedActor();

   ResetUI();
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnTransformEventSelected(BaseEvent* event)
{
   bool enable = false;

   KeyFrameEvent* transformEvent = dynamic_cast<KeyFrameEvent*>(event);
   // Set the time slider to the position of the event.
   if (transformEvent)
   {
      mUI.mTimeSlider->setValue(transformEvent->GetStartTime());
      mUI.mTransformTrack->SelectEvent(transformEvent);
      enable = true;

      mUI.mAddTransformButton->setEnabled(true);
      mUI.mRemoveTransformButton->setEnabled(true);

      TransformData* data = GetTransformData(transformEvent);

      if (data)
      {
         dtCore::Transform transform = data->mTransform;
         osg::Vec3 pos = transform.GetTranslation();
         osg::Vec3 rot = transform.GetRotation();
         osg::Vec3 scale = data->mScale;

         mUI.mPosXEdit->setText(dtUtil::ToString(pos.x()).c_str());
         mUI.mPosYEdit->setText(dtUtil::ToString(pos.y()).c_str());
         mUI.mPosZEdit->setText(dtUtil::ToString(pos.z()).c_str());
         mUI.mRotXEdit->setText(dtUtil::ToString(rot.x()).c_str());
         mUI.mRotYEdit->setText(dtUtil::ToString(rot.y()).c_str());
         mUI.mRotZEdit->setText(dtUtil::ToString(rot.z()).c_str());
         mUI.mScaleXEdit->setText(dtUtil::ToString(scale.x()).c_str());
         mUI.mScaleYEdit->setText(dtUtil::ToString(scale.y()).c_str());
         mUI.mScaleZEdit->setText(dtUtil::ToString(scale.z()).c_str());
      }
   }
   else
   {
      mUI.mAddTransformButton->setEnabled(true);
      mUI.mRemoveTransformButton->setEnabled(false);
      mUI.mTransformTrack->DeselectEvents();
   }

   mUI.mPosXEdit->setEnabled(enable);
   mUI.mPosYEdit->setEnabled(enable);
   mUI.mPosZEdit->setEnabled(enable);
   mUI.mRotXEdit->setEnabled(enable);
   mUI.mRotYEdit->setEnabled(enable);
   mUI.mRotZEdit->setEnabled(enable);
   mUI.mScaleXEdit->setEnabled(enable);
   mUI.mScaleYEdit->setEnabled(enable);
   mUI.mScaleZEdit->setEnabled(enable);

   mTransformEvent = transformEvent;
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnTransformEventTimesChanged(int start, int end)
{
   int index = -1;
   TransformData* data = GetTransformData(mTransformEvent, &index);
   if (data)
   {
      TransformData sortData = *data;
      sortData.mTime = start;

      mActorData[mSelectedActor].mTransformData.erase(mActorData[mSelectedActor].mTransformData.begin() + index);

      InsertTransform(sortData.mTime, sortData.mTransform, sortData.mScale, true, mTransformEvent);
   }

   OnTransformEventSelected(mTransformEvent);
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnTransformEventRemoved(BaseEvent* event)
{
   int index = -1;
   TransformData* data = GetTransformData(dynamic_cast<KeyFrameEvent*>(event), &index);
   if (data)
   {
      mActorData[mSelectedActor].mTransformData.erase(mActorData[mSelectedActor].mTransformData.begin() + index);
      OnTimeSliderValueChanged(mUI.mTimeSlider->value());
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnPosXChanged(QString value)
{
   TransformData* data = GetTransformData(mTransformEvent);
   if (data)
   {
      osg::Vec3 pos = data->mTransform.GetTranslation();
      pos.x() = dtUtil::ToType<float>(value.toStdString());
      data->mTransform.SetTranslation(pos);

      dtCore::Transformable* actor = NULL;
      mActorData[mSelectedActor].mActor->GetActor(actor);
      if (actor)
      {
         actor->SetTransform(data->mTransform);
         ViewportManager::GetInstance().refreshAllViewports();
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnPosYChanged(QString value)
{
   TransformData* data = GetTransformData(mTransformEvent);
   if (data)
   {
      osg::Vec3 pos = data->mTransform.GetTranslation();
      pos.y() = dtUtil::ToType<float>(value.toStdString());
      data->mTransform.SetTranslation(pos);

      dtCore::Transformable* actor = NULL;
      mActorData[mSelectedActor].mActor->GetActor(actor);
      if (actor)
      {
         actor->SetTransform(data->mTransform);
         ViewportManager::GetInstance().refreshAllViewports();
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnPosZChanged(QString value)
{
   TransformData* data = GetTransformData(mTransformEvent);
   if (data)
   {
      osg::Vec3 pos = data->mTransform.GetTranslation();
      pos.z() = dtUtil::ToType<float>(value.toStdString());
      data->mTransform.SetTranslation(pos);

      dtCore::Transformable* actor = NULL;
      mActorData[mSelectedActor].mActor->GetActor(actor);
      if (actor)
      {
         actor->SetTransform(data->mTransform);
         ViewportManager::GetInstance().refreshAllViewports();
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnRotXChanged(QString value)
{
   TransformData* data = GetTransformData(mTransformEvent);
   if (data)
   {
      osg::Vec3 rot = data->mTransform.GetRotation();
      rot.x() = dtUtil::ToType<float>(value.toStdString());
      data->mTransform.SetRotation(rot);

      dtCore::Transformable* actor = NULL;
      mActorData[mSelectedActor].mActor->GetActor(actor);
      if (actor)
      {
         actor->SetTransform(data->mTransform);
         ViewportManager::GetInstance().refreshAllViewports();
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnRotYChanged(QString value)
{
   TransformData* data = GetTransformData(mTransformEvent);
   if (data)
   {
      osg::Vec3 rot = data->mTransform.GetRotation();
      rot.y() = dtUtil::ToType<float>(value.toStdString());
      data->mTransform.SetRotation(rot);

      dtCore::Transformable* actor = NULL;
      mActorData[mSelectedActor].mActor->GetActor(actor);
      if (actor)
      {
         actor->SetTransform(data->mTransform);
         ViewportManager::GetInstance().refreshAllViewports();
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnRotZChanged(QString value)
{
   TransformData* data = GetTransformData(mTransformEvent);
   if (data)
   {
      osg::Vec3 rot = data->mTransform.GetRotation();
      rot.z() = dtUtil::ToType<float>(value.toStdString());
      data->mTransform.SetRotation(rot);

      dtCore::Transformable* actor = NULL;
      mActorData[mSelectedActor].mActor->GetActor(actor);
      if (actor)
      {
         actor->SetTransform(data->mTransform);
         ViewportManager::GetInstance().refreshAllViewports();
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnScaleXChanged(QString value)
{
   TransformData* data = GetTransformData(mTransformEvent);
   if (data)
   {
      data->mScale.x() = dtUtil::ToType<float>(value.toStdString());

      dtCore::Object* actor = NULL;
      mActorData[mSelectedActor].mActor->GetActor(actor);
      if (actor)
      {
         actor->SetScale(data->mScale);
         ViewportManager::GetInstance().refreshAllViewports();
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnScaleYChanged(QString value)
{
   TransformData* data = GetTransformData(mTransformEvent);
   if (data)
   {
      data->mScale.y() = dtUtil::ToType<float>(value.toStdString());

      dtCore::Object* actor = NULL;
      mActorData[mSelectedActor].mActor->GetActor(actor);
      if (actor)
      {
         actor->SetScale(data->mScale);
         ViewportManager::GetInstance().refreshAllViewports();
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnScaleZChanged(QString value)
{
   TransformData* data = GetTransformData(mTransformEvent);
   if (data)
   {
      data->mScale.z() = dtUtil::ToType<float>(value.toStdString());

      dtCore::Object* actor = NULL;
      mActorData[mSelectedActor].mActor->GetActor(actor);
      if (actor)
      {
         actor->SetScale(data->mScale);
         ViewportManager::GetInstance().refreshAllViewports();
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnAddTransform()
{
   if (mSelectedActor == -1) return;

   int time = mUI.mTimeSlider->value();

   dtDAL::ActorProxy* proxy = mActorData[mSelectedActor].mActor.get();
   if (proxy)
   {
      dtCore::Transformable* actor = NULL;
      proxy->GetActor(actor);
      if (actor)
      {
         dtCore::Transform transform;
         osg::Vec3 scale = osg::Vec3(1, 1, 1);

         actor->GetTransform(transform);

         dtCore::Object* obj = NULL;
         proxy->GetActor(obj);
         if (obj) scale = obj->GetScale();

         mTransformEvent = InsertTransform(time, transform, scale);
         OnTransformEventSelected(mTransformEvent);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnRemoveTransform()
{
   if (mSelectedActor == -1) return;

   if (mTransformEvent && mTransformEvent->IsMovable())
   {
      mUI.mTransformTrack->RemoveEvent(mTransformEvent);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnCurrentTimeChanged(double time)
{
   mUI.mTimeSlider->setValue(time * 1000);
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnTotalTimeChanged(double time)
{
   // If there is no duration, force at least one second.
   if (time <= 0.0)
   {
      mUI.mTotalTimeEdit->setValue(1.0);
      time = 1.0;
   }

   mTotalTime = (float)time;

   // Setup various UI elements with the new duration
   mUI.mTransformTrack->SetMaximum(mTotalTime * 1000);
   mUI.mAnimationTrack->SetMaximum(mTotalTime * 1000);
   mUI.mEventTrack->SetMaximum(mTotalTime * 1000);
   mUI.mSelectionTrack->SetMaximum(mTotalTime * 1000);
   mUI.mTimeSlider->setMaximum(mTotalTime * 1000);

   OnTimeLineFinished();
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnTimeLineFinished()
{
   // If we are playing, then stop the current play.
   if (mPlaying)
   {
      OnPlay();
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnTimeSliderValueChanged(int value)
{
   mUI.mTransformTrack->SetCurrent(value);
   mUI.mAnimationTrack->SetCurrent(value);
   mUI.mEventTrack->SetCurrent(value);
   mUI.mSelectionTrack->SetCurrent(value);

   OnTransformEventSelected(mUI.mTransformTrack->GetEventAtTime(value));
   //OnAnimationEventSelected(mUI.mAnimationTrack->GetEventAtTime(value));
   //OnEventEventSelected(mUI.mEventTrack->GetEventAtTime(value));

   LerpActors(value);

   double time = value * 0.001f;
   mUI.mCurrentTimeEdit->blockSignals(true);
   mUI.mCurrentTimeEdit->setValue(time);
   mUI.mCurrentTimeEdit->blockSignals(false);
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnSelectionEdited()
{
   // Reset the playback only if we are playing the current selection.
   if (mUI.mPlaySelectionFlag->isChecked() && mPlaying)
   {
      OnPlay();
      OnPlay();
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnPlay()
{
   // Start playing.
   if (!mPlaying)
   {
      int minFrame = 0;
      int maxFrame = mUI.mTimeSlider->maximum();

      // Check if we are only playing the selection.
      if (mUI.mPlaySelectionFlag->isChecked())
      {
         BaseEvent* selectionEvent = mUI.mSelectionTrack->GetEventAtIndex(0);
         if (selectionEvent)
         {
            minFrame = selectionEvent->GetStartTime();
            maxFrame = selectionEvent->GetEndTime();
         }
      }

      // Check if we are looping the play.
      if (mUI.mLoopFlag->isChecked())
      {
         mTimeLine->setLoopCount(0);
      }
      else
      {
         mTimeLine->setLoopCount(1);
      }

      mTimeLine->setDuration(maxFrame - minFrame);
      mTimeLine->setFrameRange(minFrame, maxFrame);
      mTimeLine->setCurrentTime(0);
      mTimeLine->start();

      mUI.mPlayButton->setText("Stop");
      mUI.mLoopFlag->setEnabled(false);
      mUI.mPlaySelectionFlag->setEnabled(false);
      mPlaying = true;
   }
   // Stop playing.
   else
   {
      mTimeLine->stop();
      mUI.mPlayButton->setText("Play");
      mUI.mLoopFlag->setEnabled(true);
      mUI.mPlaySelectionFlag->setEnabled(true);
      mPlaying = false;
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnLoad()
{
   if (!GetGraph()) return;

   std::vector<dtDirector::Node*> nodes;
   GetGraph()->GetNodes("Scheduler", "Cinematic", nodes);

   // Find the scheduler node, if it exists.
   dtDirector::Node* schedulerNode = NULL;
   if (nodes.size())
   {
      schedulerNode = nodes[0];
   }

   if (schedulerNode)
   {
      // Determine the total cinematic time.
      mTotalTime = schedulerNode->GetFloat("TotalTime");

      // Load all scheduled event outputs.
      dtDAL::ArrayActorPropertyBase* arrayProp = dynamic_cast<dtDAL::ArrayActorPropertyBase*>(schedulerNode->GetProperty("EventList"));
      if (arrayProp)
      {
         dtDAL::ContainerActorProperty* containerProp = dynamic_cast<dtDAL::ContainerActorProperty*>(arrayProp->GetArrayProperty());
         if (containerProp)
         {
            dtDAL::StringActorProperty* nameProp = dynamic_cast<dtDAL::StringActorProperty*>(containerProp->GetProperty(0));
            dtDAL::FloatActorProperty*  timeProp = dynamic_cast<dtDAL::FloatActorProperty*>(containerProp->GetProperty(1));

            if (nameProp && timeProp)
            {
               int count = arrayProp->GetArraySize();
               for (int index = 0; index < count; ++index)
               {
                  arrayProp->SetIndex(index);

                  OutputData data(timeProp->GetValue(), nameProp->GetValue(), NULL);
                  mOutputData.push_back(data);
               }
            }
         }
      }
   }

   // Find all Starting remote events and follow their chains to find all cinematic data.
   nodes.clear();
   GetGraph()->GetNodes("Remote Event", "Core", nodes);
   int eventCount = (int)nodes.size();
   for (int eventIndex = 0; eventIndex < eventCount; ++eventIndex)
   {
      dtDirector::Node* node = nodes[eventIndex];
      if (node && node->GetString("EventName") == "Started")
      {
         mSelectedActor = (int)mActorData.size();
         mActorData.push_back(ActorData());

         ActorData& actorData = mActorData[mSelectedActor];

         // Iterate to the next node in the chain.
         if (node->GetOutputLink("Out")->GetLinks().empty()) continue;

         node = node->GetOutputLink("Out")->GetLinks()[0]->GetOwner();
         while (node)
         {
            dtDirector::OutputLink* outLink = NULL;

            // Load a Translation lerp node.
            if (node->GetType().GetFullName() == "Cinematic.Lerp Actor Translation")
            {
               if (!actorData.mActor)
               {
                  actorData.mActor = node->GetActor("Actor");
               }
               {
                  int time = node->GetFloat("StartTime") * 1000;
                  osg::Vec4 pos = node->GetVec("StartPosition");
                  TransformData* data = GetOrCreateTransformData(time);
                  if (data) data->mTransform.SetTranslation(pos.x(), pos.y(), pos.z());
               }
               {
                  int time = node->GetFloat("EndTime") * 1000;
                  osg::Vec4 pos = node->GetVec("EndPosition");
                  TransformData* data = GetOrCreateTransformData(time);
                  if (data) data->mTransform.SetTranslation(pos.x(), pos.y(), pos.z());
               }

               outLink = node->GetOutputLink("Started");
            }
            // Load a Rotation lerp node.
            if (node->GetType().GetFullName() == "Cinematic.Lerp Actor Rotation")
            {
               if (!actorData.mActor)
               {
                  actorData.mActor = node->GetActor("Actor");
               }
               {
                  int time = node->GetFloat("StartTime") * 1000;
                  osg::Vec4 rot = node->GetVec("StartRotation");
                  TransformData* data = GetOrCreateTransformData(time);
                  if (data) data->mTransform.SetRotation(rot.z(), rot.x(), rot.y());
               }
               {
                  int time = node->GetFloat("EndTime") * 1000;
                  osg::Vec4 rot = node->GetVec("EndRotation");
                  TransformData* data = GetOrCreateTransformData(time);
                  if (data) data->mTransform.SetRotation(rot.z(), rot.x(), rot.y());
               }

               outLink = node->GetOutputLink("Started");
            }
            // Load a Scale lerp node.
            if (node->GetType().GetFullName() == "Cinematic.Lerp Actor Scale")
            {
               if (!actorData.mActor)
               {
                  actorData.mActor = node->GetActor("Actor");
               }
               {
                  int time = node->GetFloat("StartTime") * 1000;
                  osg::Vec4 scale = node->GetVec("StartScale");
                  TransformData* data = GetOrCreateTransformData(time);
                  if (data) data->mTransform.SetRotation(scale.x(), scale.y(), scale.z());
               }
               {
                  int time = node->GetFloat("EndTime") * 1000;
                  osg::Vec4 scale = node->GetVec("EndScale");
                  TransformData* data = GetOrCreateTransformData(time);
                  if (data) data->mTransform.SetRotation(scale.x(), scale.y(), scale.z());
               }

               outLink = node->GetOutputLink("Started");
            }

            // Continue down the chain.
            node = NULL;
            if (outLink && !outLink->GetLinks().empty())
            {
               node = outLink->GetLinks()[0]->GetOwner();
            }
         }
      }
   }

   mSelectedActor = -1;
   ResetUI();

   // Update the play selection so it covers the entire duration.
   BaseEvent* selectionEvent = mUI.mSelectionTrack->GetEventAtIndex(0);
   if (selectionEvent)
   {
      selectionEvent->SetEndTime(mTotalTime * 1000);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnSave()
{
   GetEditor()->GetUndoManager()->BeginMultipleEvents();

   // Start by removing all nodes from the current sub-graph.
   std::vector<dtDirector::Node*> nodes;
   GetGraph()->GetAllNodes(nodes);

   dtDirector::Node* playNode    = NULL;
   dtDirector::Node* reverseNode = NULL;
   dtDirector::Node* stopNode    = NULL;
   dtDirector::Node* pauseNode   = NULL;
   std::vector<dtDirector::Node*> outputNodes;

   int count = (int)nodes.size();
   for (int index = 0; index < count; ++index)
   {
      dtDirector::Node* node = nodes[index];
      if (!node) continue;

      if (node->GetType().GetNodeType() == dtDirector::NodeType::LINK_NODE)
      {
         // Input nodes can only be "Play", "Reverse", "Stop", and "Pause".
         if (node->GetType().GetFullName() == "Core.Input Link")
         {
            std::string name = node->GetString("Name");
            if (name == "Play")
            {
               playNode = node;
               continue;
            }
            else if (name == "Reverse")
            {
               reverseNode = node;
               continue;
            }
            else if (name == "Stop")
            {
               stopNode = node;
               continue;
            }
            else if (name == "Pause")
            {
               pauseNode = node;
               continue;
            }
         }
         // Output links can only remain if they match one of the output links listed
         // in the cinematic.
         else if (node->GetType().GetFullName() == "Core.Output Link")
         {
            std::string name = node->GetString("Name");

            if (name == "Started" || name == "Ended")
            {
               outputNodes.push_back(node);
               continue;
            }

            int outputCount = (int)mOutputData.size();
            for (int outputIndex = 0; outputIndex < outputCount; ++outputIndex)
            {
               OutputData& data = mOutputData[outputIndex];
               if (data.mName == name)
               {
                  outputNodes.push_back(node);
                  continue;
               }
            }
         }
      }

      // If we reach this point, this node should be removed.
      GetEditor()->DeleteNode(node->GetID());
   }

   // Create our scheduler node.
   dtDirector::Node* schedulerNode = dtDirector::NodeManager::GetInstance().CreateNode("Scheduler", "Cinematic", GetGraph());
   if (schedulerNode)
   {
      schedulerNode->SetFloat(mTotalTime, "TotalTime");

      // Create our input links.
      dtDirector::Node* newPlayNode = dtDirector::NodeManager::GetInstance().CreateNode("Input Link", "Core", GetGraph());
      dtDirector::Node* newReverseNode = dtDirector::NodeManager::GetInstance().CreateNode("Input Link", "Core", GetGraph());
      dtDirector::Node* newStopNode = dtDirector::NodeManager::GetInstance().CreateNode("Input Link", "Core", GetGraph());
      dtDirector::Node* newPauseNode = dtDirector::NodeManager::GetInstance().CreateNode("Input Link", "Core", GetGraph());

      newPlayNode->SetString("Play", "Name");
      newReverseNode->SetString("Reverse", "Name");
      newStopNode->SetString("Stop", "Name");
      newPauseNode->SetString("Pause", "Name");

      newPlayNode->SetPosition(osg::Vec2(-200, 0));
      newReverseNode->SetPosition(osg::Vec2(-200, 50));
      newStopNode->SetPosition(osg::Vec2(-200, 100));
      newPauseNode->SetPosition(osg::Vec2(-200, 150));

      newPlayNode->GetOutputLink("Out")->Connect(schedulerNode->GetInputLink("Play"));
      newReverseNode->GetOutputLink("Out")->Connect(schedulerNode->GetInputLink("Reverse"));
      newStopNode->GetOutputLink("Out")->Connect(schedulerNode->GetInputLink("Stop"));
      newPauseNode->GetOutputLink("Out")->Connect(schedulerNode->GetInputLink("Pause"));

      // Now copy the links from the original input nodes, if able.
      if (playNode)
      {
         dtDirector::InputLink* newInLink = &newPlayNode->GetInputLinks()[0];
         dtDirector::InputLink* inLink = &playNode->GetInputLinks()[0];
         std::vector<dtDirector::OutputLink*>& outLinks = inLink->GetLinks();
         int count = (int)outLinks.size();
         for (int index = 0; index < count; ++index)
         {
            newInLink->Connect(outLinks[index]);
         }

         GetEditor()->DeleteNode(playNode->GetID());
      }
      if (reverseNode)
      {
         dtDirector::InputLink* newInLink = &newReverseNode->GetInputLinks()[0];
         dtDirector::InputLink* inLink = &reverseNode->GetInputLinks()[0];
         std::vector<dtDirector::OutputLink*>& outLinks = inLink->GetLinks();
         int count = (int)outLinks.size();
         for (int index = 0; index < count; ++index)
         {
            newInLink->Connect(outLinks[index]);
         }

         GetEditor()->DeleteNode(reverseNode->GetID());
      }
      if (stopNode)
      {
         dtDirector::InputLink* newInLink = &newStopNode->GetInputLinks()[0];
         dtDirector::InputLink* inLink = &stopNode->GetInputLinks()[0];
         std::vector<dtDirector::OutputLink*>& outLinks = inLink->GetLinks();
         int count = (int)outLinks.size();
         for (int index = 0; index < count; ++index)
         {
            newInLink->Connect(outLinks[index]);
         }

         GetEditor()->DeleteNode(stopNode->GetID());
      }
      if (pauseNode)
      {
         dtDirector::InputLink* newInLink = &newPauseNode->GetInputLinks()[0];
         dtDirector::InputLink* inLink = &pauseNode->GetInputLinks()[0];
         std::vector<dtDirector::OutputLink*>& outLinks = inLink->GetLinks();
         int count = (int)outLinks.size();
         for (int index = 0; index < count; ++index)
         {
            newInLink->Connect(outLinks[index]);
         }

         GetEditor()->DeleteNode(pauseNode->GetID());
      }

      GetEditor()->OnNodeCreated(newPlayNode);
      GetEditor()->OnNodeCreated(newReverseNode);
      GetEditor()->OnNodeCreated(newStopNode);
      GetEditor()->OnNodeCreated(newPauseNode);

      // Create our scheduled event list.
      dtDAL::ArrayActorPropertyBase* arrayProp = dynamic_cast<dtDAL::ArrayActorPropertyBase*>(schedulerNode->GetProperty("EventList"));
      if (arrayProp)
      {
         dtDAL::ContainerActorProperty* containerProp = dynamic_cast<dtDAL::ContainerActorProperty*>(arrayProp->GetArrayProperty());
         if (containerProp)
         {
            dtDAL::StringActorProperty* nameProp = dynamic_cast<dtDAL::StringActorProperty*>(containerProp->GetProperty(0));
            dtDAL::FloatActorProperty*  timeProp = dynamic_cast<dtDAL::FloatActorProperty*>(containerProp->GetProperty(1));
            
            if (nameProp && timeProp)
            {
               int outIndex = 0;

               int count = (int)mOutputData.size();
               for (int index = 0; index < count; ++index)
               {
                  OutputData& data = mOutputData[index];

                  arrayProp->Insert(outIndex);
                  arrayProp->SetIndex(outIndex);
                  nameProp->SetValue(data.mName);
                  timeProp->SetValue(data.mTime);

                  outIndex++;
               }
            }
         }
      }

      // Create our started remote event caller.
      dtDirector::Node* startedCallNode = dtDirector::NodeManager::GetInstance().CreateNode("Call Remote Event", "Core", GetGraph());
      if (startedCallNode)
      {
         schedulerNode->GetOutputLink("Started")->Connect(startedCallNode->GetInputLink("Call Event"));
         startedCallNode->SetString("Started", "EventName");
         startedCallNode->SetPosition(osg::Vec2(400, 0));
         GetEditor()->OnNodeCreated(startedCallNode);
      }

      // Now create our output links for each scheduled event.
      int height = 50;
      std::vector<dtDirector::OutputLink>& links = schedulerNode->GetOutputLinks();
      int outNodeCount = (int)outputNodes.size();
      int linkCount = (int)links.size();
      for (int linkIndex = 0; linkIndex < linkCount; ++linkIndex)
      {
         dtDirector::OutputLink& link = links[linkIndex];

         // find out if the output already exists.
         dtDirector::Node* outputNode = NULL;
         for (int outNodeIndex = 0; outNodeIndex < outNodeCount; ++outNodeIndex)
         {
            if (outputNodes[outNodeIndex]->GetString("Name") == link.GetName())
            {
               outputNode = outputNodes[outNodeIndex];
               outputNodes.erase(outputNodes.begin() + outNodeIndex);
               break;
            }
         }

         // Create the output node if needed.
         dtDirector::Node* newOutputNode = dtDirector::NodeManager::GetInstance().CreateNode("Output Link", "Core", GetGraph());

         // Position and connect this output node.
         if (newOutputNode)
         {
            link.Connect(newOutputNode->GetInputLink("In"));
            newOutputNode->SetPosition(osg::Vec2(400, height));
            newOutputNode->SetString(link.GetName(), "Name");

            // Copy links from the original output node, if able
            if (outputNode)
            {
               dtDirector::OutputLink* newOutLink = &newOutputNode->GetOutputLinks()[0];
               dtDirector::OutputLink* outLink = &outputNode->GetOutputLinks()[0];
               std::vector<dtDirector::InputLink*>& inLinks = outLink->GetLinks();
               int count = (int)inLinks.size();
               for (int index = 0; index < count; ++index)
               {
                  newOutLink->Connect(inLinks[index]);
               }

               GetEditor()->DeleteNode(outputNode->GetID());
            }

            GetEditor()->OnNodeCreated(newOutputNode);
            height += 50;
         }
      }

      // Create our ended remote event caller.
      dtDirector::Node* endedCallNode = dtDirector::NodeManager::GetInstance().CreateNode("Call Remote Event", "Core", GetGraph());
      if (endedCallNode)
      {
         schedulerNode->GetOutputLink("Ended")->Connect(endedCallNode->GetInputLink("Call Event"));
         endedCallNode->SetString("Ended", "EventName");
         endedCallNode->SetPosition(osg::Vec2(400, height));
         GetEditor()->OnNodeCreated(endedCallNode);
         height += 50;
      }

      // Create our current time value node.
      dtDirector::Node* timeValueNode = dtDirector::NodeManager::GetInstance().CreateNode("Float", "General", GetGraph());
      if (timeValueNode)
      {
         timeValueNode->SetPosition(osg::Vec2(0, height));
         timeValueNode->SetString("Current Time", "Name");
         schedulerNode->GetValueLink("Time")->Connect(dynamic_cast<dtDirector::ValueNode*>(timeValueNode));
      }

      GetEditor()->OnNodeCreated(schedulerNode);

      // Now create our transformation chains for each actor.
      height += 100;
      int actorCount = (int)mActorData.size();
      for (int actorIndex = 0; actorIndex < actorCount; ++actorIndex)
      {
         ActorData& actorData = mActorData[actorIndex];

         int column = 0;

         dtDirector::OutputLink* startLink = NULL;
         dtDirector::OutputLink* stopLink = NULL;

         // Start with our start and end remote events to start the chain.
         dtDirector::Node* startEvent = dtDirector::NodeManager::GetInstance().CreateNode("Remote Event", "Core", GetGraph());
         if (startEvent)
         {
            startEvent->SetString("Started", "EventName");
            startEvent->SetPosition(osg::Vec2(column, height));
            GetEditor()->OnNodeCreated(startEvent);
            startLink = startEvent->GetOutputLink("Out");
         }

         dtDirector::Node* endEvent   = dtDirector::NodeManager::GetInstance().CreateNode("Remote Event", "Core", GetGraph());
         if (endEvent)
         {
            endEvent->SetString("Ended", "EventName");
            endEvent->SetPosition(osg::Vec2(column + 6, height + 100));
            GetEditor()->OnNodeCreated(endEvent);
            stopLink = endEvent->GetOutputLink("Out");
         }

         column += 200;

         // Iterate through each transformation key frame.
         int count = (int)actorData.mTransformData.size();
         for (int index = 0; index < count - 1; ++index)
         {
            TransformData& prevData = actorData.mTransformData[index];
            TransformData& nextData = actorData.mTransformData[index+1];

            // Translation.
            if (prevData.mTransform.GetTranslation() != nextData.mTransform.GetTranslation())
            {
               dtDirector::Node* lerpNode = dtDirector::NodeManager::GetInstance().CreateNode("Lerp Actor Translation", "Cinematic", GetGraph());
               if (lerpNode)
               {
                  lerpNode->GetInputLink("Start")->Connect(startLink);
                  lerpNode->GetInputLink("Stop")->Connect(stopLink);
                  lerpNode->SetActorID(actorData.mActor->GetId(), "Actor");
                  lerpNode->SetFloat(prevData.mTime * 0.001f, "StartTime");
                  lerpNode->SetFloat(nextData.mTime * 0.001f, "EndTime");
                  lerpNode->SetVec(osg::Vec4(prevData.mTransform.GetTranslation(), 0.0f), "StartPosition");
                  lerpNode->SetVec(osg::Vec4(nextData.mTransform.GetTranslation(), 0.0f), "EndPosition");
                  lerpNode->SetPosition(osg::Vec2(column, height));
                  GetEditor()->OnNodeCreated(lerpNode);
                  startLink = lerpNode->GetOutputLink("Started");
                  stopLink = lerpNode->GetOutputLink("Stopped");

                  dtDirector::Node* timeRefValue = dtDirector::NodeManager::GetInstance().CreateNode("Reference", "Core", GetGraph());
                  if (timeRefValue)
                  {
                     timeRefValue->SetString("Current Time", "Reference");
                     timeRefValue->SetPosition(osg::Vec2(column + 35, height + 200));
                     lerpNode->GetValueLink("Time")->Connect(dynamic_cast<dtDirector::ValueNode*>(timeRefValue));
                     GetEditor()->OnNodeCreated(timeRefValue);
                     column += 400;
                  }
               }
            }

            // Rotation.
            if (prevData.mTransform.GetRotation() != nextData.mTransform.GetRotation())
            {
               dtDirector::Node* lerpNode = dtDirector::NodeManager::GetInstance().CreateNode("Lerp Actor Rotation", "Cinematic", GetGraph());
               if (lerpNode)
               {
                  lerpNode->GetInputLink("Start")->Connect(startLink);
                  lerpNode->GetInputLink("Stop")->Connect(stopLink);
                  lerpNode->SetActorID(actorData.mActor->GetId(), "Actor");
                  lerpNode->SetFloat(prevData.mTime * 0.001f, "StartTime");
                  lerpNode->SetFloat(nextData.mTime * 0.001f, "EndTime");
                  osg::Vec3 prevRot = prevData.mTransform.GetRotation();
                  osg::Vec3 nextRot = nextData.mTransform.GetRotation();
                  lerpNode->SetVec(osg::Vec4(prevRot.y(), prevRot.z(), prevRot.x(), 0.0f), "StartRotation");
                  lerpNode->SetVec(osg::Vec4(nextRot.y(), nextRot.z(), nextRot.x(), 0.0f), "EndRotation");
                  lerpNode->SetPosition(osg::Vec2(column, height));
                  GetEditor()->OnNodeCreated(lerpNode);
                  startLink = lerpNode->GetOutputLink("Started");
                  stopLink = lerpNode->GetOutputLink("Stopped");

                  dtDirector::Node* timeRefValue = dtDirector::NodeManager::GetInstance().CreateNode("Reference", "Core", GetGraph());
                  if (timeRefValue)
                  {
                     timeRefValue->SetString("Current Time", "Reference");
                     timeRefValue->SetPosition(osg::Vec2(column + 35, height + 200));
                     lerpNode->GetValueLink("Time")->Connect(dynamic_cast<dtDirector::ValueNode*>(timeRefValue));
                     GetEditor()->OnNodeCreated(timeRefValue);
                     column += 400;
                  }
               }
            }

            // Scale.
            if (prevData.mScale != nextData.mScale)
            {
               dtDirector::Node* lerpNode = dtDirector::NodeManager::GetInstance().CreateNode("Lerp Actor Scale", "Cinematic", GetGraph());
               if (lerpNode)
               {
                  lerpNode->GetInputLink("Start")->Connect(startLink);
                  lerpNode->GetInputLink("Stop")->Connect(stopLink);
                  lerpNode->SetActorID(actorData.mActor->GetId(), "Actor");
                  lerpNode->SetFloat(prevData.mTime * 0.001f, "StartTime");
                  lerpNode->SetFloat(nextData.mTime * 0.001f, "EndTime");
                  lerpNode->SetVec(osg::Vec4(prevData.mScale, 0.0f), "StartScale");
                  lerpNode->SetVec(osg::Vec4(nextData.mScale, 0.0f), "EndScale");
                  lerpNode->SetPosition(osg::Vec2(column, height));
                  GetEditor()->OnNodeCreated(lerpNode);
                  startLink = lerpNode->GetOutputLink("Started");
                  stopLink = lerpNode->GetOutputLink("Stopped");

                  dtDirector::Node* timeRefValue = dtDirector::NodeManager::GetInstance().CreateNode("Reference", "Core", GetGraph());
                  if (timeRefValue)
                  {
                     timeRefValue->SetString("Current Time", "Reference");
                     timeRefValue->SetPosition(osg::Vec2(column + 35, height + 200));
                     lerpNode->GetValueLink("Time")->Connect(dynamic_cast<dtDirector::ValueNode*>(timeRefValue));
                     GetEditor()->OnNodeCreated(timeRefValue);
                     column += 400;
                  }
               }
            }
         }
      
         height += 400;
      }
   }

   GetEditor()->Refresh();

   //GetEditor()->GetPropertyEditor()->GetScene()->CreateNode();
   GetEditor()->GetUndoManager()->EndMultipleEvents();
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnSaveAndClose()
{
   OnSave();
   OnCancel();
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnCancel()
{
   Close();
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::GotoSelectedActor()
{
   if (mSelectedActor > -1)
   {
      std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > selected;
      selected.push_back(mActorData[mSelectedActor].mActor.get());
      EditorEvents::GetInstance().emitActorsSelected(selected);
      //EditorEvents::GetInstance().emitGotoActor(selected[0]);
   }
}

////////////////////////////////////////////////////////////////////////////////
KeyFrameEvent* DirectorCinematicEditorPlugin::InsertTransform(int time, const dtCore::Transform& transform, const osg::Vec3& scale, bool movable, KeyFrameEvent* event)
{
   if (mSelectedActor == -1) return NULL;

   KeyFrameEvent* newEvent = event;
   
   if (!newEvent)
   {
      newEvent = new KeyFrameEvent(time);
      newEvent->SetMovable(movable);
      mUI.mTransformTrack->AddEvent(newEvent);
   }

   std::vector<TransformData>& transformList = mActorData[mSelectedActor].mTransformData;
   int count = (int)transformList.size();
   for (int index = 0; index < count; ++index)
   {
      TransformData& data = transformList[index];

      if (data.mTime >= time)
      {
         transformList.insert(transformList.begin() + index, TransformData(time, transform, scale, newEvent));
         return newEvent;
      }
   }

   // If we get this far, it means the new transform will be appended to the end instead.
   transformList.push_back(TransformData(time, transform, scale, newEvent));
   return newEvent;
}

////////////////////////////////////////////////////////////////////////////////
DirectorCinematicEditorPlugin::TransformData* DirectorCinematicEditorPlugin::GetTransformData(KeyFrameEvent* event, int* outIndex)
{
   if (mSelectedActor > -1)
   {
      std::vector<TransformData>& transformList = mActorData[mSelectedActor].mTransformData;
      int count = (int)transformList.size();
      for (int index = 0; index < count; ++index)
      {
         TransformData& data = transformList[index];
         if (data.mEvent == event)
         {
            if (outIndex) *outIndex = index;
            return &data;
         }
      }
   }

   if (outIndex) *outIndex = -1;
   return NULL;
}

////////////////////////////////////////////////////////////////////////////////
DirectorCinematicEditorPlugin::TransformData* DirectorCinematicEditorPlugin::GetOrCreateTransformData(int time)
{
   if (mSelectedActor > -1)
   {
      dtCore::Transform transform;
      osg::Vec3 scale(1.0f, 1.0f, 1.0f);

      std::vector<TransformData>& transformList = mActorData[mSelectedActor].mTransformData;
      int count = (int)transformList.size();
      for (int index = 0; index < count; ++index)
      {
         TransformData& data = transformList[index];
         if (data.mTime == time)
         {
            return &data;
         }

         // If we have surpassed our time in this sorted list, then it won't exist, so create it now.
         if (data.mTime >= time)
         {
            transformList.insert(transformList.begin() + index, TransformData(time, transform, scale, NULL));
            return &transformList[index];
         }
      }

      // If we get this far, it means the new transform will be appended to the end instead.
      transformList.push_back(TransformData(time, transform, scale, NULL));
      return &transformList[transformList.size() - 1];
   }

   return NULL;
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::LerpActors(int time)
{
   // Iterate through all actors in the list.
   int actorCount = (int)mActorData.size();
   for (int actorIndex = 0; actorIndex < actorCount; ++actorIndex)
   {
      ActorData& actorData = mActorData[actorIndex];
      if (!actorData.mActor.valid()) continue;

      dtCore::Transformable* actor = NULL;
      actorData.mActor->GetActor(actor);
      if (!actor) continue;

      // Transformation
      {
         // Find the previous and next key frames.
         TransformData* prev = NULL;
         TransformData* next = NULL;

         int count = (int)actorData.mTransformData.size();
         for (int index = 0; index < count; ++index)
         {
            TransformData& data = actorData.mTransformData[index];

            if (data.mTime <= time)
            {
               prev = &data;
               next = &data;

               if (index + 1 < count)
               {
                  next = &actorData.mTransformData[index + 1];
               }
               continue;
            }
            break;
         }

         // Lerp
         if (prev && next)
         {
            // Determine lerp alpha
            int startTime = prev->mTime;
            int endTime = next->mTime;
            int curTime = time;
            float alpha = 0.0f;

            if (curTime < startTime) curTime = startTime;
            if (curTime > endTime) curTime = endTime;

            int duration = endTime - startTime;
            if (duration > 0)
            {
               alpha = (curTime - startTime) / (float)duration;
            }

            // Lerp position
            osg::Vec3 startPos = prev->mTransform.GetTranslation();
            osg::Vec3 endPos = next->mTransform.GetTranslation();
            osg::Vec3 newPos = startPos + ((endPos - startPos) * alpha);

            // Lerp Rotation
            osg::Quat startRot(osg::Vec4(prev->mTransform.GetRotation(), 0.0f));
            osg::Quat endRot(osg::Vec4(next->mTransform.GetRotation(), 0.0f));
            osg::Quat lerpRot;
            lerpRot.slerp(alpha, startRot, endRot);
            osg::Vec3 newRot = lerpRot.asVec3();

            // Lerp Scale
            osg::Vec3 startScale = prev->mScale;
            osg::Vec3 endScale = next->mScale;
            osg::Vec3 newScale = startScale + ((endScale - startScale) * alpha);

            // Apply new transform
            dtCore::Transform transform;
            actor->GetTransform(transform);
            transform.SetTranslation(newPos);
            transform.SetRotation(newRot.x(), newRot.y(), newRot.z());
            actor->SetTransform(transform);

            dtCore::Object* obj = NULL;
            actorData.mActor->GetActor(obj);
            if (obj) obj->SetScale(newScale);
         }
      }
   }

   ViewportManager::GetInstance().refreshAllViewports();
}

//////////////////////////////////////////////////////////////////////////
namespace DirectorCinematicEditor
{
class DT_DIRECTOR_CINEMATIC_EDITOR_EXPORT PluginFactory : public dtEditQt::PluginFactory
{
public:

   PluginFactory() {}
   ~PluginFactory() {}

   /** get the name of the plugin */
   virtual std::string GetName() { return DirectorCinematicEditorPlugin::PLUGIN_NAME; }

   /** get a description of the plugin */
   virtual std::string GetDescription() { return "Plugin to access the editing tool for a Director Script."; }

   virtual void GetDependencies(std::list<std::string>& deps) 
   {
      deps.push_back("Director Tool");
   }

   /** construct the plugin and return a pointer to it */
   virtual Plugin* Create(MainWindow* mw) 
   {
      mPlugin = new DirectorCinematicEditorPlugin(mw);
      return mPlugin;
   }

   virtual void Destroy() 
   {
      delete mPlugin;
   }

private:

   Plugin* mPlugin;
}; 
} //namespace DirectorCinematicEditorPlugin

extern "C" DT_DIRECTOR_CINEMATIC_EDITOR_EXPORT dtEditQt::PluginFactory* CreatePluginFactory()
{
   return new DirectorCinematicEditor::PluginFactory;
}
