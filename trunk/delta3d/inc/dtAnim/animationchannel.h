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

#ifndef __DELTA_ANIMATIONCHANNEL_H__
#define __DELTA_ANIMATIONCHANNEL_H__

#include <dtAnim/export.h>
#include <dtCore/refptr.h>
#include <dtAnim/animatable.h>

namespace dtAnim
{
   class AnimationWrapper;
   class Cal3DModelWrapper;

/**
* AnimationChannel derives from Animatable and holds an AnimationWrapper, and
* contains semantics for playing an animation using the Cal3DModelWrapper API.
*/
class DT_ANIM_EXPORT AnimationChannel: public Animatable
{

public:
   typedef Animatable BaseClass;

   /**
    * If you use the default constructor you must call SetAnimation,
    * and SetModel
    */
   AnimationChannel();

   /**
    * @param the model wrapper used to play animations with
    * @param the animation wrapper to specify which animation to play
    */
   AnimationChannel(Cal3DModelWrapper* pModelWrapper, AnimationWrapper* pAnimationWrapper);


   /**
    * This function associates this channel with the supplied animation wrapper
    * @param the animation wrapper this channel will play
    */
   void SetAnimation(AnimationWrapper* pAnimation);

   AnimationWrapper* GetAnimation();
   const AnimationWrapper* GetAnimation() const;

   /// @return the model wrapper assigned to this channel.
   Cal3DModelWrapper* GetModel();

   /// @return the const model wrapper assigned to this channel.
   const Cal3DModelWrapper* GetModel() const;

   /**
    * This function sets the model wrapper used to make the calls to play the animation.
    * @param the associated model wrapper
    */
   void SetModel(Cal3DModelWrapper* pWrapper);

   /**
    * This function copies the animation channel
    * @param
    */
   virtual dtCore::RefPtr<Animatable> Clone(Cal3DModelWrapper* wrapper) const;

   /**
    * If an animation is not looping then it will be considered an action
    * and the blend weights will be ignored.
    */
   bool IsLooping() const;
   void SetLooping(bool b);

   /**
    * If an animation is an action it will play once and automatically delete itself
    * it will also be weighted to override all other conflicting animations.
    */
   bool IsAction() const;
   void SetAction(bool b);

   /**
    * The Max Duration will allow a user to set the amount of time this animation
    * will play in seconds.
    */
   float GetMaxDuration() const;
   void SetMaxDuration(float seconds);

   /**
    * Convenience method for turning an absolute time to a time
    * relative to the animation's scope.
    * @param timeToConvert Absolute time that should be mapped to this
    *        animation's time line scope.
    * @return A value between 0 and duration of one iteration of
    *         of the animation; 0 is at the beginning and the
    *         duration is the end time.
    */
   virtual float ConvertToRelativeTimeInAnimationScope(double timeToConvert) const;

   /**
    * The per frame update function.
    * @param delta time
    */
   /*virtual*/ void Update(float dt);

   /**
    * Recalculate is called on PlayAnimation()
    * it calculates the start and end times of our animation
    */
   /*virtual*/ void Recalculate();

   /**
    * Override of a base class method.
    * @return Length of the animation in seconds; INFINITE_TIME if looping.
    */
   virtual float CalculateDuration() const;

   /**
    * Prune is called before the animation is deleted.
    */
   /*virtual*/ void Prune();

   /**
    * ForceFadeOut will ignore the EndTime and automatically fade out
    * this animation over the time specified.
    *
    * @param the time to fade out over
    */
   /*virtual*/ void ForceFadeOut(float time);

protected:
   /*virtual*/ ~AnimationChannel();

   /**
    * Copy Constructor
    * Only clone should call this.
    */
   AnimationChannel(const AnimationChannel&);

   /**
    * Operator Equal
    * Hide this to prevent assigning
    */
   AnimationChannel& operator=(const AnimationChannel&);


private:

   bool mIsAction;
   bool mIsLooping;
   float mMaxDuration;
   float mLastWeight;

   // todo, verify holding a refptr to the model wrapper is ok
   dtCore::RefPtr<Cal3DModelWrapper> mModelWrapper;
   dtCore::RefPtr<AnimationWrapper> mAnimationWrapper;


};

} // namespace dtAnim

#endif // __DELTA_ANIMATIONCHANNEL_H__

