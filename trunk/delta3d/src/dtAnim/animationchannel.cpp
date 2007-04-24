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

namespace dtAnim
{

AnimationChannel::AnimationChannel(Cal3DModelWrapper* pModelWrapper, AnimationWrapper* pAnimationWrapper)
: mModelWrapper(pModelWrapper)
, mAnimationWrapper(pAnimationWrapper)
{
}


AnimationChannel::~AnimationChannel()
{
}


void AnimationChannel::Update(float dt, float parent_weight)
{
   mElapsedTime += dt;
   mCurrentWeight = parent_weight * mBaseWeight;

   if(mElapsedTime >= mStartTime)
   {
      if(mElapsedTime >= (mEndTime - mFadeOut))
      {
         ForceFadeOut(mFadeOut);
      }
      else
      {
         if(!IsActive())
         {
            SetActive(true);       
         }

         float t = (mStartTime + mFadeIn) - mElapsedTime;

         mModelWrapper->BlendCycle(mAnimationWrapper->GetID(), mCurrentWeight, (t > 0.0f) ? t : 0.0f);
      }
   }
   

}

void AnimationChannel::ForceFadeOut(float time)
{
   mPrune = true;
   mModelWrapper->ClearCycle(mAnimationWrapper->GetID(), time);
}

const std::string& AnimationChannel::GetName() const
{
   return mAnimationWrapper->GetName();
}

void AnimationChannel::SetName(const std::string& pName)
{
   mAnimationWrapper->SetName(pName);
}

}//namespace dtAnim
