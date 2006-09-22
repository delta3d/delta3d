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

#ifndef __DELTA_FPSCOLLIDER_H__
#define __DELTA_FPSCOLLIDER_H__

#include <dtCore/export.h>
#include <osg/Vec3>
#include <osg/Matrix>

#include <ode/ode.h>

#include <vector>

namespace dtCore
{
   /**
    * This class uses ode to simulate fps movement 
    */
   class DT_CORE_EXPORT FPSCollider
   {
      public:
         enum eMode{WALKING = 0, FALLING, SLIDING};

      public:
   
         FPSCollider(float pHeight, float pRadius, float k, float theta, dSpaceID pSpaceToCollideWith, const osg::Vec3& pGravity);
         virtual ~FPSCollider();


         osg::Vec3 Update(const osg::Vec3& p0, const osg::Vec3& v0, float deltaFrameTime, bool pJump);
   
      private:

         void UpdateBoundingVolumes(const osg::Vec3& xyz);

         dGeomID GetFeetGeom();
         dGeomID GetTorsoGeom();

         ///ODE collision callback
         static void NearCallbackFeet(void *data, dGeomID o1, dGeomID o2);
         static void NearCallbackTorso(void *data, dGeomID o1, dGeomID o2);

         static void dTriArrayCallback(dGeomID TriMesh, dGeomID RefObject, const int* TriIndices, int TriCount);

         void CreateCollisionCylinder(dWorldID pWorldId, dSpaceID pSpaceId, dGeomID& pId, const osg::Vec3& pLengths);

         void InitBoundingVolumes();
         void InitDrawable();

         void HandleCollideFeet(dGeomID pFeet, dGeomID pObject);
         void HandleCollideTorso(dGeomID pTorso, dGeomID pObject);

         bool CollideTorso();
         bool CollideFeet();

         bool TestPosition(osg::Vec3& newPos);

         dGeomID mBBFeet;
         dGeomID mBBTorso;

         dSpaceID mSpaceID;

         osg::Vec3 mBBFeetOffset;
         osg::Vec3 mBBTorsoOffset;

         osg::Vec3 mBBFeetLengths;
         osg::Vec3 mBBTorsoLengths;

         std::vector<osg::Vec3> mNormals;

         int mNumFeetContactPoints;
         int mNumTorsoContactPoints;

         bool mStartCollideFeet;
         dContactGeom mLastFeetContact;

         bool mJumped;

         float mAirControl;
         bool mFreeFall;
         double mFreeFallCounter;

         eMode mCurrentMode;

         float mSlideThreshold;
         float mSlideSpeed;
         float mJumpSpeed;
         float mHeightAboveTerrain;
         float mMaxStepUpDistance;

         osg::Vec3 mTerminalVelocity;
         osg::Vec3 mLastVelocity;
         osg::Vec3 mSlideVelocity;
         osg::Vec3 mFallingVelocity;
         osg::Vec3 mGravity;

         dSpaceID mCollisionSpace;
   
   };
}//namespace dtCore

#endif // __DELTA_FPSCOLLIDER_H__
