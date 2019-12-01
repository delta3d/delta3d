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
 */

// DELTA3D
#include <dtAnim/posemesh.h>
#include <dtAnim/posemath.h>
#include <dtAnim/posemeshxml.h>
#include <dtAnim/posemeshutility.h>
#include <dtAnim/animationinterface.h>
#include <dtAnim/basemodelwrapper.h>
#include <dtUtil/log.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/matrixutil.h>
#include <dtUtil/exception.h>
#include <dtUtil/stringutils.h>

#include <iostream>
#include <sstream>
#include <cassert>

using namespace dtAnim;



////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
////////////////////////////////////////////////////////////////////////////////
PoseMesh::TargetTriangle::TargetTriangle()
   : mIsInside(false)
   , mTriangleID(-1)
   , mAzimuth(0.0f)
   , mElevation(0.0f)
{}



////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
////////////////////////////////////////////////////////////////////////////////
PoseMesh::PoseMesh(dtAnim::BaseModelWrapper* model,
                   const PoseMeshData& meshData)
  : mName(meshData.mName)
  , mBoneName(meshData.mEffectorName)
{
   std::vector<unsigned int> animids;
   GetAnimationIDsByName(model, meshData.mAnimations, animids);

   dtAnim::AnimationUpdaterInterface* animator = model->GetAnimator();
   dtAnim::BoneInterface* rootBone = model->GetBone(meshData.mRootName);
   dtAnim::BoneInterface* effectorBone = model->GetBone(meshData.mEffectorName);
   mRootID     = rootBone->GetID();
   mEffectorID = effectorBone->GetID();

   // We need to have valid bones here in order to continue
   if (mRootID == -1 || mEffectorID == -1)
   {
      throw dtUtil::Exception("Unable to find bone '" + meshData.mEffectorName +
         "--" + meshData.mRootName + "' for pose mesh", __FILE__, __LINE__);
   }

   mBindPoseForward = meshData.mBindPoseForward;

   // Update the skeleton to initialize bone data
   animator->ClearAll(0.0f);
   animator->Update(0.0f);

   // Calculate the forward direction
   osg::Quat rootRotation = rootBone->GetAbsoluteRotation();

   mRootForward = rootRotation.inverse() * mBindPoseForward;

   osg::Quat bindRotation = effectorBone->GetAbsoluteRotation();

   mEffectorForward = bindRotation.inverse() * mBindPoseForward;

   // Store off the effector and root axis for this mesh
   //mEffectorForward = meshData.mEffectorForward;
   //mRootForward = meshData.mRootForward;

   // Allocate space for osg to triangulate our verts
   std::vector<osg::Vec3> posePoints;

   typedef std::map<unsigned int, unsigned int> AnimVertMap;
   AnimVertMap vertMap;

   typedef unsigned int AnimId;
   typedef unsigned int VertIndex;
   typedef std::vector<AnimId> UIVector;
   UIVector::const_iterator curIter = animids.begin();
   UIVector::const_iterator endIter = animids.end();

   VertIndex vertIndex = 0;
   AnimId animID = -1;
   AnimationInterface* curAnim = NULL;
   for ( ; curIter != endIter; ++curIter)
   {
      animID = *curIter;

      // If we've already handled this animation,
      // map it to the previously computed data
      AnimVertMap::iterator mapIter = vertMap.find(animID);

      curAnim = model->GetAnimationByIndex(animID);
      if (curAnim == NULL)
      {
         LOG_ERROR("Could not access pose animation [" + dtUtil::ToString(animID)
            + "] for character model \"" + model->GetModelData()->GetModelName() + "\"");
         continue;
      }

      if (mapIter != vertMap.end())
      {
         vertMap[animID] = mapIter->second;
         continue;
      }

      // This anim maps to this vert
      vertMap[animID] = vertIndex;

      animator->BlendPose(*curAnim, 1.0f, 0.0f);
      animator->Update(0.0f);

      osg::Quat finalRotation = effectorBone->GetAbsoluteRotation();

      animator->ClearPose(*curAnim, 0.0f);
      animator->Update(0.0f);

      // calculate a vector transformed by the rotation data.
      osg::Vec3 worldForward = finalRotation * mEffectorForward;
      worldForward.normalize();

      // calculate the local azimuth and elevation for the transformed vector
      float az = 0.0f;
      float el = 0.0f;

      dtAnim::GetCelestialCoordinates(worldForward, mBindPoseForward, az, el);

      std::string debugName = curAnim->GetName();

      osg::Vec3 debugDirection;
      dtAnim::GetCelestialDirection(az, el, mBindPoseForward, osg::Z_AXIS, debugDirection);

      debugDirection.normalize();
      float debugDotTranformed = debugDirection * worldForward;
      dtUtil::Clamp(debugDotTranformed, -1.0f, 1.0f);

      float precision = acosf(debugDotTranformed);

      // Store the vert for triangulation
      // - osg::PI_2
      osg::Vec3 newVertPoint(az, el, 0.0f);

      posePoints.push_back(newVertPoint);

      // add a (az,el) vertex
      PoseMesh::Vertex newVert(newVertPoint, animID);
      mVertices.push_back(newVert);

      // Store debug info
      mVertices.back().mDebugPrecision = precision;
      mVertices.back().mDebugData      = worldForward;
      mVertices.back().mDebugRotation  = finalRotation;

      ++vertIndex;
   }

   mTriangles.clear();

   typedef std::map<PoseMesh::MeshIndexPair, std::pair<int, int> > EdgeCountMap;
   EdgeCountMap edgeCounts;

   // Populate the mesh with triangles
   for (VertIndex vertIndex = 0; vertIndex < animids.size(); vertIndex += 3)
   {
      int animId0 = animids.at(vertIndex + 0);
      int animId1 = animids.at(vertIndex + 1);
      int animId2 = animids.at(vertIndex + 2);

      int vertIndex0 = vertMap[animId0];
      int vertIndex1 = vertMap[animId1];
      int vertIndex2 = vertMap[animId2];

      PoseMesh::VertexVector::value_type& vert0 = mVertices[vertIndex0];
      PoseMesh::VertexVector::value_type& vert1 = mVertices[vertIndex1];
      PoseMesh::VertexVector::value_type& vert2 = mVertices[vertIndex2];

      PoseMesh::Triangle newTri(&vert0, &vert1, &vert2, vertIndex0, vertIndex1, vertIndex2);
      mTriangles.push_back(newTri);

      // Debug
      VertIndex triIndex = vertIndex / 3;
      dtAnim::AnimationInterface* anim0 = model->GetAnimationByIndex(mVertices[vertIndex0].mAnimID);
      dtAnim::AnimationInterface* anim1 = model->GetAnimationByIndex(mVertices[vertIndex1].mAnimID);
      dtAnim::AnimationInterface* anim2 = model->GetAnimationByIndex(mVertices[vertIndex2].mAnimID);
      const std::string& animName0 = anim0->GetName();
      const std::string& animName1 = anim1->GetName();
      const std::string& animName2 = anim2->GetName();

      // Tally the number of edges so that we can determine
      // which ones are the silhouettes
      PoseMesh::MeshIndexPair pair0(vertIndex0, vertIndex1);
      PoseMesh::MeshIndexPair pair1(vertIndex1, vertIndex2);
      PoseMesh::MeshIndexPair pair2(vertIndex2, vertIndex0);

      ++edgeCounts[pair0].first;
      ++edgeCounts[pair1].first;
      ++edgeCounts[pair2].first;

      edgeCounts[pair0].second = triIndex;
      edgeCounts[pair1].second = triIndex;
      edgeCounts[pair2].second = triIndex;

      if (dtUtil::Log::GetInstance("posemesh.cpp").IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         std::ostringstream oss;
         oss << "Triangle #" << triIndex << " contains (" << vertIndex0 << ", " << vertIndex1 <<
            ", " << vertIndex2 << ")" << "  (" << animName0 << ", " << animName1 <<
            ", " << animName2 << ")" << std::endl;

         LOGN_DEBUG("posemesh.cpp", oss.str());
      }
   }

   // Find all edges that belong to a single face and store them
   for (EdgeCountMap::iterator edgeIter = edgeCounts.begin();
        edgeIter != edgeCounts.end();
        ++edgeIter)
   {
      PoseMesh::MeshIndexPair edge = edgeIter->first;
      PoseMesh::MeshIndexPair swappedEdge(edge.second, edge.first);

      int total = edgeCounts[edge].first + edgeCounts[swappedEdge].first;
      assert(total != 0);

      // If we found an edge with
      // only one face
      if (total == 1)
      {
         int triangleIndex = edgeIter->second.second;
         PoseMesh::TriangleEdge boundaryEdge(edge, triangleIndex);
         mSilhouetteEdges.push_back(boundaryEdge);

         // Mark this edge as done
         edgeIter->second.first = 69;
      }
   }

   ///\todo now also build the barycentric array
   size_t numpolygons = mTriangles.size();
   mBarySpaces.resize(numpolygons);
   for (size_t polygon=0; polygon < numpolygons; ++polygon)
   {
      const osg::Vec3& a = mTriangles[polygon].mVertices[0]->mData;
      const osg::Vec3& b = mTriangles[polygon].mVertices[1]->mData;
      const osg::Vec3& c = mTriangles[polygon].mVertices[2]->mData;
      mBarySpaces[polygon] = PoseMesh::Barycentric2D(a,b,c);
   }
}

