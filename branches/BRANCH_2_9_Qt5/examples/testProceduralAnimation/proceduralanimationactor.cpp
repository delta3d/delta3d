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
#include <dtAnim/posesequence.h> // for PoseController

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
   dtAnim::AnimationGameActor::BuildInvokables();
}

////////////////////////////////////////////////////////////////////////////////
void ProceduralAnimationActorProxy::BuildPropertyMap()
{
   dtAnim::AnimationGameActor::BuildPropertyMap();
}

////////////////////////////////////////////////////////////////////////////////
void ProceduralAnimationActorProxy::BuildActorComponents()
{
   dtAnim::AnimationGameActor::BuildActorComponents();
   GetComponent<dtAnim::AnimationHelper>()->SetLoadModelAsynchronously(false);
}

////////////////////////////////////////////////////////////////////////////////
void ProceduralAnimationActorProxy::CreateDrawable()
{
   ProceduralAnimationActor* pActor = new ProceduralAnimationActor(*this);
   SetDrawable(*pActor);
}

//////////////////////////////////////////////////////////
// Actor code
//////////////////////////////////////////////////////////
const dtUtil::RefString ProceduralAnimationActor::POSE_MESH_EYE_LEFT("Poses_LeftEye");
const dtUtil::RefString ProceduralAnimationActor::POSE_MESH_EYE_RIGHT("Poses_RightEye");
const dtUtil::RefString ProceduralAnimationActor::POSE_MESH_HEAD("Poses_Head");
const dtUtil::RefString ProceduralAnimationActor::POSE_MESH_TORSO("Poses_Torso");
const dtUtil::RefString ProceduralAnimationActor::POSE_MESH_GUN("Poses_Gun");


////////////////////////////////////////////////////////////////////////////////
ProceduralAnimationActor::ProceduralAnimationActor(ProceduralAnimationActorProxy& owner)
   : GameActor(owner)
   , mMode(MODE_AIM)
   , mCurrentTarget(NULL)
{}

////////////////////////////////////////////////////////////////////////////////
ProceduralAnimationActor::~ProceduralAnimationActor()
{}

////////////////////////////////////////////////////////////////////////////////
void ProceduralAnimationActor::OnEnteredWorld()
{
   dtGame::GameActor::OnEnteredWorld();

   // Make sure we receive the tick messages
   GetGameActorProxy().RegisterForMessages(dtGame::MessageType::TICK_LOCAL,
      dtGame::GameActorProxy::TICK_LOCAL_INVOKABLE);
}

////////////////////////////////////////////////////////////////////////////////
void ProceduralAnimationActor::SetTarget(dtCore::Transformable* target, osg::Vec3* offset)
{
   mCurrentTarget = target;

   if (offset != NULL)
   {
      mTargetOffset = *offset;
   }

   dtAnim::PoseController* controller = GetHelper()->GetPoseController();
   controller->SetTarget(mCurrentTarget.get());
   controller->SetTargetOffset(mTargetOffset);
}

////////////////////////////////////////////////////////////////////////////////
void ProceduralAnimationActor::SetBlendTime(float blendTime)
{
   GetHelper()->GetPoseController()->SetBlendTime(blendTime);
}

////////////////////////////////////////////////////////////////////////////////
void ProceduralAnimationActor::SetMode(eMode mode)
{
   if (mMode != mode)
   {
      mMode = mode;

      // Reset the controller so that old controls do not conflict.
      dtAnim::PoseController* controller = GetHelper()->GetPoseController();
      controller->ClearPoseControls();

      if (mMode == MODE_AIM)
      {
         // Set the head pose mesh needed in pose calculations.
         controller->SetHeadPoseMesh(POSE_MESH_HEAD);

         controller->AddPoseControl(POSE_MESH_GUN, 0);
      }
      else if (mMode == MODE_WATCH) // Default - MODE_WATCH
      {
         controller->AddPoseControl(POSE_MESH_EYE_LEFT, 0);
         controller->AddPoseControl(POSE_MESH_EYE_RIGHT, 0);
         controller->AddPoseControl(POSE_MESH_HEAD, 1, true);
         controller->AddPoseControl(POSE_MESH_TORSO, 2);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void ProceduralAnimationActor::OnTickLocal(const dtGame::TickMessage& tickMessage)
{
   float dt = tickMessage.GetDeltaSimTime();

   // inverse kinematics
   TickIK(dt);
}

////////////////////////////////////////////////////////////////////////////////
void ProceduralAnimationActor::TickIK(float dt)
{
   if (mMode == MODE_NONE)
   {
      SetMode(MODE_WATCH);
   }

   /*dtCore::Transform targetTransform;
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
      mPoseMeshUtil->BlendPoses(eyeMesh1, GetModelWrapper(), eyeTarget1, mBlendTime);
      mPoseMeshUtil->BlendPoses(eyeMesh2, GetModelWrapper(), eyeTarget2, mBlendTime);
      mPoseMeshUtil->BlendPoses(headMesh, GetModelWrapper(), headTarget, mBlendTime);
      mPoseMeshUtil->BlendPoses(torsoMesh,GetModelWrapper(), torsoTarget, mBlendTime);
   }
   else // we are aiming
   {
      float remAzimuth   = 0.0f;
      float remElevation = 0.0f;

      dtAnim::GetCelestialCoordinates(lookDirection, actorForward, remAzimuth, remElevation);

      dtAnim::PoseMesh* gunMesh = mMarinePoseData.mPoseMeshes[ProceduralAnimationData::GUN];
      dtAnim::PoseMesh::TargetTriangle& gunTarget = mMarinePoseData.mTargetTriangles[ProceduralAnimationData::GUN];

      gunMesh->GetTargetTriangleData(remAzimuth, remElevation, gunTarget);
      mPoseMeshUtil->BlendPoses(gunMesh, GetModelWrapper(), gunTarget, mBlendTime);

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
   }*/
}

////////////////////////////////////////////////////////////////////////////////
void ProceduralAnimationActor::AddedToScene(dtCore::Scene* scene)
{
   dtGame::GameActor::AddedToScene(scene);
}

////////////////////////////////////////////////////////////////////////////////
dtAnim::BaseModelWrapper* ProceduralAnimationActor::GetModelWrapper()
{
   return GetHelper()->GetModelWrapper();
}

////////////////////////////////////////////////////////////////////////////////
osg::Vec3 ProceduralAnimationActor::GetForwardDirection() const
{
   return GetHelper()->GetPoseController()->GetForwardDirection();
}

////////////////////////////////////////////////////////////////////////////////
osg::Vec3 ProceduralAnimationActor::GetGazeDirection() const
{
   return GetHelper()->GetPoseController()->GetHeadDirection();
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
osg::Vec3 ProceduralAnimationActor::GetDirection(const std::string& poseMeshName) const
{
   return GetHelper()->GetPoseController()->GetDirection(poseMeshName);
}

////////////////////////////////////////////////////////////////////////////////
osg::Vec3 ProceduralAnimationActor::GetPosition(const std::string& poseMeshName) const
{
   return GetHelper()->GetPoseController()->GetPosition(poseMeshName);
}

////////////////////////////////////////////////////////////////////////////////
dtAnim::AnimationHelper* ProceduralAnimationActor::GetHelper()
{
   return GetComponent<dtAnim::AnimationHelper>();
}

////////////////////////////////////////////////////////////////////////////////
dtAnim::AnimationHelper* ProceduralAnimationActor::GetHelper() const
{
   return GetComponent<dtAnim::AnimationHelper>();
}

////////////////////////////////////////////////////////////////////////////////
