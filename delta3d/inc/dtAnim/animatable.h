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


/**
*  This class is used to specify the base class of an object which has semantics for 
*  animating.  
*/
class	DT_ANIM_EXPORT Animatable: public osg::Referenced
{

public:
   Animatable();

protected:
   virtual ~Animatable();

public:
   
   /**
   * The start time is the time in ms this animation will start playing 
   * after it was added to an AnimationSequence.
   */
   void SetStartTime(float t);
   float GetStartTime() const;

   /**
   * The end time is the time in ms that this animation will start
   * fading out relative to when it was added to an AnimationSequence.
   */
   float GetEndTime() const;
   void SetEndTime(float t);

   /**
   *  The FadeIn time, is the amount of time takes for an animation to blend
   *  linearally from a weight of 0 to the weight specified by BaseWeight
   *  after the animation starts playing specified by StartTime.
   */
   float GetFadeIn() const; 
   void SetFadeIn(float f);

   /**
   *  The FadeOut time is the amount of time it will take for an animation to
   *  blend linearly from its BaseWeight to 0 after the EndTime.
   */
   float GetFadeOut() const;
   void SetFadeOut(float f);

   /**
   *  The BaseWeight specifies the weight of this animation without
   *  being affected by blending.
   */
   float GetBaseWeight() const;
   void SetBaseWeight(float f);

   /**
   * The CurrentWeight is the weight of this animation in the current frame
   * this is calculated from the BaseWeight, linear blending from fades, and the parent weight
   */
   float GetCurrentWeight() const;

   /**
   *  An animation is active if it is currently playing.
   */
   bool IsActive() const;
   void SetActive(bool b);

   /**
   *  If an animation is not looping then it will be considered an action
   *  and the blend weights will be ignored.
   */
   bool IsLooping() const;
   void SetLooping(bool b);

   /**
   *  This flag specifies whether or not this animation has stopped playing
   */
   bool Prune() const;


   /**
   *  The virtual update, should be called every frame
   *
   *  @param delta time
   *  @param the parents weight to be multiplied to the base weight
   */
   virtual void Update(float dt, float parent_weight) = 0;

   /**
   * ForceFadeOut will ignore the EndTime and automatically fade out
   * this animation over the time specified.
   * 
   * @param the time to fade out over
   */
   virtual void ForceFadeOut(float time) = 0;

   /**
   * @return the name of this animation
   */
   virtual const std::string& GetName() const = 0;
   
   /**
   * @param the name to set this animation to
   */
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