////////////////////////////////////////////////////////////////////////////////
PoseMesh::~PoseMesh()
{
}

////////////////////////////////////////////////////////////////////////////////
PoseMesh::Vertex::Vertex(const osg::Vec3& data, unsigned int animID)
   : mData(data)
   , mAnimID(animID)
   , mDebugPrecision()

{
}

PoseMesh::Triangle::Triangle(const PoseMesh::Vertex* a,
                             const PoseMesh::Vertex* b,
                             const PoseMesh::Vertex* c,
                             unsigned short aIndex,
                             unsigned short bIndex,
                             unsigned short cIndex)
{
   mVertices[0] = a;
   mVertices[1] = b;
   mVertices[2] = c;

   mIndices[0] = aIndex;
   mIndices[1] = bIndex;
   mIndices[2] = cIndex;
}

////////////////////////////////////////////////////////////////////////////////
void PoseMesh::GetIndexPairsForTriangle(int triangleID,
                                        MeshIndexPair& pair0,
                                        MeshIndexPair& pair1,
                                        MeshIndexPair& pair2) const
{
   assert(triangleID < (int)mTriangles.size());

   // Get the pointer to the verts that make up this tri
   unsigned short index0 = mTriangles[triangleID].mIndices[0];
   unsigned short index1 = mTriangles[triangleID].mIndices[1];
   unsigned short index2 = mTriangles[triangleID].mIndices[2];

   // Make 3 pairs of points to represent the 3 edges of the tri
   pair0 = MeshIndexPair(index0, index1);
   pair1 = MeshIndexPair(index1, index2);
   pair2 = MeshIndexPair(index2, index0);
}

