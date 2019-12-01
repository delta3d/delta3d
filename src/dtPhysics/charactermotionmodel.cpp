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


#include <dtPhysics/charactermotionmodel.h>

#include <dtCore/scene.h>
#include <dtCore/logicalinputdevice.h>
#include <dtCore/mouse.h>
#include <dtCore/isector.h>
#include <dtCore/keyboard.h>
#include <dtCore/transform.h>
#include <dtCore/system.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/matrixutil.h>

#include <dtPhysics/collisioncontact.h>
#include <dtPhysics/palphysicsworld.h>

namespace dtPhysics
{
   ///////////////////////////////////////////////////////////////////////////
   CharacterMotionModel::CharacterMotionModel(dtCore::Keyboard* kb, dtCore::Mouse* mouse, CharacterController* controller,
            float maxWalkSpeed,
            float maxTurnSpeed,
            float maxSidestepSpeed,
            float height,
            float maxStepUpDist,
            bool useWASD,
            bool useArrowKeys)
   : dtCore::FPSMotionModel(kb, mouse, maxWalkSpeed, maxTurnSpeed, maxSidestepSpeed, height, 1.0f, useWASD, useArrowKeys)
   , mUseWaypoints(false)
   , mWaitToEnable(false)
   , mMovingCamera(false)
   , mRotationSign(true)
   , mTargetHeading(0.0f)
   , mTurnSpeed(0.0f)
   , mAccumDeltaTime(0.0f)
   , mCharacterController(NULL)
   {
      // This sets some other states based on the set controller.
      SetController(controller);
   }

   ///////////////////////////////////////////////////////////////////////////
   CharacterMotionModel::~CharacterMotionModel()
   {

   }

   ///////////////////////////////////////////////////////////////////////////
   void CharacterMotionModel::SetController(CharacterController* controller)
   {
      mCharacterController = controller;
      
      if(controller != NULL)
      {
         SetMaximumStepUpDistance(controller->GetStepHeight());
      }
   }

   ///////////////////////////////////////////////////////////////////////////
   CharacterController* CharacterMotionModel::GetController()
   {
      return mCharacterController.get();
   }

   ///////////////////////////////////////////////////////////////////////////
   const CharacterController* CharacterMotionModel::GetController() const
   {
      return mCharacterController.get();
   }

   ///////////////////////////////////////////////////////////////////////////
   void CharacterMotionModel::ResetMouse()
   {
      GetLookUpDownAxis()->SetState(0.0);
      GetTurnLeftRightAxis()->SetState(0.0);
      GetMouse()->SetPosition(0.0f, 0.0f);
      mWaitToEnable = true;
   }

   ///////////////////////////////////////////////////////////////////////////
   void CharacterMotionModel::OnSystem(const dtUtil::RefString& str, double deltaSim, double /*deltaReal*/)

