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

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <dtAnim/animatable.h>
#include <dtAnim/animationinterface.h>

namespace dtAnim
{

////////////////////////////////////////////////////////////////////////////////
// CONSTANTS
////////////////////////////////////////////////////////////////////////////////
const float Animatable::INFINITE_TIME = -1.0f;

////////////////////////////////////////////////////////////////////////////////
Animatable::Animatable()
   : mID(-1)
   , mSpeed(1.0f)
   , mInsertTime(0.0)
   , mStartTime(0.0f)
   , mStartDelay(0.0f)
   , mEndTime(0.0f)
   , mFadeIn(0.0f)
   , mFadeOut(0.0f)
   , mElapsedTime(0.0)
   , mBaseWeight(1.0f)
   , mCurrentWeight(1.0f)
   , mActive(false)
   , mShouldPrune(false)
   , mName()
{
}


////////////////////////////////////////////////////////////////////////////////
Animatable::~Animatable()
{
}

////////////////////////////////////////////////////////////////////////////////
Animatable::Animatable(const Animatable& pAnim)
   : mID(-1)
   , mSpeed(pAnim.GetSpeed())
   , mInsertTime(0.0)
   , mStartTime(pAnim.GetStartTime())
   , mStartDelay(pAnim.GetStartDelay())
   , mEndTime(pAnim.GetEndTime())
   , mFadeIn(pAnim.GetFadeIn())
   , mFadeOut(pAnim.GetFadeOut())
   , mElapsedTime(pAnim.GetElapsedTime())
   , mBaseWeight(pAnim.GetBaseWeight())
   , mCurrentWeight(0.0f)
   , mActive(false)
   , mShouldPrune(false)
   , mName(pAnim.GetName())
{
}


////////////////////////////////////////////////////////////////////////////////
Animatable& Animatable::operator=(const Animatable& pAnim)
{
   mSpeed = pAnim.GetSpeed();
   mStartTime = pAnim.GetStartTime();
   mStartDelay = pAnim.GetStartDelay();
   mEndTime = pAnim.GetEndTime();
   mFadeIn = pAnim.GetFadeIn();
   mFadeOut = pAnim.GetFadeOut();
   mElapsedTime = pAnim.GetElapsedTime();
   mBaseWeight = pAnim.GetBaseWeight();
   mCurrentWeight = 0.0f;
   mActive = false;
   mShouldPrune = false;
   mName = pAnim.GetName();

   // NOTE: Do not clone the time-event name map.
   
   return *this;
}

/////////////////////////////////////////////////////////////////////////////////
int Animatable::GetID() const
{
   return mID;
}

/////////////////////////////////////////////////////////////////////////////////
void Animatable::SetID(int id)
{
   mID = id;
}

////////////////////////////////////////////////////////////////////////////////
double Animatable::GetInsertTime() const
{
   return mInsertTime;
}

////////////////////////////////////////////////////////////////////////////////
void Animatable::SetInsertTime(double t)
{
   mInsertTime = t;
}

////////////////////////////////////////////////////////////////////////////////
float Animatable::GetStartTime() const
{
   return mStartTime;
}

////////////////////////////////////////////////////////////////////////////////
void Animatable::SetStartTime(float t)
{
   mStartTime = t;
}

////////////////////////////////////////////////////////////////////////////////
void Animatable::SetStartDelay(float t)
{
   mStartDelay = t;
}

////////////////////////////////////////////////////////////////////////////////
float Animatable::GetStartDelay() const
{ 
   return mStartDelay;
}

////////////////////////////////////////////////////////////////////////////////
float Animatable::GetEndTime() const
{
   return mEndTime;
}

////////////////////////////////////////////////////////////////////////////////
void Animatable::SetEndTime(float t)
{
   mEndTime = t;
}

////////////////////////////////////////////////////////////////////////////////
double Animatable::GetElapsedTime() const
{
   return mElapsedTime;
}

////////////////////////////////////////////////////////////////////////////////
float Animatable::GetRelativeElapsedTimeInAnimationScope() const
{
   return ConvertToRelativeTimeInAnimationScope(GetElapsedTime());
}

////////////////////////////////////////////////////////////////////////////////
float Animatable::ConvertToRelativeTimeInAnimationScope(double timeToConvert) const
{
   timeToConvert = timeToConvert - mStartTime - mInsertTime;
   return timeToConvert < 0.0 ? 0.0f : float(timeToConvert);
}

////////////////////////////////////////////////////////////////////////////////
void Animatable::SetElapsedTime(double t)
{
   mElapsedTime = t;
}

////////////////////////////////////////////////////////////////////////////////
float Animatable::GetFadeIn() const
{
   return mFadeIn;
}

////////////////////////////////////////////////////////////////////////////////
void Animatable::SetFadeIn(float f)
{
   mFadeIn = f;
}

////////////////////////////////////////////////////////////////////////////////
float Animatable::GetFadeOut() const
{
   return mFadeOut;
}

////////////////////////////////////////////////////////////////////////////////
void Animatable::SetFadeOut(float f)
{
   mFadeOut = f;
}

////////////////////////////////////////////////////////////////////////////////
float Animatable::GetBaseWeight() const
{
   return mBaseWeight;
}

////////////////////////////////////////////////////////////////////////////////
void Animatable::SetBaseWeight(float f)
{
   mBaseWeight = f;
}

////////////////////////////////////////////////////////////////////////////////
float Animatable::GetCurrentWeight() const
{
   return mCurrentWeight;
}

////////////////////////////////////////////////////////////////////////////////
bool Animatable::IsActive() const
{
   return mActive;
}

////////////////////////////////////////////////////////////////////////////////
void Animatable::SetActive(bool b)
{
   mActive = b;
}

////////////////////////////////////////////////////////////////////////////////
float Animatable::GetSpeed() const
{
   return mSpeed;
}

////////////////////////////////////////////////////////////////////////////////
void Animatable::SetSpeed(float speed)
{
   mSpeed = speed;
}

////////////////////////////////////////////////////////////////////////////////
bool Animatable::HasDefiniteEnd() const
{
   return CalculateDuration() >= 0.0f;
}

////////////////////////////////////////////////////////////////////////////////
bool Animatable::ShouldPrune() const
{
   return mShouldPrune;
}

////////////////////////////////////////////////////////////////////////////////
void Animatable::SetCurrentWeight(float weight)
{
   mCurrentWeight = weight;
}

////////////////////////////////////////////////////////////////////////////////
void Animatable::SetPrune(bool b)
{
   mShouldPrune = b;

   if(b && mEndCallback.valid())
   {
      mEndCallback(*this);
   }
}

////////////////////////////////////////////////////////////////////////////////
const std::string& Animatable::GetName() const
{
   return mName;
}

////////////////////////////////////////////////////////////////////////////////
void Animatable::SetName(const std::string& name)
{
   mName = name;
}

////////////////////////////////////////////////////////////////////////////////
void Animatable::SetEndCallback(AnimationCallback callback)
{
   mEndCallback = callback;
}

////////////////////////////////////////////////////////////////////////////////
bool Animatable::AddEventOnStart(const std::string& eventName)
{
   return AddEventOnTime(eventName, 0.0f);
}

////////////////////////////////////////////////////////////////////////////////
bool Animatable::AddEventOnEnd(const std::string& eventName)
{
   return AddEventOnTime(eventName, -1.0f);
}

////////////////////////////////////////////////////////////////////////////////
bool Animatable::AddEventOnTime(const std::string& eventName, float timeOffset)
{
   bool success = false;

   if(timeOffset < 0.0f)
   {
      timeOffset = CalculateDuration();
   }

   if(timeOffset >= 0.0f && ! HasEventOnTime(eventName, timeOffset))
   {
      success = mTimeEventMap.insert(std::make_pair(eventName, timeOffset)) != mTimeEventMap.end();
   }

   return success;
}

////////////////////////////////////////////////////////////////////////////////
bool Animatable::RemoveEventOnTime(const std::string& eventName, float timeOffset)
{
   return RemoveEventForTimeRange(eventName, timeOffset, timeOffset);
}

////////////////////////////////////////////////////////////////////////////////
unsigned Animatable::GetEventTimeOffsets(const std::string& eventName,
   Animatable::TimeOffsetArray& outArray) const
{
   unsigned count = 0;

   TimeEventMap::const_iterator curIter = mTimeEventMap.lower_bound(eventName);
   TimeEventMap::const_iterator endIter = mTimeEventMap.upper_bound(eventName);
   for (; curIter != endIter; ++curIter)
   {
      ++count;
      outArray.push_back(curIter->second);
   }

   return count;
}

////////////////////////////////////////////////////////////////////////////////
unsigned Animatable::RemoveEvent(const std::string& eventName,
                                 Animatable::TimeOffsetArray* outArray)
{
   if (outArray != NULL)
   {
      GetEventTimeOffsets(eventName, *outArray);
   }

   size_t count = mTimeEventMap.size();
   mTimeEventMap.erase(
      mTimeEventMap.lower_bound(eventName),
      mTimeEventMap.upper_bound(eventName));

   return unsigned(count - mTimeEventMap.size());
}

////////////////////////////////////////////////////////////////////////////////
bool Animatable::HasEventOnTime(const std::string& eventName, float timeOffset) const
{
   return HasEventForTimeRange(eventName, timeOffset, timeOffset);
}

////////////////////////////////////////////////////////////////////////////////
bool Animatable::HasEventForTimeRange(const std::string& eventName, float startTime, float endTime) const
{
   float curTime = 0.0f;
   TimeEventMap::const_iterator curIter = mTimeEventMap.lower_bound(eventName);
   TimeEventMap::const_iterator endIter = mTimeEventMap.upper_bound(eventName);
   for (; curIter != endIter; ++curIter)
   {
      curTime = curIter->second;
      if (curTime >= startTime && curTime <= endTime)
      {
         return true;
      }
   }

   return false;
}

////////////////////////////////////////////////////////////////////////////////
unsigned Animatable::GetEventsForTimeRange(float startTime, float endTime,
   EventNameArray& outEventArray) const
{
   unsigned count = 0;

   float curTime = 0.0f;
   TimeEventMap::const_iterator curIter = mTimeEventMap.begin();
   TimeEventMap::const_iterator endIter = mTimeEventMap.end();
   for (; curIter != endIter; ++curIter)
   {
      curTime = curIter->second;
      if(curTime >= startTime && curTime <= endTime)
      {
         outEventArray.push_back(curIter->first);
         ++count;
      }
   }

   return count;
}

////////////////////////////////////////////////////////////////////////////////
unsigned Animatable::GetEvents(float timeOffset, EventNameArray& outEventArray) const
{
   return GetEventsForTimeRange(timeOffset, timeOffset, outEventArray);
}

////////////////////////////////////////////////////////////////////////////////
unsigned Animatable::RemoveEventsForTimeRange(float startTime, float endTime,
   EventNameArray* outEventArray)
{
   if (outEventArray != NULL)
   {
      GetEventsForTimeRange(startTime, endTime, *outEventArray);
   }

   unsigned count = 0;

   float curTime = 0.0f;
   TimeEventMap::iterator curIter = mTimeEventMap.begin();
   for (; curIter != mTimeEventMap.end(); ++curIter)
   {
      curTime = curIter->second;
      if (curTime >= startTime && curTime <= endTime)
      {
         TimeEventMap::iterator tmp = curIter;
         ++curIter;
         mTimeEventMap.erase(tmp);
         ++count;
      }
   }

   return count;
}

////////////////////////////////////////////////////////////////////////////////
bool Animatable::RemoveEventForTimeRange(const std::string& eventName, float startTime, float endTime)
{
   bool found = false;

   float curTime = 0.0f;
   TimeEventMap::iterator curIter = mTimeEventMap.lower_bound(eventName);
   TimeEventMap::iterator endIter = mTimeEventMap.upper_bound(eventName);
   for (; curIter != endIter; ++curIter)
   {
      curTime = curIter->second;
      if (curTime >= startTime && curTime <= endTime)
      {
         mTimeEventMap.erase(curIter);
         found = true;

         // Restart the loop since the iterators may be invalid.
         curIter = mTimeEventMap.lower_bound(eventName);
         endIter = mTimeEventMap.upper_bound(eventName);
      }
   }

   return found;
}

////////////////////////////////////////////////////////////////////////////////
unsigned Animatable::ClearEvents()
{
   unsigned count = unsigned(mTimeEventMap.size());
   mTimeEventMap.clear();
   return count;
}

////////////////////////////////////////////////////////////////////////////////
const Animatable::TimeEventMap& Animatable::GetTimeEventMap() const
{
   return mTimeEventMap;
}

}//namespace dtAnim
