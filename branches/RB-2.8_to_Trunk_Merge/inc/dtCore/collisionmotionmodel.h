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
 * Erik Johnson and Jeff Houde 07/05/2011
 */

#ifndef __DELTA_collisionmotionmodel_H__
#define __DELTA_collisionmotionmodel_H__

#include <dtCore/export.h>
#include <dtCore/fpsmotionmodel.h>
#include <dtCore/fpscollider.h>


#include <dtUtil/deprecationmgr.h>

namespace dtCore
{
   /**
    * Collision Motion Model uses ode collision meshes to allow typical FPS Camera interaction with the environment
    */
   class DT_CORE_EXPORT CollisionMotionModel : public FPSMotionModel
   {
      DECLARE_MANAGEMENT_LAYER(CollisionMotionModel);

   public:
      /// Deprecated 7/28/2011
      DEPRECATE_FUNC CollisionMotionModel(float pHeight, float pRadius, float k, float theta,
                           dtCore::Scene* pScene,
                           Keyboard* keyboard,
                           Mouse* mouse,
                           float maxWalkSpeed = 5.0f,
                           float maxTurnSpeed = 1.5f,
                           float maxSidestepSpeed = 5.0f,
                           bool useWASD = true,
                           bool useArrowKeys = true);

      /**
       * Constructor.
       *
       * @param[in]  pHeight           The height of the target.
       * @param[in]  pRadius           The radius of the target.
       * @param[in]  stepUpHeight      The height in which the target can step over an obsticle.
       * @param[in]  pScene            The scene.
       * @param[in]  keyboard          The applications Keyboard.
       * @param[in]  mouse             The applications Mouse.
       * @param[in]  maxWalkSpeed      The walk speed of the target.
       * @param[in]  maxTurnSpeed      The turn speed of the target.
       * @param[in]  maxSidestepSpeed  The sidestep speed of the target.
       * @param[in]  jumpSpeed         The jump speed of the target.
       * @param[in]  slideSpeed        The slide speed of the target.
       * @param[in]  slideThreshold    The threshold in which the target will slide on a surface.
       * @param[in]  canJump           True if the target can jump.
       * @param[in]  useWASD           True if the target can be controlled using the W A S D keys.
       * @param[in]  useArrowKeys      True if the target can be controlled using the arrow keys.
       */
      CollisionMotionModel(float pHeight,
                           float pRadius,
                           float stepUpHeight,
                           dtCore::Scene* pScene,
                           Keyboard* keyboard,
                           Mouse* mouse,
                           float maxWalkSpeed = 5.0f,
                           float maxTurnSpeed = 1.5f,
                           float maxSidestepSpeed = 5.0f,
                           float jumpSpeed = 5.0f,
                           float slideSpeed = 5.0f,
                           float slideThreshold = 0.1f,
                           bool canJump = true,
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

      /**
       * Sets if the Target can jump
       * @param jump True to enable jumping
       */
      void SetCollisionEnabled(bool enabled) { mCollisionEnabled = enabled; }

      /**
       * Returns true if the target can jump
       * @return mCanJump
       */
      bool GetCollisionEnabled() const { return mCollisionEnabled; }

   private:

      FPSCollider mCollider;
      bool mCanJump;
      bool mCollisionEnabled;
   };
} // namespace dtCore

#endif // __DELTA_collisionmotionmodel_H__
