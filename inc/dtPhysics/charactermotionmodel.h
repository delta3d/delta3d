/* -*-c++-*- 
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008, Alion Science and Technology, BMH Operation
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
 * William E. Johnson II
 */
#ifndef CHARACTER_MOTION_MODEL_H
#define CHARACTER_MOTION_MODEL_H

#include <dtCore/fpsmotionmodel.h>
#include <dtPhysics/physicsexport.h>
#include <dtPhysics/charactercontroller.h>

namespace dtPhysics
{
   class DT_PHYSICS_EXPORT CharacterMotionModel : public dtCore::FPSMotionModel
   {
   public:
      typedef dtCore::FPSMotionModel BaseClass;

      /**
       * Constructor for the charactor motion model.
       * @param controller a pre-created and initialized character controller.
       * see the base class for most of the parameter documentation.  The step up height is controlled
       * by the character controller, so that field is not included in this api.
       * @param height The height to adjust between the origin of the collision shape and the target.  This should be negative
       * for a visual model that has the origin of the ground (half the height of the collision shape in the character controller) and
       * positive if you attaching the camera to the shape so you get a decent eye point.
       * @see dtCore::FPSMotionModel
       */
      CharacterMotionModel(dtCore::Keyboard* kb, dtCore::Mouse* mouse, CharacterController* controller,
               float maxWalkSpeed = 5.0f,
               float maxTurnSpeed = 1.5f,
               float maxSidestepSpeed = 5.0f,
               float height = 0.75f, // This adjustment is for putting a camera above the center of mass of the shape in the character controller.
               float maxStepUpDist = 1.0f,
               bool useWASD = true,
               bool useArrowKeys = true
               );

      /// Override of base method used to update the target
      virtual void OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)
;

      void ResetMouse();

      void SetEnableLook(bool b);

      void SetEnableMovement(bool b);

      virtual void TickMotionModel(double deltaTime);

      void SetController(CharacterController* controller);
      CharacterController* GetController();
      const CharacterController* GetController() const;

      void SetPosition(const osg::Vec3& pos);

   protected:

      /// Destructor
      virtual ~CharacterMotionModel();

      virtual void PerformTranslation(const double deltaTime);

   private:
      bool mUseWaypoints;
      bool mWaitToEnable;
      bool mMovingCamera;
      bool mRotationSign;
      float mTargetHeading;
      float mTurnSpeed;
      float mAccumDeltaTime;
      dtCore::RefPtr<CharacterController> mCharacterController;
      osg::Vec3 mLastXYZ;
   };
}

#endif
