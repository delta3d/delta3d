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
 * Bradley Anderegg and Chris Darken 09/21/2006
 * Erik Johnson and Jeff Houde 07/05/2011
 */

#include <prefix/dtcoreprefix.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/log.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/mathdefines.h>
#include <dtCore/fpscollider.h>
#include <dtCore/scene.h>
#include <dtCore/transform.h>
#include <ode/collision.h>
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/Shape>
#include <cassert>


namespace dtCore
{
   ////////////////////////////////////////////////////////////////////////////
   FPSCollider::FPSCollider(float pHeight, float pRadius, float k, float theta, dtCore::Scene* pScene)
      : mBBFeet(0)
      , mFeetGeom(NULL)
      , mBBTorso(0)
      , mTorsoGeom(NULL)
      , mLocalSpaceID(0)
      , mTorsoOffset(0.0f)
      , mTorsoLengths()
      , mNumFeetContactPoints(0)
      , mNumTorsoContactPoints(0)
      , mLastFeetContact()
      , mCurrentMode(IDLE)
      , mSlideSpeed(5.0f)
      , mSlideThreshold(0.1f)
      , mJumpSpeed(5.0f)
      , mTerminalSpeed(-50.0f)
      , mHeightAboveTerrain(pHeight)
      , mMaxStepUpDistance(k)
      , mGroundNormal(0.0f, 0.0f, 1.0f)
      , mSmoothingSpeed(20.0f)
      , mCollisionSpace()
      , mPhysicsController(pScene->GetPhysicsController())
   {
      if (mPhysicsController.valid())
      {
         mCollisionSpace = mPhysicsController->GetSpaceID();
      }

      mLocalSpaceID = dSimpleSpaceCreate(0);
      InitBoundingVolumes();
      SetDimensions(mHeightAboveTerrain, pRadius, k);
   }

   ////////////////////////////////////////////////////////////////////////////
   FPSCollider::FPSCollider(float pHeight, float pRadius, float k, float theta, dSpaceID pSpaceToCollideWith, const osg::Vec3& pGravity)
      : mBBFeet(0)
      , mFeetGeom(NULL)
      , mBBTorso(0)
      , mTorsoGeom(NULL)
      , mLocalSpaceID(0)
      , mTorsoOffset(0.0f)
      , mTorsoLengths()
      , mNumFeetContactPoints(0)
      , mNumTorsoContactPoints(0)
      , mLastFeetContact()
      , mCurrentMode(IDLE)
      , mSlideSpeed(5.0f)
      , mSlideThreshold(0.1f)
      , mJumpSpeed(5.0f)
      , mHeightAboveTerrain(pHeight)
      , mMaxStepUpDistance(k)
      , mGroundNormal(0.0f, 0.0f, 1.0f)
      , mSmoothingSpeed(20.0f)
      , mCollisionSpace(pSpaceToCollideWith)
   {
      mLocalSpaceID = dSimpleSpaceCreate(0);
      InitBoundingVolumes();
      SetDimensions(mHeightAboveTerrain, pRadius, k);
   }

   ////////////////////////////////////////////////////////////////////////////
   FPSCollider::~FPSCollider()
   {
      mFeetGeom->ClearCollisionGeometry();
      mTorsoGeom->ClearCollisionGeometry();
   }

   ////////////////////////////////////////////////////////////////////////////
   dSpaceID FPSCollider::GetSpaceID() const
   {
      return mLocalSpaceID;
   }

