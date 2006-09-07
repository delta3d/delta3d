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
 * Bradley Anderegg 08/21/2006
 * modified original FPSMotionModel by Terry Jordan
 */

#include <prefix/dtcoreprefix-src.h>
#include <dtCore/collisionmotionmodel.h>

#include <dtCore/keyboard.h>
#include <dtCore/mouse.h>
#include <dtCore/inputdevice.h>
#include <dtCore/logicalinputdevice.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtCore/transformable.h>
#include <dtCore/isector.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/matrixutil.h>

namespace dtCore
{

   CollisionMotionModel::FPSAxisListener::FPSAxisListener(const SetFunctor& setFunc):
   mSetFunctor(setFunc)
   {
   }

   ///When the axis changes, just call the functor with the new values
   bool CollisionMotionModel::FPSAxisListener::AxisStateChanged(const Axis* axis,
      double oldState, 
      double newState, 
      double delta)
   {
      return mSetFunctor(newState, delta);
   }


   /**
   * Constructor.
   *
   * @param keyboard the keyboard instance, or 0 to
   * avoid creating default input mappings
   * @param mouse the mouse instance, or 0 to avoid
   * creating default input mappings
   */
   CollisionMotionModel::CollisionMotionModel(  Keyboard* keyboard,
      Mouse* mouse, Scene* scene ) : MotionModel("CollisionMotionModel"),
      mWalkForwardBackwardAxis(0),
      mTurnLeftRightAxis(0),
      mLookUpDownAxis(0),
      mSidestepLeftRightAxis(0),
      mSidestepListener(0),
      mForwardBackwardListener(0),
      mLookLeftRightListener(0),
      mLookUpDownListener(0),
      mMaximumWalkSpeed(10.0f),
      mMaximumTurnSpeed(14440.0f),
      mMaximumSidestepSpeed(10.0f),
      mHeightAboveTerrain(1.0f),
      mMaximumStepUpDistance(.15f),
      mFallingHeight(0.25f),
      mFallingVec(0.f, 0.f, 0.f),
      mFalling(false),
      mForwardBackCtrl(0.f),
      mSidestepCtrl(0.f),
      mLookLeftRightCtrl(0.f),
      mLookUpDownCtrl(0.f),
      mBBFeetOffset(0.0f, 0.0f, -1.0f),
      mBBTorsoOffset(0.0f, 0.0f, -0.5f),
      mBBFeetLengths(0.5f, 0.5f, 0.5f),
      mBBTorsoLengths(0.5f, 0.5f, 1.0f),
      mNumFeetContactPoints(0),
      mNumTorsoContactPoints(0),
      mScene(scene),
      mJumpLength(0.25f),
      mJumpTimer(0.0f),
      mJumped(false),
      mMouse(mouse),
      mKeyboard(keyboard),
      mAirControl(0.35f),
      mFreeFall(false),
      mFreeFallCounter(0)

   {

      //setup some axis listeners with functors 
      FPSAxisListener::SetFunctor fbFunc(this, &CollisionMotionModel::OnForwardBackwardChanged);
      FPSAxisListener::SetFunctor sideStepFunc(this, &CollisionMotionModel::OnSidestepChanged);  
      FPSAxisListener::SetFunctor lookLeftRightFunc(this, &CollisionMotionModel::OnLookLeftRightChanged);
      FPSAxisListener::SetFunctor lookUpDownFunc(this, &CollisionMotionModel::OnLookUpDownChanged);

      mLookUpDownListener = new FPSAxisListener( lookUpDownFunc );
      mLookLeftRightListener = new FPSAxisListener( lookLeftRightFunc );
      mSidestepListener = new FPSAxisListener( sideStepFunc );
      mForwardBackwardListener = new FPSAxisListener( fbFunc );

      if(keyboard != 0 && mouse != 0)
      {
         SetDefaultMappings(keyboard, mouse);
      }

      mMouse = mouse;

      InitBoundingVolumes();

      AddSender(System::Instance());
   }

   /**
   * Destructor.
   */
   CollisionMotionModel::~CollisionMotionModel()
   {
      RemoveSender(System::Instance());

      mLookUpDownAxis->RemoveAxisListener(mLookUpDownListener);
      mTurnLeftRightAxis->RemoveAxisListener(mLookLeftRightListener);
      mSidestepLeftRightAxis->RemoveAxisListener(mSidestepListener);
      mWalkForwardBackwardAxis->RemoveAxisListener(mForwardBackwardListener);

      delete mLookUpDownListener;
      delete mLookLeftRightListener;
      delete mSidestepListener;
      delete mForwardBackwardListener;
   }

