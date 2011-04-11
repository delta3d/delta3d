/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009 MOVES Institute
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
 * Michael Guerrero
 */

#include "proceduralanimationactor.h"

#include <dtAnim/animationhelper.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/posemath.h>
#include <dtAnim/posemeshdatabase.h>

#include <dtCore/transform.h>

#include <dtGame/basemessages.h>
#include <dtGame/gameactor.h>
#include <dtGame/gamemanager.h>
#include <dtGame/messagetype.h>

#include <dtUtil/mathdefines.h>

#include <cassert>

//////////////////////////////////////////////////////////
// Proxy code
//////////////////////////////////////////////////////////
ProceduralAnimationActorProxy::ProceduralAnimationActorProxy()
{
   SetClassName("ProceduralAnimationActor");
}

////////////////////////////////////////////////////////////////////////////////
ProceduralAnimationActorProxy::~ProceduralAnimationActorProxy()
{
}

////////////////////////////////////////////////////////////////////////////////
void ProceduralAnimationActorProxy::BuildInvokables()
{
   dtAnim::AnimationGameActorProxy::BuildInvokables();
}

////////////////////////////////////////////////////////////////////////////////
void ProceduralAnimationActorProxy::BuildPropertyMap()
{
   dtAnim::AnimationGameActorProxy::BuildPropertyMap();
}

////////////////////////////////////////////////////////////////////////////////
void ProceduralAnimationActorProxy::CreateActor()
{
   ProceduralAnimationActor* pActor = new ProceduralAnimationActor(*this);
   SetActor(*pActor);
}

//////////////////////////////////////////////////////////
// Actor code
//////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
ProceduralAnimationActor::ProceduralAnimationActor(ProceduralAnimationActorProxy& proxy)
   : AnimationGameActor(proxy)
   , mMode(MODE_AIM)
   , mPoseMeshDatabase(NULL)
   , mPoseMeshUtil(NULL)
   , mBlendTime(0.3f)
   , mCurrentTarget(NULL)
{
   memset(mMarinePoseData.mPoseMeshes, 0, sizeof(dtAnim::PoseMesh*) * ProceduralAnimationData::PMP_TOTAL);

   // Initialize ik target data
   for (size_t partIndex = ProceduralAnimationData::PMP_FIRST;
      partIndex < ProceduralAnimationData::PMP_TOTAL;
      ++partIndex)
   {
      mMarinePoseData.mTargetTriangles[partIndex].mIsInside = false;
      mMarinePoseData.mTargetTriangles[partIndex].mTriangleID = -1;
   }
}

////////////////////////////////////////////////////////////////////////////////
ProceduralAnimationActor::~ProceduralAnimationActor()
{
}

////////////////////////////////////////////////////////////////////////////////
void ProceduralAnimationActor::OnEnteredWorld()
{
   AnimationGameActor::OnEnteredWorld();

   // Make sure we receive the tick messages
   GetGameActorProxy().RegisterForMessages(dtGame::MessageType::TICK_LOCAL,
      dtGame::GameActorProxy::TICK_LOCAL_INVOKABLE);
}

////////////////////////////////////////////////////////////////////////////////
void ProceduralAnimationActor::SetPoseMeshDatabase(dtAnim::PoseMeshDatabase* poseMeshDatabase)
{
   // Is there any reason to set this a second time?
   assert(!mPoseMeshDatabase);

   mPoseMeshDatabase = poseMeshDatabase;

   // Make sure we have a utility to use on the posemesh data
   if (!mPoseMeshUtil.valid())
   {
      mPoseMeshUtil = new dtAnim::PoseMeshUtility;
   }

   // Get access to each individual part
   AssemblePoseData();
}

////////////////////////////////////////////////////////////////////////////////
void ProceduralAnimationActor::SetTarget(const dtCore::Transformable* target, osg::Vec3* offset)
{
   mCurrentTarget = target;

   if (offset != NULL)
   {
      mTargetOffset = *offset;
   }
}

