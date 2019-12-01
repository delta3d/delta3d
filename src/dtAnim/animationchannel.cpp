 /* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007, Alion Science and Technology
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
 * Bradley Anderegg 04/20/2007
 */

#include <dtAnim/animationchannel.h>

#include <dtAnim/basemodelwrapper.h>
#include <dtAnim/animationinterface.h>
#include <dtUtil/log.h>

#include <osg/Math>

namespace dtAnim
{

/////////////////////////////////////////////////////////////////////////////////
AnimationChannel::AnimationChannel()
   : mIsAction(false)
   , mIsLooping(true)
   , mDuration(0.0f)
   , mMaxDuration(0.0f)
   , mLastWeight(0.0f)
   , mModelWrapper(NULL)
{
}

/////////////////////////////////////////////////////////////////////////////////
AnimationChannel::AnimationChannel(BaseModelWrapper* modelWrapper)
   : mIsAction(false)
   , mIsLooping(true)
   , mDuration(0.0f)
   , mMaxDuration(0.0f)
   , mLastWeight(0.0f)
   , mModelWrapper(modelWrapper)
{
}

/////////////////////////////////////////////////////////////////////////////////
AnimationChannel::~AnimationChannel()
{
}

/////////////////////////////////////////////////////////////////////////////////
AnimationChannel::AnimationChannel(const AnimationChannel& channel)
   : Animatable(channel)
   , mIsAction(channel.mIsAction)
   , mIsLooping(channel.mIsLooping)
   , mDuration(channel.mDuration)
   , mMaxDuration(channel.mMaxDuration)
   , mLastWeight(0.0f)
   , mAnimName(channel.mAnimName)
   , mAnim(channel.mAnim)
   , mModelWrapper(channel.mModelWrapper)
{
}

/////////////////////////////////////////////////////////////////////////////////
AnimationChannel& AnimationChannel::operator=(const AnimationChannel& channel)
{
   Animatable::operator=(channel);

   mIsAction         = channel.mIsAction;
   mIsLooping        = channel.mIsLooping;
   mDuration         = channel.mDuration;
   mMaxDuration      = channel.mMaxDuration;
   mLastWeight       = 0.0f;
   mAnimName         = channel.mAnimName;
   mAnim             = channel.mAnim;
   mModelWrapper     = channel.mModelWrapper;
   return *this;
}

/////////////////////////////////////////////////////////////////////////////////
dtCore::RefPtr<Animatable> AnimationChannel::Clone(BaseModelWrapper* wrapper) const
{
   dtCore::RefPtr<AnimationChannel> channel = new AnimationChannel(*this);
   channel->SetModel(wrapper);
   return dtCore::RefPtr<Animatable>(channel.get());
}

/////////////////////////////////////////////////////////////////////////////////
BaseModelWrapper* AnimationChannel::GetModel()
{
   return mModelWrapper.get();
}

/////////////////////////////////////////////////////////////////////////////////
const BaseModelWrapper* AnimationChannel::GetModel() const
{
   return mModelWrapper.get();
}

/////////////////////////////////////////////////////////////////////////////////
dtAnim::AnimationInterface* AnimationChannel::GetAnimation()
{
   if (mModelWrapper.valid())
   {
      mAnim = mModelWrapper->GetAnimation(GetAnimationName());
   }
   return mAnim.get();
}

/////////////////////////////////////////////////////////////////////////////////
const dtAnim::AnimationInterface* AnimationChannel::GetAnimation() const
{
   if (mModelWrapper.valid())
   {
      mAnim = mModelWrapper->GetAnimation(GetAnimationName());
   }
   return mAnim.get();
}

/////////////////////////////////////////////////////////////////////////////////
void AnimationChannel::SetAnimationName(const std::string& name)
{
   mAnimName = name;
}

/////////////////////////////////////////////////////////////////////////////////
const std::string& AnimationChannel::GetAnimationName() const
{
   return mAnimName.Get();
}

/////////////////////////////////////////////////////////////////////////////////
void AnimationChannel::SetModel(BaseModelWrapper* modelWrapper)
{
   mModelWrapper = modelWrapper;
}

/////////////////////////////////////////////////////////////////////////////////
void AnimationChannel::Update(float dt)
{
   if (!mModelWrapper.valid())
   {
      LOG_ERROR("AnimationChannel '" + GetName() + "' does not have a valid ModelWrapper.");
      return;
   }
   
   if (!mAnim.valid())
   {
      mAnim = GetAnimation();
   }

   if (GetEndTime() > 0.0f && GetElapsedTime() >= GetEndTime())
   {
      SetPrune(true);
   }
   else if (mAnim.valid())
   {
      if (!IsActive())
      {
         if (IsAction())
         {
            mAnim->PlayAction(GetFadeIn(), GetFadeOut(), GetBaseWeight());
         }
         else
         {
            mAnim->PlayCycle(GetCurrentWeight(), 0.0f);
            mLastWeight = GetCurrentWeight();
         }

         SetActive(true);
      }
      else
      {
         if (!IsAction() && !osg::equivalent(mLastWeight, GetCurrentWeight()))
         {
            mAnim->PlayCycle(GetCurrentWeight(), 0.0f);
            mLastWeight = GetCurrentWeight();
         }
      }
   }

}

/////////////////////////////////////////////////////////////////////////////////
void AnimationChannel::Recalculate()
{
   if (mIsLooping && GetMaxDuration() > 0.0f)
   {
      SetEndTime(GetStartTime() + GetMaxDuration());
   }
   else if (IsAction() && !mIsLooping)
   {
      SetEndTime(GetStartTime() + GetDuration());
   }
   else
   {
      SetEndTime(0.0f);
   }

}

/////////////////////////////////////////////////////////////////////////////////
float AnimationChannel::CalculateDuration() const
{
   float duration = mDuration > 0.0f ? mDuration : mMaxDuration;
   
   if (IsLooping() && mMaxDuration <= 0.0f)
   {
      duration = Animatable::INFINITE_TIME;
   }
   
   if(mMaxDuration > 0.0f && duration > mMaxDuration)
   {
      duration = mMaxDuration;
   }

   return duration;
}

/////////////////////////////////////////////////////////////////////////////////
void AnimationChannel::Prune()
{
   if (IsActive())
   {
      if ( ! mAnim.valid())
      {
         LOG_ERROR("Animation reference \"" + GetName() + "\" is invalid. Cannot complete Prune.");
      }
      else // valid animation reference
      {
         if (IsAction())
         {
            mAnim->ClearAction();
         }
         else
         {
            mAnim->ClearCycle(0.0f);
         }
      }

      SetActive(false);
   }

}

/////////////////////////////////////////////////////////////////////////////////
void AnimationChannel::ForceFadeOut(float time)
{
   SetPrune(true);

   // only non actions can fade out, else by keeping active true
   // we will do a remove action if necessary on prune
   if (!mIsAction)
   {
      if ( ! mAnim.valid())
      {
         LOG_ERROR("Animation reference \"" + GetName() + "\" is invalid. Cannot complete ForceFadeOut.");
      }
      else // valid animation reference
      {
         mAnim->ClearCycle(time);
      }
   }
}

/////////////////////////////////////////////////////////////////////////////////
bool AnimationChannel::IsLooping() const
{
   return mIsLooping;
}