   /**
   * Enables or disables this motion model.
   *
   * @param enabled true to enable this motion model, false
   * to disable it
   */
   void CollisionMotionModel::SetEnabled(bool enabled)
   {
      if(enabled && !MotionModel::IsEnabled())
      {
         mMouse->SetPosition(0.0f,0.0f);

         mLookUpDownAxis->AddAxisListener(mLookUpDownListener);
         mTurnLeftRightAxis->AddAxisListener(mLookLeftRightListener);
         mSidestepLeftRightAxis->AddAxisListener(mSidestepListener);
         mWalkForwardBackwardAxis->AddAxisListener(mForwardBackwardListener);
      }
      else
      {
         mLookUpDownAxis->RemoveAxisListener(mLookUpDownListener);
         mTurnLeftRightAxis->RemoveAxisListener(mLookLeftRightListener);
         mSidestepLeftRightAxis->RemoveAxisListener(mSidestepListener);
         mWalkForwardBackwardAxis->RemoveAxisListener(mForwardBackwardListener);
      }

      MotionModel::SetEnabled(enabled);
   }

   /**
   * Sets the input axes to a set of default mappings for mouse
   * and keyboard.
   *
   * @param keyboard the keyboard instance
   * @param mouse the mouse instance
   */
   void CollisionMotionModel::SetDefaultMappings(Keyboard* keyboard, Mouse* mouse)
   {
      if(mDefaultInputDevice.get() == 0)
      {
         mDefaultInputDevice = new LogicalInputDevice("FPSLogicalInputDevice");

         Axis* leftRightMouseMovement = mDefaultInputDevice->AddAxis(
            "left/right mouse movement",
            mLeftRightMouseMovement = new AxisToAxis(mouse->GetAxis(0)));

         Axis* upDownMouseMovement = mDefaultInputDevice->AddAxis(
            "up/down mouse movement",
            mUpDownMouseMovement = new AxisToAxis(mouse->GetAxis(1)));

         Axis* arrowKeysUpAndDown = mDefaultInputDevice->AddAxis(
            "arrow keys up/down",
            mArrowKeysUpDownMapping = new ButtonsToAxis(
            keyboard->GetButton(Producer::Key_S),
            keyboard->GetButton(Producer::Key_W)
            )
            );
         Axis* arrowKeysLeftAndRight = mDefaultInputDevice->AddAxis(
            "arrow keys left/right",
            mArrowKeysLeftRightMapping = new ButtonsToAxis(
            keyboard->GetButton(Producer::Key_A),
            keyboard->GetButton(Producer::Key_D)
            )
            );

         mDefaultWalkForwardBackwardAxis = mDefaultInputDevice->AddAxis(
            "default walk forward/backward",
            new AxesToAxis(arrowKeysUpAndDown)
            );

         mDefaultTurnLeftRightAxis = mDefaultInputDevice->AddAxis(
            "default turn left/right",
            new AxesToAxis(leftRightMouseMovement)
            );

         mDefaultLookUpDownAxis = mDefaultInputDevice->AddAxis(
            "default look up/down",
            new AxesToAxis(upDownMouseMovement)
            );

         mDefaultSidestepLeftRightAxis = mDefaultInputDevice->AddAxis(
            "default sidestep left/right",
            new AxesToAxis(arrowKeysLeftAndRight)
            );
      }

      SetWalkForwardBackwardAxis(mDefaultWalkForwardBackwardAxis);

      SetTurnLeftRightAxis(mDefaultTurnLeftRightAxis);

      SetLookUpDownAxis(mDefaultLookUpDownAxis);

      SetSidestepLeftRightAxis(mDefaultSidestepLeftRightAxis);
   }

   /**
   * Sets the axis that moves the target forwards (for positive
   * values) or backwards (for negative values).
   *
   * @param walkForwardBackwardAxis the new forward/backward axis
   */
   void CollisionMotionModel::SetWalkForwardBackwardAxis(Axis* walkForwardBackwardAxis)
   {
      if (mWalkForwardBackwardAxis) 
      {
         mWalkForwardBackwardAxis->RemoveAxisListener(mForwardBackwardListener);
      }

      mWalkForwardBackwardAxis = walkForwardBackwardAxis;

      mWalkForwardBackwardAxis->AddAxisListener(mForwardBackwardListener);
   }

