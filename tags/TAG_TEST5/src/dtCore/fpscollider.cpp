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
 */

#include <prefix/dtcoreprefix.h>
#include <dtUtil/mathdefines.h>
#include <dtCore/fpscollider.h>
#include <dtCore/scene.h>
#include <ode/collision.h>

namespace dtCore
{
   ////////////////////////////////////////////////////////////////////////////
   FPSCollider::FPSCollider(float pHeight, float pRadius, float k, float theta, dtCore::Scene* pScene)
      : mBBFeet(0)
      , mBBTorso(0)
      , mSpaceID(0)
      , mBBFeetOffset()
      , mBBTorsoOffset()
      , mBBFeetLengths()
      , mBBTorsoLengths()
      , mNormals()
      , mNumFeetContactPoints(0)
      , mNumTorsoContactPoints(0)
      , mStartCollideFeet(false)
      , mLastFeetContact()
      , mJumped(false)
      , mCurrentMode(FALLING)
      , mSlideThreshold(0.65f)
      , mSlideSpeed(5.0f)
      , mJumpSpeed(5.0f)
      , mFallSpeed(0.0f)
      , mTerminalSpeed(-50.0f)
      , mHeightAboveTerrain(pHeight)
      , mMaxStepUpDistance(k)
      , mLastVelocity(0.0f, 0.0f, 0.0f)
      , mSlideVelocity(0.0f, 0.0f, 0.0f)
      , mGravity()
      , mCollisionSpace()
   {
      pScene->GetGravity(mGravity);
      mCollisionSpace = pScene->GetSpaceID();
      mSpaceID = dSimpleSpaceCreate(0);
      SetDimensions(pHeight, pRadius, k, theta);
   }

   ////////////////////////////////////////////////////////////////////////////
   FPSCollider::FPSCollider(float pHeight, float pRadius, float k, float theta, dSpaceID pSpaceToCollideWith, const osg::Vec3& pGravity)
      : mBBFeet(0)
      , mBBTorso(0)
      , mSpaceID(0)
      , mBBFeetOffset()
      , mBBTorsoOffset()
      , mBBFeetLengths()
      , mBBTorsoLengths()
      , mNormals()
      , mNumFeetContactPoints(0)
      , mNumTorsoContactPoints(0)
      , mStartCollideFeet(false)
      , mLastFeetContact()
      , mJumped(false)
      , mCurrentMode(FALLING)
      , mSlideThreshold(0.65f)
      , mSlideSpeed(9.8f/4.0f)
      , mJumpSpeed(5.0f)
      , mHeightAboveTerrain(pHeight)
      , mMaxStepUpDistance(k)
      , mLastVelocity(0.0f, 0.0f, 0.0f)
      , mSlideVelocity(0.0f, 0.0f, 0.0f)
      , mGravity(pGravity)
      , mCollisionSpace(pSpaceToCollideWith)
   {
      mSpaceID = dSimpleSpaceCreate(0);
      SetDimensions(pHeight, pRadius, k, theta);
   }

   ////////////////////////////////////////////////////////////////////////////
   FPSCollider::~FPSCollider()
   {
      if (mBBFeet) dGeomDestroy(mBBFeet);
      if (mBBTorso) dGeomDestroy(mBBTorso);
      if (mSpaceID) dSpaceDestroy(mSpaceID);
   }

   ////////////////////////////////////////////////////////////////////////////
   dSpaceID FPSCollider::GetSpaceID() const
   {
      return mSpaceID;
   }

   ////////////////////////////////////////////////////////////////////////////
   float FPSCollider::GetSlideThreshold() const
   {
      return mSlideThreshold;
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
   }

   ////////////////////////////////////////////////////////////////////////////
   float FPSCollider::GetSlideSpeed() const
   {
      return mSlideSpeed;
   }

