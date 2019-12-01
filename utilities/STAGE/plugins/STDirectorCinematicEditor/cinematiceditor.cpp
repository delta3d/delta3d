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

#include <dtDirectorQt/propertyeditor.h>
#include <dtDirectorQt/editorscene.h>

#include <dtDirectorNodes/scheduleraction.h>
#include <dtDirectorAnimNodes/animateactoraction.h>

#include <dtCore/object.h>
#include <dtCore/arrayactorpropertybase.h>
#include <dtCore/containeractorproperty.h>
#include <dtCore/stringactorproperty.h>
#include <dtCore/floatactorproperty.h>
#include <dtCore/booleanactorproperty.h>

#include <QtCore/QTimeLine>

#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/animationgameactor.h>
#include <dtAnim/animationhelper.h>
#include <dtAnim/sequencemixer.h>

#include <cal3d/model.h>
#include <cal3d/mixer.h>

const std::string DirectorCinematicEditorPlugin::PLUGIN_NAME = "Director Cinematic Editor";

////////////////////////////////////////////////////////////////////////////////
DirectorCinematicEditorPlugin::DirectorCinematicEditorPlugin(MainWindow* mw)
: dtDirector::CustomEditorTool("Cinematic")
, mTimer(new QTimer())
, mMainWindow(mw)
, mSelectedActor(-1)
, mTransformEvent(NULL)
, mAnimationEvent(NULL)
, mOutputEvent(NULL)
, mPlaying(false)
, mTotalTime(1.0f)
{
   mUI.setupUi(this);

   setWindowTitle("Director Cinematic Editor");

   Initialize();
}