   {
      if (str == dtCore::System::MESSAGE_POST_EVENT_TRAVERSAL)
      {
         TickMotionModel(deltaSim);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void CharacterMotionModel::TickMotionModel(double deltaTime)
   {
      if (!mCharacterController.valid())
      {
         return;
      }

      if (mWaitToEnable)
      {
         ResetMouse();
         mWaitToEnable = false;
      }
      else
      {
         deltaTime = dtUtil::Min(deltaTime, 0.333); // clamp to prevent wild spikes
         if (GetTarget() != NULL && IsEnabled())
         {
            dtCore::Transform transform;

            GetTarget()->GetTransform(transform, dtCore::Transformable::ABS_CS);
            osg::Vec3 lastPos;
            transform.GetTranslation(lastPos);

            // Get the last position.
            osg::Vec3 position(mCharacterController->GetTranslation());
            // Set the new position.
            float height = GetHeightAboveTerrain();
            position.z() += height;
            transform.SetTranslation(position);
            GetTarget()->SetTransform(transform, dtCore::Transformable::ABS_CS);

            mAccumDeltaTime = deltaTime;

            // Update the player
            if (IsCurrentlyActive())
            {
               // Call our parent methods to perform movement.
               UpdateMouse(mAccumDeltaTime); // read mouse state to perform rotations (and reset mouse state)
               PerformTranslation(mAccumDeltaTime); // perform translations
            }

            // Get the new translation
            osg::Vec3 movement;
            GetTarget()->GetTransform(transform, dtCore::Transformable::ABS_CS);
            transform.GetTranslation(movement);
            movement -= position;

            // Set back to the old translation
            transform.SetTranslation(position);
            GetTarget()->SetTransform(transform);

            if (movement.length() > FLT_EPSILON)
            {
               mCharacterController->Walk(movement / deltaTime, 0.25f);
            }
            else
            {
               mCharacterController->WalkClear();
            }
            //mCharacterController->Move(movement);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void CharacterMotionModel::PerformTranslation(const double deltaTime)
   {
      dtCore::Transform transform;
      osg::Vec3 xyz, newXYZ;
      osg::Matrix rot;

      // query initial status (to change from)
      GetTarget()->GetTransform(transform);
      transform.GetRotation(rot);
      transform.GetTranslation(xyz);

      osg::Vec3 forwardVector = dtUtil::MatrixUtil::GetRow3(rot, 1);
      forwardVector.z() = 0.0f;
      forwardVector.normalize();
      osg::Vec3 transForward = forwardVector * GetForwardBackFactor() * deltaTime;

      osg::Vec3 rightVector = dtUtil::MatrixUtil::GetRow3(rot, 0);
      osg::Vec3 transRight = rightVector * GetSidestepFactor() * deltaTime;

      // calculate x/y delta
      osg::Vec3 translationNormal(transForward + transRight);
      translationNormal.normalize();
      osg::Vec3 translation(translationNormal * GetMaximumWalkSpeed());

      // integration step
      newXYZ = xyz + translation * deltaTime;

      // apply collision detection/response
      static const osg::Vec3 UP(0.0f, 0.0f, 1.0f);
      //bool allowChange = true;
      /*RayCast ray;
      ray.SetOrigin(newXYZ - osg::Vec3(0.0f, 0.0f, 1.1f));
      ray.SetDirection(VectorType(a0.0f, 0.0f, -10.0f));
      RayCast::Report report;
      PhysicsWorld::GetInstance().TraceRay(ray, report);

      // If a high slope is hit...
      if(report.mHasHitObject && report.mHitNormal * UP < 0.9f)
      {
         // DEBUG:
         printf("[ %f,\t%f,\t%f ]\n", report.mHitNormal.x(), report.mHitNormal.y(), report.mHitNormal.z());

         // and trying to climb up the slope...
         if(newXYZ.z() > mLastXYZ.z())
         {
            allowChange = false;
         }
      }*/

      /*typedef std::vector<CollisionContact > ContactList;
      ContactList contacts;
      PhysicsWorld::GetInstance().GetContacts(mCharacterController->GetPhysicsObject(), contacts);
      if( ! contacts.empty())
      {
         float moveDist2 = translation.length2();
         ContactList::iterator curIter = contacts.begin();
         ContactList::iterator endIter = contacts.end();
         for( ; curIter != endIter; ++curIter)
         {
            if(moveDist2 >= (curIter->mDistance * curIter->mDistance) &&
               curIter->mNormal * UP < 0.07071f)
            {
// DEBUG:               printf("[ %f,\t%f,\t%f ]\n", curIter->mNormal.x(),curIter->mNormal.y(),curIter->mNormal.z());
               continue;
            }

            osg::Vec3 normal(curIter->mPosition - newXYZ);
            normal.normalize();
            if(normal * translationNormal > 0.0f)
            {
               allowChange = false;
            }
         }

// DEBUG:         printf("Contact (%i) passed: %i\n", contacts.size(), allowChange?1:0);
      }
      
      if(allowChange)
      {
         mLastXYZ = newXYZ;
      }
      else
      {
         newXYZ = mLastXYZ;
      }*/

      // apply changes (new position)
      transform.SetTranslation(newXYZ);
      GetTarget()->SetTransform(transform);
   }

   void CharacterMotionModel::SetEnableLook(bool b)
   {
      if (b)
      {
         SetMaximumTurnSpeed(1.87f);
      }
      else
      {
         SetMaximumTurnSpeed(0.0f);
      }
   }

   void CharacterMotionModel::SetEnableMovement(bool b)
   {
      if (b)
      {
         SetMaximumSidestepSpeed(3.0f);
         SetMaximumWalkSpeed(3.0f);
      }
      else
      {
         SetMaximumSidestepSpeed(0.0f);
         SetMaximumWalkSpeed(0.0f);
      }
   }

   void CharacterMotionModel::SetPosition(const osg::Vec3& pos)
   {
      if(mCharacterController.valid())
      {
         mCharacterController->Warp(pos);
      }
   }

}