////////////////////////////////////////////////////////////////////////////////
void PoseMesh::GetAnimationIDsByName(const dtAnim::BaseModelWrapper* model,
                                     const std::vector<std::string>& animNames,
                                     std::vector<unsigned int>& animIDs) const
{
   // Convert the animation string into a list of ID's
   for (unsigned int animIndex = 0; animIndex < animNames.size(); ++animIndex)
   {
      dtAnim::AnimationInterface* anim = model->GetAnimation(animNames[animIndex]);
      int id = anim->GetID();

      // The mesh is invalid if we cannot find it's animations
      if (id == -1)
      {
         std::string errorMessage("Unable to find animation id by name: ");
         errorMessage.append(animNames[animIndex]);
         errorMessage.append(".  The animation needs to exist in the main .xml file.");

         throw dtUtil::Exception(errorMessage, __FILE__, __LINE__);
      }

      animIDs.push_back(id);
   }
}

////////////////////////////////////////////////////////////////////////////////
osg::Vec2 PoseMesh::GetTargetTriangleData(const float deltaAzimuth,
                                     const float deltaElevation,
                                     TargetTriangle& outTriangle) const
{
   float targetAz = outTriangle.mAzimuth + deltaAzimuth;
   float targetEl = outTriangle.mElevation + deltaElevation;
   float origAz = outTriangle.mAzimuth;
   float origEl = outTriangle.mElevation;

   int triangleID = FindPoseTriangleID(targetAz, targetEl);

   //printf ("-Posemesh target Az %f El %f\n", osg::RadiansToDegrees(targetAz), osg::RadiansToDegrees(targetEl));
   //printf ("    posemesh last Az %f El %f\n", osg::RadiansToDegrees(outTriangle.mAzimuth), osg::RadiansToDegrees(outTriangle.mElevation));

   // At this point, we know if we're in or out
   outTriangle.mIsInside = (triangleID != -1);

   // Find the closest triangle point
   if (triangleID == -1)
   {
      osg::Vec3 closestPoint;
      int closestTriangleID = 0;

      osg::Vec3 refPoint(targetAz, targetEl, 0);
      float minDistance = FLT_MAX;

      const PoseMesh::TriangleEdgeVector& silhouetteList = GetSilhouette();
      for (unsigned int edgeIndex = 0; edgeIndex < silhouetteList.size(); ++edgeIndex)
      {
         PoseMesh::MeshIndexPair edge = silhouetteList[edgeIndex].mEdge;
         const PoseMesh::VertexVector& vertices = GetVertices();

         osg::Vec3 startPoint = vertices[edge.first].mData;
         osg::Vec3 endPoint   = vertices[edge.second].mData;

         osg::Vec3 closestPointToCurrentEdge;

         dtAnim::GetClosestPointOnSegment(startPoint, endPoint, refPoint, closestPointToCurrentEdge);

         // We don't need exact distance, just a way to compare (this is faster)
         float distance = (refPoint - closestPointToCurrentEdge).length2();

         if (distance < minDistance)
         {
            minDistance       = distance;
            closestPoint      = closestPointToCurrentEdge;
            closestTriangleID = silhouetteList[edgeIndex].mTriangleID;
            //printf ("    posemesh triangle %d distance %f\n", closestTriangleID, distance);
         }
      }

      outTriangle.mTriangleID = closestTriangleID;
      outTriangle.mAzimuth    = closestPoint.x();
      outTriangle.mElevation  = closestPoint.y();
      //printf ("  posemesh triangle %d azimuth %f elevation %f\n", closestTriangleID, osg::RadiansToDegrees(closestPoint.x()), osg::RadiansToDegrees(closestPoint.y()));
      //printf ("  posemesh deltas %d azimuth %f elevation %f\n", closestTriangleID, osg::RadiansToDegrees(closestPoint.x() - origAz), osg::RadiansToDegrees(closestPoint.y() - origEl));
      return osg::Vec2(closestPoint.x() - origAz, closestPoint.y() - origEl);
   }

   outTriangle.mTriangleID = triangleID;
   outTriangle.mAzimuth    = targetAz;
   outTriangle.mElevation  = targetEl;
   //printf ("  posemesh triangle %d azimuth %f elevation %f\n", triangleID, osg::RadiansToDegrees(targetAz), osg::RadiansToDegrees(targetEl));
   return osg::Vec2(deltaAzimuth, deltaElevation);
}

////////////////////////////////////////////////////////////////////////////////
/// Algorithm in detail at http://www.blackpawn.com/texts/pointinpoly/default.html
int PoseMesh::FindPoseTriangleID(float azimuth, float elevation) const
{
   const PoseMesh::TriangleVector& triangles = GetTriangles();
   unsigned int numTris = triangles.size();

   osg::Vec3f point(azimuth, elevation, 0.0f);

   int animationIndex = TRIANGLE_NOT_FOUND;

   for (unsigned int triIndex = 0; triIndex < numTris; ++triIndex)
   {
      const osg::Vec3& A = triangles[triIndex].mVertices[0]->mData;
      const osg::Vec3& B = triangles[triIndex].mVertices[1]->mData;
      const osg::Vec3& C = triangles[triIndex].mVertices[2]->mData;

      if (!dtAnim::IsPointBetweenVectors(point, A, B, C)) { continue; }
      if (!dtAnim::IsPointBetweenVectors(point, B, A, C)) { continue; }

      animationIndex = triIndex;
      break;
   }

   return animationIndex;
}