////////////////////////////////////////////////////////////////////////////////
void ProceduralAnimationActor::SetBlendTime(float blendTime)
{
   mBlendTime = blendTime;
}

////////////////////////////////////////////////////////////////////////////////
void ProceduralAnimationActor::AssemblePoseData()
{
   mMarinePoseData.mPoseMeshes[ProceduralAnimationData::LEFT_EYE] =
      mPoseMeshDatabase->GetPoseMeshByName("Poses_LeftEye");

   mMarinePoseData.mPoseMeshes[ProceduralAnimationData::RIGHT_EYE] =
      mPoseMeshDatabase->GetPoseMeshByName("Poses_RightEye");

   mMarinePoseData.mPoseMeshes[ProceduralAnimationData::HEAD] =
      mPoseMeshDatabase->GetPoseMeshByName("Poses_Head");

   mMarinePoseData.mPoseMeshes[ProceduralAnimationData::TORSO] =
      mPoseMeshDatabase->GetPoseMeshByName("Poses_Torso");

   mMarinePoseData.mPoseMeshes[ProceduralAnimationData::GUN] =
      mPoseMeshDatabase->GetPoseMeshByName("Poses_Gun");
}

////////////////////////////////////////////////////////////////////////////////
void ProceduralAnimationActor::OnTickLocal(const dtGame::TickMessage& tickMessage)
{
   float dt = tickMessage.GetDeltaSimTime();

   // inverse kinematics
   TickIK(dt);

   // canned animation update
   GetHelper()->GetModelWrapper()->Update(dt);
}

