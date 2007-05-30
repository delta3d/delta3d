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

#include <dtAnim/animationwrapper.h>


namespace dtAnim
{

AnimationWrapper::AnimationWrapper(const std::string& pName, int pAnimationID)
: mAnimationID(pAnimationID)
, mDuration(0.0f)
, mSpeed(1.0f)
, mName(pName)
{
}


AnimationWrapper::~AnimationWrapper()
{
}


int AnimationWrapper::GetID() const
{
   return mAnimationID;
}

float AnimationWrapper::GetDuration() const
{
   return mDuration;
}

void AnimationWrapper::SetDuration(float duration)
{
   mDuration = duration;
}

float AnimationWrapper::GetSpeed() const
{
   return mSpeed;
}

void AnimationWrapper::SetSpeed(float pSpeed)
{
   mSpeed = pSpeed;
}

const std::string& AnimationWrapper::GetName() const
{
   return mName;
}

void AnimationWrapper::SetName(const std::string& pName)
{
   mName = pName;
}


}//namespace dtAnim
