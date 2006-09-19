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
* Bradley Anderegg and Chris Darken 08/21/2006
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
CollisionMotionModel::CollisionMotionModel( float pHeight, float pRadius, float k, float theta, Keyboard* keyboard,
      Mouse* mouse, Scene* scene ) : MotionModel("CollisionMotionModel"),
      mBBFeet(),
      mBBTorso(),
      mSpaceID(),
      mBBFeetOffset(0.0f, 0.0f, pHeight + (theta * 0.5f) - (k * 0.5f)),
      mBBTorsoOffset(0.0f, 0.0f, (pHeight - k) * 0.5f),
      mBBFeetLengths(pRadius, pRadius, theta + k),
      mBBTorsoLengths(pRadius, pRadius, pHeight - k),
      mNormals(),
      mNumFeetContactPoints(0),
      mNumTorsoContactPoints(0),
      mStartCollideFeet(false),
      mLastFeetContact(),
      mJumped(false),
      mAirControl(0.35f),
      mFreeFall(false),
      mFreeFallCounter(0), 
      mCurrentMode(FALLING),
      mSlideThreshold(0.65f),
      mSlideSpeed(3.0f),
      mJumpSpeed(5.0f),
      mTerminalVelocity(0.0f, 0.0f, -50.0f),
      mLastVelocity(0.0f, 0.0f, 0.0f),
      mSlideVelocity(0.0f, 0.0f, 0.0f),
      mFallingVelocity(0.f, 0.f, 0.f),
      mScene(scene),
      mDefaultInputDevice(),
      mLeftRightMouseMovement(0),
      mUpDownMouseMovement(0),
      mArrowKeysUpDownMapping(0),
      mArrowKeysLeftRightMapping(0),
      mADKeysLeftRightMapping(0),
      mDefaultWalkForwardBackwardAxis(0),
      mDefaultTurnLeftRightAxis(0),
      mDefaultLookUpDownAxis(0),
      mDefaultSidestepLeftRightAxis(0),
      mWalkForwardBackwardAxis(0),
      mTurnLeftRightAxis(0),
      mLookUpDownAxis(0),
      mSidestepLeftRightAxis(0),
      mSidestepListener(0),
      mForwardBackwardListener(0),
      mLookLeftRightListener(0),
      mLookUpDownListener(0),
      mMaximumWalkSpeed(3.0f),
      mMaximumTurnSpeed(10000.0f),
      mMaximumSidestepSpeed(3.0f),
      mHeightAboveTerrain(pHeight),
      mMouse(mouse),
      mKeyboard(keyboard),
      mForwardBackCtrl(0.0f),
      mSidestepCtrl(0.0f),
      mLookLeftRightCtrl(0.0f),
      mLookUpDownCtrl(0.0f)
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

dGeomID CollisionMotionModel::GetFeetGeom()
{
   return mBBFeet;
}

dGeomID CollisionMotionModel::GetTorsoGeom()
{
   return mBBTorso;
}


void CollisionMotionModel::InitBoundingVolumes()
{ 
   mSpaceID = dSimpleSpaceCreate(0);
   CreateCollisionCylinder(mScene->GetWorldID(), mSpaceID, mBBFeet, mBBFeetLengths);
   CreateCollisionCylinder(mScene->GetWorldID(), mSpaceID, mBBTorso, mBBTorsoLengths);
}


