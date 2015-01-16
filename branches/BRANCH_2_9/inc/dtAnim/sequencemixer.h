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

#ifndef __DELTA_SEQUENCEMIXER_H__
#define __DELTA_SEQUENCEMIXER_H__

#include <dtAnim/export.h>
#include <dtAnim/animationsequence.h>

#include <osg/Referenced>
#include <dtCore/refptr.h>

#include <string>
#include <map>
#include <vector>

namespace dtAnim
{

   class Animatable;


   /**
    * The SequenceMixer's job is to manage animations and animation sequences.
    * Animations are registered with the SequenceMixer using an AnimationHelper
    * on LoadModel().  Once an animation or sequence is registered the user can call
    * PlayAnimation() by name. To clear the animation use the name specified by
    * Animatable* GetName(), or the name used to play the animation.
    */
   class DT_ANIM_EXPORT SequenceMixer: public osg::Referenced
   {

      public:
         typedef std::map<std::string, dtCore::RefPtr<const Animatable> > AnimationTable;
         typedef AnimationTable::allocator_type::value_type TableKey;

      public:
         SequenceMixer();

         /**
          * Accessor to the root sequence for read-only purposes.
          * @return Reference to the root sequence that contains and
          *         updates all sub-animations.
          */
         const AnimationSequence& GetRootSequence() const;

         /**
          * The Update() must be call every frame, this is done automatically using an AnimationHelper
          * @param delta time
          */
         void Update(float dt);

         /**
          * PlayAnimation adds the given animation to active play list.
          *
          * @param The name of the animation registered with the mixer
          */
         void PlayAnimation(Animatable* pAnim);

         /**
          * This function will clear all currently playing animations from the mixer
          * over the fade out time specified.
          *
          * @param the time to fade out over
          */
         void ClearActiveAnimations(float time);

         /**
          * This function will clear an animation from the play list by name
          * and fade it out over the time specifed.
          *
          * @param the name of the animation to fade out
          * @param the time to fade out over
          */
         void ClearAnimation(const std::string& pAnim, float time);

         /**
          * This function will clear all animations registered with the mixer.
          */
         void ClearRegisteredAnimations();

         /**
          * This function will remove a registered animation by name from the mixer.
          */
         void RemoveRegisteredAnimation(const std::string& pAnim);

         /**
          * This function returns a pointer to the active animation specified
          * by name.
          *
          * @param the name of the active animation
          * @return the active animation within the system, 0 if this animation does not exist
          */
         Animatable* GetActiveAnimation(const std::string& pAnim);

         /**
          * This function returns a pointer to the active animation specified
          * by name.
          *
          * @param the name of the active animation
          * @return the active animation within the system, 0 if this animation does not exist
          */
         const Animatable* GetActiveAnimation(const std::string& pAnim) const;

         /**
          * This function returns a pointer to the registered animation specified
          * by name.
          *
          * @param the name of the active animation
          * @return the active animation within the system, 0 if this animation does not exist
          */
         const Animatable* GetRegisteredAnimation(const std::string& pAnim) const;


         /**
          * This function registers and animation within the system.  Registered animation
          * can be configured and played by name.  This animation is registered with the name
          * specified by GetName().
          *
          * @param the animation to register, the name this animation is registered with comes
          *        from calling GetName() on the animatable.
          */
         void RegisterAnimation(const Animatable* pAnimation);

         /**
          * Fills the given vector with all of the animatables registered with the mixer.
          *
          * @param toFill the vector of pointers to fill.
          */
         void GetRegisteredAnimations(std::vector<const Animatable*>& toFill) const;

         /**
          * This function forces all non-active animations to recalculate their start and end times
          */
         void ForceRecalculate();

         /**
          *	@return whether or not the specified animation is playing
          */
         bool IsAnimationPlaying(const std::string& pAnim) const;

         /**
          * Get references to all the currently active animations.
          */
         typedef std::vector<Animatable*> AnimatableArray;
         void GetActiveAnimations(AnimatableArray& toFill);

      protected:
         virtual ~SequenceMixer();

      private:

         const Animatable* Lookup(const std::string& pAnimation) const;

         AnimationTable mAnimatables;
         dtCore::RefPtr<AnimationSequence> mRootSequence;

   };

} // namespace dtAnim

#endif // __DELTA_SEQUENCEMIXER_H__