////////////////////////////////////////////////////////////////////////////////
DirectorCinematicEditorPlugin::~DirectorCinematicEditorPlugin()
{
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::Initialize()
{
   connect(&EditorEvents::GetInstance(), SIGNAL(selectedActors(ActorRefPtrVector &)),
      this, SLOT(onActorsSelected(ActorRefPtrVector &)));
   connect(&ViewportManager::GetInstance(), SIGNAL(endActorMode(Viewport*, QMouseEvent*, bool*)),
      this, SLOT(onEndActorMode(Viewport*, QMouseEvent*, bool*)));

   // Actor listing
   connect(mUI.mActorCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(OnActorComboChanged(int)));
   connect(mUI.mAddActorButton, SIGNAL(clicked()), this, SLOT(OnAddActor()));
   connect(mUI.mRemoveActorButton, SIGNAL(clicked()), this, SLOT(OnRemoveActor()));

   // Transform Event Track.
   connect(mUI.mTransformEnabled, SIGNAL(stateChanged(int)), this, SLOT(OnTransformEnabled(int)));
   connect(mUI.mTransformTrack, SIGNAL(EventSelected(BaseEvent*)), this, SLOT(OnTransformEventSelected(BaseEvent*)));
   connect(mUI.mTransformTrack, SIGNAL(EventTimesChanged(int, int)), this, SLOT(OnTransformEventTimesChanged(int, int)));
   connect(mUI.mTransformTrack, SIGNAL(EventDeleted(BaseEvent*)), this, SLOT(OnTransformEventRemoved(BaseEvent*)));
   connect(mUI.mAddTransformButton, SIGNAL(clicked()), this, SLOT(OnAddTransform()));
   connect(mUI.mRemoveTransformButton, SIGNAL(clicked()), this, SLOT(OnRemoveTransform()));

   connect(mUI.mTransformPosXEdit, SIGNAL(textEdited(QString)), this, SLOT(OnPosXChanged(QString)));
   connect(mUI.mTransformPosYEdit, SIGNAL(textEdited(QString)), this, SLOT(OnPosYChanged(QString)));
   connect(mUI.mTransformPosZEdit, SIGNAL(textEdited(QString)), this, SLOT(OnPosZChanged(QString)));
   connect(mUI.mTransformRotXEdit, SIGNAL(textEdited(QString)), this, SLOT(OnRotXChanged(QString)));
   connect(mUI.mTransformRotYEdit, SIGNAL(textEdited(QString)), this, SLOT(OnRotYChanged(QString)));
   connect(mUI.mTransformRotZEdit, SIGNAL(textEdited(QString)), this, SLOT(OnRotZChanged(QString)));
   connect(mUI.mTransformScaleXEdit, SIGNAL(textEdited(QString)), this, SLOT(OnScaleXChanged(QString)));
   connect(mUI.mTransformScaleYEdit, SIGNAL(textEdited(QString)), this, SLOT(OnScaleYChanged(QString)));
   connect(mUI.mTransformScaleZEdit, SIGNAL(textEdited(QString)), this, SLOT(OnScaleZChanged(QString)));

   // Animation Event Track.
   connect(mUI.mAnimationTrack, SIGNAL(EventSelected(BaseEvent*)), this, SLOT(OnAnimationEventSelected(BaseEvent*)));
   connect(mUI.mAnimationTrack, SIGNAL(EventTimesChanged(int, int)), this, SLOT(OnAnimationEventTimesChanged(int, int)));
   connect(mUI.mAnimationTrack, SIGNAL(EventDeleted(BaseEvent*)), this, SLOT(OnAnimationEventRemoved(BaseEvent*)));
   connect(mUI.mAddAnimationButton, SIGNAL(clicked()), this, SLOT(OnAddAnimation()));
   connect(mUI.mRemoveAnimationButton, SIGNAL(clicked()), this, SLOT(OnRemoveAnimation()));

   connect(mUI.mAnimationCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(OnAnimationComboChanged(int)));
   connect(mUI.mAnimationSpeedEdit, SIGNAL(valueChanged(double)), this, SLOT(OnAnimationSpeedChanged(double)));
   connect(mUI.mAnimationStartTimeEdit, SIGNAL(valueChanged(double)), this, SLOT(OnAnimationStartTimeChanged(double)));
   connect(mUI.mAnimationEndTimeEdit, SIGNAL(valueChanged(double)), this, SLOT(OnAnimationEndTimeChanged(double)));
   connect(mUI.mAnimationFadeInTimeEdit, SIGNAL(valueChanged(double)), this, SLOT(OnAnimationBlendInTimeChanged(double)));
   connect(mUI.mAnimationFadeOutTimeEdit, SIGNAL(valueChanged(double)), this, SLOT(OnAnimationBlendOutTimeChanged(double)));
   connect(mUI.mAnimationWeightEdit, SIGNAL(valueChanged(double)), this, SLOT(OnAnimationWeightChanged(double)));

   // Output Link Event Track.
   connect(mUI.mOutputTrack, SIGNAL(EventSelected(BaseEvent*)), this, SLOT(OnOutputEventSelected(BaseEvent*)));
   connect(mUI.mOutputTrack, SIGNAL(EventTimesChanged(int, int)), this, SLOT(OnOutputEventTimesChanged(int, int)));
   connect(mUI.mOutputTrack, SIGNAL(EventDeleted(BaseEvent*)), this, SLOT(OnOutputEventRemoved(BaseEvent*)));
   connect(mUI.mAddOutputButton, SIGNAL(clicked()), this, SLOT(OnAddOutput()));
   connect(mUI.mRemoveOutputButton, SIGNAL(clicked()), this, SLOT(OnRemoveOutput()));
   connect(mUI.mOutputNameEdit, SIGNAL(textEdited(QString)), this, SLOT(OnOutputNameChanged(QString)));
   connect(mUI.mOutputTriggerPlay, SIGNAL(stateChanged(int)), this, SLOT(OnOutputTriggerPlay(int)));
   connect(mUI.mOutputTriggerReverse, SIGNAL(stateChanged(int)), this, SLOT(OnOutputTriggerReverse(int)));

   // Create a time line.
   mTimeLine = new QTimeLine(1000, this);
   mTimeLine->setCurveShape(QTimeLine::LinearCurve);

   connect(mUI.mStartTimeEdit, SIGNAL(valueChanged(double)), this, SLOT(OnStartTimeChanged(double)));
   connect(mUI.mEndTimeEdit, SIGNAL(valueChanged(double)), this, SLOT(OnEndTimeChanged(double)));

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

   // Update
   connect(mTimer, SIGNAL(timeout()), this, SLOT(OnUpdate()));
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
      dtCore::BaseActorObject* proxy = mActorData[index].mActor.get();
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
      mUI.mTransformEnabled->setEnabled(false);
   }
   mUI.mActorCombo->blockSignals(false);

   OnTransformEventSelected(NULL);
   OnAnimationEventSelected(NULL);
   OnOutputEventSelected(NULL);

   mUI.mTransformTrack->ClearEvents();
   mUI.mAnimationTrack->ClearEvents();
   mUI.mAnimationCombo->clear();

   if (mSelectedActor > -1)
   {
      mUI.mTransformEnabled->setEnabled(true);

      // Set up the transform track.
      if (mActorData[mSelectedActor].mTransformEnabled)
      {
         mUI.mTransformEnabled->setChecked(true);
         mUI.mTransformTrack->setEnabled(true);

         std::vector<TransformData>& transforms = mActorData[mSelectedActor].mTransformData;
         int count = (int)transforms.size();
         for (int index = 0; index < count; ++index)
         {
            TransformData& data = transforms[index];
            data.mEvent = new KeyFrameEvent(data.mTime);
            if (index == 0) data.mEvent->SetMovable(false);
            mUI.mTransformTrack->AddEvent(data.mEvent);
         }
      }
      else
      {
         mUI.mTransformEnabled->setChecked(false);
      }

      // Set up the animation track.
      mUI.mAnimationTrack->setEnabled(false);
      mUI.mAddAnimationButton->setEnabled(false);
      mUI.mRemoveAnimationButton->setEnabled(false);

      dtGame::GameActorProxy* animActor = dynamic_cast<dtGame::GameActorProxy*>(mActorData[mSelectedActor].mActor.get());
      if (animActor)
      {
         dtAnim::AnimationHelper* helper = animActor->GetComponent<dtAnim::AnimationHelper>();
         if (helper)
         {
            dtAnim::SequenceMixer& mixer = helper->GetSequenceMixer();
            std::vector<const dtAnim::Animatable*> anims;
            mixer.GetRegisteredAnimations(anims);
            if (anims.size())
            {
               mUI.mAnimationTrack->setEnabled(true);
               mUI.mAddAnimationButton->setEnabled(true);
               mUI.mRemoveAnimationButton->setEnabled(true);

               count = (int)anims.size();
               for (int index = 0; index < count; ++index)
               {
                  const dtAnim::Animatable* anim = anims[index];
                  std::string name = anim->GetName();

                  mUI.mAnimationCombo->addItem(name.c_str());
               }
            }
         }

         // Add all our current animation events.
         std::vector<AnimationData>& anims = mActorData[mSelectedActor].mAnimationData;
         int count = (int)anims.size();
         for (int index = 0; index < count; ++index)
         {
            AnimationData& data = anims[index];
            int duration = (data.mEndTime - data.mStartTime) * 1000 / data.mSpeed;
            data.mEvent = new AnimationEvent(data.mTime, data.mTime + duration);
            data.mEvent->SetBlendIn(data.mBlendInTime * 1000);
            data.mEvent->SetBlendOut(data.mBlendOutTime * 1000);
            data.mEvent->SetWeight(data.mWeight);
            mUI.mAnimationTrack->AddEvent(data.mEvent);
         }
      }
   }
   else
   {
      mUI.mTransformTrack->setEnabled(false);
      mUI.mAddTransformButton->setEnabled(false);
      mUI.mRemoveTransformButton->setEnabled(false);
   }

   mUI.mOutputTrack->setEnabled(true);
   OnTimeSliderValueChanged(mUI.mTimeSlider->value());

   float startTime = mUI.mStartTimeEdit->value();
   float endTime   = mUI.mEndTimeEdit->value();

   mUI.mTransformTrack->SetMinimum(startTime * 1000, false);
   mUI.mAnimationTrack->SetMinimum(startTime * 1000, false);
   mUI.mOutputTrack->SetMinimum(startTime * 1000, false);
   mUI.mSelectionTrack->SetMinimum(startTime * 1000);
   mUI.mTimeSlider->setMinimum(startTime * 1000);

   mUI.mTransformTrack->SetMaximum(endTime * 1000, false);
   mUI.mAnimationTrack->SetMaximum(endTime * 1000, false);
   mUI.mOutputTrack->SetMaximum(endTime * 1000, false);
   mUI.mSelectionTrack->SetMaximum(endTime * 1000);
   mUI.mTimeSlider->setMaximum(endTime * 1000);

   if (!NEAR_EQUAL(mUI.mTotalTimeEdit->value(), mTotalTime)) mUI.mTotalTimeEdit->setValue(mTotalTime);

   dtCore::ActorPtrVector selection;
   EditorData::GetInstance().GetSelectedActors(selection);

   mUI.mAddActorButton->setEnabled(!selection.empty());

}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::Open(dtDirector::DirectorEditor* editor, dtDirector::DirectorGraph* graph)
{
   CustomEditorTool::Open(editor, graph);

   OnLoad();

   show();

   mTimer->setInterval(100);
   mTimer->setSingleShot(false);
   mTimer->start();
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::Close()
{
   // Stop current playback.
   OnTimeLineFinished();

   // Reset the preview time slider back to the start so all actors reset.
   mUI.mTimeSlider->setValue(0);
   mTimer->stop();

   // Make sure we clear all animations on all skeletal mesh actors.
   bool refresh = false;
   int count = (int)mActorData.size();
   for (int index = 0; index < count; ++index)
   {
      dtCore::Transformable* actor = NULL;
      mActorData[index].mActor->GetDrawable(actor);

      dtAnim::AnimationGameActor* animActor = dynamic_cast<dtAnim::AnimationGameActor*>(actor);
      if (animActor)
      {
         dtAnim::SequenceMixer& mixer = animActor->GetComponent<dtAnim::AnimationHelper>()->GetSequenceMixer();
         mixer.ClearActiveAnimations(0.0f);
         animActor->GetComponent<dtAnim::AnimationHelper>()->Update(0.0f);
         refresh = true;
      }
   }

   if (refresh)
   {
      ViewportManager::GetInstance().refreshAllViewports();
   }

   CustomEditorTool::Close();
   hide();
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::Destroy()
{
   Close();
}

//////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::closeEvent(QCloseEvent* /*event*/)
{
   Close();
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnUpdate()
{
   //int count = (int)mActorData.size();
   //for (int index = 0; index < count; ++index)
   //{
   //   dtAnim::AnimationGameActor* animActor = NULL;
   //   animActor = dynamic_cast<dtAnim::AnimationGameActor*>(mActorData[index].mActor->GetDrawable());
   //   if (animActor)
   //   {
   //      animActor->GetComponent<dtAnim::AnimationHelper>()->Update(0.0f);
   //   }
   //}
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::onActorsSelected(ActorRefPtrVector& actors)
{
   mSelectedActor = -1;

   // If we select an actor in STAGE, try and find that actor in our cinematic actor list.
   if (actors.size() == 1)
   {
      int count = (int)mActorData.size();
      for (int index = 0; index < count; ++index)
      {
         dtCore::BaseActorObject* proxy = mActorData[index].mActor.get();
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
void DirectorCinematicEditorPlugin::onEndActorMode(Viewport* /*vp*/, QMouseEvent* /*e*/, bool* /*overrideDefault*/)
{
   if (mSelectedActor == -1 || !mTransformEvent) return;

   // Update the transform data if we have one selected.
   TransformData* data = GetTransformData(mTransformEvent);
   if (data)
   {
      dtCore::Transformable* txable = NULL;
      mActorData[mSelectedActor].mActor->GetDrawable(txable);
      if (txable)
      {
         dtCore::Transform transform;
         txable->GetTransform(transform);
         data->mTransform = transform;

         dtCore::Object* obj = dynamic_cast<dtCore::Object*>(txable);
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
   dtCore::ActorPtrVector selection;
   EditorData::GetInstance().GetSelectedActors(selection);

   int addCount = (int)selection.size();
   for (int addIndex = 0; addIndex < addCount; ++addIndex)
   {
      bool canAdd = true;
      int count = (int)mActorData.size();
      for (int index = 0; index < count; ++index)
      {
         dtCore::BaseActorObject* proxy = mActorData[index].mActor.get();
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
            dtCore::Transformable* txable = NULL;
            data.mActor->GetDrawable(txable);
            if (txable)
            {
               dtCore::Transform transform;
               osg::Vec3 scale = osg::Vec3(1, 1, 1);

               txable->GetTransform(transform);

               bool canScale = false;
               dtCore::Object* obj = dynamic_cast<dtCore::Object*>(txable);
               if (obj)
               {
                  scale = obj->GetScale();
                  canScale = true;
               }

               InsertTransform(0, transform, scale, canScale, false);
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

   // Make sure we clear all animations on the removed skeletal mesh actor.
   dtCore::Transformable* txable = NULL;
   mActorData[mSelectedActor].mActor->GetDrawable(txable);

   dtAnim::AnimationGameActor* animActor = dynamic_cast<dtAnim::AnimationGameActor*>(txable);
   if (animActor)
   {
      dtAnim::SequenceMixer& mixer = animActor->GetComponent<dtAnim::AnimationHelper>()->GetSequenceMixer();
      mixer.ClearActiveAnimations(0.0f);
      animActor->GetComponent<dtAnim::AnimationHelper>()->Update(0.0f);
      ViewportManager::GetInstance().refreshAllViewports();
   }

   //dtCore::BaseActorObject* proxy = mActorData[mSelectedActor].mActor.get();
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
void DirectorCinematicEditorPlugin::OnTransformEnabled(int state)
{
   if (mSelectedActor == -1) return;

   ActorData& data = mActorData[mSelectedActor];

   // Turn on.
   if (state)
   {
      data.mTransformEnabled = true;

      // Make sure we have our origin transform key frame.
      if (data.mTransformData.empty())
      {
         // Add our root transform event.
         dtCore::Transformable* txable = NULL;
         data.mActor->GetDrawable(txable);
         if (txable)
         {
            dtCore::Transform transform;
            osg::Vec3 scale = osg::Vec3(1, 1, 1);

            txable->GetTransform(transform);

            bool canScale = false;
            dtCore::Object* obj = dynamic_cast<dtCore::Object*>(txable);
            if (obj)
            {
               scale = obj->GetScale();
               canScale = true;
            }

            InsertTransform(0, transform, scale, canScale, false);
         }
      }
   }
   // Turn off.
   else
   {
      // First, make sure we lerp everything back to time zero.
      LerpActors(0);

      // Now disable transforms on this actor.
      data.mTransformEnabled = false;

      // And restore the current cinematic lerp position.
      LerpActors(mUI.mTimeSlider->value());
   }

   ResetUI();
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnTransformEventSelected(BaseEvent* event)
{
   bool enable = false;
   bool canScale = false;

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
         canScale = data->mCanScale;

         mUI.mTransformPosXEdit->setText(dtUtil::ToString(pos.x()).c_str());
         mUI.mTransformPosYEdit->setText(dtUtil::ToString(pos.y()).c_str());
         mUI.mTransformPosZEdit->setText(dtUtil::ToString(pos.z()).c_str());
         mUI.mTransformRotXEdit->setText(dtUtil::ToString(rot.x()).c_str());
         mUI.mTransformRotYEdit->setText(dtUtil::ToString(rot.y()).c_str());
         mUI.mTransformRotZEdit->setText(dtUtil::ToString(rot.z()).c_str());
         mUI.mTransformScaleXEdit->setText(dtUtil::ToString(scale.x()).c_str());
         mUI.mTransformScaleYEdit->setText(dtUtil::ToString(scale.y()).c_str());
         mUI.mTransformScaleZEdit->setText(dtUtil::ToString(scale.z()).c_str());
      }
   }
   else
   {
      mUI.mAddTransformButton->setEnabled(true);
      mUI.mRemoveTransformButton->setEnabled(false);
      mUI.mTransformTrack->DeselectEvents();
   }

   mUI.mTransformPosXEdit->setEnabled(enable);
   mUI.mTransformPosYEdit->setEnabled(enable);
   mUI.mTransformPosZEdit->setEnabled(enable);
   mUI.mTransformRotXEdit->setEnabled(enable);
   mUI.mTransformRotYEdit->setEnabled(enable);
   mUI.mTransformRotZEdit->setEnabled(enable);
   mUI.mTransformScaleXEdit->setEnabled(enable && canScale);
   mUI.mTransformScaleYEdit->setEnabled(enable && canScale);
   mUI.mTransformScaleZEdit->setEnabled(enable && canScale);

   mTransformEvent = transformEvent;
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnTransformEventTimesChanged(int start, int /*end*/)
{
   int index = -1;
   TransformData* data = GetTransformData(mTransformEvent, &index);
   if (data)
   {
      TransformData sortData = *data;
      sortData.mTime = start;

      mActorData[mSelectedActor].mTransformData.erase(mActorData[mSelectedActor].mTransformData.begin() + index);

      InsertTransform(sortData.mTime, sortData.mTransform, sortData.mScale, sortData.mCanScale, true, mTransformEvent);
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
void DirectorCinematicEditorPlugin::OnAddTransform()
{
   if (mSelectedActor == -1) return;

   int time = mUI.mTimeSlider->value();

   dtCore::BaseActorObject* actor = mActorData[mSelectedActor].mActor.get();
   if (actor)
   {
      dtCore::Transformable* txable = NULL;
      actor->GetDrawable(txable);
      if (txable)
      {
         dtCore::Transform transform;
         osg::Vec3 scale = osg::Vec3(1, 1, 1);

         txable->GetTransform(transform);

         bool canScale = false;
         dtCore::Object* obj = dynamic_cast<dtCore::Object*>(txable);
         if (obj)
         {
            canScale = true;
            scale = obj->GetScale();
         }

         mTransformEvent = InsertTransform(time, transform, scale, canScale);
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
void DirectorCinematicEditorPlugin::UpdateTransform(TransformData* data)
{
   dtCore::Transformable* txable = NULL;
   mActorData[mSelectedActor].mActor->GetDrawable(txable);
   if (txable)
   {
      txable->SetTransform(data->mTransform);
      ViewportManager::GetInstance().refreshAllViewports();
   }

   dtCore::Object* obj = NULL;
   mActorData[mSelectedActor].mActor->GetDrawable(obj);
   if (obj != NULL)
   {
      obj->SetScale(data->mScale);
      ViewportManager::GetInstance().refreshAllViewports();
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

      UpdateTransform(data);

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

      UpdateTransform(data);
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

      UpdateTransform(data);
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

      UpdateTransform(data);
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

      UpdateTransform(data);
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

      UpdateTransform(data);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnScaleXChanged(QString value)
{
   TransformData* data = GetTransformData(mTransformEvent);
   if (data)
   {
      data->mScale.x() = dtUtil::ToType<float>(value.toStdString());

      UpdateTransform(data);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnScaleYChanged(QString value)
{
   TransformData* data = GetTransformData(mTransformEvent);
   if (data)
   {
      data->mScale.y() = dtUtil::ToType<float>(value.toStdString());

      UpdateTransform(data);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnScaleZChanged(QString value)
{
   TransformData* data = GetTransformData(mTransformEvent);
   if (data)
   {
      data->mScale.z() = dtUtil::ToType<float>(value.toStdString());

      UpdateTransform(data);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnAnimationEventSelected(BaseEvent* event)
{
   bool enable = false;

   AnimationEvent* animEvent = dynamic_cast<AnimationEvent*>(event);
   mAnimationEvent = animEvent;
   // Set the time slider to the position of the event.
   if (animEvent)
   {
      int curTime = mUI.mTimeSlider->value();
      if (curTime < animEvent->GetStartTime() || curTime > animEvent->GetEndTime())
      {
         mUI.mTimeSlider->setValue(animEvent->GetStartTime());
      }
      else
      {
         LerpActors(mUI.mTimeSlider->value());
      }

      mUI.mAnimationTrack->SelectEvent(animEvent);
      enable = true;

      mUI.mAddAnimationButton->setEnabled(true);
      mUI.mRemoveAnimationButton->setEnabled(true);

      AnimationData* data = GetAnimationData(animEvent);
      if (data)
      {
         mUI.mAnimationCombo->blockSignals(true);
         mUI.mAnimationDurationText->blockSignals(true);
         mUI.mAnimationSpeedEdit->blockSignals(true);
         mUI.mAnimationStartTimeEdit->blockSignals(true);
         mUI.mAnimationEndTimeEdit->blockSignals(true);
         mUI.mAnimationFadeInTimeEdit->blockSignals(true);
         mUI.mAnimationFadeOutTimeEdit->blockSignals(true);
         mUI.mAnimationWeightEdit->blockSignals(true);

         int index = mUI.mAnimationCombo->findText(data->mName.c_str());
         mUI.mAnimationCombo->setCurrentIndex(index);
         mUI.mAnimationDurationText->setText(dtUtil::ToString(data->mDuration).c_str());
         if (!NEAR_EQUAL(mUI.mAnimationSpeedEdit->value(), data->mSpeed)) mUI.mAnimationSpeedEdit->setValue(data->mSpeed);
         if (!NEAR_EQUAL(mUI.mAnimationStartTimeEdit->value(), data->mStartTime)) mUI.mAnimationStartTimeEdit->setValue(data->mStartTime);
         if (!NEAR_EQUAL(mUI.mAnimationEndTimeEdit->value(), data->mEndTime)) mUI.mAnimationEndTimeEdit->setValue(data->mEndTime);
         if (!NEAR_EQUAL(mUI.mAnimationFadeInTimeEdit->value(), data->mBlendInTime)) mUI.mAnimationFadeInTimeEdit->setValue(data->mBlendInTime);
         if (!NEAR_EQUAL(mUI.mAnimationFadeOutTimeEdit->value(), data->mBlendOutTime)) mUI.mAnimationFadeOutTimeEdit->setValue(data->mBlendOutTime);
         if (!NEAR_EQUAL(mUI.mAnimationWeightEdit->value(), data->mWeight)) mUI.mAnimationWeightEdit->setValue(data->mWeight);

         mUI.mAnimationCombo->blockSignals(false);
         mUI.mAnimationDurationText->blockSignals(false);
         mUI.mAnimationSpeedEdit->blockSignals(false);
         mUI.mAnimationStartTimeEdit->blockSignals(false);
         mUI.mAnimationEndTimeEdit->blockSignals(false);
         mUI.mAnimationFadeInTimeEdit->blockSignals(false);
         mUI.mAnimationFadeOutTimeEdit->blockSignals(false);
         mUI.mAnimationWeightEdit->blockSignals(false);
      }
   }
   else
   {
      mUI.mAddAnimationButton->setEnabled(true);
      mUI.mRemoveAnimationButton->setEnabled(false);
      mUI.mAnimationTrack->DeselectEvents();
   }

   mUI.mAnimationCombo->setEnabled(enable);
   mUI.mAnimationDurationText->setEnabled(enable);
#ifdef MANUAL_ANIMATIONS
   mUI.mAnimationSpeedEdit->setEnabled(enable);
#else
   mUI.mAnimationSpeedEdit->setEnabled(false);
#endif
   mUI.mAnimationStartTimeEdit->setEnabled(enable);
   mUI.mAnimationEndTimeEdit->setEnabled(enable);
   mUI.mAnimationFadeInTimeEdit->setEnabled(enable);
   mUI.mAnimationFadeOutTimeEdit->setEnabled(enable);
   mUI.mAnimationWeightEdit->setEnabled(enable);
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnAnimationEventTimesChanged(int start, int /*end*/)
{
   int index = -1;
   AnimationData* data = GetAnimationData(mAnimationEvent, &index);
   if (data)
   {
      AnimationData sortData = *data;
      sortData.mTime = start;

      mActorData[mSelectedActor].mAnimationData.erase(mActorData[mSelectedActor].mAnimationData.begin() + index);

      InsertAnimation(sortData, mAnimationEvent);
   }

   OnAnimationEventSelected(mAnimationEvent);
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnAnimationEventRemoved(BaseEvent* event)
{
   int index = -1;
   AnimationData* data = GetAnimationData(dynamic_cast<AnimationEvent*>(event), &index);
   if (data)
   {
      mActorData[mSelectedActor].mAnimationData.erase(mActorData[mSelectedActor].mAnimationData.begin() + index);
      OnAnimationEventSelected(NULL);
      OnTimeSliderValueChanged(mUI.mTimeSlider->value());
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnAddAnimation()
{
   if (mSelectedActor == -1) return;

   int time = mUI.mTimeSlider->value();

   dtCore::BaseActorObject* actor = mActorData[mSelectedActor].mActor.get();
   if (actor)
   {
      dtAnim::AnimationGameActor* animDraw = NULL;
      actor->GetDrawable(animDraw);
      if (animDraw)
      {
         // Always default to the first animation in the combo list.
         std::string animName = mUI.mAnimationCombo->itemText(0).toStdString();

         mAnimationEvent = InsertAnimation(time, animName);
         OnAnimationEventSelected(mAnimationEvent);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnRemoveAnimation()
{
   if (mSelectedActor == -1) return;

   if (mAnimationEvent)
   {
      mUI.mAnimationTrack->RemoveEvent(mAnimationEvent);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnAnimationComboChanged(int index)
{
   if (mSelectedActor == -1) return;

   // Retrieve the newly selected animation data.
   std::string animName = mUI.mAnimationCombo->itemText(index).toStdString();

   dtAnim::AnimationGameActor* animActor = NULL;
   dtCore::Transformable* txable = NULL;
   mActorData[mSelectedActor].mActor->GetDrawable(txable);
   if (txable)
   {
      animActor = dynamic_cast<dtAnim::AnimationGameActor*>(txable);
      if (!animActor)
      {
         return;
      }
   }

   dtAnim::SequenceMixer& mixer = animActor->GetComponent<dtAnim::AnimationHelper>()->GetSequenceMixer();
   const dtAnim::AnimationChannel* anim = dynamic_cast<const dtAnim::AnimationChannel*>(mixer.GetRegisteredAnimation(animName));
   if (anim)
   {
      AnimationData* data = GetAnimationData(mAnimationEvent);
      if (data)
      {
         float duration = anim->GetAnimation()->GetDuration();

         data->mName = animName;
         data->mWeight = anim->GetBaseWeight();
         data->mSpeed = anim->GetSpeed();
         data->mDuration = duration;
         if (data->mStartTime > duration) data->mStartTime = duration;
         data->mEndTime = duration;
         if (data->mBlendInTime + data->mBlendOutTime > (data->mEndTime - data->mStartTime) / data->mSpeed)
         {
            float blendMod = ((data->mEndTime - data->mStartTime) / data->mSpeed) / (data->mBlendInTime + data->mBlendOutTime);
            data->mBlendInTime *= blendMod;
            data->mBlendOutTime *= blendMod;
         }

#ifdef MANUAL_ANIMATIONS
         if (data->mAnimation > -1)
         {
            dtAnim::Cal3DModelWrapper* calWrapper = dynamic_cast<dtAnim::Cal3DModelWrapper*>(animActor->GetComponent<dtAnim::AnimationHelper>()->GetModelWrapper());
            if (calWrapper != NULL)
            {
               CalMixer* calMixer = calWrapper->GetCalModel()->getMixer();
               calMixer->removeManualAnimation(data->mAnimation);
            }
            animActor->GetComponent<dtAnim::AnimationHelper>()->Update(0.0f);
            data->mAnimation = -1;

            LerpActors(mUI.mTimeSlider->value());
         }
#else
         mixer.ClearActiveAnimations(0.0f);
         mixer.Update(0.0f);
#endif

         mAnimationEvent->SetEndTime(mAnimationEvent->GetStartTime() + (((data->mDuration * 1000) - (data->mStartTime * 1000) - ((data->mDuration - data->mEndTime) * 1000)) / data->mSpeed));
         mAnimationEvent->SetBlendIn(data->mBlendInTime * 1000);
         mAnimationEvent->SetBlendOut(data->mBlendOutTime * 1000);
         mAnimationEvent->SetWeight(data->mWeight);
         mUI.mAnimationTrack->ComputeEventBoundingBox(mAnimationEvent);

         OnAnimationEventSelected(mAnimationEvent);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnAnimationSpeedChanged(double value)
{
   if (mSelectedActor == -1) return;

   AnimationData* data = GetAnimationData(mAnimationEvent);
   if (data)
   {
      data->mSpeed = (float)value;
      if (data->mSpeed <= 0.1f) data->mSpeed = 0.1f;

      if (data->mBlendInTime + data->mBlendOutTime > (data->mEndTime - data->mStartTime) / data->mSpeed)
      {
         float blendMod = ((data->mEndTime - data->mStartTime) / data->mSpeed) / (data->mBlendInTime + data->mBlendOutTime);
         data->mBlendInTime *= blendMod;
         data->mBlendOutTime *= blendMod;
      }

      mAnimationEvent->SetEndTime(mAnimationEvent->GetStartTime() + (((data->mDuration * 1000) - (data->mStartTime * 1000) - ((data->mDuration - data->mEndTime) * 1000)) / data->mSpeed));
      mAnimationEvent->SetBlendIn(data->mBlendInTime * 1000);
      mAnimationEvent->SetBlendOut(data->mBlendOutTime * 1000);
      mAnimationEvent->SetWeight(data->mWeight);
      mUI.mAnimationTrack->ComputeEventBoundingBox(mAnimationEvent);

      OnAnimationEventSelected(mAnimationEvent);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnAnimationStartTimeChanged(double value)
{
   if (mSelectedActor == -1) return;

   AnimationData* data = GetAnimationData(mAnimationEvent);
   if (data)
   {
      data->mStartTime = (float)value;
      if (data->mStartTime > data->mEndTime) data->mStartTime = data->mEndTime;

      if (data->mBlendInTime + data->mBlendOutTime > (data->mEndTime - data->mStartTime) / data->mSpeed)
      {
         float blendMod = ((data->mEndTime - data->mStartTime) / data->mSpeed) / (data->mBlendInTime + data->mBlendOutTime);
         data->mBlendInTime *= blendMod;
         data->mBlendOutTime *= blendMod;
      }

      mAnimationEvent->SetEndTime(mAnimationEvent->GetStartTime() + (((data->mDuration * 1000) - (data->mStartTime * 1000) - ((data->mDuration - data->mEndTime) * 1000)) / data->mSpeed));
      mAnimationEvent->SetBlendIn(data->mBlendInTime * 1000);
      mAnimationEvent->SetBlendOut(data->mBlendOutTime * 1000);
      mAnimationEvent->SetWeight(data->mWeight);
      mUI.mAnimationTrack->ComputeEventBoundingBox(mAnimationEvent);

      OnAnimationEventSelected(mAnimationEvent);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnAnimationEndTimeChanged(double value)
{
   if (mSelectedActor == -1) return;

   AnimationData* data = GetAnimationData(mAnimationEvent);
   if (data)
   {
      data->mEndTime = (float)value;
      if (data->mEndTime > data->mDuration) data->mEndTime = data->mDuration;
      if (data->mEndTime < data->mStartTime) data->mEndTime = data->mStartTime;

      if (data->mBlendInTime + data->mBlendOutTime > (data->mEndTime - data->mStartTime) / data->mSpeed)
      {
         float blendMod = ((data->mEndTime - data->mStartTime) / data->mSpeed) / (data->mBlendInTime + data->mBlendOutTime);
         data->mBlendInTime *= blendMod;
         data->mBlendOutTime *= blendMod;
      }

      mAnimationEvent->SetEndTime(mAnimationEvent->GetStartTime() + (((data->mDuration * 1000) - (data->mStartTime * 1000) - ((data->mDuration - data->mEndTime) * 1000)) / data->mSpeed));
      mAnimationEvent->SetBlendIn(data->mBlendInTime * 1000);
      mAnimationEvent->SetBlendOut(data->mBlendOutTime * 1000);
      mAnimationEvent->SetWeight(data->mWeight);
      mUI.mAnimationTrack->ComputeEventBoundingBox(mAnimationEvent);

      OnAnimationEventSelected(mAnimationEvent);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnAnimationBlendInTimeChanged(double value)
{
   if (mSelectedActor == -1) return;

   AnimationData* data = GetAnimationData(mAnimationEvent);
   if (data)
   {
      data->mBlendInTime = (float)value;
      if (data->mBlendInTime + data->mBlendOutTime > (data->mEndTime - data->mStartTime) / data->mSpeed)
      {
         float blendMod = ((data->mEndTime - data->mStartTime) / data->mSpeed) / (data->mBlendInTime + data->mBlendOutTime);
         data->mBlendInTime *= blendMod;
         data->mBlendOutTime *= blendMod;
      }

      mAnimationEvent->SetEndTime(mAnimationEvent->GetStartTime() + (((data->mDuration * 1000) - (data->mStartTime * 1000) - ((data->mDuration - data->mEndTime) * 1000)) / data->mSpeed));
      mAnimationEvent->SetBlendIn(data->mBlendInTime * 1000);
      mAnimationEvent->SetBlendOut(data->mBlendOutTime * 1000);
      mAnimationEvent->SetWeight(data->mWeight);
      mUI.mAnimationTrack->ComputeEventBoundingBox(mAnimationEvent);

      OnAnimationEventSelected(mAnimationEvent);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnAnimationBlendOutTimeChanged(double value)
{
   if (mSelectedActor == -1) return;

   AnimationData* data = GetAnimationData(mAnimationEvent);
   if (data)
   {
      data->mBlendOutTime = (float)value;
      if (data->mBlendInTime + data->mBlendOutTime > (data->mEndTime - data->mStartTime) / data->mSpeed)
      {
         float blendMod = ((data->mEndTime - data->mStartTime) / data->mSpeed) / (data->mBlendInTime + data->mBlendOutTime);
         data->mBlendInTime *= blendMod;
         data->mBlendOutTime *= blendMod;
      }

      mAnimationEvent->SetEndTime(mAnimationEvent->GetStartTime() + (((data->mDuration * 1000) - (data->mStartTime * 1000) - ((data->mDuration - data->mEndTime) * 1000)) / data->mSpeed));
      mAnimationEvent->SetBlendIn(data->mBlendInTime * 1000);
      mAnimationEvent->SetBlendOut(data->mBlendOutTime * 1000);
      mAnimationEvent->SetWeight(data->mWeight);
      mUI.mAnimationTrack->ComputeEventBoundingBox(mAnimationEvent);

      OnAnimationEventSelected(mAnimationEvent);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnAnimationWeightChanged(double value)
{
   if (mSelectedActor == -1) return;

   AnimationData* data = GetAnimationData(mAnimationEvent);
   if (data)
   {
      data->mWeight = (float)value;
      if (data->mBlendInTime + data->mBlendOutTime > (data->mEndTime - data->mStartTime) / data->mSpeed)
      {
         float blendMod = ((data->mEndTime - data->mStartTime) / data->mSpeed) / (data->mBlendInTime + data->mBlendOutTime);
         data->mBlendInTime *= blendMod;
         data->mBlendOutTime *= blendMod;
      }

      mAnimationEvent->SetEndTime(mAnimationEvent->GetStartTime() + (((data->mDuration * 1000) - (data->mStartTime * 1000) - ((data->mDuration - data->mEndTime) * 1000)) / data->mSpeed));
      mAnimationEvent->SetBlendIn(data->mBlendInTime * 1000);
      mAnimationEvent->SetBlendOut(data->mBlendOutTime * 1000);
      mAnimationEvent->SetWeight(data->mWeight);
      mUI.mAnimationTrack->ComputeEventBoundingBox(mAnimationEvent);

      OnAnimationEventSelected(mAnimationEvent);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnOutputEventSelected(BaseEvent* event)
{
   bool enable = false;

   KeyFrameEvent* keyEvent = dynamic_cast<KeyFrameEvent*>(event);
   // Set the time slider to the position of the event.
   if (keyEvent)
   {
      mUI.mTimeSlider->setValue(keyEvent->GetStartTime());
      mUI.mOutputTrack->SelectEvent(keyEvent);
      enable = true;

      mUI.mAddOutputButton->setEnabled(true);
      mUI.mRemoveOutputButton->setEnabled(true);

      OutputData* data = GetOutputData(keyEvent);

      if (data)
      {
         mUI.mOutputNameEdit->blockSignals(true);
         mUI.mOutputTriggerPlay->blockSignals(true);
         mUI.mOutputTriggerReverse->blockSignals(true);

         mUI.mOutputNameEdit->setText(data->mName.c_str());
         mUI.mOutputTriggerPlay->setChecked(data->mTriggerPlay);
         mUI.mOutputTriggerReverse->setChecked(data->mTriggerReverse);

         mUI.mOutputNameEdit->blockSignals(false);
         mUI.mOutputTriggerPlay->blockSignals(false);
         mUI.mOutputTriggerReverse->blockSignals(false);
      }
   }
   else
   {
      mUI.mAddOutputButton->setEnabled(true);
      mUI.mRemoveOutputButton->setEnabled(false);
      mUI.mOutputTrack->DeselectEvents();
   }

   mUI.mOutputNameEdit->setEnabled(enable);
   mUI.mOutputTriggerPlay->setEnabled(enable);
   mUI.mOutputTriggerReverse->setEnabled(enable);

   mOutputEvent = keyEvent;
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnOutputEventTimesChanged(int start, int /*end*/)
{
   int index = -1;
   OutputData* data = GetOutputData(mOutputEvent, &index);
   if (data)
   {
      OutputData sortData = *data;
      sortData.mTime = start;

      mOutputData.erase(mOutputData.begin() + index);

      InsertOutput(sortData.mTime, sortData.mName, sortData.mTriggerPlay, sortData.mTriggerReverse, mOutputEvent);
   }

   OnOutputEventSelected(mOutputEvent);
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnOutputEventRemoved(BaseEvent* event)
{
   int index = -1;
   OutputData* data = GetOutputData(dynamic_cast<KeyFrameEvent*>(event), &index);
   if (data)
   {
      mOutputData.erase(mOutputData.begin() + index);
      OnTimeSliderValueChanged(mUI.mTimeSlider->value());
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnAddOutput()
{
   int time = mUI.mTimeSlider->value();

   mOutputEvent = InsertOutput(time, "Output Name", true, true);
   OnOutputEventSelected(mOutputEvent);
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnRemoveOutput()
{
   if (mOutputEvent)
   {
      mUI.mOutputTrack->RemoveEvent(mOutputEvent);
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnOutputNameChanged(QString value)
{
   OutputData* data = GetOutputData(mOutputEvent);
   if (data)
   {
      data->mName = value.toStdString();
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnOutputTriggerPlay(int value)
{
   OutputData* data = GetOutputData(mOutputEvent);
   if (data)
   {
      data->mTriggerPlay = value;
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnOutputTriggerReverse(int value)
{
   OutputData* data = GetOutputData(mOutputEvent);
   if (data)
   {
      data->mTriggerReverse = value;
   }
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnCurrentTimeChanged(double time)
{
   mUI.mTimeSlider->setValue(time * 1000);
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnStartTimeChanged(double time)
{
   // Make sure the end time can not be before the start time.
   mUI.mEndTimeEdit->setMinimum(time + 0.01);

   ResetUI();
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnEndTimeChanged(double time)
{
   // Make sure the start time can not go beyond the end time.
   mUI.mStartTimeEdit->setMaximum(time - 0.01);

   ResetUI();
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::OnTotalTimeChanged(double time)
{
   // If there is no duration, force at least one second.
   if (time <= 0.0)
   {
      mUI.mTotalTimeEdit->setValue(0.01);
      time = 0.01;
   }

   float oldTime = mTotalTime;
   mTotalTime = (float)time;
   mUI.mEndTimeEdit->setMaximum(time);

   // Increase the viewed end time if we are viewing up to the total time.
   if NEAR_EQUAL(oldTime, mUI.mEndTimeEdit->value())
   {
      mUI.mEndTimeEdit->blockSignals(true);
      mUI.mEndTimeEdit->setValue(time);
      mUI.mStartTimeEdit->setMaximum(time - 0.01);
      mUI.mEndTimeEdit->blockSignals(false);
   }

   OnTimeLineFinished();

   ResetUI();
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
   mUI.mOutputTrack->SetCurrent(value);
   mUI.mSelectionTrack->SetCurrent(value);

   OnTransformEventSelected(mUI.mTransformTrack->GetEventAtTime(value));
   OnOutputEventSelected(mUI.mOutputTrack->GetEventAtTime(value));

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

   // Set all cinematic values to defaults.
   mActorData.clear();
   mOutputData.clear();
   mTotalTime = 1.0f;
   if (mPlaying) OnPlay();

   blockSignals(true);
   mUI.mStartTimeEdit->setValue(0.0);
   mUI.mEndTimeEdit->setMinimum(0.01);
   mUI.mEndTimeEdit->setMaximum(1.0);
   mUI.mEndTimeEdit->setValue(1.0);
   blockSignals(false);

   std::vector<dtDirector::Node*> nodes;
   GetGraph()->GetNodes("Scheduler", "Cinematic", nodes);

   // Find the scheduler node, if it exists.
   dtDirector::SchedulerAction* schedulerNode = NULL;
   if (nodes.size())
   {
      schedulerNode = dynamic_cast<dtDirector::SchedulerAction*>(nodes[0]);
   }

   if (schedulerNode)
   {
      // Determine the total cinematic time.
      mTotalTime = schedulerNode->GetFloat("TotalTime");

      blockSignals(true);
      mUI.mEndTimeEdit->setMaximum(mTotalTime);
      mUI.mEndTimeEdit->setValue(mTotalTime);
      blockSignals(false);

      // Load all scheduled event outputs.
      mUI.mOutputTrack->ClearEvents();
      std::vector<dtDirector::SchedulerAction::OutputEventData> eventList =
         schedulerNode->GetEventList();
      int eventCount = (int)eventList.size();
      for (int eventIndex = 0; eventIndex < eventCount; ++eventIndex)
      {
         dtDirector::SchedulerAction::OutputEventData& data =
            eventList[eventIndex];

         InsertOutput(data.time * 1000, data.name, data.triggerNormal, data.triggerReverse);
      }
   }

   // Find all Starting remote events and follow their chains to find all cinematic data.
   nodes.clear();
   GetGraph()->GetNodes("Remote Event", "Core", "EventName", "Started", nodes);
   int eventCount = (int)nodes.size();
   for (int eventIndex = 0; eventIndex < eventCount; ++eventIndex)
   {
      dtDirector::Node* node = nodes[eventIndex];
      if (node)
      {
         mSelectedActor = (int)mActorData.size();
         mActorData.push_back(ActorData());
         dtCore::Transform lastTransform;
         osg::Vec3 lastScale = osg::Vec3(1, 1, 1);
         bool canScale = false;

         ActorData& actorData = mActorData[mSelectedActor];
         actorData.mTransformEnabled = false;

         // Iterate to the next node in the chain.
         std::vector<dtDirector::InputLink*> nextLinks;
         if (!GetNext(node, "Out", nextLinks))
         {
            continue;
         }

         node = nextLinks[0]->GetOwner();
         while (node)
         {
            nextLinks.clear();

            // Load an Animate Actor data.
            if (node->GetType().GetFullName() == "Cinematic.Animate Actor")
            {
               if (!actorData.mActor)
               {
                  actorData.mActor = node->GetActor("Actor");

                  dtCore::Transformable* txable = NULL;
                  actorData.mActor->GetDrawable(txable);
                  if (txable)
                  {
                     txable->GetTransform(lastTransform);

                     dtCore::Object* obj = dynamic_cast<dtCore::Object*>(txable);
                     if (obj)
                     {
                        lastScale = obj->GetScale();
                        canScale = true;
                     }
                  }
               }

               dtAnim::AnimationGameActor* animActor = NULL;
               animActor = dynamic_cast<dtAnim::AnimationGameActor*>(actorData.mActor->GetDrawable());
               if (animActor)
               {
                  dtAnim::SequenceMixer& mixer = animActor->GetComponent<dtAnim::AnimationHelper>()->GetSequenceMixer();

                  dtDirector::AnimateActorAction* animNode =
                     dynamic_cast<dtDirector::AnimateActorAction*>(node);
                  if (animNode)
                  {
                     // Load our scheduled animation list.
                     std::vector<dtDirector::AnimateActorAction::AnimData>
                        animList = animNode->GetAnimArray();
                     int animCount = (int)animList.size();
                     for (int animIndex = 0; animIndex < animCount; ++animIndex)
                     {
                        dtDirector::AnimateActorAction::AnimData& data =
                           animList[animIndex];

                        AnimationData newData(0, "", 0.0f, 1.0f, 1.0f, NULL);

                        newData.mName = data.mName;
                        const dtAnim::AnimationChannel* anim = dynamic_cast<const dtAnim::AnimationChannel*>(mixer.GetRegisteredAnimation(data.mName));
                        if (anim)
                        {
                           newData.mDuration = anim->GetAnimation()->GetDuration();
                        }

                        newData.mSpeed = data.mSpeed;
                        newData.mTime = data.mTime * 1000;
                        newData.mStartTime = data.mStartOffset;
                        newData.mEndTime = (data.mDuration * newData.mSpeed) + newData.mStartTime;
                        newData.mBlendInTime = data.mBlendInTime;
                        newData.mBlendOutTime = data.mBlendOutTime;
                        newData.mWeight = data.mWeight;

                        InsertAnimation(newData);
                     }
                  }
               }

               GetNext(node, "Started", nextLinks);
            }
            // Load a Translation lerp data.
            else if (node->GetType().GetFullName() == "Cinematic.Lerp Actor Translation")
            {
               actorData.mTransformEnabled = true;
               if (!actorData.mActor)
               {
                  actorData.mActor = node->GetActor("Actor");

                  dtCore::Transformable* txable = NULL;
                  actorData.mActor->GetDrawable(txable);
                  if (txable)
                  {
                     txable->GetTransform(lastTransform);

                     dtCore::Object* obj = dynamic_cast<dtCore::Object*>(txable);
                     if (obj)
                     {
                        lastScale = obj->GetScale();
                        canScale = true;
                     }
                  }
               }
               {
                  int time = node->GetFloat("StartTime") * 1000;
                  osg::Vec3 pos = node->GetVec3("StartPosition");
                  TransformData* data = GetOrCreateTransformData(time, lastTransform, lastScale, canScale);
                  if (data) data->mTransform.SetTranslation(pos.x(), pos.y(), pos.z());
               }
               {
                  int time = node->GetFloat("EndTime") * 1000;
                  osg::Vec3 pos = node->GetVec3("EndPosition");
                  TransformData* data = GetOrCreateTransformData(time, lastTransform, lastScale, canScale);
                  if (data) data->mTransform.SetTranslation(pos.x(), pos.y(), pos.z());
                  lastTransform.SetTranslation(pos.x(), pos.y(), pos.z());
               }

               GetNext(node, "Started", nextLinks);
            }
            // Load a Rotation lerp node.
            else if (node->GetType().GetFullName() == "Cinematic.Lerp Actor Rotation")
            {
               actorData.mTransformEnabled = true;
               if (!actorData.mActor)
               {
                  actorData.mActor = node->GetActor("Actor");

                  dtCore::Transformable* txable = NULL;
                  actorData.mActor->GetDrawable(txable);
                  if (txable)
                  {
                     txable->GetTransform(lastTransform);

                     dtCore::Object* obj = dynamic_cast<dtCore::Object*>(txable);
                     if (obj)
                     {
                        lastScale = obj->GetScale();
                        canScale = true;
                     }
                  }
               }
               {
                  int time = node->GetFloat("StartTime") * 1000;
                  osg::Vec3 rot = node->GetVec3("StartRotation");
                  TransformData* data = GetOrCreateTransformData(time, lastTransform, lastScale, canScale);
                  if (data) data->mTransform.SetRotation(rot.z(), rot.x(), rot.y());
               }
               {
                  int time = node->GetFloat("EndTime") * 1000;
                  osg::Vec3 rot = node->GetVec3("EndRotation");
                  TransformData* data = GetOrCreateTransformData(time, lastTransform, lastScale, canScale);
                  if (data) data->mTransform.SetRotation(rot.z(), rot.x(), rot.y());
                  lastTransform.SetRotation(rot.z(), rot.x(), rot.y());
               }

               GetNext(node, "Started", nextLinks);
            }
            // Load a Scale lerp node.
            else if (node->GetType().GetFullName() == "Cinematic.Lerp Actor Scale")
            {
               actorData.mTransformEnabled = true;
               if (!actorData.mActor)
               {
                  actorData.mActor = node->GetActor("Actor");

                  dtCore::Transformable* txable = NULL;
                  actorData.mActor->GetDrawable(txable);
                  if (txable)
                  {
                     txable->GetTransform(lastTransform);
                  }

                  dtCore::Object* obj = dynamic_cast<dtCore::Object*>(txable);
                  if (obj)
                  {
                     lastScale = obj->GetScale();
                     canScale = true;
                  }
               }
               {
                  int time = node->GetFloat("StartTime") * 1000;
                  osg::Vec3 scale = node->GetVec3("StartScale");
                  TransformData* data = GetOrCreateTransformData(time, lastTransform, lastScale, canScale);
                  if (data) data->mScale.set(scale.x(), scale.y(), scale.z());
               }
               {
                  int time = node->GetFloat("EndTime") * 1000;
                  osg::Vec3 scale = node->GetVec3("EndScale");
                  TransformData* data = GetOrCreateTransformData(time, lastTransform, lastScale, canScale);
                  if (data) data->mScale.set(scale.x(), scale.y(), scale.z());
                  lastScale.set(scale.x(), scale.y(), scale.z());
               }

               GetNext(node, "Started", nextLinks);
            }

            // Continue down the chain.
            node = NULL;
            if (!nextLinks.empty())
            {
               node = nextLinks[0]->GetOwner();
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
   BeginSave();

   // Create our input links.
   dtDirector::Node* newPlayNode    = CreateNode("Input Link", "Core", NULL, 80);
   dtDirector::Node* newReverseNode = CreateNode("Input Link", "Core", NULL, 80);
   dtDirector::Node* newStopNode    = CreateNode("Input Link", "Core", NULL, 80);
   dtDirector::Node* newPauseNode   = CreateNode("Input Link", "Core", NULL, 80);

   newPlayNode->SetString("Play", "Name");
   newReverseNode->SetString("Reverse", "Name");
   newStopNode->SetString("Stop", "Name");
   newPauseNode->SetString("Pause", "Name");

   // Create our scheduler node.
   dtDirector::SchedulerAction* schedulerNode =
      dynamic_cast<dtDirector::SchedulerAction*>(
      CreateNode("Scheduler", "Cinematic", newPlayNode));
   if (schedulerNode)
   {
      schedulerNode->SetFloat(mTotalTime, "TotalTime");

      // Connect our input link nodes to the scheduler.
      Connect(newPlayNode,    schedulerNode, "Out", "Play");
      Connect(newReverseNode, schedulerNode, "Out", "Reverse");
      Connect(newStopNode,    schedulerNode, "Out", "Stop");
      Connect(newPauseNode,   schedulerNode, "Out", "Pause");

      // Create a call remote event node that will trigger on start.
      dtDirector::Node* startedCallNode = CreateNode("Call Remote Event", "Core", schedulerNode, 80);
      if (startedCallNode)
      {
         startedCallNode->SetString("Started", "EventName");
         startedCallNode->SetString("Local Scope", "Event Scope");

         Connect(schedulerNode, startedCallNode, "Started", "Call Event");
      }

      // Create our output link to trigger on start.
      dtDirector::Node* startedOutputNode = CreateNode("Output Link", "Core", schedulerNode, 80);
      if (startedOutputNode)
      {
         startedOutputNode->SetString("Started", "Name");

         Connect(schedulerNode, startedOutputNode, "Started", "In");
      }

      // Create our output links for each scheduled event.
      std::vector<dtDirector::SchedulerAction::OutputEventData> eventList;
      int eventCount = (int)mOutputData.size();
      for (int eventIndex = 0; eventIndex < eventCount; ++eventIndex)
      {
         OutputData& data = mOutputData[eventIndex];
         dtDirector::SchedulerAction::OutputEventData newData;

         newData.name = data.mName;
         newData.time = data.mTime * 0.001f;
         newData.triggerNormal = data.mTriggerPlay;
         newData.triggerReverse = data.mTriggerReverse;
         eventList.push_back(newData);
      }
      schedulerNode->SetEventList(eventList);

      // Connect each of our output links to an output link node.
      for (int eventIndex = 0; eventIndex < eventCount; ++eventIndex)
      {
         dtDirector::SchedulerAction::OutputEventData& data = eventList[eventIndex];

         dtDirector::Node* outputNode = CreateNode("Output Link", "Core", schedulerNode, 80);
         if (outputNode)
         {
            outputNode->SetString(data.name, "Name");

            Connect(schedulerNode, outputNode, data.name, "In");
         }
      }

      // Create our output link to trigger on end.
      dtDirector::Node* endedOutputNode = CreateNode("Output Link", "Core", schedulerNode, 80);
      if (endedOutputNode)
      {
         endedOutputNode->SetString("Ended", "Name");

         Connect(schedulerNode, endedOutputNode, "Ended", "In");
      }

      // Create a call remote event node that will trigger on end.
      dtDirector::Node* endedCallNode = CreateNode("Call Remote Event", "Core", schedulerNode);
      if (endedCallNode)
      {
         endedCallNode->SetString("Ended", "EventName");
         endedCallNode->SetString("Local Scope", "Event Scope");

         Connect(schedulerNode, endedCallNode, "Ended", "Call Event");
         Connect(schedulerNode, endedCallNode, "Stopped", "Call Event");
      }

      // Create our current time value node.
      dtDirector::Node* timeValueNode = CreateNode("Float", "General");
      if (timeValueNode)
      {
         timeValueNode->SetString("Current Time", "Name");

         Connect(schedulerNode, timeValueNode, "Time");
      }

      // Now create our transformation chains for each actor.
      int actorCount = (int)mActorData.size();
      for (int actorIndex = 0; actorIndex < actorCount; ++actorIndex)
      {
         ActorData& actorData = mActorData[actorIndex];

         dtDirector::Node* startNode = NULL;
         dtDirector::Node* endNode = NULL;
         std::string startLink = "";
         std::string endLink = "";

         // Start with our start and end remote events to start the chains.
         dtDirector::Node* startEvent = CreateNode("Remote Event", "Core", NULL, 80);
         if (startEvent)
         {
            startEvent->SetString("Started", "EventName");
            startNode = startEvent;
            startLink = "Out";
         }

         dtDirector::Node* endEvent   = CreateNode("Remote Event", "Core");
         if (endEvent)
         {
            endEvent->SetString("Ended", "EventName");
            endNode = endEvent;
            endLink = "Out";
         }

         // Create our animation action.
         if (actorData.mAnimationData.size())
         {
            if (!GetGraph()->GetDirector()->HasLibrary("dtDirectorAnimNodes"))
            {
               GetGraph()->GetDirector()->AddLibrary("dtDirectorAnimNodes");
            }

            dtDirector::AnimateActorAction* animNode =
               dynamic_cast<dtDirector::AnimateActorAction*>(
               CreateNode("Animate Actor", "Cinematic", startNode));
            if (animNode)
            {
               // Create our scheduled animation list.
               std::vector<dtDirector::AnimateActorAction::AnimData> animList;

               int animCount = (int)actorData.mAnimationData.size();
               for (int animIndex = 0; animIndex < animCount; ++animIndex)
               {
                  AnimationData& data = actorData.mAnimationData[animIndex];
                  float duration = (data.mEndTime - data.mStartTime) / data.mSpeed;

                  dtDirector::AnimateActorAction::AnimData newData;

                  newData.mName = data.mName;
                  newData.mTime = data.mTime * 0.001f;
                  newData.mDuration = duration;
                  newData.mBlendInTime = data.mBlendInTime;
                  newData.mBlendOutTime = data.mBlendOutTime;
                  newData.mWeight = data.mWeight;
                  newData.mStartOffset = data.mStartTime;
                  newData.mSpeed = data.mSpeed;
                  animList.push_back(newData);
               }
               animNode->SetAnimArray(animList);

               // Create an actor value node to store the animated actor.
               dtDirector::Node* actorValue = CreateNode("Actor", "General");
               if (actorValue)
               {
                  actorValue->SetActorID(actorData.mActor->GetId());

                  Connect(animNode, actorValue, "Actor");
               }

               // Create our reference to the current time.
               dtDirector::Node* timeRefValue = CreateNode("Reference", "Core");
               if (timeRefValue)
               {
                  timeRefValue->SetString("Current Time", "Reference");

                  Connect(animNode, timeRefValue, "Time");
               }

               // Now connect this node to the previous node in the chain.
               Connect(startNode, animNode, startLink, "Start");
               Connect(endNode, animNode, endLink, "Stop");
               startNode = endNode = animNode;
               startLink = "Started";
               endLink = "Stopped";
            }
         }

         // Iterate through each transformation key frame.
         if (actorData.mTransformEnabled)
         {
            int transformCount = (int)actorData.mTransformData.size();
            int nextIndex = 0;
            if (transformCount > 1)
            {
               transformCount--;
               nextIndex = 1;
            }

            for (int transformIndex = 0; transformIndex < transformCount; ++transformIndex, ++nextIndex)
            {
               TransformData& prevData = actorData.mTransformData[transformIndex];
               TransformData& nextData = actorData.mTransformData[nextIndex];

               // Translation.
               if (transformIndex == 0 || prevData.mTransform.GetTranslation() != nextData.mTransform.GetTranslation())
               {
                  dtDirector::Node* lerpNode = CreateNode("Lerp Actor Translation", "Cinematic", startNode);
                  if (lerpNode)
                  {
                     lerpNode->SetFloat(prevData.mTime * 0.001f, "StartTime");
                     lerpNode->SetFloat(nextData.mTime * 0.001f, "EndTime");
                     lerpNode->SetVec3(prevData.mTransform.GetTranslation(), "StartPosition");
                     lerpNode->SetVec3(nextData.mTransform.GetTranslation(), "EndPosition");

                     // Create an actor value node to store the animated actor.
                     dtDirector::Node* actorValue = CreateNode("Actor", "General");
                     if (actorValue)
                     {
                        actorValue->SetActorID(actorData.mActor->GetId());

                        Connect(lerpNode, actorValue, "Actor");
                     }

                     // Create our reference to the current time.
                     dtDirector::Node* timeRefValue = CreateNode("Reference", "Core");
                     if (timeRefValue)
                     {
                        timeRefValue->SetString("Current Time", "Reference");

                        Connect(lerpNode, timeRefValue, "Time");
                     }

                     // Now connect this node to the previous node in the chain.
                     Connect(startNode, lerpNode, startLink, "Start");
                     Connect(endNode, lerpNode, endLink, "Stop");
                     startNode = endNode = lerpNode;
                     startLink = "Started";
                     endLink = "Stopped";
                  }
               }

               // Rotation.
               if (transformIndex == 0 || prevData.mTransform.GetRotation() != nextData.mTransform.GetRotation())
               {
                  dtDirector::Node* lerpNode = CreateNode("Lerp Actor Rotation", "Cinematic", startNode);
                  if (lerpNode)
                  {
                     lerpNode->SetFloat(prevData.mTime * 0.001f, "StartTime");
                     lerpNode->SetFloat(nextData.mTime * 0.001f, "EndTime");
                     osg::Vec3 prevRot = prevData.mTransform.GetRotation();
                     osg::Vec3 nextRot = nextData.mTransform.GetRotation();
                     lerpNode->SetVec3(osg::Vec3(prevRot.y(), prevRot.z(), prevRot.x()), "StartRotation");
                     lerpNode->SetVec3(osg::Vec3(nextRot.y(), nextRot.z(), nextRot.x()), "EndRotation");

                     // Create an actor value node to store the animated actor.
                     dtDirector::Node* actorValue = CreateNode("Actor", "General");
                     if (actorValue)
                     {
                        actorValue->SetActorID(actorData.mActor->GetId());

                        Connect(lerpNode, actorValue, "Actor");
                     }

                     // Create our reference to the current time.
                     dtDirector::Node* timeRefValue = CreateNode("Reference", "Core");
                     if (timeRefValue)
                     {
                        timeRefValue->SetString("Current Time", "Reference");

                        Connect(lerpNode, timeRefValue, "Time");
                     }

                     // Now connect this node to the previous node in the chain.
                     Connect(startNode, lerpNode, startLink, "Start");
                     Connect(endNode, lerpNode, endLink, "Stop");
                     startNode = endNode = lerpNode;
                     startLink = "Started";
                     endLink = "Stopped";
                  }
               }

               // Scale.
               if (prevData.mCanScale && (transformIndex == 0 || prevData.mScale != nextData.mScale))
               {
                  dtDirector::Node* lerpNode = CreateNode("Lerp Actor Scale", "Cinematic", startNode);
                  if (lerpNode)
                  {
                     lerpNode->SetFloat(prevData.mTime * 0.001f, "StartTime");
                     lerpNode->SetFloat(nextData.mTime * 0.001f, "EndTime");
                     lerpNode->SetVec3(prevData.mScale, "StartScale");
                     lerpNode->SetVec3(nextData.mScale, "EndScale");

                     // Create an actor value node to store the animated actor.
                     dtDirector::Node* actorValue = CreateNode("Actor", "General");
                     if (actorValue)
                     {
                        actorValue->SetActorID(actorData.mActor->GetId());

                        Connect(lerpNode, actorValue, "Actor");
                     }

                     // Create our reference to the current time.
                     dtDirector::Node* timeRefValue = CreateNode("Reference", "Core");
                     if (timeRefValue)
                     {
                        timeRefValue->SetString("Current Time", "Reference");

                        Connect(lerpNode, timeRefValue, "Time");
                     }

                     // Now connect this node to the previous node in the chain.
                     Connect(startNode, lerpNode, startLink, "Start");
                     Connect(endNode, lerpNode, endLink, "Stop");
                     startNode = endNode = lerpNode;
                     startLink = "Started";
                     endLink = "Stopped";
                  }
               }
            }
         }
      }
   }

   EndSave();
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
      dtCore::ActorRefPtrVector selected;
      selected.push_back(mActorData[mSelectedActor].mActor.get());
      EditorEvents::GetInstance().emitActorsSelected(selected);
      //EditorEvents::GetInstance().emitGotoActor(selected[0]);
   }
}

////////////////////////////////////////////////////////////////////////////////
KeyFrameEvent* DirectorCinematicEditorPlugin::InsertTransform(int time, const dtCore::Transform& transform, const osg::Vec3& scale, bool canScale, bool movable, KeyFrameEvent* event)
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
         transformList.insert(transformList.begin() + index, TransformData(time, transform, scale, canScale, newEvent));
         return newEvent;
      }
   }

   // If we get this far, it means the new transform will be appended to the end instead.
   transformList.push_back(TransformData(time, transform, scale, canScale, newEvent));
   return newEvent;
}

////////////////////////////////////////////////////////////////////////////////
DirectorCinematicEditorPlugin::TransformData* DirectorCinematicEditorPlugin::GetTransformData(KeyFrameEvent* event, int* outIndex)
{
   if (!event) return NULL;

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
DirectorCinematicEditorPlugin::TransformData* DirectorCinematicEditorPlugin::GetOrCreateTransformData(int time, const dtCore::Transform& transform, const osg::Vec3& scale, bool canScale)
{
   if (mSelectedActor > -1)
   {
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
            transformList.insert(transformList.begin() + index, TransformData(time, transform, scale, canScale, NULL));
            return &transformList[index];
         }
      }

      // If we get this far, it means the new transform will be appended to the end instead.
      transformList.push_back(TransformData(time, transform, scale, canScale, NULL));
      return &transformList[transformList.size() - 1];
   }

   return NULL;
}

////////////////////////////////////////////////////////////////////////////////
KeyFrameEvent* DirectorCinematicEditorPlugin::InsertOutput(int time, const std::string& name, bool triggerPlay, bool triggerReverse, KeyFrameEvent* event)
{
   KeyFrameEvent* newEvent = event;

   if (!newEvent)
   {
      newEvent = new KeyFrameEvent(time);
      mUI.mOutputTrack->AddEvent(newEvent);
   }

   int count = (int)mOutputData.size();
   for (int index = 0; index < count; ++index)
   {
      OutputData& data = mOutputData[index];

      if (data.mTime >= time)
      {
         mOutputData.insert(mOutputData.begin() + index, OutputData(time, name, triggerPlay, triggerReverse, newEvent));
         return newEvent;
      }
   }

   // If we get this far, it means the new transform will be appended to the end instead.
   mOutputData.push_back(OutputData(time, name, triggerPlay, triggerReverse, newEvent));
   return newEvent;
}

////////////////////////////////////////////////////////////////////////////////
DirectorCinematicEditorPlugin::OutputData* DirectorCinematicEditorPlugin::GetOutputData(KeyFrameEvent* event, int* outIndex)
{
   if (!event) return NULL;

   int count = (int)mOutputData.size();
   for (int index = 0; index < count; ++index)
   {
      OutputData& data = mOutputData[index];
      if (data.mEvent == event)
      {
         if (outIndex) *outIndex = index;
         return &data;
      }
   }

   if (outIndex) *outIndex = -1;
   return NULL;
}

////////////////////////////////////////////////////////////////////////////////
AnimationEvent* DirectorCinematicEditorPlugin::InsertAnimation(int time, const std::string& animName, AnimationEvent* event)
{
   if (mSelectedActor == -1) return NULL;

   dtAnim::AnimationGameActor* animActor = NULL;
   dtCore::Transformable* txable = NULL;
   mActorData[mSelectedActor].mActor->GetDrawable(txable);
   if (txable)
   {
      animActor = dynamic_cast<dtAnim::AnimationGameActor*>(txable);
      if (!animActor)
      {
         return NULL;
      }
   }

   dtAnim::SequenceMixer& mixer = animActor->GetComponent<dtAnim::AnimationHelper>()->GetSequenceMixer();
   const dtAnim::AnimationChannel* anim = dynamic_cast<const dtAnim::AnimationChannel*>(mixer.GetRegisteredAnimation(animName));
   if (anim)
   {
      AnimationData data(time, animName, anim->GetAnimation()->GetDuration(), anim->GetBaseWeight(), anim->GetSpeed(), NULL);

      return InsertAnimation(data, event);
   }

   return NULL;
}

////////////////////////////////////////////////////////////////////////////////
AnimationEvent* DirectorCinematicEditorPlugin::InsertAnimation(AnimationData& animData, AnimationEvent* event)
{
   if (mSelectedActor == -1) return NULL;

   AnimationEvent* newEvent = event;

   int duration = (animData.mEndTime - animData.mStartTime) * 1000 / animData.mSpeed;

   if (!newEvent)
   {
      newEvent = new AnimationEvent(animData.mTime, animData.mTime + duration);
      newEvent->SetBlendIn(animData.mBlendInTime * 1000);
      newEvent->SetBlendOut(animData.mBlendOutTime * 1000);
      newEvent->SetWeight(animData.mWeight);
      mUI.mAnimationTrack->AddEvent(newEvent);
   }
   else
   {
      newEvent->SetNewTimes(animData.mTime, animData.mTime + duration);
   }

   animData.mEvent = newEvent;

   InsertAnimation(animData);

   return newEvent;
}

////////////////////////////////////////////////////////////////////////////////
void DirectorCinematicEditorPlugin::InsertAnimation(AnimationData& animData)
{
   if (mSelectedActor == -1) return;

   std::vector<AnimationData>& animationList = mActorData[mSelectedActor].mAnimationData;
   int count = (int)animationList.size();
   for (int index = 0; index < count; ++index)
   {
      AnimationData& data = animationList[index];

      if (data.mTime >= animData.mTime)
      {
         animationList.insert(animationList.begin() + index, animData);
         return;
      }
   }

   // If we get this far, it means the new transform will be appended to the end instead.
   animationList.push_back(animData);
}

////////////////////////////////////////////////////////////////////////////////
DirectorCinematicEditorPlugin::AnimationData* DirectorCinematicEditorPlugin::GetAnimationData(AnimationEvent* event, int* outIndex)
{
   if (!event) return NULL;

   if (mSelectedActor > -1)
   {
      std::vector<AnimationData>& animationList = mActorData[mSelectedActor].mAnimationData;
      int count = (int)animationList.size();
      for (int index = 0; index < count; ++index)
      {
         AnimationData& data = animationList[index];
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
void DirectorCinematicEditorPlugin::LerpActors(int time)
{
   // Iterate through all actors in the list.
   int actorCount = (int)mActorData.size();
   for (int actorIndex = 0; actorIndex < actorCount; ++actorIndex)
   {
      ActorData& actorData = mActorData[actorIndex];
      if (!actorData.mActor.valid()) continue;

      dtCore::Transformable* txable = NULL;
      actorData.mActor->GetDrawable(txable);
      if (!txable) continue;

      // Transformation
      if (actorData.mTransformEnabled)
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
            osg::Quat startRot, endRot;
            prev->mTransform.GetRotation(startRot);
            next->mTransform.GetRotation(endRot);
            osg::Quat lerpRot;
            lerpRot.slerp(alpha, startRot, endRot);

            // Lerp Scale
            osg::Vec3 startScale = prev->mScale;
            osg::Vec3 endScale = next->mScale;
            osg::Vec3 newScale = startScale + ((endScale - startScale) * alpha);

            // Apply new transform
            dtCore::Transform transform;
            txable->GetTransform(transform);
            transform.SetTranslation(newPos);
            transform.SetRotation(lerpRot);
            txable->SetTransform(transform);

            dtCore::Object* obj = dynamic_cast<dtCore::Object*>(txable);
            if (obj) obj->SetScale(newScale);
         }
      }

      // Animation
#ifdef MANUAL_ANIMATIONS
      {
         dtAnim::AnimationGameActor* animActor = NULL;
         animActor = dynamic_cast<dtAnim::AnimationGameActor*>(txable);
         if (animActor)
         {
            dtAnim::Cal3DModelWrapper* calWrapper = dynamic_cast<dtAnim::Cal3DModelWrapper*>
               (animActor->GetComponent<dtAnim::AnimationHelper>()->GetModelWrapper());

            if (calWrapper == NULL)
            {
               return;
            }

            CalMixer* calMixer = calWrapper->GetCalModel()->getMixer();
            dtAnim::SequenceMixer& mixer = animActor->GetComponent<dtAnim::AnimationHelper>()->GetSequenceMixer();

            int count = (int)actorData.mAnimationData.size();
            for (int index = 0; index < count; ++index)
            {
               AnimationData& data = actorData.mAnimationData[index];

               int eventLength = ((data.mEndTime - data.mStartTime) * 1000 / data.mSpeed);

               // Determine if this current animation should be playing.
               if (time >= data.mTime && time <= data.mTime + eventLength)
               {
                  CalAnimationAction* calAnim = NULL;
                  if (data.mAnimation > -1)
                  {
                     calAnim = calMixer->animationActionFromCoreAnimationId(data.mAnimation);;
                  }

                  if (!calAnim)
                  {
                     // Create the animation.
                     const dtAnim::AnimationChannel* anim = dynamic_cast<const dtAnim::AnimationChannel*>(mixer.GetRegisteredAnimation(data.mName));
                     if (anim)
                     {
                        data.mAnimation = anim->GetAnimation()->GetID();

                        calMixer->addManualAnimation(data.mAnimation);
                        calAnim = calMixer->animationActionFromCoreAnimationId(data.mAnimation);
                        if (calAnim)
                        {
                           calMixer->setManualAnimationOn(calAnim, true);
                           calMixer->setManualAnimationWeight(calAnim, data.mWeight);
                           calMixer->setManualAnimationCompositionFunction(calAnim, CalAnimation::CompositionFunctionAverage);
                        }
                     }
                  }

                  if (calAnim)
                  {
                     float animTime = ((time - data.mTime) * 0.001f);
                     float duration = eventLength * 0.001f;

                     float weight = data.mWeight;
                     // Blending in.
                     if (data.mBlendInTime > 0.0f && animTime < data.mBlendInTime)
                     {
                        weight *= animTime / data.mBlendInTime;
                     }
                     // Blending out.
                     else if (data.mBlendOutTime > 0.0f && duration - animTime < data.mBlendOutTime)
                     {
                        weight *= (duration - animTime) / data.mBlendOutTime;
                     }

                     animTime *= data.mSpeed;
                     animTime += data.mStartTime;

                     // Update the animation.
                     calMixer->setManualAnimationTime(calAnim, animTime);
                     calMixer->setManualAnimationWeight(calAnim, weight);
                  }

                  animActor->GetComponent<dtAnim::AnimationHelper>()->Update(0.0f);
               }
               else
               {
                  // Turn off the animation if it is still valid.
                  if (data.mAnimation > -1)
                  {
                     calMixer->removeManualAnimation(data.mAnimation);
                     animActor->GetComponent<dtAnim::AnimationHelper>()->Update(0.0f);
                     data.mAnimation = -1;
                  }
               }
            }
         }
      }
#else
      {
         dtAnim::AnimationGameActor* animActor = NULL;
         animActor = dynamic_cast<dtAnim::AnimationGameActor*>(txable);
         if (animActor)
         {
            // Morph Target stuff.
            //Cal3DModelWrapper* wrapper = animActor->GetComponent<dtAnim::AnimationHelper>()->GetModelWrapper();
            //CalMesh* mesh = wrapper->GetCalModel()->getMesh(0);
            //CalSubmesh *subMesh = mesh->getSubmesh(0);
            //if (subMesh)
            //{
            //   subMesh->setMorphTargetWeight(0, 1);
            //}
            dtAnim::SequenceMixer& mixer = animActor->GetComponent<dtAnim::AnimationHelper>()->GetSequenceMixer();
            mixer.ClearActiveAnimations(0.0f);
            mixer.Update(0.0f);
            int elapsedTime = 0;

            int count = (int)actorData.mAnimationData.size();
            for (int index = 0; index < count; ++index)
            {
               AnimationData& data = actorData.mAnimationData[index];

               int eventLength = ((data.mEndTime - data.mStartTime) * 1000 / data.mSpeed);

               // Determine if this current animation should be playing.
               if (time >= data.mTime && time <= data.mTime + eventLength)
               {
                  int startTime = data.mTime - (data.mStartTime * 1000);
                  if (startTime < elapsedTime)
                  {
                     elapsedTime = startTime;
                  }

                  // Step the animator time up to the time of this event.
                  if (elapsedTime < startTime)
                  {
                     float increment = (startTime - elapsedTime) * 0.001f;
                     elapsedTime = startTime;

                     animActor->GetComponent<dtAnim::AnimationHelper>()->Update(increment);
                  }

                  // Update the animation weight.
                  float weight = data.mWeight;
                  float duration = eventLength * 0.001f;

                  // Blending in.
                  float animTime = ((time - data.mTime) * 0.001f);
                  if (data.mBlendInTime > 0.0f && animTime < data.mBlendInTime)
                  {
                     weight *= animTime / data.mBlendInTime;
                  }
                  // Blending out.
                  else if (data.mBlendOutTime > 0.0f && duration - animTime < data.mBlendOutTime)
                  {
                     weight *= (duration - animTime) / data.mBlendOutTime;
                  }

                  // Create the animation.
                  const dtAnim::AnimationChannel* anim = dynamic_cast<const dtAnim::AnimationChannel*>(mixer.GetRegisteredAnimation(data.mName));
                  if (anim)
                  {
                     dtCore::RefPtr<dtAnim::AnimationChannel> newAnim = NULL;
                     newAnim = dynamic_cast<dtAnim::AnimationChannel*>(anim->Clone(animActor->GetComponent<dtAnim::AnimationHelper>()->GetModelWrapper()).get());
                     newAnim->SetLooping(false);
                     newAnim->SetAction(true);
                     newAnim->SetBaseWeight(weight);

                     mixer.PlayAnimation(newAnim);
                  }
               }
            }

            // Step the animator time up to the time slider position.
            if (elapsedTime < time)
            {
               float increment = (time - elapsedTime) * 0.001f;
               elapsedTime = time;

               animActor->GetComponent<dtAnim::AnimationHelper>()->Update(increment);
            }
         }
      }
#endif
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
