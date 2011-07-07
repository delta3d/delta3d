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
      , mTorsoOffset(0.f)
      , mTorsoLengths()
      , mNormals()
      , mNumFeetContactPoints(0)
      , mNumTorsoContactPoints(0)
      , mStartCollideFeet(false)
      , mStartCollideTorso(false)
      , mLastFeetContact()
      , mCurrentMode(WALKING)
      , mSlideSpeed(5.0f)
      , mSlideThreshold(0.1f)
      , mJumpSpeed(5.0f)
      , mTerminalSpeed(-50.0f)
      , mHeightAboveTerrain(pHeight)
      , mMaxStepUpDistance(k)
      , mJumping(false)
      , mLastVelocity(0.0f, 0.0f, 0.0f)
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

      mFeetTransformable = new dtCore::Transformable("feet");
      mFeetTransformable->GetOSGNode()->asGroup()->addChild(mFeetGeom->CreateRenderedCollisionGeometry());
      pScene->AddChild(mFeetTransformable.get());

      mTorsoTransformable = new dtCore::Transformable("torso");
      mTorsoTransformable->GetOSGNode()->asGroup()->addChild(mTorsoGeom->CreateRenderedCollisionGeometry());
      pScene->AddChild(mTorsoTransformable.get());
   }

   ////////////////////////////////////////////////////////////////////////////
   FPSCollider::FPSCollider(float pHeight, float pRadius, float k, float theta, dSpaceID pSpaceToCollideWith, const osg::Vec3& pGravity)
      : mBBFeet(0)
      , mFeetGeom(NULL)
      , mBBTorso(0)
      , mTorsoGeom(NULL)
      , mLocalSpaceID(0)
      , mTorsoOffset(0.f)
      , mTorsoLengths()
      , mNormals()
      , mNumFeetContactPoints(0)
      , mNumTorsoContactPoints(0)
      , mStartCollideFeet(false)
      , mStartCollideTorso(false)
      , mLastFeetContact()
      , mCurrentMode(WALKING)
      , mSlideSpeed(5.0f)
      , mSlideThreshold(0.1f)
      , mJumpSpeed(5.0f)
      , mHeightAboveTerrain(pHeight)
      , mMaxStepUpDistance(k)
      , mJumping(false)
      , mLastVelocity(0.0f, 0.0f, 0.0f)
      , mCollisionSpace(pSpaceToCollideWith)
   {
      mLocalSpaceID = dSimpleSpaceCreate(0);
      InitBoundingVolumes();
      SetDimensions(mHeightAboveTerrain, pRadius, k);
   }

   ////////////////////////////////////////////////////////////////////////////
   FPSCollider::~FPSCollider()
   {
      if (mFeetTransformable->GetOSGNode()->asGroup()->getNumChildren() > 0)
      {
         mFeetTransformable->GetOSGNode()->asGroup()->removeChild(mFeetTransformable->GetOSGNode()->asGroup()->getChild(0));
      }

      if (mTorsoTransformable->GetOSGNode()->asGroup()->getNumChildren() > 0)
      {
         mTorsoTransformable->GetOSGNode()->asGroup()->removeChild(mTorsoTransformable->GetOSGNode()->asGroup()->getChild(0));
      }

      mFeetGeom->ClearCollisionGeometry();
      mTorsoGeom->ClearCollisionGeometry();

      if (mBBFeet) dGeomDestroy(mBBFeet);
      if (mBBTorso) dGeomDestroy(mBBTorso);
      if (mLocalSpaceID) dSpaceDestroy(mLocalSpaceID);
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
      mFeetGeom->SetCollisionRay(0.1f);
      mFeetGeom->SetCollisionCategoryBits(0);
      mFeetGeom->SetCollisionCollideBits(0xFFFFFFFF);
      mFeetGeom->SetCollisionDetection(true);
      mBBFeet = mFeetGeom->GetGeomID();
      dSpaceAdd(mLocalSpaceID, mBBFeet);
      dGeomSetData(mBBFeet, this);

      mTorsoGeom = new dtCore::ODEGeomWrap();
      mTorsoGeom->SetCollisionCylinder(mTorsoLengths[0], mTorsoLengths[1]);
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

      //{
      //   osg::Vec3 colPos = newFeetPos;
      //   osg::Vec3 vec = newFeetPos - oldFeetPos;
      //   float length = vec.length();

      //   if (length > 0.0f)
      //   {
      //      osg::Vec3 rightVec = vec ^ osg::Vec3(0.0f, 0.0f, 1.0f);
      //      rightVec.normalize();
      //      rightVec *= mTorsoLengths[0];
      //      float rightLength = rightVec.length();

      //      osg::Vec3 upVec = osg::Vec3(0.0f, 0.0f, 0.0f);
      //      float depth = 10000.0f;

      //      //if (mFeetTransformable->GetOSGNode()->asGroup()->getNumChildren() > 0)
      //      //{
      //      //   dtCore::RefPtr<osg::Node> node = mFeetTransformable->GetOSGNode()->asGroup()->getChild(0);
      //      //   if (node.valid())
      //      //   {
      //      //      mFeetTransformable->GetOSGNode()->asGroup()->removeChild(node);
      //      //   }
      //      //}

      //      while (upVec.z() <= mHeightAboveTerrain - mMaxStepUpDistance)
      //      {
      //         ApplyMovementVector(vec, length, oldFeetPos, upVec, newFeetPos, colPos, depth);
      //         ApplyMovementVector(vec, length, oldFeetPos, upVec + rightVec, newFeetPos, colPos, depth);
      //         ApplyMovementVector(vec, length, oldFeetPos, upVec - rightVec, newFeetPos, colPos, depth);

      //         upVec.z() += 0.05f;
      //      }

      //      if (upVec.z() != mHeightAboveTerrain - mMaxStepUpDistance)
      //      {
      //         upVec.z() = mHeightAboveTerrain - mMaxStepUpDistance;
      //         
      //         ApplyMovementVector(vec, length, oldFeetPos, upVec, newFeetPos, colPos, depth);
      //         ApplyMovementVector(vec, length, oldFeetPos, upVec + rightVec, newFeetPos, colPos, depth);
      //         ApplyMovementVector(vec, length, oldFeetPos, upVec - rightVec, newFeetPos, colPos, depth);
      //      }

      //      vec = newFeetPos - colPos;
      //      vec.z() = colPos.z();
      //      length = vec.length();
      //      if (length > 0.0f)
      //      {
      //         depth = 10000.0f;
      //         osg::Vec3 outColPos;

      //         if (ApplyMovementVector(vec, length, colPos, osg::Vec3(), outColPos, outColPos, depth))
      //         {
      //            newFeetPos = oldFeetPos;
      //         }
      //      }
      //   }
      //}

      // now perform gravity/jump collision
      {
         //osg::Vec3 vec = newFeetPos - oldFeetPos;
         //vec.normalize();


         oldFeetPos = newFeetPos;// + (vec * mTorsoLengths[0]);
         oldFeetPos[2] += mMaxStepUpDistance;

         newFeetPos = oldFeetPos;
         newFeetPos[2] += (mLastVelocity[2] * dt) - mMaxStepUpDistance;

         float length = abs(oldFeetPos.z() - newFeetPos.z());

         dtCore::Transform xform;
         xform.Set(osg::Matrix::rotate(osg::Vec3(0.f, 0.f, 1.f), osg::Vec3(0.0f, 0.0f, -1.0f)));
         xform.SetTranslation(oldFeetPos);
         mFeetGeom->SetCollisionRay(length);
         mFeetGeom->SetCollisionDetection(true);
         mFeetGeom->UpdateGeomTransform(xform);

         if (CollideFeet())
         {
            // If the ground clamping is colliding, then the upward velocity
            // will always be reset.
            mLastVelocity[2] *= 1.0f - abs(mLastFeetContact.normal[2]);

            //set our new height
            newFeetPos.set(mLastFeetContact.pos[0],
                           mLastFeetContact.pos[1],
                           mLastFeetContact.pos[2]);

            // Apply the sliding threshold to the surface based on its normal.
            osg::Vec3 normal(mLastFeetContact.normal[0],
                             mLastFeetContact.normal[1],
                             mLastFeetContact.normal[2]);

            mJumping = false;

            if (normal.z() < 1.0f - mSlideThreshold)
            {
               newFeetPos[0] += normal[0] * dt * mSlideSpeed;
               newFeetPos[1] += normal[1] * dt * mSlideSpeed;
               newFeetPos[2] -= normal[2] * dt * mSlideSpeed;
               mGroundNormal = normal;
            }
            else
            {
               mGroundNormal = osg::Vec3(0.0f, 0.0f, 1.0f);
            }

            SetCurrentMode(WALKING);
         }
         else
         {
            SetCurrentMode(IN_AIR);
         }

         //newFeetPos -= vec * mTorsoLengths[0];
      }

      // Now apply the ambient torso collision.
      {
         dtCore::Transform xform;
         xform.SetTranslation(newFeetPos[0], newFeetPos[1], newFeetPos[2] + mTorsoOffset);
         mTorsoGeom->SetCollisionCylinder(mTorsoLengths[0], mTorsoLengths[1]);
         mTorsoGeom->SetCollisionDetection(true);
         mTorsoGeom->UpdateGeomTransform(xform);

         //if (mTorsoTransformable->GetOSGNode()->asGroup()->getNumChildren() > 0)
         //{
         //   mTorsoTransformable->GetOSGNode()->asGroup()->removeChild(mTorsoTransformable->GetOSGNode()->asGroup()->getChild(0));
         //}

         //mTorsoTransformable->GetOSGNode()->asGroup()->addChild(mTorsoGeom->CreateRenderedCollisionGeometry(true));
         //xform.SetTranslation(xform.GetTranslation() + osg::Vec3(0.0f, 0.0f, mTorsoLengths[0]));
         //mTorsoTransformable->SetTransform(xform);
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

      const float LERP_SPEED = 20.0f;

      // Lerp the target from its current position to its end position
      // over time and then return the lerp position for a smooth translation.
      osg::Vec3 targetP1 = mStartPosition + (mEndPosition - mStartPosition) * dt * LERP_SPEED;

      return targetP1;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool FPSCollider::ApplyMovementVector(const osg::Vec3& vec, float length, const osg::Vec3& oldPos, const osg::Vec3& offset, osg::Vec3& newPos, osg::Vec3& colPos, float& depth)
   {
      dtCore::Transform xform;
      xform.Set(osg::Matrix::rotate(osg::Vec3(0.f, 0.f, 1.f), vec));
      xform.SetTranslation(oldPos + offset);
      mTorsoGeom->UpdateGeomTransform(xform);
      mTorsoGeom->SetCollisionRay(length);
      mTorsoGeom->SetCollisionDetection(true);

      //dtCore::RefPtr<osg::Cylinder> line = new osg::Cylinder(vec * 0.5f, 0.01f, length);
      //dtCore::RefPtr<osg::ShapeDrawable> drawable = new osg::ShapeDrawable(line);
      //dtCore::RefPtr<osg::Geode> geode = new osg::Geode();
      //geode->addDrawable(drawable);
      //dtCore::RefPtr<dtCore::Transformable> transformable = new dtCore::Transformable();
      //transformable->GetOSGNode()->asGroup()->addChild(geode);

      //transformable->SetTransform(xform);
      //mFeetTransformable->GetOSGNode()->asGroup()->addChild(transformable->GetOSGNode());

      if (CollideTorso())
      {
         osg::Vec3 testColPos(mLastTorsoContact.pos[0],
                              mLastTorsoContact.pos[1],
                              mLastTorsoContact.pos[2]);
         osg::Vec3 vec = testColPos - (oldPos + offset);
         float colDepth = vec.length();
         vec.normalize();
         float dot = vec * osg::Vec3(mLastTorsoContact.normal[0],
                                     mLastTorsoContact.normal[1],
                                     mLastTorsoContact.normal[2]);

         if (depth > colDepth && dot < 0.0f)
         {
            colPos = testColPos;
            newPos.set(mLastTorsoContact.pos[0],
                       mLastTorsoContact.pos[1],
                       mLastTorsoContact.pos[2]);

            osg::Vec3 normal(mLastTorsoContact.normal[0],
                             mLastTorsoContact.normal[1],
                             mLastTorsoContact.normal[2]);

            newPos[0] += normal[0] * mTorsoLengths[0];
            newPos[1] += normal[1] * mTorsoLengths[0];

            newPos[0] -= offset[0] * (1.0f - abs(normal[0]));
            newPos[1] -= offset[1] * (1.0f - abs(normal[1]));
            newPos[2] -= offset[2];

            depth = colDepth;
            return true;
         }
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////
   bool FPSCollider::CollideTorso()
   {
      mNormals.clear();
      mNumTorsoContactPoints = 0;

      mStartCollideTorso = true;
      dSpaceCollide2((dGeomID)mCollisionSpace, (dGeomID)mLocalSpaceID, this, NearCallbackTorso);
      mStartCollideTorso = false;

      return mNumTorsoContactPoints > 0;
   }

   ////////////////////////////////////////////////////////////////////////////
   bool FPSCollider::CollideFeet()
   {
      mNormals.clear();
      mNumFeetContactPoints = 0;

      mStartCollideFeet = true;
      dSpaceCollide2((dGeomID)mCollisionSpace, (dGeomID)mLocalSpaceID, this, NearCallbackFeet);
      mStartCollideFeet = false;

      return mNumFeetContactPoints > 0;
   }

   ////////////////////////////////////////////////////////////////////////////
   void FPSCollider::HandleCollideTorso(dGeomID pTorso, dGeomID pObject)
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

      dContactGeom contactGeoms[10];
      int contactPoints = dCollide(pTorso, pObject, 10, contactGeoms, sizeof(dContactGeom));


      if (contactPoints)
      {
         for (int index = 0; index < contactPoints; ++index)
         {
            if (mStartCollideTorso)
            {
               mLastTorsoContact = contactGeoms[index];
               mStartCollideTorso = false;

               mNumTorsoContactPoints = contactPoints;
            }
            else
            {
               if (contactGeoms[index].depth > mLastTorsoContact.depth)
               {
                  mLastTorsoContact = contactGeoms[index];

                  mNumTorsoContactPoints = contactPoints;
               }
            }
         }
      }

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

      dContactGeom contactGeoms[1];
      int contactPoints = dCollide(pFeet, pObject, 1, contactGeoms, sizeof(dContactGeom));

      if (contactPoints)
      {
         if (mStartCollideFeet)
         {
            mLastFeetContact = contactGeoms[0];
            mStartCollideFeet = false;

            mNumFeetContactPoints = contactPoints;
         }
         else
         {
            if (contactGeoms[0].depth > mLastFeetContact.depth)
            {
               mLastFeetContact = contactGeoms[0];

               mNumFeetContactPoints = contactPoints;
            }
         }
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
   osg::Vec3 FPSCollider::Update(const osg::Vec3& initialTargetPosition, 
                                 const osg::Vec3& initialVelocity,
                                 float deltaFrameTime, bool pJump)
   {
      mLastVelocity[0] = initialVelocity[0];
      mLastVelocity[1] = initialVelocity[1];
 
      osg::Vec3 targetPosition = initialTargetPosition;

      //double timeToGo = deltaFrameTime;
      //const double kTimeStep = 1.0/30.0;

      return Step(targetPosition, deltaFrameTime, pJump);

      //while (timeToGo > kTimeStep)
      //{
      //   targetPosition = Step(targetPosition, kTimeStep, pJump);

      //   timeToGo -= kTimeStep;
      //}

      //if (timeToGo > 0.0)
      //{
      //   targetPosition = Step(targetPosition, timeToGo, pJump);
      //}
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Vec3 FPSCollider::GetLastVelocity() const
   {
      return mLastVelocity;
   }

   //////////////////////////////////////////////////////////////////////////
   void FPSCollider::SetCurrentMode(eMode newMode)
   {
      if (newMode != mCurrentMode)
      {
         mCurrentMode = newMode;
         //LOGN_DEBUG("FPSCollider", "Am now: " + dtUtil::ToString(mCurrentMode));
      }
   }

   //////////////////////////////////////////////////////////////////////////
   osg::Vec3 FPSCollider::Step(const osg::Vec3& targetPosition, const double deltaFrameTime, bool pJump)
   {
      // Check if the target has moved away significantly.
      // This happens if the target was moved independantly of this collider.
      if ((targetPosition - mStartPosition).length2() > 0.1f)
      {
         mStartPosition = targetPosition;
         mEndPosition = targetPosition;
         mLastVelocity[2] = 0.0f;
      }

      // Only allow jumping if we are walking.
      if (mCurrentMode == WALKING && pJump)
      {
         mLastVelocity += mGroundNormal * mJumpSpeed;
         mJumping = true;
      }
      else if (mJumping)
      {
         mLastVelocity[0] += mGroundNormal[0] * mJumpSpeed;
         mLastVelocity[1] += mGroundNormal[1] * mJumpSpeed;
      }

      // Apply Gravity.
      const float zGravity = mPhysicsController.valid() ? mPhysicsController->GetGravity()[2]: 0.0f;
      mLastVelocity[2] += zGravity * deltaFrameTime;
      dtUtil::ClampMin(mLastVelocity[2], mTerminalSpeed);

      mStartPosition = AdjustPosition(mEndPosition, deltaFrameTime);

      return mStartPosition;
   }

}//namespace dtCore
