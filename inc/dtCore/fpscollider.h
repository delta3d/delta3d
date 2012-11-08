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

#ifndef DELTA_FPSCOLLIDER
#define DELTA_FPSCOLLIDER

#include <dtCore/export.h>
#include <dtCore/refptr.h>
#include <dtCore/odecontroller.h>
#include <dtCore/odegeomwrap.h>

#include <osg/Vec3>
#include <osg/Vec2>
#include <osg/Matrix>

#include <ode/contact.h>
#include <vector>

namespace dtCore
{
   class Scene;


   /**
    * This class uses ode to simulate fps movement on collision meshes
    * note: this is a simplified physical model and has a few caveats listed below
    *
    * -We currently only support collision meshes
    * -There is a maximum slope that we can go up (SlideThreshold), when on a poly with normal
    *   dotted with Up (0,0,1) < SlideThreshold we begin sliding, the user has no control
    *   until sliding is resolved and we are on a "good" poly again
    * -Because user has no control during Sliding we can get stuck where two polys come together 
    *   where each one is too steep to climb up, typical ravine like so \/
    * -Also when sliding we will continue sliding even if our momentum should carry us over the edge of a cliff
    * -
    */
   class DT_CORE_EXPORT FPSCollider
   {
      public:
         enum eMode{IDLE, WALKING, SLIDING, JUMPING, FALLING};

      public:
         /**
         * Constructor.
         * @param height, the height of the character and camera, in meters      
         * @param radius, the width of our character  
         * @param k the distance from the bottom of the knees to the ground, this represents the maximum step up height
         * @param theta (No longer used)
         * @param Scene is used to get the gravity and the ode space
         */
         FPSCollider(float pHeight, float pRadius, float k, float theta, dtCore::Scene* pScene);
         FPSCollider(float pHeight, float pRadius, float k, float theta, dSpaceID pSpaceToCollideWith, const osg::Vec3& pGravity);
         virtual ~FPSCollider();

         /**
         * @param p0 is the current position of the camera which is our position plus our height above the ground
         * @param v0 is our instantaneous velocity, or the direction we want to go
         *        as a note the last velocity is saved for adjusting falling speed
         *        so if you want to warp you may need to set velocity to 0 for one frame
         * @param deltaFrameTime is the elapsed time in seconds since the last frame
         * @param pJump specifies whether or not we want to jump
         * @return our new position in world coordinates, whatever this is controlling should be set to this pos
         */
         osg::Vec3 Update(const osg::Vec3& initialTargetPosition, const osg::Vec3& v0, float deltaFrameTime, bool pJump);

         /**
         * @return returns the space id used for the FPSCollider
         *  this can be useful for resolving collisions between the collider
         *  and other objects (perhaps another collider)
         */
         dSpaceID GetSpaceID() const;

         /**
         * @return our current mode, WALKING, FALLING, or SLIDING
         * @note we start in the FALLING state
         */
         eMode GetMode() const;

         /*
         * @return the slide threshold is used to determine the maximum slope
         *  we can climb Normal DOT UP < SlideThreshold means we are sliding
         */
         float GetSlideThreshold() const;
         void SetSlideThreshold(float pSlide);

         /**
          * @return the slide speed used to determine how fast you slide down
          * a sloped surface.
          */
         float GetSlideSpeed() const;
         void SetSlideSpeed(float speed);
         
         /**
         * Jump speed is the force we apply to our body when jumping in meters per second
         */
         float GetJumpSpeed() const;
         void SetJumpSpeed(float pSpeed);

         /**
          * Smoothing speed is the speed in which the motion will smooth
          * the translation of the target.
          */
         float GetSmoothingSpeed() const;
         void SetSmoothingSpeed(float speed);

         /**
         * @return height above the terrain is the camera height and the top of our bv
         */
         float GetHeightAboveTerrain() const;

         /**
         * @param pHeight is the height of our camera and the top of our bv
         * @param pRadius is the width of our character 
         * @param k is the max distance we can step up and the size of our feet bv
         */
         void SetDimensions(float targetHeightAboveTerrain, float pRadius, float stepUpHeight);

         /**
         * @return the bits used to determine what the feet can collide with
         */
         unsigned long GetCollisionBitsForFeet() const;
         void SetCollisionBitsForFeet(unsigned long bits);    

         /**
         * @return the bits used to determine what the torso can collide with
         */
         unsigned long GetCollisionBitsForTorso() const;
         void SetCollisionBitsForTorso(unsigned long bits);

         /**
         * Resets the current velocities to 0.
         */
         void Reset();

         /** 
         * @return The last velocity vector used
         */
         osg::Vec3 GetLastVelocity() const;

      private:

         dGeomID GetFeetGeom();
         dGeomID GetTorsoGeom();

         ///ODE collision callback
         static void NearCallbackFeet(void *data, dGeomID o1, dGeomID o2);
         static void NearCallbackTorso(void *data, dGeomID o1, dGeomID o2);

         //static void dTriArrayCallback(dGeomID TriMesh, dGeomID RefObject, const int* TriIndices, int TriCount);

         void InitBoundingVolumes();

         void HandleCollideFeet(dGeomID pFeet, dGeomID pObject);
         void HandleCollideTorso(dGeomID pTorso, dGeomID pObject);

         bool CollideTorso();
         bool CollideFeet();

         ///@return true if torso collided with anything
         osg::Vec3 AdjustPosition(const osg::Vec3& newPos, float dt);

         void SetCurrentMode(eMode newMode);

         dGeomID mBBFeet;
         dtCore::RefPtr<dtCore::ODEGeomWrap> mFeetGeom;

         dGeomID mBBTorso;
         dtCore::RefPtr<dtCore::ODEGeomWrap> mTorsoGeom;

         ///The local collision space that holds our collision geometry
         dSpaceID mLocalSpaceID;

         ///From feet collision shape to MotionModel target
         float mTorsoOffset;

         osg::Vec2 mTorsoLengths;///<radius, length

         int mNumFeetContactPoints;
         int mNumTorsoContactPoints;

         dContactGeom mLastFeetContact;
         dContactGeom mLastTorsoContact;

         eMode mCurrentMode;

         float mSlideSpeed;
         float mSlideThreshold;
         float mJumpSpeed;
         float mTerminalSpeed;
         float mHeightAboveTerrain;
         float mMaxStepUpDistance;

         osg::Vec3 mLastVelocity;
         osg::Vec3 mStartPosition;
         osg::Vec3 mEndPosition;
         osg::Vec3 mGroundNormal;

         float mSmoothingSpeed;

         ///The Delta3D collision space that holds all other collision geometry
         dSpaceID mCollisionSpace;
         dtCore::RefPtr<ODEController> mPhysicsController;
   };
}//namespace dtCore

#endif // __DELTA_FPSCOLLIDER_H__