   /**
   * Returns the axis that moves the target forwards (for positive
   * values) or backwards (for negative values).
   *
   * @return the current forward/backward axis
   */
   Axis* CollisionMotionModel::GetWalkForwardBackwardAxis()
   {
      return mWalkForwardBackwardAxis;
   }

   /**
   * Sets the axis that turns the target left (for negative values)
   * or right (for positive values).
   *
   * @param turnLeftRightAxis the new turn left/right axis
   */
   void CollisionMotionModel::SetTurnLeftRightAxis(Axis* turnLeftRightAxis)
   {
      if (mTurnLeftRightAxis)
      {
         mTurnLeftRightAxis->RemoveAxisListener(mLookLeftRightListener);
      }

      mTurnLeftRightAxis = turnLeftRightAxis;  

      mTurnLeftRightAxis->AddAxisListener(mLookLeftRightListener);
   }

   /**
   * Returns the axis that turns the target left (for negative values)
   * or right (for positive values).
   *
   * @return the current turn left/right axis
   */
   Axis* CollisionMotionModel::GetTurnLeftRightAxis()
   {
      return mTurnLeftRightAxis;
   }

   /**
   * Sets the axis that looks down (for negative values)
   * or up (for positive values).
   *
   * @param lookUpDownAxis the new look up/down axis
   */
   void CollisionMotionModel::SetLookUpDownAxis(Axis* lookUpDownAxis)
   {
      if (mLookUpDownAxis)
      {
         mLookUpDownAxis->RemoveAxisListener(mLookUpDownListener);
      }

      mLookUpDownAxis = lookUpDownAxis;  

      mLookUpDownAxis->AddAxisListener(mLookUpDownListener);
   }

   /**
   * Returns the axis that looks down (for negative values)
   * or up (for positive values).
   *
   * @return the current look up/down axis
   */
   Axis* CollisionMotionModel::GetLookUpDownAxis()
   {
      return mLookUpDownAxis;
   }

   /**
   * Sets the axis that sidesteps the target left (for negative values)
   * or right (for positive values).
   *
   * @param sidestepLeftRightAxis the new sidestep left/right axis
   */
   void CollisionMotionModel::SetSidestepLeftRightAxis(Axis* sidestepLeftRightAxis)
   {
      if (mSidestepLeftRightAxis)
      {
         mSidestepLeftRightAxis->RemoveAxisListener(mSidestepListener);
      }

      mSidestepLeftRightAxis = sidestepLeftRightAxis;

      mSidestepLeftRightAxis->AddAxisListener(mSidestepListener);
   }

   /**
   * Returns the axis that sidesteps the target left (for negative values)
   * or right (for positive values).
   *
   * @return the current sidestep left/right axis
   */
   Axis* CollisionMotionModel::GetSidestepLeftRightAxis()
   {
      return mSidestepLeftRightAxis;
   }

   /**
   * Sets the maximum walk speed (meters per second).
   *
   * @param maximumWalkSpeed the new maximum walk speed
   */
   void CollisionMotionModel::SetMaximumWalkSpeed(float maximumWalkSpeed)
   {
      mMaximumWalkSpeed = maximumWalkSpeed;
   }

   /**
   * Returns the maximum walk speed (meters per second).
   *
   * @return the current maximum walk speed
   */
   float CollisionMotionModel::GetMaximumWalkSpeed()
   {
      return mMaximumWalkSpeed;
   }

   /**
   * Sets the maximum turn speed (degrees per second).
   *
   * @param maximumTurnSpeed the new maximum turn speed
   */
   void CollisionMotionModel::SetMaximumTurnSpeed(float maximumTurnSpeed)
   {
      mMaximumTurnSpeed = maximumTurnSpeed;
   }

   /**
   * Returns the maximum turn speed (degrees per second).
   *
   * @return the current maximum turn speed
   */
   float CollisionMotionModel::GetMaximumTurnSpeed()
   {
      return mMaximumTurnSpeed;
   }

   /**
   * Sets the maximum sidestep speed (meters per second).
   *
   * @param maximumSidestepSpeed the new maximum sidestep speed
   */
   void CollisionMotionModel::SetMaximumSidestepSpeed(float maximumSidestepSpeed)
   {
      mMaximumSidestepSpeed = maximumSidestepSpeed;
   }

   /**
   * Returns the maximum sidestep speed (meters per second).
   *
   * @return the current maximum sidestep speed
   */
   float CollisionMotionModel::GetMaximumSidestepSpeed()
   {
      return mMaximumSidestepSpeed;
   }