   ////////////////////////////////////////////////////////////////////////////
   void FPSCollider::SetSlideSpeed(float pSpeed)
   {
      mSlideSpeed = pSpeed;
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

   ////////////////////////////////////////////////////////////////////////////
   float FPSCollider::GetHeightAboveTerrain() const
   {
      return mHeightAboveTerrain;
   }

   ////////////////////////////////////////////////////////////////////////////
   void FPSCollider::SetDimensions(float pHeight, float pRadius, float k, float theta)
   {
      if (mBBFeet) dGeomDestroy(mBBFeet);
      if (mBBTorso) dGeomDestroy(mBBTorso);

      mBBFeetOffset.set(0.0f, 0.0f, pHeight + (theta * 0.5f) - (k * 0.5f));
      mBBTorsoOffset.set(0.0f, 0.0f, (pHeight - k) * 0.5f);
      mBBFeetLengths.set(pRadius, pRadius, theta + k);
      mBBTorsoLengths.set(pRadius, pRadius, pHeight - k);

      InitBoundingVolumes();
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
      mSlideVelocity = osg::Vec3();
      mFallSpeed = 0.0f;
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
      CreateCollisionCylinder((dWorldID)mCollisionSpace, mSpaceID, mBBFeet, mBBFeetLengths);
      CreateCollisionCylinder((dWorldID)mCollisionSpace, mSpaceID, mBBTorso, mBBTorsoLengths);
   }

   ////////////////////////////////////////////////////////////////////////////
   void FPSCollider::CreateCollisionCylinder(dWorldID pWorldId, dSpaceID pSpaceId, dGeomID& pId, const osg::Vec3& pLengths)
   {
      pId = dCreateGeomTransform(0);

      dGeomTransformSetGeom(pId, dCreateCylinder(0, pLengths[1], pLengths[2]));

      dSpaceAdd(pSpaceId, pId);
      dGeomSetData(pId, this);

      dGeomSetCategoryBits(pId, 0);
      dGeomSetCollideBits(pId, 0xFFFFFFFF);
   }

   ////////////////////////////////////////////////////////////////////////////
   bool FPSCollider::TestPosition(osg::Vec3& newPos, float dt)
   {
      UpdateBoundingVolumes(newPos);

      bool pCollided = CollideFeet();
      int normalIndex = 0;

      if (!pCollided || mJumped)
      {
         if (mJumped && !pCollided)
         {
            mJumped = false;
         }
         mCurrentMode = FALLING;
      }
      else
      {
         //set our new height
         double diff = (mHeightAboveTerrain + mLastFeetContact.pos[2]) - newPos[2];
         double zPrime = dtUtil::Min<double>(diff, mJumpSpeed * dt);

         newPos[2] += zPrime;

         float highestZ = 1.0f;
         if (mNormals.size())
         {
            //find the collided normal with with max z value
            highestZ = mNormals[normalIndex][2];
            for (unsigned i = 0; i < mNormals.size(); ++i)
            {
               if (mNormals[i][2] > highestZ)
               {
                  highestZ = mNormals[i][2];
                  normalIndex = i;
               }
            }
         }

         float dotZ = highestZ;
         if (dotZ < mSlideThreshold)
         {
            mCurrentMode = SLIDING;
            //set mSlideVelocity = ((mSlideSpeed * dt) / (1 - n.z) * (n.z*n.x, n.z*n.y, n.z*n.z - 1))
            float speed = mSlideSpeed / (1.0 - mNormals[normalIndex][2]);
            mSlideVelocity.set(speed * mNormals[normalIndex][2] * mNormals[normalIndex][0], speed * mNormals[normalIndex][2] * mNormals[normalIndex][1], speed * (mNormals[normalIndex][2] * mNormals[normalIndex][2] - 1.0));
         }
         else
         {
            mCurrentMode = WALKING;
         }
      }

      UpdateBoundingVolumes(newPos);

      return CollideTorso();
   }

   ////////////////////////////////////////////////////////////////////////////
   bool FPSCollider::CollideTorso()
   {
      mNormals.clear();
      mNumTorsoContactPoints = 0;

      dSpaceCollide2((dGeomID)mCollisionSpace, (dGeomID)mSpaceID, this, NearCallbackTorso);

      return mNumTorsoContactPoints > 0;
   }

   ////////////////////////////////////////////////////////////////////////////
   bool FPSCollider::CollideFeet()
   {
      mNormals.clear();
      mNumFeetContactPoints = 0;
      mStartCollideFeet = true;

      dSpaceCollide2((dGeomID)mCollisionSpace, (dGeomID)mSpaceID, this, NearCallbackFeet);

      mStartCollideFeet = false;
      return mNumFeetContactPoints > 0;
   }

   ////////////////////////////////////////////////////////////////////////////
   void FPSCollider::HandleCollideTorso(dGeomID pFeet, dGeomID pObject)
   {
      if (pObject == GetFeetGeom()) return;

      bool set = false;
      void* data = NULL;
      dGeomID pID = pObject;


      while (dGeomGetClass(pID) == dGeomTransformClass)
      {
         if (!pID) return;
         pID = dGeomTransformGetGeom(pID);
      }

      if (dGeomGetClass(pID) == dTriMeshClass)
      {
         set = true;
         data = dGeomGetData(pID);
         dGeomSetData(pID, this);
         dGeomTriMeshSetArrayCallback(pID, dTriArrayCallback);
      }

      dContactGeom contactGeoms[1];
      mNumTorsoContactPoints += dCollide(pFeet, pObject, 1, contactGeoms, sizeof(dContactGeom));

      if (set)
      {
         dGeomSetData(pID, data);
         dGeomTriMeshSetArrayCallback(pID, 0);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void FPSCollider::HandleCollideFeet(dGeomID pFeet, dGeomID pObject)
   {
      if (pObject == GetTorsoGeom()) return;

      bool set = false;
      void* data = NULL;
      dGeomID pID = pObject;

      while (dGeomGetClass(pID) == dGeomTransformClass)
      {
         if (!pID) return;
         pID = dGeomTransformGetGeom(pID);
      }

      if (dGeomGetClass(pID) == dTriMeshClass)
      {
         set = true;
         data = dGeomGetData(pID);
         dGeomSetData(pID, this);
         dGeomTriMeshSetArrayCallback(pID, dTriArrayCallback);
      }

      dContactGeom contactGeoms[8];
      int contactPoints = dCollide(pFeet, pObject, 8, contactGeoms, sizeof(dContactGeom));

      //find the contact point with the highest z value
      if (contactPoints)
      {
         float highestZ;
         if (mStartCollideFeet)
         {
            highestZ = contactGeoms[0].pos[2];
            mLastFeetContact = contactGeoms[0];
            mStartCollideFeet = false;
         }
         else
         {
            highestZ = mLastFeetContact.pos[2];
         }

         for (int i = 0; i < contactPoints; ++i)
         {
            if (contactGeoms[i].pos[2] > highestZ)
            {
               highestZ = contactGeoms[i].pos[2];
               mLastFeetContact = contactGeoms[i];
            }
         }

         mNumFeetContactPoints += contactPoints;
      }


      if (set)
      {
         dGeomSetData(pID, data);
         dGeomTriMeshSetArrayCallback(pID, 0);
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
      else if (cmm->GetFeetGeom())
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
   void FPSCollider::UpdateBoundingVolumes(const osg::Vec3& xyz)
   {
      osg::Vec3 newVec = xyz - mBBFeetOffset;
      dGeomSetPosition(mBBFeet, newVec[0], newVec[1], newVec[2]);

      osg::Vec3 torso = xyz - mBBTorsoOffset;
      dGeomSetPosition(mBBTorso, torso[0], torso[1], torso[2]);
   }

   ////////////////////////////////////////////////////////////////////////////
   void FPSCollider::dTriArrayCallback(dGeomID TriMesh, dGeomID RefObject, const int* TriIndices, int TriCount)
   {

      FPSCollider* cmm = static_cast<FPSCollider*>(dGeomGetData(TriMesh));

      for (int i = 0; i < TriCount; ++i)
      {
         dVector3 v1, v2, v3;

         dGeomTriMeshGetTriangle(TriMesh, TriIndices[i], &v1, &v2, &v3);

         osg::Vec3 middle(v2[0], v2[1], v2[2]);

         osg::Vec3 side1(v1[0], v1[1], v1[2]);
         osg::Vec3 side2(v3[0], v3[1], v3[2]);
         osg::Vec3 normal;

         side1 -= middle;
         side2 = middle - side2;

         normal.set(side1 ^ side2);
         normal.normalize();

         cmm->mNormals.push_back(normal);
      }

   }

   ////////////////////////////////////////////////////////////////////////////
   osg::Vec3 FPSCollider::Update(const osg::Vec3& p0, const osg::Vec3& velocity, float deltaFrameTime, bool pJump)
   {
      osg::Vec3 v0, v1, p1, newXYZ;
      v0 = velocity;

      eMode oldMode = mCurrentMode;

      ////added flag for jumping, used true when we are on the way up
      if (mJumped && mLastVelocity[2] <= 0.0f)
      {
         mJumped = false;
      }

      switch (mCurrentMode)
      {
      case FALLING:
         {
            mFallSpeed = mLastVelocity[2] + mGravity[2] * deltaFrameTime;
            if (mFallSpeed < mTerminalSpeed) 
            {
               mFallSpeed = mTerminalSpeed;
            }
            v0 = osg::Vec3(mLastVelocity[0], mLastVelocity[1], mFallSpeed);
            p1 = p0 + osg::Vec3(v0[0] * deltaFrameTime, v0[1] * deltaFrameTime, v0[2] * deltaFrameTime);
         }
         break;

      case SLIDING:
         {
            v0[2] = 0.0f;
            v0 = mSlideVelocity;

            p1 = p0 + osg::Vec3(v0[0] * deltaFrameTime, v0[1] * deltaFrameTime, v0[2] * deltaFrameTime);
         }
         break;

      case WALKING:
         {
            v0[2] = 0.0f;

            if (pJump)
            {
               v0[2] = mJumpSpeed;
               mJumped = true;
            }

            p1 = p0 + osg::Vec3(v0[0] * deltaFrameTime, v0[1] * deltaFrameTime, v0[2] * deltaFrameTime);
         }
         break;
      }

      if (!TestPosition(p1, deltaFrameTime))
      {
         // Prevent camera bounce - return previous values if only z-changing and walk mode.
         if( p0[0] == p1[0] && p0[1] == p1[1] && p0[2] != p1[2] && mCurrentMode == WALKING )
         {
            newXYZ = p0;
         }
         else
         {
            newXYZ = p1;
         }

         mLastVelocity = v0;
      }
      else
      {
         v1 = v0;
         for (unsigned i = 0; i < mNormals.size(); ++i)
         {
            float dot = (mNormals[i] * v1);
            if (dot < 0.0f) v1 -= osg::Vec3(mNormals[i][0] * dot, mNormals[i][1] * dot, mNormals[i][2] * dot);
         }

         osg::Vec3 p2 = p0 + osg::Vec3(v1[0] * deltaFrameTime, v1[1] * deltaFrameTime, v1[2] * deltaFrameTime);

         if (!TestPosition(p2, deltaFrameTime))
         {
            mLastVelocity = v1;
            newXYZ = p2;
         }
         else
         {
            mLastVelocity.set(0.0f, 0.0f, 0.0f);
            newXYZ = p0;
            mCurrentMode = oldMode;
         }
      }

      return newXYZ;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec3 FPSCollider::GetLastVelocity() const
   {
      return mLastVelocity;
   }

}//namespace dtCore
