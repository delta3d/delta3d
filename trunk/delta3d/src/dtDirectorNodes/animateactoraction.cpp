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
#include <prefix/dtdirectornodesprefix.h>
#include <dtDirectorNodes/animateactoraction.h>

#include <dtDAL/actoridactorproperty.h>
#include <dtDAL/floatactorproperty.h>
#include <dtDAL/stringactorproperty.h>
#include <dtDAL/vectoractorproperties.h>
#include <dtDAL/arrayactorproperty.h>
#include <dtDAL/containeractorproperty.h>

#include <dtDirector/director.h>

#include <dtAnim/animationgameactor.h>
#include <dtAnim/animationchannel.h>
#include <dtAnim/animationhelper.h>
#include <dtAnim/sequencemixer.h>
#include <dtAnim/animationwrapper.h>

#include <cal3d/model.h>
#include <cal3d/mixer.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   AnimateActorAction::AnimateActorAction()
   : ActionNode()
   , mTime(0.0f)
   , mIsActive(false)
   , mAnimIndex(0)
   {
      mAnimActor = "";

      AddAuthor("Jeff P. Houde");
   }

   ////////////////////////////////////////////////////////////////////////////////
   AnimateActorAction::~AnimateActorAction()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AnimateActorAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      // Create multiple inputs for different operations.
      mInputs.clear();
      mInputs.push_back(InputLink(this, "Start", "Starts the animation."));
      mInputs.push_back(InputLink(this, "Stop", "Stops the animation."));

      mOutputs.clear();
      mOutputs.push_back(OutputLink(this, "Started", "Activated when the animation has started."));
      mOutputs.push_back(OutputLink(this, "Stopped", "Activated when the animation was forcibly stopped."));
      mOutputs.push_back(OutputLink(this, "Finished", "Activated when the animation has finished playing."));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AnimateActorAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      // Create our value links.
      dtDAL::ActorIDActorProperty* actorProp = new dtDAL::ActorIDActorProperty(
         "Actor", "Actor",
         dtDAL::ActorIDActorProperty::SetFuncType(this, &AnimateActorAction::SetAnimActor),
         dtDAL::ActorIDActorProperty::GetFuncType(this, &AnimateActorAction::GetAnimActor),
         "dtActors::AnimationGameActor", "The actor to animate.");
      AddProperty(actorProp);

      // Animation Data Properties
      {
         dtDAL::ContainerActorProperty* animDataProp = new dtDAL::ContainerActorProperty(
            "Anim Data", "Anim Data", "The animation data.", "");

         dtDAL::StringActorProperty* nameProp = new dtDAL::StringActorProperty(
            "Name", "Name",
            dtDAL::StringActorProperty::SetFuncType(this, &AnimateActorAction::SetAnimName),
            dtDAL::StringActorProperty::GetFuncType(this, &AnimateActorAction::GetAnimName),
            "The name of the animation to play.");
         animDataProp->AddProperty(nameProp);

         dtDAL::FloatActorProperty* timeProp = new dtDAL::FloatActorProperty(
            "Start Time", "Start Time",
            dtDAL::FloatActorProperty::SetFuncType(this, &AnimateActorAction::SetAnimTime),
            dtDAL::FloatActorProperty::GetFuncType(this, &AnimateActorAction::GetAnimTime),
            "The time to start the animation.");
         animDataProp->AddProperty(timeProp);

         dtDAL::FloatActorProperty* durationProp = new dtDAL::FloatActorProperty(
            "Duration", "Duration",
            dtDAL::FloatActorProperty::SetFuncType(this, &AnimateActorAction::SetAnimDuration),
            dtDAL::FloatActorProperty::GetFuncType(this, &AnimateActorAction::GetAnimDuration),
            "The duration of the animation.");
         animDataProp->AddProperty(durationProp);

         dtDAL::FloatActorProperty* blendInProp = new dtDAL::FloatActorProperty(
            "Blend in Time", "Blend in Time",
            dtDAL::FloatActorProperty::SetFuncType(this, &AnimateActorAction::SetBlendInTime),
            dtDAL::FloatActorProperty::GetFuncType(this, &AnimateActorAction::GetBlendInTime),
            "The animation blend in time.");
         animDataProp->AddProperty(blendInProp);

         dtDAL::FloatActorProperty* blendOutProp = new dtDAL::FloatActorProperty(
            "Blend out Time", "Blend out Time",
            dtDAL::FloatActorProperty::SetFuncType(this, &AnimateActorAction::SetBlendOutTime),
            dtDAL::FloatActorProperty::GetFuncType(this, &AnimateActorAction::GetBlendOutTime),
            "The animation blend out time.");
         animDataProp->AddProperty(blendOutProp);

         dtDAL::FloatActorProperty* weightProp = new dtDAL::FloatActorProperty(
            "Weight", "Weight",
            dtDAL::FloatActorProperty::SetFuncType(this, &AnimateActorAction::SetAnimWeight),
            dtDAL::FloatActorProperty::GetFuncType(this, &AnimateActorAction::GetAnimWeight),
            "The animation weight.");
         animDataProp->AddProperty(weightProp);

         dtDAL::FloatActorProperty* offsetProp = new dtDAL::FloatActorProperty(
            "Start Offset", "Start Offset",
            dtDAL::FloatActorProperty::SetFuncType(this, &AnimateActorAction::SetAnimOffset),
            dtDAL::FloatActorProperty::GetFuncType(this, &AnimateActorAction::GetAnimOffset),
            "The animation start offset.");
         animDataProp->AddProperty(offsetProp);

         dtDAL::FloatActorProperty* speedProp = new dtDAL::FloatActorProperty(
            "Speed", "Speed",
            dtDAL::FloatActorProperty::SetFuncType(this, &AnimateActorAction::SetAnimSpeed),
            dtDAL::FloatActorProperty::GetFuncType(this, &AnimateActorAction::GetAnimSpeed),
            "The animation speed.");
         animDataProp->AddProperty(speedProp);

         dtDAL::ArrayActorPropertyBase* animListProp = new dtDAL::ArrayActorProperty<AnimData>(
            "Animations", "Animations", "List of animations to play.",
            dtDAL::ArrayActorProperty<AnimData>::SetIndexFuncType(this, &AnimateActorAction::SetAnimIndex),
            dtDAL::ArrayActorProperty<AnimData>::GetDefaultFuncType(this, &AnimateActorAction::GetDefaultAnim),
            dtDAL::ArrayActorProperty<AnimData>::GetArrayFuncType(this, &AnimateActorAction::GetAnimArray),
            dtDAL::ArrayActorProperty<AnimData>::SetArrayFuncType(this, &AnimateActorAction::SetAnimArray),
            animDataProp, "");
         AddProperty(animListProp);
      }

      dtDAL::FloatActorProperty* timeProp = new dtDAL::FloatActorProperty(
         "Time", "Time",
         dtDAL::FloatActorProperty::SetFuncType(this, &AnimateActorAction::SetTime),
         dtDAL::FloatActorProperty::GetFuncType(this, &AnimateActorAction::GetTime),
         "The current time.");

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, actorProp, true, true, true));
      mValues.push_back(ValueLink(this, timeProp, false, false, true));
   }

   //////////////////////////////////////////////////////////////////////////
   bool AnimateActorAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      switch (input)
      {
      case INPUT_START:
         {
            if (firstUpdate)
            {
               OutputLink* link = GetOutputLink("Started");
               if (link) link->Activate();
            }

            // On the first activation, initialize.
            if (!mIsActive)
            {
               if (firstUpdate)
               {
                  mIsActive = true;

                  // Activate the "Out" output link.
                  ActionNode::Update(simDelta, delta, input, firstUpdate);
               }
               else
               {
                  // TODO: Stop all animations.

                  OutputLink* link = GetOutputLink("Finished");
                  if (link) link->Activate();
                  return false;
               }
            }

            float curTime = GetFloat("Time");
            int count = 0;

#ifndef MANUAL_ANIMATIONS
            // Find any animations that need to play.
            std::vector<AnimData> playList;
            count = (int)mAnimList.size();
            for (int index = 0; index < count; ++index)
            {
               AnimData& data = mAnimList[index];

               if (data.mTime <= curTime && data.mTime + data.mDuration >= curTime)
               {
                  // Add this animation to the play list, sorted by start time.
                  bool added = false;
                  int sortCount = (int)playList.size();
                  for (int sortIndex = 0; sortIndex < sortCount; ++sortIndex)
                  {
                     if (playList[sortIndex].mTime >= data.mTime)
                     {
                        playList.insert(playList.begin() + sortIndex, data);
                        added = true;
                     }
                  }

                  if (!added)
                  {
                     playList.push_back(data);
                  }
               }
            }
#endif

            count = GetPropertyCount("Actor");
            for (int index = 0; index < count; ++index)
            {
               dtDAL::BaseActorObject* proxy = GetActor("Actor", index);
               if (proxy)
               {
                  dtAnim::AnimationGameActor* actor = dynamic_cast<dtAnim::AnimationGameActor*>(proxy->GetActor());
                  if (actor)
                  {
#ifdef MANUAL_ANIMATIONS
                     CalMixer* calMixer = actor->GetHelper()->GetModelWrapper()->GetCalModel()->getMixer();
                     dtAnim::SequenceMixer& mixer = actor->GetHelper()->GetSequenceMixer();

                     int animCount = (int)mAnimList.size();
                     for (int animIndex = 0; animIndex < animCount; ++animIndex)
                     {
                        AnimData& data = mAnimList[animIndex];

                        if (data.mTime <= curTime && data.mTime + data.mDuration >= curTime)
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
                              float animTime = curTime - data.mTime;

                              // Update the animation weight.
                              float weight = data.mWeight;

                              // Blending in.
                              if (data.mBlendInTime > 0.0f && animTime < data.mBlendInTime)
                              {
                                 weight *= animTime / data.mBlendInTime;
                              }
                              // Blending out.
                              else if (data.mBlendOutTime > 0.0f && data.mDuration - animTime < data.mBlendOutTime)
                              {
                                 weight *= (data.mDuration - animTime) / data.mBlendOutTime;
                              }

                              animTime *= data.mSpeed;
                              animTime += data.mStartOffset;

                              // Update the animation.
                              calMixer->setManualAnimationTime(calAnim, animTime);
                              calMixer->setManualAnimationWeight(calAnim, weight);
                           }

                           actor->GetHelper()->Update(0.0f);
                        }
                        else
                        {
                           // Turn off the animation if it is still valid.
                           if (data.mAnimation > -1)
                           {
                              calMixer->removeManualAnimation(data.mAnimation);
                              actor->GetHelper()->Update(0.0f);
                              data.mAnimation = -1;
                           }
                        }
                     }

#else
                     // First clear all animations currently playing in this actor.
                     dtAnim::SequenceMixer& mixer = actor->GetHelper()->GetSequenceMixer();
                     mixer.ClearActiveAnimations(0.0f);
                     mixer.Update(0.0f);

                     float elapsedTime = 0.0f;
                     int animCount = (int)playList.size();
                     for (int animIndex = 0; animIndex < animCount; ++animIndex)
                     {
                        AnimData& data = playList[animIndex];

                        float startTime = data.mTime - data.mStartOffset;
                        if (startTime < elapsedTime)
                        {
                           elapsedTime = startTime;
                        }

                        if (elapsedTime < startTime)
                        {
                           float increment = startTime - elapsedTime;
                           elapsedTime = startTime;

                           actor->GetHelper()->Update(increment);
                        }

                        // Update the animation weight.
                        float weight = data.mWeight;

                        // Blending in.
                        float animTime = curTime - data.mTime;
                        if (data.mBlendInTime > 0.0f && animTime < data.mBlendInTime)
                        {
                           weight *= animTime / data.mBlendInTime;
                        }
                        // Blending out.
                        else if (data.mBlendOutTime > 0.0f && data.mDuration - animTime < data.mBlendOutTime)
                        {
                           weight *= (data.mDuration - animTime) / data.mBlendOutTime;
                        }

                        // Create the animation.
                        const dtAnim::AnimationChannel* anim = dynamic_cast<const dtAnim::AnimationChannel*>(mixer.GetRegisteredAnimation(data.mName));
                        if (anim)
                        {
                           dtCore::RefPtr<dtAnim::AnimationChannel> newAnim = NULL;
                           newAnim = dynamic_cast<dtAnim::AnimationChannel*>(anim->Clone(actor->GetHelper()->GetModelWrapper()).get());
                           newAnim->SetLooping(false);
                           newAnim->SetAction(true);
                           newAnim->SetBaseWeight(weight);

                           mixer.PlayAnimation(newAnim);
                        }
                     }

                     if (elapsedTime <= curTime)
                     {
                        float increment = curTime - elapsedTime;
                        elapsedTime = curTime;

                        actor->GetHelper()->Update(increment);
                     }
#endif
                  }
               }
            }

            return true;
         }
         break;

      case INPUT_STOP:
         {
            mIsActive = false;

            int count = GetPropertyCount("Actor");
            for (int index = 0; index < count; ++index)
            {
               dtDAL::BaseActorObject* proxy = GetActor("Actor", index);
               if (proxy)
               {
                  dtAnim::AnimationGameActor* actor = dynamic_cast<dtAnim::AnimationGameActor*>(proxy->GetActor());
                  if (actor)
                  {
                     // Clear all animations currently playing in this actor.
#ifdef MANUAL_ANIMATIONS
                     CalMixer* calMixer = actor->GetHelper()->GetModelWrapper()->GetCalModel()->getMixer();

                     int animCount = (int)mAnimList.size();
                     for (int animIndex = 0; animIndex < animCount; ++animIndex)
                     {
                        AnimData& data = mAnimList[animIndex];

                        // Turn off the animation if it is still valid.
                        if (data.mAnimation > -1)
                        {
                           calMixer->removeManualAnimation(data.mAnimation);
                           actor->GetHelper()->Update(0.0f);
                           data.mAnimation = -1;
                        }
                     }
#else
                     dtAnim::SequenceMixer& mixer = actor->GetHelper()->GetSequenceMixer();
                     mixer.ClearActiveAnimations(0.0f);
                     actor->GetHelper()->Update(0.0f);
#endif
                  }
               }
            }

            // Activate the "Stopped" output link.
            if (firstUpdate)
            {
               OutputLink* link = GetOutputLink("Stopped");
               if (link) link->Activate();
            }
         }
         break;
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AnimateActorAction::SetAnimActor(const dtCore::UniqueId& value)
   {
      mAnimActor = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId AnimateActorAction::GetAnimActor()
   {
      return mAnimActor;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AnimateActorAction::SetAnimName(const std::string& value)
   {
      if (mAnimIndex >= (int)mAnimList.size()) return;

      AnimData& data = mAnimList[mAnimIndex];
      data.mName = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string AnimateActorAction::GetAnimName()
   {
      if (mAnimIndex >= (int)mAnimList.size()) return "";

      AnimData& data = mAnimList[mAnimIndex];
      return data.mName;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AnimateActorAction::SetAnimTime(float value)
   {
      if (mAnimIndex >= (int)mAnimList.size()) return;

      AnimData& data = mAnimList[mAnimIndex];
      data.mTime = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float AnimateActorAction::GetAnimTime()
   {
      if (mAnimIndex >= (int)mAnimList.size()) return 0.0f;

      AnimData& data = mAnimList[mAnimIndex];
      return data.mTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AnimateActorAction::SetAnimDuration(float value)
   {
      if (mAnimIndex >= (int)mAnimList.size()) return;

      AnimData& data = mAnimList[mAnimIndex];
      data.mDuration = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float AnimateActorAction::GetAnimDuration()
   {
      if (mAnimIndex >= (int)mAnimList.size()) return 0.0f;

      AnimData& data = mAnimList[mAnimIndex];
      return data.mDuration;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AnimateActorAction::SetBlendInTime(float value)
   {
      if (mAnimIndex >= (int)mAnimList.size()) return;

      AnimData& data = mAnimList[mAnimIndex];
      data.mBlendInTime = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float AnimateActorAction::GetBlendInTime()
   {
      if (mAnimIndex >= (int)mAnimList.size()) return 0.0f;

      AnimData& data = mAnimList[mAnimIndex];
      return data.mBlendInTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AnimateActorAction::SetBlendOutTime(float value)
   {
      if (mAnimIndex >= (int)mAnimList.size()) return;

      AnimData& data = mAnimList[mAnimIndex];
      data.mBlendOutTime = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float AnimateActorAction::GetBlendOutTime()
   {
      if (mAnimIndex >= (int)mAnimList.size()) return 0.0f;

      AnimData& data = mAnimList[mAnimIndex];
      return data.mBlendOutTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AnimateActorAction::SetAnimWeight(float value)
   {
      if (mAnimIndex >= (int)mAnimList.size()) return;

      AnimData& data = mAnimList[mAnimIndex];
      data.mWeight = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float AnimateActorAction::GetAnimWeight()
   {
      if (mAnimIndex >= (int)mAnimList.size()) return 1.0f;

      AnimData& data = mAnimList[mAnimIndex];
      return data.mWeight;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AnimateActorAction::SetAnimOffset(float value)
   {
      if (mAnimIndex >= (int)mAnimList.size()) return;

      AnimData& data = mAnimList[mAnimIndex];
      data.mStartOffset = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float AnimateActorAction::GetAnimOffset()
   {
      if (mAnimIndex >= (int)mAnimList.size()) return 0.0f;

      AnimData& data = mAnimList[mAnimIndex];
      return data.mStartOffset;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AnimateActorAction::SetAnimSpeed(float value)
   {
      if (mAnimIndex >= (int)mAnimList.size()) return;

      AnimData& data = mAnimList[mAnimIndex];
      data.mSpeed = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float AnimateActorAction::GetAnimSpeed()
   {
      if (mAnimIndex >= (int)mAnimList.size()) return 0.0f;

      AnimData& data = mAnimList[mAnimIndex];
      return data.mSpeed;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AnimateActorAction::SetAnimIndex(int index)
   {
      mAnimIndex = index;
   }

   ////////////////////////////////////////////////////////////////////////////////
   AnimateActorAction::AnimData AnimateActorAction::GetDefaultAnim()
   {
      AnimData data;
      data.mName = "";
      data.mTime = 0.0f;
      data.mDuration = 0.0f;
      data.mBlendInTime = 0.0f;
      data.mBlendOutTime = 0.0f;
      data.mWeight = 1.0f;
      data.mStartOffset = 0.0f;
      data.mSpeed = 1.0f;
      return data;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<AnimateActorAction::AnimData> AnimateActorAction::GetAnimArray()
   {
      return mAnimList;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AnimateActorAction::SetAnimArray(const std::vector<AnimateActorAction::AnimData>& value)
   {
      mAnimList = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AnimateActorAction::SetTime(float value)
   {
      mTime = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float AnimateActorAction::GetTime()
   {
      return mTime;
   }
}

////////////////////////////////////////////////////////////////////////////////
