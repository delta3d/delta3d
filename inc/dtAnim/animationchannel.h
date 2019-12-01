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
#include <dtCore/observerptr.h>
#include <dtCore/refptr.h>
#include <dtAnim/animatable.h>

namespace dtAnim
{
   class BaseModelWrapper;

/**
* AnimationChannel derives from Animatable and
* contains semantics for playing an animation using the BaseModelWrapper API.
*/
class DT_ANIM_EXPORT AnimationChannel : public Animatable
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
   AnimationChannel(BaseModelWrapper* modelWrapper);

   /// @return the model wrapper assigned to this channel.
   BaseModelWrapper* GetModel();

   /// @return the const model wrapper assigned to this channel.
   const BaseModelWrapper* GetModel() const;

   /// @return the animation object associated with this object.
   dtAnim::AnimationInterface* GetAnimation();
   const dtAnim::AnimationInterface* GetAnimation() const;

   /// @return the name that refers to the associated animation object.
   void SetAnimationName(const std::string& name);
   const std::string& GetAnimationName() const;

   /**
    * This function sets the model wrapper used to make the calls to play the animation.
    * @param the associated model wrapper
    */
   void SetModel(BaseModelWrapper* modelWrapper);

   /**
    * This function copies the animation channel
    * @param
    */
   virtual dtCore::RefPtr<Animatable> Clone(BaseModelWrapper* modelWrapper) const;

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
    * The duration of an animation is defined to be the amount of time
    * it takes to play through a single cycle
    */
   float GetDuration() const;
   void SetDuration(float duration);

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
   float mDuration;
   float mMaxDuration;
   float mLastWeight;
   dtUtil::RefString mAnimName;
   
   mutable dtCore::ObserverPtr<dtAnim::AnimationInterface> mAnim;

   // todo, verify holding a refptr to the model wrapper is ok
   dtCore::RefPtr<BaseModelWrapper> mModelWrapper;
};

} // namespace dtAnim

#endif // __DELTA_ANIMATIONCHANNEL_H__

