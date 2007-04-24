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

#ifndef __DELTA_ANIMATABLE_H__
#define __DELTA_ANIMATABLE_H__

#include <dtAnim/export.h>
#include <osg/Referenced>

#include <string>

namespace dtAnim
{

class	DT_ANIM_EXPORT Animatable: public osg::Referenced
{

public:
   Animatable();

protected:
   virtual ~Animatable();

public:

   float GetStartTime() const;
   void SetStartTime(float t);

   float GetEndTime() const;
   void SetEndTime(float t);

   float GetFadeIn() const; 
   void SetFadeIn(float f);

   float GetFadeOut() const;
   void SetFadeOut(float f);

   float GetBaseWeight() const;
   void SetBaseWeight(float f);

   float GetCurrentWeight() const;

   bool IsActive() const;
   void SetActive(bool b);

   bool IsLooping() const;
   void SetLooping(bool b);

   bool Prune() const;


   virtual void Update(float dt, float parent_weight) = 0;

   virtual void ForceFadeOut(float time) = 0;

   virtual const std::string& GetName() const = 0;
   virtual void SetName(const std::string&) = 0;

protected:

   void SetCurrentWeight(float weight);
   void SetPrune(bool b);

   float mStartTime, mEndTime;
   float mFadeIn, mFadeOut;
   float mElapsedTime;

   float mBaseWeight, mCurrentWeight;

   bool mActive, mLooping, mPrune;

};

}//namespace dtAnim

#endif // __DELTA_ANIMATABLE_H__