////////////////////////////////////////////////////////////////////////////////
void ProceduralAnimationActor::TickIK(float dt)
{
   dtCore::Transform targetTransform;
   mCurrentTarget->GetTransform(targetTransform);

   // The 2 unit offset here is a crude approximation for this
   osg::Vec3 ownPosition = GetHeadPosition();

   // We might want to get a point slightly offset from the base position so add it here
   osg::Vec3 targetPosition = targetTransform.GetTranslation() + mTargetOffset;

   // This is the direction from us to the target
   osg::Vec3 lookDirection = targetPosition - ownPosition;
   lookDirection.normalize();

   osg::Vec3 actorForward = GetForwardDirection();

   if (mMode == MODE_WATCH)
   {
      assert(mCurrentTarget);

      float remAzimuth   = 0.0f;
      float remElevation = 0.0f;

      // Get the relative azimuth and elevation
      dtAnim::GetCelestialCoordinates(lookDirection, actorForward, remAzimuth, remElevation);

      // Get convenient short handles to all of our pose mesh data
      dtAnim::PoseMesh* eyeMesh1  = mMarinePoseData.mPoseMeshes[ProceduralAnimationData::LEFT_EYE];
      dtAnim::PoseMesh* eyeMesh2  = mMarinePoseData.mPoseMeshes[ProceduralAnimationData::RIGHT_EYE];
      dtAnim::PoseMesh* headMesh  = mMarinePoseData.mPoseMeshes[ProceduralAnimationData::HEAD];
      dtAnim::PoseMesh* torsoMesh = mMarinePoseData.mPoseMeshes[ProceduralAnimationData::TORSO];

      dtAnim::PoseMesh::TargetTriangle& eyeTarget1  = mMarinePoseData.mTargetTriangles[ProceduralAnimationData::LEFT_EYE];
      dtAnim::PoseMesh::TargetTriangle& eyeTarget2  = mMarinePoseData.mTargetTriangles[ProceduralAnimationData::RIGHT_EYE];
      dtAnim::PoseMesh::TargetTriangle& headTarget  = mMarinePoseData.mTargetTriangles[ProceduralAnimationData::HEAD];
      dtAnim::PoseMesh::TargetTriangle& torsoTarget = mMarinePoseData.mTargetTriangles[ProceduralAnimationData::TORSO];

      eyeMesh1->GetTargetTriangleData(remAzimuth, remElevation, eyeTarget1);
      eyeMesh2->GetTargetTriangleData(remAzimuth, remElevation, eyeTarget2);

      // The eyes have moved over, does the head need to move after this?
      remAzimuth   -= eyeTarget1.mAzimuth;
      remElevation -= eyeTarget2.mElevation;

      // The head has moved over, does the torso need to move
      headMesh->GetTargetTriangleData(remAzimuth, remElevation, headTarget);

      remAzimuth   -= headTarget.mAzimuth;
      remElevation -= headTarget.mElevation;

      torsoMesh->GetTargetTriangleData(remAzimuth, remElevation, torsoTarget);

      // Use the pose mesh util to apply the blends
      mPoseMeshUtil->BlendPoses(eyeMesh1, GetHelper()->GetModelWrapper(), eyeTarget1, mBlendTime);
      mPoseMeshUtil->BlendPoses(eyeMesh2, GetHelper()->GetModelWrapper(), eyeTarget2, mBlendTime);
      mPoseMeshUtil->BlendPoses(headMesh, GetHelper()->GetModelWrapper(), headTarget, mBlendTime);
      mPoseMeshUtil->BlendPoses(torsoMesh, GetHelper()->GetModelWrapper(), torsoTarget, mBlendTime);
   }
   else // we are aiming
   {
      float remAzimuth   = 0.0f;
      float remElevation = 0.0f;

      dtAnim::GetCelestialCoordinates(lookDirection, actorForward, remAzimuth, remElevation);

      dtAnim::PoseMesh* gunMesh = mMarinePoseData.mPoseMeshes[ProceduralAnimationData::GUN];
      dtAnim::PoseMesh::TargetTriangle& gunTarget = mMarinePoseData.mTargetTriangles[ProceduralAnimationData::GUN];

      gunMesh->GetTargetTriangleData(remAzimuth, remElevation, gunTarget);
      mPoseMeshUtil->BlendPoses(gunMesh, GetHelper()->GetModelWrapper(), gunTarget, mBlendTime);

      // Only shoot when in sight and standing still
      if (gunTarget.mIsInside)
      {
         // do something here?
      }
      else
      {
         float rotationSign = 1.0f;

         // Determine which direction to rotate toward
         if ((lookDirection ^ actorForward).z() > 0.0f)
         {
            rotationSign = -1.0f;
         }

         dtCore::Transform currentTransform;
         GetTransform(currentTransform);

         osg::Vec3 hpr;
         currentTransform.GetRotation(hpr);

         // Start rotating closer to putting the target in reach
         hpr.x() += 100.0f * dt * rotationSign;
         currentTransform.SetRotation(hpr);
         SetTransform(currentTransform);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void ProceduralAnimationActor::AddedToScene(dtCore::Scene* scene)
{
   dtAnim::AnimationGameActor::AddedToScene(scene);
}

////////////////////////////////////////////////////////////////////////////////
osg::Vec3 ProceduralAnimationActor::GetPoseMeshEndEffectorDirection(const dtAnim::PoseMesh* poseMesh) const
{
   const dtAnim::Cal3DModelWrapper* modelWrapper = GetHelper()->GetModelWrapper();

   // If we have the ik system attached
   if (poseMesh != NULL)
   {
      osg::Quat boneRotation = modelWrapper->GetBoneAbsoluteRotation(poseMesh->GetEffectorID());
      osg::Vec3 nativeBoneForward = poseMesh->GetEffectorForwardAxis();

      dtCore::Transform transform;
      GetTransform(transform);

      osg::Matrix modelRotation;
      transform.GetRotation(modelRotation);

      // Get the direction the head is facing
      osg::Vec3 boneDirection = boneRotation * nativeBoneForward;
      boneDirection = boneDirection * modelRotation;

      osg::Quat rotationCorrection(osg::DegreesToRadians(180.0f), osg::Z_AXIS);
      return rotationCorrection * boneDirection;
   }

   return GetForwardDirection();
}

////////////////////////////////////////////////////////////////////////////////
dtAnim::Cal3DModelWrapper* ProceduralAnimationActor::GetModelWrapper()
{
   return GetHelper()->GetModelWrapper();
}

////////////////////////////////////////////////////////////////////////////////
osg::Vec3 ProceduralAnimationActor::GetForwardDirection() const
{
   dtCore::Transform currentTransform;
   GetTransform(currentTransform);

   osg::Matrix matrix;
   currentTransform.Get(matrix);

   // Many characters face backwards so we negate the forward direction
   return -osg::Vec3(matrix(1, 0), matrix(1, 1), matrix(1, 2));
}

////////////////////////////////////////////////////////////////////////////////
osg::Vec3 ProceduralAnimationActor::GetGazeDirection() const
{
   dtAnim::PoseMesh* headMesh = mMarinePoseData.mPoseMeshes[ProceduralAnimationData::HEAD];

   if (headMesh)
   {
      return GetPoseMeshEndEffectorDirection(headMesh);
   }
   else
   {
      return GetForwardDirection();
   }
}

////////////////////////////////////////////////////////////////////////////////
osg::Vec3 ProceduralAnimationActor::GetGunDirection() const
{
   dtAnim::PoseMesh* gunMesh = mMarinePoseData.mPoseMeshes[ProceduralAnimationData::GUN];
   return GetPoseMeshEndEffectorDirection(gunMesh);
}

////////////////////////////////////////////////////////////////////////////////
osg::Vec3 ProceduralAnimationActor::GetGunPosition() const
{
   const dtAnim::Cal3DModelWrapper* modelWrapper = GetHelper()->GetModelWrapper();
   dtAnim::PoseMesh* gunMesh = mMarinePoseData.mPoseMeshes[ProceduralAnimationData::GUN];

   dtCore::Transform transform;
   GetTransform(transform);

   osg::Matrix rotation;
   osg::Vec3 translation;
   transform.GetRotation(rotation);
   transform.GetTranslation(translation);

   // Get the bone position relative to its root
   osg::Vec3 bonePosition = modelWrapper->GetBoneAbsoluteTranslation(gunMesh->GetEffectorID());

   // Get the gun position in the world
   osg::Quat rotationCorrection(osg::DegreesToRadians(180.0f), osg::Z_AXIS);
   bonePosition  = rotationCorrection * (bonePosition * rotation);
   bonePosition += translation;


   return bonePosition;
}

////////////////////////////////////////////////////////////////////////////////
// This could be more efficient
osg::Vec3 ProceduralAnimationActor::GetWorldPosition() const
{
   dtCore::Transform transform;
   GetTransform(transform);

   osg::Vec3 translation;
   transform.GetTranslation(translation);

   return translation;
}

////////////////////////////////////////////////////////////////////////////////
osg::Vec3 ProceduralAnimationActor::GetHeadPosition() const
{
   const dtAnim::Cal3DModelWrapper* modelWrapper = GetHelper()->GetModelWrapper();
   dtAnim::PoseMesh* headMesh = mMarinePoseData.mPoseMeshes[ProceduralAnimationData::HEAD];

   dtCore::Transform transform;
   GetTransform(transform);

   osg::Matrix rotation;
   osg::Vec3 translation;
   transform.GetRotation(rotation);
   transform.GetTranslation(translation);

   // Get the bone position relative to its root
   osg::Vec3 bonePosition = modelWrapper->GetBoneAbsoluteTranslation(headMesh->GetEffectorID());

   // Get the gun position in the world
   bonePosition  = bonePosition * rotation;
   bonePosition += translation;

   return bonePosition;
}

////////////////////////////////////////////////////////////////////////////////