void CollisionMotionModel::CreateCollisionCylinder(dWorldID pWorldId, dSpaceID pSpaceId, dGeomID& pId, const osg::Vec3& pLengths)
{
   pId = dCreateGeomTransform(0);

   dGeomTransformSetGeom(pId, dCreateCylinder(0, pLengths[1], pLengths[2]));

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
      osg::Vec3 translation;
      translation[0] = mSidestepCtrl * mMaximumSidestepSpeed;
      translation[1] = mForwardBackCtrl * mMaximumWalkSpeed;

      //transform our x/y delta by our new heading
      osg::Matrix mat;
      mat.makeRotate(osg::DegreesToRadians(newH), osg::Vec3(0.0f, 0.0f, 1.0f) );
      translation = translation * mat;

      osg::Vec3 gravity;
      mScene->GetGravity(gravity);

      osg::Vec3 p0 = xyz;
      osg::Vec3 v0 = translation;
      osg::Vec3 v1, p1;
      
      //added flag for jumping, used true when we are on the way up
      if(mJumped && mLastVelocity[2] <= 0.0f)
      {
         mJumped = false;
      }

      switch(mCurrentMode)
      {
          case FALLING:
           {
              mFallingVelocity[2] = mLastVelocity[2] + gravity[2] * deltaFrameTime;
              if(mFallingVelocity[2] < mTerminalVelocity[2]) mFallingVelocity[2] = mTerminalVelocity[2];
              v0 = osg::Vec3(mLastVelocity[0], mLastVelocity[1], mFallingVelocity[2]);
              p1 = p0 + osg::Vec3(v0[0] * deltaFrameTime, v0[1] * deltaFrameTime, v0[2] * deltaFrameTime);
           }
           break;
          
          case SLIDING:
            {
              v0[2] = 0.0f;
              v0 += mSlideVelocity;

              if(mCurrentMode != FALLING && mKeyboard->GetKeyState(Producer::Key_space))
              {
                 v0[2] = mJumpSpeed;  
                 mJumped = true;
              }

              p1 = p0 + osg::Vec3(v0[0] * deltaFrameTime, v0[1] * deltaFrameTime, v0[2] * deltaFrameTime);
            }
            break;

          case WALKING:
            {
              v0[2] = 0.0f;

              if(mCurrentMode != FALLING && mKeyboard->GetKeyState(Producer::Key_space))
              {
                 v0[2] = mJumpSpeed;  
                 mJumped = true;
              }

              p1 = p0 + osg::Vec3(v0[0] * deltaFrameTime, v0[1] * deltaFrameTime, v0[2] * deltaFrameTime);            
            }
            break;
      }

      if(!TestPosition(p1))
      {
         newXYZ = p1;
         mLastVelocity = v0;
      }
      else
      {
         v1 = v0;
         for(unsigned i = 0; i < mNormals.size(); ++i)
         {
            float dot = (mNormals[i] * v1); 
            if(dot < 0.0f) v1 -= osg::Vec3(mNormals[i][0] * dot, mNormals[i][1] * dot, mNormals[i][2] * dot);
         }
         
         osg::Vec3 p2 = p0 + osg::Vec3(v1[0] * deltaFrameTime, v1[1] * deltaFrameTime, v1[2] * deltaFrameTime);

         if(!TestPosition(p2))
         {
            mLastVelocity = v1;
            newXYZ = p2;
         }
         else
         {
            mLastVelocity.set(0.0f, 0.0f, 0.0f);
            newXYZ = p0;
         }
      }

      transform.SetTranslation(newXYZ);
      if(mMouse->GetButtonState(Mouse::LeftButton)) 
         transform.SetRotation(newH, newP, 0.f);
      GetTarget()->SetTransform(transform); 

      if(mMouse->GetButtonState(Mouse::LeftButton)) 
         mMouse->SetPosition(0.0f,0.0f);//keeps cursor at center of screen
   }
}

bool CollisionMotionModel::TestPosition(osg::Vec3& newPos)
{
   UpdateBoundingVolumes(newPos);

   bool pCollided = CollideFeet();
   float relativeDepth = 0.0f;

   int normalIndex = 0;
   osg::Vec3 pCollidePos;


   float pBottomOfFeetBB = newPos[2] - (mHeightAboveTerrain + 0.25f);
   const dReal* pODE_B = dGeomGetPosition(mBBFeet);
   
   if(!pCollided || mJumped)
   {
      mCurrentMode = FALLING;
   }
   else
   {
      //set our new height
      newPos[2] = mLastFeetContact.pos[2] + mHeightAboveTerrain;

      //find the collided normal with with max z value 
      float highestZ = mNormals[normalIndex][2];
      for(unsigned i = 0; i < mNormals.size(); ++i)
      {
         if(mNormals[i][2] > highestZ)
         {
            highestZ = mNormals[i][2];
            normalIndex = i;
         }
      }

      float dotZ = highestZ;
      if(dotZ < mSlideThreshold)
      {
         mCurrentMode = SLIDING;
         //set mSlideVelocity = (mSlideSpeed / (1 - n.z) * (n.z*n.x, n.z*n.y, n.z*n.z - 1))
         float speed = mSlideSpeed / (1.0 - mNormals[normalIndex][2]);
         mSlideVelocity.set(speed * mNormals[normalIndex][2] * mNormals[normalIndex][0], speed * mNormals[normalIndex][2] * mNormals[normalIndex][1], speed * mNormals[normalIndex][2] * mNormals[normalIndex][2] - 1.0);
      }
      else
      {
         mCurrentMode = WALKING;
      }
   }

   UpdateBoundingVolumes(newPos);

   return CollideTorso();
}