   /**
   * Sets the height to maintain above the terrain (meters).
   *
   * @param heightAboveTerrain the height to maintain above the
   * terrain
   */
   void CollisionMotionModel::SetHeightAboveTerrain(float heightAboveTerrain)
   {
      mHeightAboveTerrain = heightAboveTerrain;
   }

   /**
   * Returns the height to maintain above the terrain (meters).
   *
   * @return the height to maintain above the terrain
   */
   float CollisionMotionModel::GetHeightAboveTerrain()
   {
      return mHeightAboveTerrain;
   }

   /**
   * Sets the maximum step-up distance.  When clamping to the ground, the
   * maximum step-up distance determines whether to rise to a new level
   * (as when the model climbs a staircase) or to stay at the current level
   * (as when the model passes under a roof).  The default is 1.0.
   *
   * @param maximumStepUpDistance the new maximum step-up distance
   */
   void CollisionMotionModel::SetMaximumStepUpDistance(float maximumStepUpDistance)
   {
      mMaximumStepUpDistance = maximumStepUpDistance;
   }

   /**
   * Returns the current maximum step-up distance.
   *
   * @return the maximum step-up distance
   */
   float CollisionMotionModel::GetMaximumStepUpDistance()
   {
      return mMaximumStepUpDistance;
   }


   float CollisionMotionModel::GetFallingHeight() const
   {
      return mFallingHeight;
   }

   bool CollisionMotionModel::OnForwardBackwardChanged(double newState, double delta)
   {
      mForwardBackCtrl = newState;
      return true;
   }

   bool CollisionMotionModel::OnSidestepChanged(double newState, double delta)
   {
      mSidestepCtrl = newState;
      return true;
   }

   bool CollisionMotionModel::OnLookLeftRightChanged(double newState, double delta)
   {
      mLookLeftRightCtrl = newState;
      return true;
   }

   bool CollisionMotionModel::OnLookUpDownChanged(double newState, double delta)
   {
      mLookUpDownCtrl = newState;
      return true;
   }


   void CollisionMotionModel::InitBoundingVolumes()
   { 
      mSpaceID = dSimpleSpaceCreate(0);
      CreateCollisionBox(mScene->GetWorldID(), mSpaceID, mBBFeet, mBBFeetLengths);
      CreateCollisionBox(mScene->GetWorldID(), mSpaceID, mBBTorso, mBBTorsoLengths);
   }

   void CollisionMotionModel::CreateCollisionBox(dWorldID pWorldId, dSpaceID pSpaceId, dGeomID& pId, const osg::Vec3& pLengths)
   {
      pId = dCreateGeomTransform(0);

      dGeomTransformSetGeom(pId, dCreateBox(0, pLengths[0], pLengths[1], pLengths[2]));

      dSpaceAdd(pSpaceId, pId);
      dGeomSetData(pId, this );

      dGeomSetCategoryBits( pId, 0 );
      dGeomSetCollideBits( pId, 0xFFFFFFFF );
   }

