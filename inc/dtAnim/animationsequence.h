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

#ifndef __DELTA_ANIMATIONSEQUENCE_H__
#define __DELTA_ANIMATIONSEQUENCE_H__

#include <dtAnim/export.h>
#include <dtAnim/animatable.h>

#include <dtCore/refptr.h>
#include <dtCore/observerptr.h>

#include <list>
#include <string>

namespace dtAnim
{
   class AnimationController;
   class BaseModelWrapper;
/**
 * AnimationSequence derives from Animatable and contains a child list of
 * animations to play. The animations can be AnimationChannels or other
 * AnimationSequences.
 *
 */
class DT_ANIM_EXPORT AnimationSequence: public Animatable
{
public:
   typedef std::list<dtCore::RefPtr<Animatable> > AnimationContainer;
   typedef AnimationContainer::allocator_type::value_type ContainerType;


      /**
       * AnimationController is responsible for updating the sequences child
       * Animatables.  To change the update behavior, subclass AnimationController
       * and add it to your AnimationSequence.
       */
      class   DT_ANIM_EXPORT AnimationController: public osg::Referenced
      {

      public:
         /**
          * Constructor
          * @param the parent AnimatationSequence
          */
         AnimationController(AnimationSequence&);

         ///copy constructor
         AnimationController(const AnimationController&);
         ///operator equal
         AnimationController& operator=(const AnimationController&);

         ///classes derived from this should implement a custom Clone()
         virtual dtCore::RefPtr<AnimationController> Clone() const;

         /// @return the parent AnimationSequence
         AnimationSequence& GetParent();

         /// @return the parent AnimationSequence
         const AnimationSequence& GetParent() const;

         ///Sets the parent AnimationSequence
         void SetParent(AnimationSequence&);

         /**
          * The Update function is responsible for updating all the parent
          * sequences children.  This update consists of incrementing their
          * elapsed time, calculating their current weight, and call update
          * on them if they are active.
          *
          * @param delta time
          */
         virtual void Update(float dt);

         /**
          * The Recalculate function is responsible for calculating the start times
          * of all the parent sequences children and then calling Recalculate() on
          * them so they can calculate their end times.
          */
         virtual void Recalculate();

      protected:
         virtual ~AnimationController();

         /**
          * Helper function to compute the weight of a child animatable
          * using the parents weight.
          */
         void SetComputeWeight(Animatable* pAnim);

         /**
          * Helper function to compute the speed of a child animatable
          * using the parents speed.
          */
         void SetComputeSpeed(Animatable* pAnim);


      private:

         dtCore::ObserverPtr<AnimationSequence> mParent;
      };


public:
   AnimationSequence();
   AnimationSequence(AnimationController*);

   /**
    * Add an animation as a child of this sequence.
    * @param the child animatable
    */
   void AddAnimation(Animatable* pAnimation);

   /**
    * Fade out an animation by name.
    * @param time in seconds to fade out over
    */
   void ClearAnimation(const std::string& pAnimName, float fadeTime);

   /**
    * Get a child animation by name
    * @return the child animation, or 0 if it isnt a child
    */
   Animatable* GetAnimation(const std::string& pAnimName);
   const Animatable* GetAnimation(const std::string& pAnimName) const;

   /**
    * Get the controller for this sequence
    * @return the controller for this sequence
    */
   AnimationController* GetController();
   const AnimationController* GetController() const;

   /**
    * Override the default controller
    * @param the new controller to use
    */
   void SetController(AnimationController* pController);

   /**
    * Get a reference to the child animations
    * @return the container of child animations
    */
   AnimationContainer& GetChildAnimations();
   const AnimationContainer& GetChildAnimations() const;

   /**
    * Our virtual update function
    * @param delta time
    */
   /*virtual*/ void Update(float dt);

   /**
    * Prune is called before the animation is deleted.
    */
   /*virtual*/ void Prune();

   /**
    * This function copies sequence and all child Animatables
    */
   virtual dtCore::RefPtr<Animatable> Clone(BaseModelWrapper* modelWrapper) const;

   /**
    * Recalculate is called on PlayAnimation()
    * it calculates the start and end times of our animation
    */
   /*virtual*/ void Recalculate();

   /**
    * Override of a base class method.
    * Method to determine the actual length of the animation sequence.
    * @return Length of the animation measured in seconds. The value
    *         that is returned is calculated per method call by iterating
    *         over child animations. INFINITE_TIME is returned if any
    *         part of the sequence has no definite end.
    */
   virtual float CalculateDuration() const;

   /**
    * Force fade out will make this animation and all child animations
    * fade out over time
    * @param the time to fade out over
    */
   /*virtual*/ void ForceFadeOut(float time);


   /**
   *	@return whether or not the specified animation is playing
   */
   bool IsAnimationPlaying(const std::string& pAnim) const;

protected:
   /*virtual*/ ~AnimationSequence();

   /// No one should be calling the copy canstructor except the clone method.
   AnimationSequence(const AnimationSequence&, BaseModelWrapper* wrapper);

   /// Hide this to prevent assigning
   AnimationSequence& operator=(const AnimationSequence&);

private:

   void Insert(Animatable* pAnimation);
   void Remove(const std::string& pAnim);
   Animatable* GetAnimatable(const std::string& pAnim);
   const Animatable* GetAnimatable(const std::string& pAnim) const;
   void CalculateBaseWeight();
   void PruneChildren();

   dtCore::RefPtr<AnimationController> mController;
   AnimationContainer mChildAnimations;

};

} // namespace dtAnim

#endif // __DELTA_ANIMATIONSEQUENCE_H__

