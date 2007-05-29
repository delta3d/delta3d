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

namespace dtAnim
{

/////////////////////////////////////////////////////////////////////////////////
AnimationChannel::AnimationChannel()
: mIsAction(false)
, mIsLooping(true)
, mMaxDuration(0.0f)
, mLastWeight(0.0f)
, mModelWrapper(0)
, mAnimationWrapper(0)
{
}

/////////////////////////////////////////////////////////////////////////////////
AnimationChannel::AnimationChannel(Cal3DModelWrapper* pModelWrapper, AnimationWrapper* pAnimationWrapper)
: mIsAction(false)
, mIsLooping(true)
, mMaxDuration(0.0f)
, mModelWrapper(pModelWrapper)
, mAnimationWrapper(pAnimationWrapper)
{
}

/////////////////////////////////////////////////////////////////////////////////
AnimationChannel::~AnimationChannel()
{
}
//TODO should probably make a copy constructor for this
dtCore::RefPtr<Animatable> AnimationChannel::Clone() const
{
   return new AnimationChannel(*this);
}

/////////////////////////////////////////////////////////////////////////////////
void AnimationChannel::SetAnimation(AnimationWrapper* pAnimation)
{ 
   mAnimationWrapper = pAnimation;
}

/////////////////////////////////////////////////////////////////////////////////
void AnimationChannel::SetModel(Cal3DModelWrapper* pWrapper)
{
   mModelWrapper = pWrapper;
}


/////////////////////////////////////////////////////////////////////////////////
void AnimationChannel::Update(float dt)
{
   if(!mModelWrapper.valid())
   {
      LOG_ERROR("AnimationChannel '" + GetName() + "' does not have a valid Cal3DModelWrapper.");
      return;
   }
   
   if(mEndTime > 0.0f && mElapsedTime > mEndTime)
   {
      SetPrune(true);
   }
   else if(!IsActive())
   {
      if(mIsAction)
      {
         mModelWrapper->ExecuteAction(mAnimationWrapper->GetID(), mFadeIn, mFadeOut);         
      }
      else
      {
         mModelWrapper->BlendCycle(mAnimationWrapper->GetID(), mCurrentWeight, 0.0f);
         mLastWeight = mCurrentWeight;
      }

      SetActive(true);
   }
   else
   {
      if(!mIsAction && !osg::equivalent(mLastWeight, mCurrentWeight))
      {
         mModelWrapper->BlendCycle(mAnimationWrapper->GetID(), mCurrentWeight, 0.0f);
         mLastWeight = mCurrentWeight;
      }         
   }

}

/////////////////////////////////////////////////////////////////////////////////
void AnimationChannel::Recalculate()
{
   if(mIsLooping && GetMaxDuration() > 0.0f)
   {
      SetEndTime(GetStartTime() + GetMaxDuration());
   }
   else if(IsAction() && !mIsLooping)
   {
      SetEndTime(GetStartTime() + mAnimationWrapper->GetDuration());
   }   
   else
   {
      SetEndTime(0.0f);
   }

}

/////////////////////////////////////////////////////////////////////////////////
void AnimationChannel::Prune()
{  
   if(mActive)
   {
      if(mIsAction)
      {
         mModelWrapper->RemoveAction(mAnimationWrapper->GetID());
      }
      else
      {
         mModelWrapper->ClearCycle(mAnimationWrapper->GetID(), 0.0f);
      }

      mActive = false;
   }   

}

/////////////////////////////////////////////////////////////////////////////////
void AnimationChannel::ForceFadeOut(float time)
{
   SetPrune(true);

   //only non actions can fade out, else by keeping active true
   //we will do a remove action if necessary on prune
   if(!mIsAction)
   {
      mModelWrapper->ClearCycle(mAnimationWrapper->GetID(), time);
      SetActive(false);
   }
}

/////////////////////////////////////////////////////////////////////////////////
const std::string& AnimationChannel::GetName() const
{
   return mAnimationWrapper->GetName();
}

/////////////////////////////////////////////////////////////////////////////////
void AnimationChannel::SetName(const std::string& pName)
{
   mAnimationWrapper->SetName(pName);
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
void AnimationChannel::SetMaxDuration(float b)
{
   mMaxDuration = b;
}


}//namespace dtAnim