   /**
   * Message handler callback.
   *
   * @param data the message data
   */
   void CollisionMotionModel::OnMessage(MessageData *data)
   {      

      if(GetTarget() != 0 &&
         IsEnabled() && 
         data->message == "preframe")
      {

         const double deltaFrameTime = static_cast<const double*>(data->userData)[1];

         Transform transform;
         GetTarget()->GetTransform(transform);

         osg::Vec3 xyz, hpr;
         float newH = 0.f, newP = 0.f;
         osg::Vec3 newXYZ;

         transform.GetRotation(hpr);
         transform.GetTranslation(xyz);

         //calculate our new heading
         newH = hpr[0] - mLookLeftRightCtrl * mMaximumTurnSpeed * deltaFrameTime;

         //calculate our new pitch
         newP = hpr[1] + mLookUpDownCtrl * mMaximumTurnSpeed * deltaFrameTime;
         dtUtil::Clamp<float>(newP, -89.9f, 89.9f); //stay away from 90.0 as it causes funky gimbal lock
         mLookUpDownAxis->SetState(0.0f);//necessary to stop camera drifting down

         //calculate x/y delta
         float adjusted_dt = deltaFrameTime;
         if(mFreeFall || mJumped) adjusted_dt *= mAirControl;
         osg::Vec3 translation;
         translation[0] = mSidestepCtrl * mMaximumSidestepSpeed * adjusted_dt;
         translation[1] = mForwardBackCtrl * mMaximumWalkSpeed * adjusted_dt;

         //transform our x/y delta by our new heading
         osg::Matrix mat;
         mat.makeRotate(osg::DegreesToRadians(newH), osg::Vec3(0.f, 0.f, 1.f) );
         translation = translation * mat;

         newXYZ = xyz + translation;

         if(CollideTorso(newXYZ))
         {
            static int frame = 0;
            osg::Vec3 norm;            
            osg::Vec3 result = translation;

            std::cout << std::endl << std::endl;
            for(int i = 0; i < mNumTorsoContactPoints; ++i)
            {
               norm.set(mLastContactPoints[i].normal[0], mLastContactPoints[i].normal[1], mLastContactPoints[i].normal[2]);
               if(mLastContactPoints[i].g1 != GetTorsoGeom())
               {
                  norm.set(-norm[0], -norm[1], -norm[2]);
               }
               float dot = (norm * result);
               result -= osg::Vec3(norm[0] * dot, norm[1] * dot, norm[2] * dot);
            }
            ++frame;

            newXYZ = xyz + result;

            if(CollideTorso(newXYZ)) newXYZ = xyz;          
         }
         

         if(mScene.valid())
         {         
            //ground clamp if required
            AdjustElevation(newXYZ, deltaFrameTime);
         }


         transform.SetTranslation(newXYZ);
         if(mMouse->GetButtonState(Mouse::LeftButton)) 
         transform.SetRotation(newH, newP, 0.f);
         GetTarget()->SetTransform(transform); 

         if(mMouse->GetButtonState(Mouse::LeftButton)) 
         mMouse->SetPosition(0.0f,0.0f);//keeps cursor at center of screen


         UpdateBoundingVolumes(newXYZ, osg::Vec3(newH, newP, 0.0f), mMouse->GetButtonState(Mouse::LeftButton));

      }
   }


   ///Update the MotionModel's elevation by either ground clamping, or "falling"
   void CollisionMotionModel::AdjustElevation(osg::Vec3 &xyz, double deltaFrameTime)
   {

      CollideFeet(xyz);

      if(mNumFeetContactPoints)
      {
         mFreeFall = false;
      }
      else if(!mFreeFall)
      {                  
         mFreeFallCounter += deltaFrameTime;
         if(mFreeFallCounter > 0.25)
         {
            mFreeFall = true;
            mFreeFallCounter = 0.0;
         }       
      }

      if(mJumped)
      {
         mJumpTimer -= deltaFrameTime;
         if(mJumpTimer <= 0.0f)
         {
            mJumpTimer = 0.0f;
            mJumped = false;
            mFallingVec.set(0.f, 0.f, 0.f);
         }
         else
         {
            osg::Vec3 gravityVec;
            mScene->GetGravity(gravityVec);
            gravityVec.set(-gravityVec[0] * 0.5f, -gravityVec[1] * 0.5f, -gravityVec[2] * 0.5f);

            mFallingVec += gravityVec * deltaFrameTime;
            xyz += gravityVec * deltaFrameTime;  
         }
      }
      else
      {         

         float hot = 0.0f;

         if (mNumFeetContactPoints)
         {    
            hot = (xyz[2] + mBBFeetOffset[2]) + mLowestZValue;
            mNumFeetContactPoints = 0;
         }
         else
         {
            mFalling = true;
            hot = -100000000.0f;//std::min<float>();
         }

         ////add in the offset distance off the height of terrain
         const float targetHeight = hot + mHeightAboveTerrain;

         if (mFalling)
         {
            //adjust the position based on the gravity vector

            osg::Vec3 gravityVec;
            mScene->GetGravity(gravityVec);

            mFallingVec += gravityVec * deltaFrameTime;

            //modify our position using the falling vector
            xyz += mFallingVec * deltaFrameTime;    

            //make sure didn't fall below the terrain
            if (xyz[2] <= targetHeight)
            {
               //stop falling
               mFallingVec.set(0.f, 0.f, 0.f);
               xyz[2] = targetHeight;
               mFalling = false;
            }
         }

         //otherwise, lets clamp to the terrain
         else 
         {
            mFallingVec.set(0.f, 0.f, 0.f);
            xyz[2] = targetHeight;
         }

         if(!mFreeFall && !mJumped && mKeyboard->GetKeyState(Producer::Key_space))
         {
            mJumped = true;
            mJumpTimer = mJumpLength;
         }
      }
   }

   bool CollisionMotionModel::CollideTorso(const osg::Vec3& newPos)
   {
      osg::Vec3 torso = newPos + mBBTorsoOffset;
      dGeomSetPosition(mBBTorso, torso[0], torso[1], torso[2]);

      dSpaceCollide2((dGeomID)mScene->GetSpaceID(), (dGeomID)mSpaceID, this, NearCallbackTorso);

      return mNumTorsoContactPoints > 0;
   }

