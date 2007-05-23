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

#include <dtAnim/animatable.h>


namespace dtAnim
{

Animatable::Animatable()
: mSpeed(1.0f)
, mStartTime(0.0f)
, mStartDelay(0.0f)
, mEndTime(0.0f)
, mFadeIn(0.0f)
, mFadeOut(0.0f)
, mElapsedTime(0.0f)
, mBaseWeight(1.0f)
, mCurrentWeight(1.0)
, mActive(false)
, mShouldPrune(false)
{
}


Animatable::~Animatable()
{
}

float Animatable::GetStartTime() const
{
   return mStartTime;
}

void Animatable::SetStartTime(float t)
{
   mStartTime = t;
}

void Animatable::SetStartDelay(float t)
{
   mStartDelay = t;
}

float Animatable::GetStartDelay() const
{ 
   return mStartDelay;
}

float Animatable::GetEndTime() const
{
   return mEndTime;
}
void Animatable::SetEndTime(float t)
{
   mEndTime = t;
}

float Animatable::GetElapsedTime() const
{
   return mElapsedTime;
}

void Animatable::SetElapsedTime(float t)
{
   mElapsedTime = t;
}

float Animatable::GetFadeIn() const
{
   return mFadeIn;
}

void Animatable::SetFadeIn(float f)
{
   mFadeIn = f;
}

float Animatable::GetFadeOut() const
{
   return mFadeOut;
}

void Animatable::SetFadeOut(float f)
{
   mFadeOut = f;
}

float Animatable::GetBaseWeight() const
{
   return mBaseWeight;
}

void Animatable::SetBaseWeight(float f)
{
   mBaseWeight = f;
}

float Animatable::GetCurrentWeight() const
{
   return mCurrentWeight;
}

bool Animatable::IsActive() const
{
   return mActive;
}
void Animatable::SetActive(bool b)
{
   mActive = b;
}

float Animatable::GetSpeed() const
{
   return mSpeed;
}

void Animatable::SetSpeed(float speed)
{
   mSpeed = speed;
}

bool Animatable::ShouldPrune() const
{
   return mShouldPrune;
}

void Animatable::SetCurrentWeight(float weight)
{
   mCurrentWeight = weight;
}

void Animatable::SetPrune(bool b)
{
   mShouldPrune = b;
}


}//namespace dtAnim