 /////////////////////////////////////////////////////////////////////////////////
void AnimationChannel::SetLooping(bool b)
{
   mIsLooping = b;
}

/////////////////////////////////////////////////////////////////////////////////
bool AnimationChannel::IsAction() const
{
   return mIsAction;
}

/////////////////////////////////////////////////////////////////////////////////
void AnimationChannel::SetAction(bool b)
{
   mIsAction = b;
}

/////////////////////////////////////////////////////////////////////////////////
float AnimationChannel::GetDuration() const
{
   return mDuration;
}

/////////////////////////////////////////////////////////////////////////////////
void AnimationChannel::SetDuration(float duration)
{
   mDuration = duration;
}

/////////////////////////////////////////////////////////////////////////////////
float AnimationChannel::GetMaxDuration() const
{
   return mMaxDuration;
}

/////////////////////////////////////////////////////////////////////////////////
void AnimationChannel::SetMaxDuration(float seconds)
{
   mMaxDuration = seconds;
}

////////////////////////////////////////////////////////////////////////////////
float AnimationChannel::ConvertToRelativeTimeInAnimationScope(double timeToConvert) const
{
   float duration = mDuration > 0.0f ? mDuration / GetSpeed() : 0.0f;

   // Get what the the Base Class would set as the relative time.
   float elapsedTime = BaseClass::ConvertToRelativeTimeInAnimationScope(timeToConvert);

   // If the animation has no determined duration,
   // then no time could have relatively elapsed.
   if(duration == 0.0f)
   {
      elapsedTime = 0.0f;
   }

   // Determine the relative elapsed time.
   if (duration != 0.0f)
   {
      // If the animation is looping, wrap the relative time around
      // back to the beginning if the time has passed the duration.
      if (IsLooping())
      {
         // Reduce the elapsed time to the remainder.
         elapsedTime -= duration * floor(elapsedTime / duration);
      }
      // If the animation was to play once, cap it to the duration.
      else if (elapsedTime > duration)
      {
         elapsedTime = duration;
      }
   }

   return elapsedTime;
}

} // namespace dtAnim
