/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2006 MOVES Institute
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
 * Bradley Anderegg 06/29/2006
 */

#ifndef __DELTA_CHARACTER_WRAPPER_H__
#define __DELTA_CHARACTER_WRAPPER_H__

#include <dtAnim/export.h>
#include <dtCore/refptr.h>
#include <dtCore/observerptr.h>
#include <dtCore/transformable.h>
#include <string>

namespace dtCore
{
   class Isector;
}

namespace dtAnim
{
   class AnimationHelper;
   class Cal3DAnimator;


   /**
    * A Wrapper around an animated character that will perform basic steering
    */
   class DT_ANIM_EXPORT CharacterWrapper: public dtCore::Transformable
   {
      public:
         typedef dtCore::Transformable BaseClass;

      public:
         CharacterWrapper(const std::string& filename);
      protected:
         /*virtual*/ ~CharacterWrapper();

      public:

         /**
          * This is a per-frame call which updates the animation helper,
          * and applies rotation and translation from Speed and RotationSpeed.
          *
          * @param time step
          */
         void Update(float dt);

         /**
          * The height above ground is used for setting the camera height after ground clamping.
          *
          * @return the current translational offset in the Z direction
          */
         float GetHeightAboveGround() const;

         /**
          * The height above ground is used for setting the camera height after ground clamping.
          * It is defined as a translational offset in the Z direction.  Note to initiate ground
          * clamping you must call SetGroundClamp().
          *
          * @param the desired translational offset in the Z direction
          */
         void SetHeightAboveGround(float heightAboveGround);

         /**
          * Initiates ground clamping using a delta transformable, the height above ground is
          * determined to be the height above the ground intersection to set as an offset translation.
          *
          * @param the node to ground clamp to
          * @param the height above ground
          */
         void SetGroundClamp(dtCore::Transformable* nodeToClampTo, float heightAboveGround);

         /**
          * Initiates ground clamping using a delta transformable, the height above ground is
          * determined to be the height above the ground intersection to set as an offset translation.
          *
          * @param a scene to perform ground clamping on
          * @param the height above ground
          */
         void SetGroundClamp(dtCore::Scene* sceneNode, float heightAboveGround);


         /**
          * The Speed of the character is defined to be the forward translation in m/s, the character
          * is moved in the update.
          *
          * @return the current speed of the character
          */
         float GetSpeed() const;

         /**
          * The Speed of the character is defined to be the forward translation in m/s, the character
          * is moved in the update.
          *
          * @param the desired speed of the character
          */
         void SetSpeed(float metersPerSecond);

         /**
          * The rotation speed of the character is defined to be a rotation about the Z axis in degrees/sec.
          * It controls the heading, and is contributed to the per-frame update.
          *
          * @return current rotation speed
          */
         float GetRotationSpeed() const;

         /**
          * The rotation speed of the character is defined to be a rotation about the Z axis in degrees/sec.
          * It controls the heading, and is contributed to the per-frame update.
          *
          * @return desired rotation speed
          */
         void SetRotationSpeed(float degreesPerSecond);

         /**
          * Rotate to Heading will rotate the character about the Z axis to match the given heading.
          * A time step parameter can be used to give a smooth rotation and control the speed at which
          * the character rotates.  A time step of 1.0 will give a full rotation to the heading specified.
          * It should be noted that this is an immediate rotation and is independent of the per frame update function.
          *
          * @param desired heading to rotate towards
          * @param current time step
          */
         void RotateToHeading(float headingInDegrees, float dt);

         /**
          * Rotate to Point will rotate the character about the Z axis to match the given direction.
          * A time step parameter can be used to give a smooth rotation and control the speed at which
          * the character rotates.  A time step of 1.0 will give a full rotation to the heading specified.
          * It should be noted that this is an immediate rotation and is independent of the per frame update function.
          *
          * @param desired point to rotate towards
          * @param current time step
          */
         void RotateToPoint(const osg::Vec3& point, float dt);

         /**
          * The Local Offset is an additional matrix between the character and the loaded art model.
          * This offset matrix can be used to rotate the model to face 'forwards' in model space, or apply a scale value
          * that will not be lost by calling SetTransform().
          *
          * @return the current local offset (starts as identity)
          */
         dtCore::Transform GetLocalOffset() const;

         /**
          * The Local Offset is an additional matrix between the character and the loaded art model.
          * This offset matrix can be used to rotate the model to face 'forwards' in model space, or apply a scale value
          * that will not be lost by calling SetTransform().
          *
          * @param the desired local offset (starts as identity)
          */
         void SetLocalOffset(const dtCore::Transform& localOffset);

         /**
          * Gets the current heading of the character in degrees.
          * @return heading in degrees
          */
         float GetHeading() const;

         /**
          * Gets the current heading of the character in degrees.
          * @param heading in degrees
          */
         void SetHeading(float degrees);


         /**
          * Just a wrapper interface to the animation helper.
          * @param the animation to play from the loaded model
          */
         void PlayAnimation(const std::string& pAnim);

         /**
          * Just a wrapper interface to the animation helper.
          * @param the animation to clear from the play list
          * @param the desired time to fade out the animation
          */
         void ClearAnimation(const std::string& pAnim, float fadeOutTime);

         /**
          * Just a wrapper interface to the animation helper. Clears all
          * animations playing.
          * @param the fade out time
          */
         void ClearAllAnimations(float fadeOutTime);

         /**
          * Searches the play list for the desired animation, returns true if the
          * animation is playing.
          * @param the name of the animation
          */
         bool IsAnimationPlaying(const std::string& name) const;

         /**
         * Get the internal AnimationHelper this instance is using.
         * @return The AnimationHelper used to manage the animations
         */
         AnimationHelper& GetAnimationHelper();

         /**
         * Get the internal AnimationHelper this instance is using.
         * @return The AnimationHelper used to manage the animations
         */
         const AnimationHelper& GetAnimationHelper() const;

      private:

         void Init(const std::string& filename);

         float mSpeed;
         float mRotationSpeed;
         float mHeightAboveGround;
         dtCore::RefPtr<osg::MatrixTransform> mLocalOffset;
         dtCore::RefPtr<dtAnim::AnimationHelper> mAnimHelper;
         dtCore::RefPtr<dtCore::Isector> mIsector;

   };
} // namespace dtAI

#endif // __DELTA_CHARACTER_WRAPPER_H__
