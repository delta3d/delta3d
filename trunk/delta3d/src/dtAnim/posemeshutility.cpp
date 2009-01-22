/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007 MOVES Institute
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
 */

#include <dtAnim/posemeshutility.h>
#include <dtAnim/posemesh.h>
#include <dtAnim/posemath.h>
#include <dtAnim/cal3dmodelwrapper.h>

#include <dtUtil/mathdefines.h>
#include <dtUtil/log.h>

#include <algorithm>

using namespace dtAnim;

// Used to sort the BaseReferencePose list
bool BaseReferencePredicate(const std::pair<int, float>& lhs, const std::pair<int, float>& rhs)
{
   return lhs.second < rhs.second;
}

////////////////////////////////////////////////////////////////////////////////
PoseMeshUtility::PoseMeshUtility()
   : mPoseList(NULL)
{
}

////////////////////////////////////////////////////////////////////////////////
PoseMeshUtility::~PoseMeshUtility()
{
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshUtility::ClearPoses(const PoseMesh* poseMesh, dtAnim::Cal3DModelWrapper* model, float delay)
{
   const PoseMesh::VertexVector& verts = poseMesh->GetVertices();

   for (size_t vertIndex = 0; vertIndex < verts.size(); ++vertIndex)
   {
      model->BlendCycle(verts[vertIndex]->mAnimID, 0.0f, delay);
   }
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshUtility::BlendPoses(const PoseMesh* poseMesh,
                                 dtAnim::Cal3DModelWrapper* model,
                                 const PoseMesh::TargetTriangle& targetTriangle,
                                 float blendDelay)
{
   osg::Vec3 weights;
   unsigned int animIDs[3];

   // grab the animation ids now that we know which polygon we are using
   const PoseMesh::TriangleVector& triangles = poseMesh->GetTriangles();
   const PoseMesh::Triangle& poly = triangles[targetTriangle.mTriangleID];
   animIDs[0] = poly.mVertices[0]->mAnimID;
   animIDs[1] = poly.mVertices[1]->mAnimID;
   animIDs[2] = poly.mVertices[2]->mAnimID;

   const PoseMesh::Barycentric2DVector& barySpaceVector = poseMesh->GetBarySpaces();
   dtUtil::BarycentricSpace<osg::Vec3>* barySpace = barySpaceVector[targetTriangle.mTriangleID];

   // calculate the weights for the known animations using the corresponding barycentric space
   weights = barySpace->Transform(osg::Vec3(targetTriangle.mAzimuth, targetTriangle.mElevation, 0.0f));

   //now play the 3 animationIDs with the associated weights
   model->BlendCycle(animIDs[0], weights[0], blendDelay);
   model->BlendCycle(animIDs[1], weights[1], blendDelay);
   model->BlendCycle(animIDs[2], weights[2], blendDelay);

   // turn off the animations for the rest of the celestial points
   const PoseMesh::VertexVector& vertices = poseMesh->GetVertices();
   unsigned int numVerts = vertices.size();

   for (unsigned int vertIndex = 0; vertIndex < numVerts; ++vertIndex)
   {
      unsigned int anim_id = vertices[vertIndex]->mAnimID;

      if (anim_id != animIDs[0] &&
          anim_id != animIDs[1] &&
          anim_id != animIDs[2])
      {
         float weight= 0.f;  // only want to turn off these animations.
         model->BlendCycle(anim_id, weight, blendDelay);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void PoseMeshUtility::SetBaseReferencePoses(std::vector<BaseReferencePose>* poseList,
                                            const dtAnim::Cal3DModelWrapper* model)
{
   assert(AreBaseReferencePosesValid(poseList));
   assert(model);

   // Store the list and make sure it is in ascending order
   mPoseList = poseList;
   std::sort(mPoseList->begin(), mPoseList->end(), BaseReferencePredicate);

   // TODO - don't make assumptions about this bone facing forward
   int headID = model->GetCoreBoneID("Bip01 Head");

   // Calculate the forward direction for each pose
   for (size_t poseIndex = 0; poseIndex < mPoseList->size(); ++poseIndex)
   {
      int currentAnimID = (*mPoseList)[poseIndex].first;
      assert(currentAnimID != -1);

      // TODO - Don't assume 30 frames!
      osg::Quat boneRotation  = model->GetBoneAbsoluteRotationForKeyFrame(currentAnimID, headID, 30);
      osg::Vec3 poseDirection = boneRotation * osg::Y_AXIS;

      poseDirection.normalize();
      mPoseDirections.push_back(poseDirection);
   }
}

////////////////////////////////////////////////////////////////////////////////
bool PoseMeshUtility::GetBaseReferenceBlend(float overallAlpha, BaseReferenceBlend& outFinalBlend)
{
   assert(AreBaseReferencePosesValid(mPoseList));

   if (mPoseList)
   {
      size_t endPoseIndex = GetEndPoseIndex(overallAlpha);

      assert(endPoseIndex != mPoseList->size());
      assert(endPoseIndex != 0);

      float startAlpha = (*mPoseList)[endPoseIndex - 1].second;
      float endAlpha   = (*mPoseList)[endPoseIndex].second;

      int startPoseIndex = endPoseIndex - 1;

      outFinalBlend.startAnimID    = (*mPoseList)[startPoseIndex].first;
      outFinalBlend.endAnimID      = (*mPoseList)[endPoseIndex].first;
      outFinalBlend.startDirection = mPoseDirections[startPoseIndex];
      outFinalBlend.endDirection   = mPoseDirections[endPoseIndex];
      outFinalBlend.blendAlpha     = 1.0f - (overallAlpha - startAlpha) / (endAlpha - startAlpha);

      return true;
   }

   LOG_ERROR("Trying to get a blend without having a pose list.");

   return false;
}

////////////////////////////////////////////////////////////////////////////////
int PoseMeshUtility::GetEndPoseIndex(float alpha)
{
   size_t endPoseIndex = 0;

   // Find the bounding poses
   while (endPoseIndex < mPoseList->size())
   {
      if (alpha < (*mPoseList)[endPoseIndex].second)
      {
         break;
      }

      ++endPoseIndex;
   }

   // If we are at the end, back up one step
   if (endPoseIndex == mPoseList->size())
   {
      --endPoseIndex;
   }

   return endPoseIndex;
}

////////////////////////////////////////////////////////////////////////////////
bool PoseMeshUtility::AreBaseReferencePosesValid(const std::vector<BaseReferencePose>* poseList)
{
   if (poseList && poseList->size() >= 2)
   {
      // Pose lists need to have the following:
      // a start pose (0.0)
      // an end Pose (1.0)
      // no duplicate time parameters (1 zero, 1 one, etc...)
      bool hasZero = false;
      bool hasOne  = false;

      for (size_t poseIndex = 0; poseIndex < poseList->size(); ++poseIndex)
      {
         if (dtUtil::Equivalent((*poseList)[poseIndex].second, 0.0f))
         {
            hasZero = true;
         }
         else if (dtUtil::Equivalent((*poseList)[poseIndex].second, 1.0f))
         {
            hasOne = true;
         }

         // Check for duplicate time parameters
         for (size_t compareIndex = poseIndex + 1; compareIndex < poseList->size(); ++compareIndex)
         {
            if (dtUtil::Equivalent((*poseList)[poseIndex].second, (*poseList)[compareIndex].second))
            {
               LOG_ERROR("Base references poses are not valid due to uplicate time parameters.");
               return false;
            }
         }
      }

      return true;
   }

   LOG_ERROR("Base references poses are not valid.");
   return false;
}