   ////////////////////////////////////////////////////////////////////////////
   float FPSCollider::GetSlideThreshold() const
   {
      return mSlideThreshold;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float FPSCollider::GetSlideSpeed() const
   {
      return mSlideSpeed;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FPSCollider::SetSlideSpeed(float speed)
   {
      mSlideSpeed = speed;
   }

   ////////////////////////////////////////////////////////////////////////////
   FPSCollider::eMode FPSCollider::GetMode() const
   {
      return mCurrentMode;
   }

   ////////////////////////////////////////////////////////////////////////////
   void FPSCollider::SetSlideThreshold(float pSlide)
   {
      mSlideThreshold = pSlide;
      SetDimensions(mHeightAboveTerrain, mTorsoLengths[0], mMaxStepUpDistance);
   }

   ////////////////////////////////////////////////////////////////////////////
   float FPSCollider::GetJumpSpeed() const
   {
      return mJumpSpeed;
   }

   ////////////////////////////////////////////////////////////////////////////
   void FPSCollider::SetJumpSpeed(float pSpeed)
   {
      mJumpSpeed = pSpeed;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float FPSCollider::GetSmoothingSpeed() const
   {
      return mSmoothingSpeed;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FPSCollider::SetSmoothingSpeed(float speed)
   {
      mSmoothingSpeed = speed;
   }

   ////////////////////////////////////////////////////////////////////////////
   float FPSCollider::GetHeightAboveTerrain() const
   {
      return mHeightAboveTerrain;
   }

   ////////////////////////////////////////////////////////////////////////////
   void FPSCollider::SetDimensions(float targetHeightAboveTerrain, float pRadius, float stepUpHeight)
   {
      if (pRadius <= 0.0f)
      {
         pRadius = 0.01f;
      }

      if (targetHeightAboveTerrain < stepUpHeight)
      {
         stepUpHeight = targetHeightAboveTerrain;
      }

      mTorsoLengths.set(pRadius, targetHeightAboveTerrain - mMaxStepUpDistance);
      mTorsoOffset = (mTorsoLengths[1] * 0.5f) + mMaxStepUpDistance;
   }

   ////////////////////////////////////////////////////////////////////////////
   unsigned long FPSCollider::GetCollisionBitsForFeet() const
   {
      return dGeomGetCollideBits(mBBFeet);
   }

   ////////////////////////////////////////////////////////////////////////////
   void FPSCollider::SetCollisionBitsForFeet(unsigned long bits)
   {
      dGeomSetCollideBits(mBBFeet, bits);
   }

   ////////////////////////////////////////////////////////////////////////////
   unsigned long FPSCollider::GetCollisionBitsForTorso() const
   {
      return dGeomGetCollideBits(mBBTorso);
   }

   ////////////////////////////////////////////////////////////////////////////
   void FPSCollider::SetCollisionBitsForTorso(unsigned long bits)
   {
      dGeomSetCollideBits(mBBTorso, bits);
   }

   /////////////////////////////////////////////////////////////////////////////
   void FPSCollider::Reset()
   {
      mLastVelocity = osg::Vec3();
      mStartPosition = osg::Vec3();
      mEndPosition = osg::Vec3();
      mGroundNormal = osg::Vec3(0.0f, 0.0f, 1.0f);
   }

   ////////////////////////////////////////////////////////////////////////////
   dGeomID FPSCollider::GetFeetGeom()
   {
      return mBBFeet;
   }

   ////////////////////////////////////////////////////////////////////////////
   dGeomID FPSCollider::GetTorsoGeom()
   {
      return mBBTorso;
   }

   ////////////////////////////////////////////////////////////////////////////
   void FPSCollider::InitBoundingVolumes()
   {
      mFeetGeom = new dtCore::ODEGeomWrap();
      mFeetGeom->SetCollisionRay(1.0f);
      mFeetGeom->SetCollisionCategoryBits(0);
      mFeetGeom->SetCollisionCollideBits(0xFFFFFFFF);
      mFeetGeom->SetCollisionDetection(true);
      mBBFeet = mFeetGeom->GetGeomID();
      dSpaceAdd(mLocalSpaceID, mBBFeet);
      dGeomSetData(mBBFeet, this);

      mTorsoGeom = new dtCore::ODEGeomWrap();
      mTorsoGeom->SetCollisionCylinder(1.0f, 1.0f);
      mTorsoGeom->SetCollisionCategoryBits(0);
      mTorsoGeom->SetCollisionCollideBits(0xFFFFFFFF);
      mTorsoGeom->SetCollisionDetection(true);
      mBBTorso = mTorsoGeom->GetGeomID();
      dSpaceAdd(mLocalSpaceID, mBBTorso);
      dGeomSetData(mBBTorso, this);
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Vec3 FPSCollider::AdjustPosition(const osg::Vec3& targetP0, float dt)
   {
      // convert target to feet position
      osg::Vec3 oldFeetPos(targetP0[0],
                           targetP0[1],
                           targetP0[2] - mHeightAboveTerrain);

      // update new feet position based on velocity
      osg::Vec3 newFeetPos = oldFeetPos;
      newFeetPos[0] += mLastVelocity[0] * dt;
      newFeetPos[1] += mLastVelocity[1] * dt;

      // Apply the forward motion body collision.
      {
         float sweepLength = mTorsoLengths[0] * 0.25f;

         osg::Vec3 vec = newFeetPos - oldFeetPos;
         float length = vec.length();
         vec.normalize();

         bool collision = false;
         while (sweepLength <= length)
         {
            osg::Vec3 sweepPos = oldFeetPos + (vec * sweepLength);

            dtCore::Transform xform;
            xform.SetTranslation(sweepPos[0], sweepPos[1], sweepPos[2] + mTorsoOffset);
            mTorsoGeom->SetCollisionCylinder(mTorsoLengths[0], mTorsoLengths[1]);
            mTorsoGeom->SetCollisionDetection(true);
            mTorsoGeom->UpdateGeomTransform(xform);

            if (CollideTorso())
            {
               osg::Vec3 normal(mLastTorsoContact.normal[0],
                                mLastTorsoContact.normal[1],
                                mLastTorsoContact.normal[2]);
               float depth = mLastTorsoContact.depth;

               newFeetPos = sweepPos + (normal * depth);
               collision = true;
               break;
            }

            sweepLength += mTorsoLengths[0] * 0.25f;
         }

         if (!collision)
         {
            dtCore::Transform xform;
            xform.SetTranslation(newFeetPos[0], newFeetPos[1], newFeetPos[2] + mTorsoOffset);
            mTorsoGeom->SetCollisionCylinder(mTorsoLengths[0], mTorsoLengths[1]);
            mTorsoGeom->SetCollisionDetection(true);
            mTorsoGeom->UpdateGeomTransform(xform);

            if (CollideTorso())
            {
               osg::Vec3 normal(mLastTorsoContact.normal[0],
                                mLastTorsoContact.normal[1],
                                mLastTorsoContact.normal[2]);
               float depth = mLastTorsoContact.depth;

               newFeetPos += normal * depth;
            }
         }
      }

      // now perform gravity/jump collision
      {
         oldFeetPos = newFeetPos;
         oldFeetPos[2] += mMaxStepUpDistance;

         newFeetPos = oldFeetPos;
         newFeetPos[2] += (mLastVelocity[2] * dt) - mMaxStepUpDistance;

         float length = dtUtil::Abs(oldFeetPos.z() - newFeetPos.z());

         dtCore::Transform xform;
         xform.Set(osg::Matrix::rotate(osg::Vec3(0.0f, 0.0f, 1.0f), osg::Vec3(0.0f, 0.0f, -1.0f)));
         xform.SetTranslation(oldFeetPos);
         mFeetGeom->SetCollisionRay(length);
         mFeetGeom->SetCollisionDetection(true);
         mFeetGeom->UpdateGeomTransform(xform);

         if (CollideFeet())
         {
            // If the ground clamping is colliding, then the upward velocity
            // will always be reset.
            mLastVelocity[2] *= 1.0f - dtUtil::Abs(mLastFeetContact.normal[2]);

            //set our new height
            newFeetPos.set(mLastFeetContact.pos[0],
                           mLastFeetContact.pos[1],
                           mLastFeetContact.pos[2]);

            // Apply the sliding threshold to the surface based on its normal.
            osg::Vec3 normal(mLastFeetContact.normal[0],
                             mLastFeetContact.normal[1],
                             mLastFeetContact.normal[2]);

            if (normal.z() < 1.0f - mSlideThreshold)
            {
               newFeetPos[0] += normal[0] * dt * mSlideSpeed;
               newFeetPos[1] += normal[1] * dt * mSlideSpeed;
               newFeetPos[2] -= normal[2] * dt * mSlideSpeed;
               mGroundNormal = normal;

               // If we are not actively walking, then we are sliding.
               if (mLastVelocity[0] == 0.0f && mLastVelocity[1] == 0.0f)
               {
                  SetCurrentMode(SLIDING);
               }
               else
               {
                  SetCurrentMode(WALKING);
               }
            }
            else
            {
               mGroundNormal = osg::Vec3(0.0f, 0.0f, 1.0f);

               // If we are not actively walking, then we are idle.
               if (mLastVelocity[0] == 0.0f && mLastVelocity[1] == 0.0f)
               {
                  SetCurrentMode(IDLE);
               }
               else
               {
                  SetCurrentMode(WALKING);
               }
            }
         }
         else
         {
            // If we are in the air, then we are jumping or falling based
            // on our upward velocity.
            if (mLastVelocity[2] > 0.0f)
            {
               SetCurrentMode(JUMPING);
            }
            else
            {
               SetCurrentMode(FALLING);
            }
         }
      }

      // Now apply the ambient torso collision.
      {
         dtCore::Transform xform;
         xform.SetTranslation(newFeetPos[0], newFeetPos[1], newFeetPos[2] + mTorsoOffset);
         mTorsoGeom->SetCollisionCylinder(mTorsoLengths[0], mTorsoLengths[1]);
         mTorsoGeom->SetCollisionDetection(true);
         mTorsoGeom->UpdateGeomTransform(xform);
      }

      if (CollideTorso())
      {
         osg::Vec3 normal(mLastTorsoContact.normal[0],
                          mLastTorsoContact.normal[1],
                          mLastTorsoContact.normal[2]);
         float depth = mLastTorsoContact.depth;

         newFeetPos += normal * depth;
      }

      mEndPosition = osg::Vec3(newFeetPos[0], newFeetPos[1], newFeetPos[2] + mHeightAboveTerrain);

      // Clamp the motion so it does not go beyond the end position.
      float delta = dtUtil::Min(dt * mSmoothingSpeed, 1.0f);

      // Lerp the target from its current position to its end position
      // over time and then return the lerp position for a smooth translation.
      osg::Vec3 targetP1 = mStartPosition + (mEndPosition - mStartPosition) * delta;

      return targetP1;
   }

   ////////////////////////////////////////////////////////////////////////////
   bool FPSCollider::CollideTorso()
   {
      mNumTorsoContactPoints = 0;

      mLastTorsoContact.depth = -1.0f;
      dSpaceCollide2((dGeomID)mCollisionSpace, (dGeomID)mLocalSpaceID, this, NearCallbackTorso);

      return mNumTorsoContactPoints > 0;
   }

   ////////////////////////////////////////////////////////////////////////////
   bool FPSCollider::CollideFeet()
   {
      mNumFeetContactPoints = 0;

      mLastFeetContact.depth = -1.0f;
      dSpaceCollide2((dGeomID)mCollisionSpace, (dGeomID)mLocalSpaceID, this, NearCallbackFeet);

      return mNumFeetContactPoints > 0;
   }

   ////////////////////////////////////////////////////////////////////////////
   void FPSCollider::HandleCollideTorso(dGeomID pTorso, dGeomID pObject)
   {
      if (pObject == GetFeetGeom()) return;

      dContactGeom contactGeoms[10];
      int contactPoints = dCollide(pTorso, pObject, 10, contactGeoms, sizeof(dContactGeom));


      if (contactPoints)
      {
         for (int index = 0; index < contactPoints; ++index)
         {
            if (contactGeoms[index].depth > mLastTorsoContact.depth)
            {
               mLastTorsoContact = contactGeoms[index];

               mNumTorsoContactPoints = contactPoints;
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void FPSCollider::HandleCollideFeet(dGeomID pFeet, dGeomID pObject)
   {
      if (pObject == GetTorsoGeom()) return;

      dContactGeom contactGeoms[1];
      int contactPoints = dCollide(pFeet, pObject, 1, contactGeoms, sizeof(dContactGeom));

      if (contactPoints)
      {
         if (contactGeoms[0].depth > mLastFeetContact.depth)
         {
            mLastFeetContact = contactGeoms[0];

            mNumFeetContactPoints = contactPoints;
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void FPSCollider::NearCallbackFeet( void* data, dGeomID o1, dGeomID o2 )
   {
      if ( data == NULL || o1 == 0 || o2 == 0 )
      {
         return;
      }

      FPSCollider* cmm = static_cast<FPSCollider*>(data);

      if (o1 == cmm->GetFeetGeom())
      {
         cmm->HandleCollideFeet(o1, o2);
      }
      else if (o2 == cmm->GetFeetGeom())
      {
         cmm->HandleCollideFeet(o2, o1);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void FPSCollider::NearCallbackTorso( void* data, dGeomID o1, dGeomID o2 )
   {
      if ( data == NULL || o1 == 0 || o2 == 0 )
      {
         return;
      }

      FPSCollider* cmm = static_cast<FPSCollider*>(data);

      if (o1 == cmm->GetTorsoGeom())
      {
         cmm->HandleCollideTorso(o1, o2);
      }
      else if (o2 == cmm->GetTorsoGeom())
      {
         cmm->HandleCollideTorso(o2, o1);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Vec3 FPSCollider::Update(const osg::Vec3& initialTargetPosition,
                                 const osg::Vec3& initialVelocity,
                                 float deltaFrameTime, bool pJump)
   {
      mLastVelocity[0] = initialVelocity[0];
      mLastVelocity[1] = initialVelocity[1];

      // Check if the target has moved away significantly.
      // This happens if the target was moved independently of this collider.
      if ((initialTargetPosition - mStartPosition).length2() > 0.1f)
      {
         mStartPosition = initialTargetPosition;
         mEndPosition = initialTargetPosition;
         mLastVelocity[2] = 0.0f;
      }

      // Handle jumping.
      switch (mCurrentMode)
      {
      case IDLE:
      case WALKING:
      case SLIDING:
         if (pJump)
         {
            mLastVelocity += mGroundNormal * mJumpSpeed;
         }
         break;

      case JUMPING:
      case FALLING:
         mLastVelocity[0] += mGroundNormal[0] * mJumpSpeed;
         mLastVelocity[1] += mGroundNormal[1] * mJumpSpeed;
         break;
      }

      // Apply Gravity.
      const float zGravity = mPhysicsController.valid() ? mPhysicsController->GetGravity()[2]: 0.0f;
      mLastVelocity[2] += zGravity * deltaFrameTime;
      dtUtil::ClampMin(mLastVelocity[2], mTerminalSpeed);

      mStartPosition = AdjustPosition(mEndPosition, deltaFrameTime);

      return mStartPosition;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec3 FPSCollider::GetLastVelocity() const
   {
      return mLastVelocity;
   }

   //////////////////////////////////////////////////////////////////////////
   void FPSCollider::SetCurrentMode(eMode newMode)
   {
      mCurrentMode = newMode;
   }
}//namespace dtCore
