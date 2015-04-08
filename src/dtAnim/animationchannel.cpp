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
#include <dtAnim/animationwrapper.h>

#include <dtAnim/cal3dmodelwrapper.h>
#include <osg/Math>
#include <dtUtil/log.h>
#include <dtUtil/mathdefines.h>

namespace dtAnim
{

/////////////////////////////////////////////////////////////////////////////////
AnimationChannel::AnimationChannel()
   : mIsAction(false)
   , mIsLooping(true)
   , mMaxDuration(0.0f)
   , mLastWeight(0.0f)
   , mModelWrapper(NULL)
   , mAnimationWrapper(NULL)
{
}

/////////////////////////////////////////////////////////////////////////////////
AnimationChannel::AnimationChannel(Cal3DModelWrapper* pModelWrapper, AnimationWrapper* pAnimationWrapper)
   : mIsAction(false)
   , mIsLooping(true)
   , mMaxDuration(0.0f)
   , mLastWeight(0.0f)
   , mModelWrapper(pModelWrapper)
   , mAnimationWrapper(pAnimationWrapper)
{
}

/////////////////////////////////////////////////////////////////////////////////
AnimationChannel::~AnimationChannel()
{
}

/////////////////////////////////////////////////////////////////////////////////
AnimationChannel::AnimationChannel(const AnimationChannel& pChannel)
   : Animatable(pChannel)
   , mIsAction(pChannel.IsAction())
   , mIsLooping(pChannel.IsLooping())
   , mMaxDuration(pChannel.GetMaxDuration())
   , mLastWeight(0.0f)
   , mModelWrapper(pChannel.mModelWrapper)
   , mAnimationWrapper(pChannel.mAnimationWrapper)
{
}

/////////////////////////////////////////////////////////////////////////////////
AnimationChannel& AnimationChannel::operator=(const AnimationChannel& pChannel)
{
   Animatable::operator=(pChannel);

   mIsAction         = pChannel.IsAction();
   mIsLooping        = pChannel.IsLooping();
   mMaxDuration      = pChannel.GetMaxDuration();
   mLastWeight       = 0.0f;
   mModelWrapper     = pChannel.mModelWrapper;
   mAnimationWrapper = pChannel.mAnimationWrapper;
   return *this;
}

/////////////////////////////////////////////////////////////////////////////////
dtCore::RefPtr<Animatable> AnimationChannel::Clone(Cal3DModelWrapper* wrapper) const
{
   dtCore::RefPtr<AnimationChannel> channel = new AnimationChannel(*this);
   channel->SetModel(wrapper);
   return dtCore::RefPtr<Animatable>(channel.get());
}

/////////////////////////////////////////////////////////////////////////////////
void AnimationChannel::SetAnimation(AnimationWrapper* pAnimation)
{
   mAnimationWrapper = pAnimation;
}

/////////////////////////////////////////////////////////////////////////////////
AnimationWrapper* AnimationChannel::GetAnimation()
{
   return mAnimationWrapper.get();
}

/////////////////////////////////////////////////////////////////////////////////
const AnimationWrapper* AnimationChannel::GetAnimation() const
{
   return mAnimationWrapper.get();
}

/////////////////////////////////////////////////////////////////////////////////
Cal3DModelWrapper* AnimationChannel::GetModel()
{
   return mModelWrapper.get();
}

/////////////////////////////////////////////////////////////////////////////////
const Cal3DModelWrapper* AnimationChannel::GetModel() const
{
   return mModelWrapper.get();
}

/////////////////////////////////////////////////////////////////////////////////
void AnimationChannel::SetModel(Cal3DModelWrapper* pWrapper)
{
   mModelWrapper = pWrapper;
}


/////////////////////////////////////////////////////////////////////////////////
void AnimationChannel::Update(float dt)
{
   if (!mModelWrapper.valid())
   {
      LOG_ERROR("AnimationChannel '" + GetName() + "' does not have a valid Cal3DModelWrapper.");
      return;
   }

   if (GetEndTime() > 0.0f && GetElapsedTime() >= GetEndTime())
   {
      SetPrune(true);
      mLastWeight = 0.0f;
   }
   else if (!IsActive())
   {
      if (IsAction())
      {
         mModelWrapper->ExecuteAction(mAnimationWrapper->GetID(), GetFadeIn(), GetFadeOut(), GetBaseWeight());
      }
      else
      {
         mModelWrapper->BlendCycle(mAnimationWrapper->GetID(), GetCurrentWeight(), 0.0);
         if (!dtUtil::Equivalent(GetSpeed(), 1.0f))
            mModelWrapper->SetSpeedFactor(mAnimationWrapper->GetID(), GetSpeed());
         mLastWeight = GetCurrentWeight();
         mAnimationWrapper->SetSpeed(GetSpeed());
      }

      SetActive(true);
   }
   else
   {
      bool forceSpeedUpdate = false;
      if (!IsAction() && !dtUtil::Equivalent(mLastWeight, GetCurrentWeight()))
      {
         mModelWrapper->BlendCycle(mAnimationWrapper->GetID(), GetCurrentWeight(), 0.0);
         mLastWeight = GetCurrentWeight();
         forceSpeedUpdate = true;
      }
      if (!IsAction() && (forceSpeedUpdate || !dtUtil::Equivalent(GetSpeed(), mAnimationWrapper->GetSpeed())))
      {
         if (!dtUtil::Equivalent(GetSpeed(), 1.0f))
         {
            mModelWrapper->SetSpeedFactor(mAnimationWrapper->GetID(), GetSpeed());
         }
         mAnimationWrapper->SetSpeed(GetSpeed());
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
      SetEndTime(GetStartTime() + mAnimationWrapper->GetDuration());
   }
   else
   {
      SetEndTime(0.0f);
   }
}

/////////////////////////////////////////////////////////////////////////////////
float AnimationChannel::CalculateDuration() const
{
   float duration = mAnimationWrapper.valid() ? mAnimationWrapper->GetDuration() / GetSpeed() : mMaxDuration;
   
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
      if (IsAction())
      {
         mModelWrapper->RemoveAction(mAnimationWrapper->GetID());
      }
      else
      {
         mModelWrapper->ClearCycle(mAnimationWrapper->GetID(), 0.0f);
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
      mModelWrapper->ClearCycle(mAnimationWrapper->GetID(), time);
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
   float duration = mAnimationWrapper.valid() ? mAnimationWrapper->GetDuration() / mAnimationWrapper->GetSpeed() : 0.0f;

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
