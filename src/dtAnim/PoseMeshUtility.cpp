#include <dtAnim/PoseMeshUtility.h>
#include <dtAnim/PoseMesh.h>
#include <dtAnim/PoseMath.h>

#include <dtUtil/mathdefines.h>
#include <dtAnim/cal3dmodelwrapper.h>

#include <algorithm>

using namespace dtAnim;

// Used to sort the BaseReferencePose list
bool BaseReferencePredicate( std::pair<int, float> &lhs, std::pair<int, float> &rhs )
{
   return lhs.second < rhs.second;
}


PoseMeshUtility::PoseMeshUtility()
: mPoseList( NULL )
{
}


PoseMeshUtility::~PoseMeshUtility()
{
}


void PoseMeshUtility::ClearPoses(const PoseMesh *poseMesh, dtAnim::Cal3DModelWrapper *model, float delay)
{
   const PoseMesh::VertexVector &verts = poseMesh->GetVertices();

   for ( size_t vertIndex = 0; vertIndex < verts.size(); ++vertIndex )
   {
      model->BlendCycle( verts[vertIndex]->mAnimID, 0.0f, delay );
   }
}


void PoseMeshUtility::BlendPoses(const PoseMesh *poseMesh,
                                 dtAnim::Cal3DModelWrapper* model,
                                 PoseMesh::TargetTriangle &targetTriangle)
{
   osg::Vec3 weights;
   unsigned int animIDs[3];  

   // grab the animation ids now that we know which polygon we are using
   const PoseMesh::TriangleVector& triangles = poseMesh->GetTriangles();
   const PoseMesh::Triangle& poly = triangles[targetTriangle.mTriangleID];
   animIDs[0] = poly.mVertices[0]->mAnimID;
   animIDs[1] = poly.mVertices[1]->mAnimID;
   animIDs[2] = poly.mVertices[2]->mAnimID;

   const PoseMesh::Barycentric2DVector &barySpaceVector = poseMesh->GetBarySpaces();
   dtUtil::BarycentricSpace<osg::Vec3> *barySpace = barySpaceVector[targetTriangle.mTriangleID];

   // calculate the weights for the known animations using the corresponding barycentric space
   weights = barySpace->Transform( osg::Vec3( targetTriangle.mAzimuth, targetTriangle.mElevation, 0.0f ) );

   //now play the 3 animationIDs with the associated weights
   model->BlendCycle( animIDs[0], weights[0], 0.f );
   model->BlendCycle( animIDs[1], weights[1], 0.f );
   model->BlendCycle( animIDs[2], weights[2], 0.f );

   // turn off the animations for the rest of the celestial points
   const PoseMesh::VertexVector& vertices = poseMesh->GetVertices();
   unsigned int numVerts = vertices.size();

   for(unsigned int vertIndex = 0; vertIndex < numVerts; ++vertIndex)
   {
      unsigned int anim_id = vertices[vertIndex]->mAnimID;

      if( anim_id != animIDs[0] &&
         anim_id != animIDs[1] &&
         anim_id != animIDs[2] )
      {
         float weight= 0.f;  // only want to turn off these animations.
         float delay = 0.f;

         model->BlendCycle( anim_id, weight, delay );
      }
   }
}


void PoseMeshUtility::SetBaseReferencePoses( std::vector<BaseReferencePose> *poseList,
                                            dtAnim::Cal3DModelWrapper* model )
{
   assert( AreBaseReferencePosesValid( poseList ) );
   assert( model );

   // Store the list and make sure it is in ascending order
   mPoseList = poseList;
   std::sort( mPoseList->begin(), mPoseList->end(), BaseReferencePredicate );

   int headID  = model->GetCoreBoneID( "Bip01 Head" );    

   // Calculate the forward direction for each pose
   for ( size_t poseIndex = 0; poseIndex < mPoseList->size(); ++poseIndex )
   {
      int currentAnimID = (*mPoseList)[poseIndex].first;
      assert( currentAnimID != -1 );

      osg::Quat boneRotation  = model->GetBoneAbsoluteRotationForKeyFrame( currentAnimID, headID, 30 );
      osg::Vec3 poseDirection = boneRotation * osg::Y_AXIS;

      poseDirection.normalize();
      mPoseDirections.push_back( poseDirection );      
   }
}


bool PoseMeshUtility::GetBaseReferenceBlend( float overallAlpha, BaseReferenceBlend &finalBlend )
{
   assert( AreBaseReferencePosesValid( mPoseList ) );

   if ( mPoseList )
   {
      size_t endPoseIndex = GetEndPoseIndex( overallAlpha );

      assert( endPoseIndex != mPoseList->size() );
      assert( endPoseIndex != 0 );

      float startAlpha = (*mPoseList)[endPoseIndex - 1].second;
      float endAlpha   = (*mPoseList)[endPoseIndex].second;

      int startPoseIndex = endPoseIndex - 1;

      finalBlend.startAnimID    = (*mPoseList)[startPoseIndex].first;
      finalBlend.endAnimID      = (*mPoseList)[endPoseIndex].first;
      finalBlend.startDirection = mPoseDirections[startPoseIndex];
      finalBlend.endDirection   = mPoseDirections[endPoseIndex];
      finalBlend.blendAlpha     = 1.0f - ( overallAlpha - startAlpha ) / ( endAlpha - startAlpha );

      return true;
   }

   return false;
}


int PoseMeshUtility::GetEndPoseIndex( float alpha )
{
   size_t endPoseIndex = 0;

   // Find the bounding poses
   while ( endPoseIndex < mPoseList->size() )
   {
      if ( alpha < (*mPoseList)[endPoseIndex].second )
      {
         break;
      }

      ++endPoseIndex; 
   }

   // If we are at the end, back up one step
   if ( endPoseIndex == mPoseList->size() )
   {
      --endPoseIndex;
   }

   return endPoseIndex;
}


bool PoseMeshUtility::AreBaseReferencePosesValid( const std::vector<BaseReferencePose> *poseList )
{
   if ( poseList && poseList->size() >= 2 )
   {
      // Pose lists need to have the following:
      // a start pose ( 0.0 )
      // an end Pose ( 1.0 )
      // no duplicate time parameters ( 1 zero, 1 one, etc...)
      bool hasZero = false;
      bool hasOne  = false;

      for ( size_t poseIndex = 0; poseIndex < poseList->size(); ++poseIndex )
      {
         if ( dtUtil::Equivalent( (*poseList)[poseIndex].second, 0.0f ) )
         {
            hasZero = true;
         }
         else if ( dtUtil::Equivalent( (*poseList)[poseIndex].second, 1.0f ) )
         {
            hasOne = true;
         }

         // Check for duplicate time parameters
         for ( size_t compareIndex = poseIndex + 1; compareIndex < poseList->size(); ++compareIndex )
         {
            if ( dtUtil::Equivalent( (*poseList)[poseIndex].second, (*poseList)[compareIndex].second ) )
            {
               return false;
            }
         }
      }

      return true;
   }

   return false;  
}