bool CollisionMotionModel::CollideTorso()
{
   mNormals.clear();
   mNumTorsoContactPoints = 0;

   dSpaceCollide2((dGeomID)mScene->GetSpaceID(), (dGeomID)mSpaceID, this, NearCallbackTorso);

   return mNumTorsoContactPoints > 0;
}

bool CollisionMotionModel::CollideFeet()
{
   mNormals.clear();
   mNumFeetContactPoints = 0;
   mStartCollideFeet = true;

   dSpaceCollide2((dGeomID)mScene->GetSpaceID(), (dGeomID)mSpaceID, this, NearCallbackFeet);

   mStartCollideFeet = false;
   return mNumFeetContactPoints > 0;
}


void CollisionMotionModel::HandleCollideTorso(dGeomID pFeet, dGeomID pObject)
{
   if(pObject == GetFeetGeom()) return;

   bool set = false;
   void* data = 0;
   dGeomID pID = pObject;


   while(pID != 0 && dGeomGetClass(pID) == dGeomTransformClass && pID != 0 )
   {
      pID = dGeomTransformGetGeom(pID);
   }

   if(!pID) return;

   if(dGeomGetClass(pID) == dTriMeshClass)
   {
      set = true;
      data = dGeomGetData(pID);
      dGeomSetData(pID, this);
      dGeomTriMeshSetArrayCallback(pID, dTriArrayCallback);
   }

   dContactGeom contactGeoms[1];
   mNumTorsoContactPoints = dCollide(pFeet, pObject, 1, contactGeoms, sizeof(dContactGeom));

   if(set)
   {
      dGeomSetData(pID, data);
      dGeomTriMeshSetArrayCallback(pID, 0);
   }
}

void CollisionMotionModel::HandleCollideFeet(dGeomID pFeet, dGeomID pObject)
{
   if(pObject == GetTorsoGeom()) return;

   bool set = false;
   void* data = 0;
   dGeomID pID = pObject;

   while(pID != 0 && dGeomGetClass(pID) == dGeomTransformClass )
   {
      pID = dGeomTransformGetGeom(pID);
   }

   if(!pID) return;
   
   if(dGeomGetClass(pID) == dTriMeshClass)
   {
      set = true;
      data = dGeomGetData(pID);
      dGeomSetData(pID, this);
      dGeomTriMeshSetArrayCallback(pID, dTriArrayCallback);
   }

   dContactGeom contactGeoms[8];
   int contactPoints = dCollide(pFeet, pObject, 8, contactGeoms, sizeof(dContactGeom));
 
   //find the contact point with the highest z value
   if(contactPoints)
   {
      float highestZ;
      if(mStartCollideFeet)
      {
         highestZ = contactGeoms[0].pos[2];
         mLastFeetContact = contactGeoms[0];
         mStartCollideFeet = false;
      }
      else
      {
         highestZ = mLastFeetContact.pos[2];
      }

      for(int i = 0; i < contactPoints; ++i)
      {
         if(contactGeoms[i].pos[2] > highestZ)
         {
            highestZ = contactGeoms[i].pos[2];
            mLastFeetContact = contactGeoms[i];
         }
      }

      mNumFeetContactPoints = contactPoints;
   }


   if(set)
   {
      dGeomSetData(pID, data);
      dGeomTriMeshSetArrayCallback(pID, 0);
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


void CollisionMotionModel::UpdateBoundingVolumes(const osg::Vec3& xyz)
{
   osg::Vec3 newVec = xyz - mBBFeetOffset;
   dGeomSetPosition(mBBFeet, newVec[0], newVec[1], newVec[2]);

   osg::Vec3 torso = xyz - mBBTorsoOffset;
   dGeomSetPosition(mBBTorso, torso[0], torso[1], torso[2]);
}

void CollisionMotionModel::dTriArrayCallback(dGeomID TriMesh, dGeomID RefObject, const int* TriIndices, int TriCount)
{

   CollisionMotionModel* cmm = static_cast<CollisionMotionModel*>(dGeomGetData(TriMesh));

   for(int i = 0; i < TriCount; ++i)
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


}
