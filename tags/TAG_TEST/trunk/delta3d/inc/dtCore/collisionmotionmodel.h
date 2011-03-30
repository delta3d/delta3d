/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2006 Bradley Anderegg
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
 * Bradley Anderegg and Chris Darken 08/21/2006
 */

#ifndef __DELTA_collisionmotionmodel_H__
#define __DELTA_collisionmotionmodel_H__

#include <dtCore/motionmodel.h>
#include <dtCore/fpscollider.h>
#include <dtCore/transformable.h>
#include <dtCore/inputdevice.h>
#include <dtCore/export.h>

#include <dtUtil/functor.h>

#include <dtCore/fpsmotionmodel.h>

namespace dtCore
{
   class Axis;
   class AxisToAxis;
   class ButtonAxisToAxis;
   class ButtonsToAxis;
   class Keyboard;
   class LogicalAxis;
   class LogicalInputDevice;
   class Mouse;
   class Scene;
   class Isector;

   /**
    * Collision Motion Model uses ode collision meshes to allow typical FPS Camera interaction with the environment
    */
   class DT_CORE_EXPORT CollisionMotionModel : public FPSMotionModel
   {
      DECLARE_MANAGEMENT_LAYER(CollisionMotionModel);

   public:
      /**
       * Constructor.
       * @param height, the height of the character and camera, in meters
       * @param radius, the width of our character
       * @param k the distance from the bottom of the knees to the ground, this represents the maximum step up height
       * @param theta the collision amount to maintain below the ground (note: this should be less then half of k,
       *        something small like 0.1 is recommended)
       * @param Scene is used to get the gravity and the ode space
       * @param keyboard the keyboard instance, or 0 to
       * avoid creating default input mappings
       * @param mouse the mouse instance, or 0 to avoid
       * creating default input mappings
       * @param maxWalkSpeed the maximum walking speed
       * @param maxTurnSpeed the maximum turning speed
       * @param maxSidestepSpeed the maximum sidestep speed       
       * @param maxStepUpDist the maximum distance we can step up to higher terrain
       */
      CollisionMotionModel(float pHeight, float pRadius, float k, float theta,
                           dtCore::Scene* pScene,
                           Keyboard* keyboard,
                           Mouse* mouse,
                           float maxWalkSpeed = 5.0f,
                           float maxTurnSpeed = 1.5f,
                           float maxSidestepSpeed = 5.0f,                           
                           bool useWASD = true,
                           bool useArrowKeys = true);

   protected:
      /**
       * Destructor.
       */
      virtual ~CollisionMotionModel();

   public:
      /**
       * This method can be overriden in subclasses to produce
       * desired translation behavior
       * Note: Any collision detection/response and other physical
       * constraints should be applied here
       * @param deltaTime The time change
       */
      virtual void PerformTranslation(const double deltaTime);

      FPSCollider& GetFPSCollider();

      /**
       * Sets if the Target can jump
       * @param jump True to enable jumping
       */
      void SetCanJump(bool jump) { mCanJump = jump; }

      /**
       * Returns true if the target can jump
       * @return mCanJump
       */
      bool GetCanJump() const { return mCanJump; }

   private:

      FPSCollider mCollider;
      bool mCanJump;
   };
} // namespace dtCore

#endif // __DELTA_collisionmotionmodel_H__