   bool CollisionMotionModel::CollideFeet(const osg::Vec3& newPos)
   {
      osg::Vec3 newVec = newPos + mBBFeetOffset;
      dGeomSetPosition(mBBFeet, newVec[0], newVec[1], newVec[2]);

      dSpaceCollide2((dGeomID)mScene->GetSpaceID(), (dGeomID)mSpaceID, this, NearCallbackFeet);

      return mNumFeetContactPoints > 0;
   }


   void CollisionMotionModel::HandleCollideTorso(dGeomID pFeet, dGeomID pObject)
   {
      if(pObject == GetFeetGeom()) return;      

      mNumTorsoContactPoints = dCollide( pFeet, pObject, 3, mLastContactPoints, sizeof(dContactGeom) );
   }

   void CollisionMotionModel::HandleCollideFeet(dGeomID pFeet, dGeomID pObject)
   {

      if(pObject == GetTorsoGeom()) return;

      dContactGeom contactGeoms[8];

      mNumFeetContactPoints = dCollide( pFeet, pObject, 8, contactGeoms, sizeof(dContactGeom) );

      if( mNumFeetContactPoints > 0)
      {
         int index = 0;
         mLowestZValue = contactGeoms[0].pos[2];

         for(int i = 0; i < mNumFeetContactPoints; ++i)
         {
            if(contactGeoms[i].pos[2] < mLowestZValue)
            {
               index = i;
               mLowestZValue = contactGeoms[i].pos[2];
            }
         }

         mLowestZValue = contactGeoms[index].depth;
      }
   }

   // ODE collision callback
   void CollisionMotionModel::NearCallbackFeet( void* data, dGeomID o1, dGeomID o2 )
   {
      if( data == 0 || o1 == 0 || o2 == 0 )
      {
         return;
      }

      CollisionMotionModel* cmm = static_cast<CollisionMotionModel*>(data);

      if (o1 == cmm->GetFeetGeom()) 
      {
         cmm->HandleCollideFeet(o1, o2);
      }
      else if (cmm->GetFeetGeom())
      {
         cmm->HandleCollideFeet(o2, o1);
      }
   }

   // ODE collision callback
   void CollisionMotionModel::NearCallbackTorso( void* data, dGeomID o1, dGeomID o2 )
   {
      if( data == 0 || o1 == 0 || o2 == 0 )
      {
         return;
      }

      CollisionMotionModel* cmm = static_cast<CollisionMotionModel*>(data);

      if (o1 == cmm->GetTorsoGeom()) 
      {
         cmm->HandleCollideTorso(o1, o2);
      }
      else if (o2 == cmm->GetTorsoGeom())
      {
         cmm->HandleCollideTorso(o2, o1);
      }
   }


   void CollisionMotionModel::UpdateBoundingVolumes(const osg::Vec3& xyz, const osg::Vec3& hpr, bool pRotate)
   {
      //change position of object
      // Set translation

      osg::Vec3 newVec = xyz + mBBFeetOffset;
      dGeomSetPosition(mBBFeet, newVec[0], newVec[1], newVec[2]);

      osg::Vec3 torso = xyz + mBBTorsoOffset;
      dGeomSetPosition(mBBTorso, torso[0], torso[1], torso[2]);

      osg::Matrix rotation; 
      dtUtil::MatrixUtil::HprToMatrix(rotation, osg::Vec3(hpr[0], 0.0f, 0.0f));

      //if(pRotate)
      //{
      //   // Set rotation
      //   dMatrix3 dRot;

      //   dRot[0] = rotation(0,0);
      //   dRot[1] = rotation(1,0);
      //   dRot[2] = rotation(2,0);

      //   dRot[4] = rotation(0,1);
      //   dRot[5] = rotation(1,1);
      //   dRot[6] = rotation(2,1);

      //   dRot[8] = rotation(0,2);
      //   dRot[9] = rotation(1,2);
      //   dRot[10] = rotation(2,2);

      //   dGeomSetRotation(mBBFeet, dRot);
      //   dGeomSetRotation(mBBTorso, dRot);
      //}

   }

   dGeomID CollisionMotionModel::GetFeetGeom()
   {
      return mBBFeet;
   }

   dGeomID CollisionMotionModel::GetTorsoGeom()
   {
      return mBBTorso;
   }


}
