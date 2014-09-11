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

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/posesequence.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/cal3dmodeldata.h>
#include <dtAnim/cal3ddatabase.h>
#include <dtAnim/posemath.h>
#include <dtAnim/posemeshutility.h>
#include <dtCore/transform.h>
#include <dtCore/transformable.h>
#include <dtUtil/log.h>
#include <dtUtil/mathdefines.h>
#include <cal3d/model.h>



namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   /////////////////////////////////////////////////////////////////////////////
   const float PoseController::DEFAULT_BLEND_TIME = 0.3f;



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   PoseController::PoseInfo::PoseInfo()
      : mIsHead(false)
      , mPriority(0)
      , mPoseMesh(NULL)
   {
      mPoseTri.mIsInside = false;
      mPoseTri.mTriangleID = -1;
   }

   PoseController::PoseInfo::PoseInfo(const PoseInfo& other)
   {
      *this = other;
   }

   void PoseController::PoseInfo::Reset()
   {
      mIsHead = false;
      mPriority = 0;
      mPoseMesh = NULL;
      mPoseTri.mIsInside = false;
      mPoseTri.mTriangleID = -1;
      mPoseTri.mAzimuth = 0.0f;
      mPoseTri.mElevation = 0.0f;
   }

   const PoseController::PoseInfo&
      PoseController::PoseInfo::operator= (const PoseInfo& other)
   {
      mIsHead = other.mIsHead;
      mPriority = other.mPriority;
      mPoseMesh = other.mPoseMesh;
      mPoseTri = other.mPoseTri;
      return *this;
   }



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   PoseController::PoseController(AnimationSequence& parent)
      : BaseClass(parent)
      , mBlendTime(DEFAULT_BLEND_TIME)
   {
      mPoseMeshUtil = new dtAnim::PoseMeshUtility;
   }

   PoseController::PoseController(const PoseController& other)
      : BaseClass(other)
      , mBlendTime(DEFAULT_BLEND_TIME)
   {
      *this = other;
      mPoseMeshUtil = new dtAnim::PoseMeshUtility;
   }

   const PoseController& PoseController::operator=(const PoseController& other)
   {
      mBlendTime = other.mBlendTime;
      mTarget = other.mTarget;
      mDrawable = other.mDrawable;
      mModelWrapper = other.mModelWrapper;
      mTargetOffset = other.mTargetOffset;
      mPoseDatabase = other.mPoseDatabase;

      if ( ! mPoseInfoList.empty())
      {
         mPoseInfoList.clear();
      }

      if ( ! other.mPoseInfoList.empty())
      {
         PoseInfo* curInfo = NULL;
         PoseInfoList::const_iterator curIter = other.mPoseInfoList.begin();
         PoseInfoList::const_iterator endIter = other.mPoseInfoList.end();
         for ( ; curIter != endIter; ++curIter)
         {
            curInfo = curIter->get();
            dtCore::RefPtr<PoseInfo> newInfo = new PoseInfo(*curInfo);
            mPoseInfoList.push_back(newInfo);

            if (newInfo->mIsHead)
            {
               mHeadPoseInfo = newInfo.get();
            }
         }
      }

      return *this;
   }

   PoseController::~PoseController()
   {
      ClearPoseControls();
   }

   dtCore::RefPtr<PoseController::BaseClass> PoseController::Clone() const
   {
      dtCore::RefPtr<PoseController> newController = new PoseController(*this);
      return newController.get();
   }

   void PoseController::SetBlendTime(float blendTime)
   {
      mBlendTime = blendTime;
   }

   float PoseController::GetBlendTime() const
   {
      return mBlendTime;
   }

   void PoseController::SetTarget(dtCore::Transformable* target)
   {
      mTarget = target;
   }

   dtCore::Transformable* PoseController::GetTarget() const
   {
      return mTarget.get();
   }

   void PoseController::SetTargetOffset(const osg::Vec3& offset)
   {
      mTargetOffset = offset;
   }

   const osg::Vec3& PoseController::GetTargetOffset() const
   {
      return mTargetOffset;
   }

   void PoseController::SetPoseDrawable(dtCore::Transformable* drawable)
   {
      mDrawable = drawable;
   }

   dtCore::Transformable* PoseController::GetPoseDrawable() const
   {
      return mDrawable.get();
   }

   void PoseController::SetModelWrapper(dtAnim::Cal3DModelWrapper* model)
   {
      mModelWrapper = model;
   }

   dtAnim::Cal3DModelWrapper* PoseController::GetModelWrapper() const
   {
      return mModelWrapper.get();
   }

   void PoseController::SetPoseMeshDatabase(dtAnim::PoseMeshDatabase* poseDatabase)
   {
      mPoseDatabase = poseDatabase;
   }

   dtAnim::PoseMeshDatabase* PoseController::GetPoseMeshDatabase() const
   {
      return mPoseDatabase.get();
   }

   struct ComparePoseInfoPred
   {
      bool operator() (PoseController::PoseInfo* infoA, PoseController::PoseInfo* infoB) const
      {
         return infoB->mPriority < infoB->mPriority;
      }
   };

   PoseMesh* PoseController::GetPoseMeshByName(const std::string& poseMeshName) const
   {
      if ( ! mPoseDatabase.valid())
      {
         LOG_ERROR("PoseMeshDatabase is not accessible.");
         return NULL;
      }

      dtAnim::PoseMesh* poseMesh = mPoseDatabase->GetPoseMeshByName(poseMeshName);
      if (poseMesh == NULL)
      {
         std::string modelName;
         if (mModelWrapper.valid())
         {
            modelName = mModelWrapper->GetCalModel()->getCoreModel()->getName();
         }
         LOG_WARNING("Cannot find pose \"" + poseMeshName + "\" for model \"" + modelName + "\"");
      }

      return poseMesh;
   }

   bool PoseController::SetHeadPoseMesh(const std::string& poseMeshName)
   {
      PoseMesh* poseMesh = NULL;
      bool changePoseMesh = true;

      if (mHeadPoseInfo.valid() && mHeadPoseInfo->mPoseMesh.valid())
      {
         if (mHeadPoseInfo->mPoseMesh->GetName() == poseMeshName)
         {
            changePoseMesh = false;

            poseMesh = mHeadPoseInfo->mPoseMesh.get();
         }
         else // No name match
         {
            mHeadPoseInfo->Reset();
         }
      }
      else // The info object is not valid.
      {
         mHeadPoseInfo = new PoseInfo;
      }

      // Change to the pose mesh specified by name.
      if (changePoseMesh)
      {
         if (poseMeshName.empty())
         {
            poseMesh = NULL;
         }
         else
         {
            poseMesh = mPoseDatabase->GetPoseMeshByName(poseMeshName);
         }
         mHeadPoseInfo->mPoseMesh = poseMesh;
         mHeadPoseInfo->mIsHead = true;
      }

      return poseMesh != NULL;
   }

   bool PoseController::AddPoseControl(const std::string& poseMeshName, int priority, bool isHead)
   {
      bool success = false;

      dtAnim::PoseMesh* poseMesh = GetPoseMeshByName(poseMeshName);
      if (poseMesh != NULL)
      {
         dtCore::RefPtr<PoseInfo> info = new PoseInfo;
         info->mIsHead = isHead;
         info->mPriority = priority;
         info->mPoseMesh = poseMesh;

         if (isHead)
         {
            mHeadPoseInfo = info.get();
         }

         mPoseInfoList.push_back(info.get());

         ComparePoseInfoPred pred;
         mPoseInfoList.sort(pred);

         success = true;
      }

      return success;
   }

   struct MatchPoseMeshNamePred
   {
      MatchPoseMeshNamePred(const std::string& poseMeshName)
         : mPoseMeshName(poseMeshName)
      {}

      bool operator() (PoseController::PoseInfo* info)
      {
         return info != NULL && info->mPoseMesh.valid()
            && info->mPoseMesh->GetName() == mPoseMeshName;
      }

      std::string mPoseMeshName;
   };

   const PoseController::PoseInfo* PoseController::GetPoseControl(const std::string& poseMeshName) const
   {
      PoseInfo* info = NULL;

      if ( ! mPoseInfoList.empty())
      {
         MatchPoseMeshNamePred pred(poseMeshName);
         PoseInfoList::const_iterator foundIter
            = std::find_if(mPoseInfoList.begin(), mPoseInfoList.end(), pred);

         if (foundIter != mPoseInfoList.end())
         {
            info = foundIter->get();
         }
      }

      return info;
   }

   int PoseController::GetPoseControlCount() const
   {
      return (int)mPoseInfoList.size();
   }

   bool PoseController::RemovePoseControl(const std::string& poseMeshName)
   {
      bool success = false;

      if ( ! mPoseInfoList.empty())
      {
         size_t prevSize = mPoseInfoList.size();

         MatchPoseMeshNamePred pred(poseMeshName);
         std::remove_if(mPoseInfoList.begin(), mPoseInfoList.end(), pred);

         // Determine if the list has decreased in size.
         success = prevSize > mPoseInfoList.size();
      }

      return success;
   }

   int PoseController::ClearPoseControls()
   {
      int count = 0;

      if ( ! mPoseInfoList.empty())
      {
         count = (int)mPoseInfoList.size();
         mPoseInfoList.clear();

         mHeadPoseInfo = NULL;
      }

      return count;
   }
   
   osg::Vec3 PoseController::GetForwardDirection() const
   {
      osg::Vec3 direction;

      if (mDrawable.valid())
      {
         dtCore::Transform currentTransform;
         mDrawable->GetTransform(currentTransform);

         osg::Matrix matrix;
         currentTransform.Get(matrix);

         // Many characters face backwards so we negate the forward direction
         direction = -osg::Vec3(matrix(1, 0), matrix(1, 1), matrix(1, 2));
      }

      return direction;
   }

   osg::Vec3 PoseController::GetHeadPosition() const
   {
      osg::Vec3 bonePosition;

      if (mModelWrapper.valid() && mHeadPoseInfo.valid() && mDrawable.valid())
      {
         dtAnim::PoseMesh* headMesh = mHeadPoseInfo->mPoseMesh.get();

         dtCore::Transform transform;
         mDrawable->GetTransform(transform);

         osg::Matrix rotation;
         osg::Vec3 translation;
         transform.GetRotation(rotation);
         transform.GetTranslation(translation);

         // Get the bone position relative to its root
         bonePosition = mModelWrapper->GetBoneAbsoluteTranslation(headMesh->GetEffectorID());

         // Get the gun position in the world
         bonePosition  = bonePosition * rotation;
         bonePosition += translation;
      }

      return bonePosition;
   }

   osg::Vec3 PoseController::GetHeadDirection() const
   {
      if (mHeadPoseInfo.valid() && mHeadPoseInfo->mPoseMesh.valid())
      {
         return GetDirection(mHeadPoseInfo->mPoseMesh.get());
      }
      else
      {
         return GetForwardDirection();
      }
   }
   
   osg::Vec3 PoseController::GetDirection(const PoseMesh* poseMesh) const
   {
      osg::Vec3 direction;

      // If we have the ik system attached
      if (poseMesh != NULL && mModelWrapper.valid() && mDrawable.valid())
      {
         osg::Quat boneRotation = mModelWrapper->GetBoneAbsoluteRotation(poseMesh->GetEffectorID());
         osg::Vec3 nativeBoneForward = poseMesh->GetEffectorForwardAxis();

         dtCore::Transform transform;
         mDrawable->GetTransform(transform);

         osg::Matrix modelRotation;
         transform.GetRotation(modelRotation);

         // Get the direction the head is facing
         osg::Vec3 boneDirection = boneRotation * nativeBoneForward;
         boneDirection = boneDirection * modelRotation;

         osg::Quat rotationCorrection(osg::DegreesToRadians(180.0f), osg::Z_AXIS);
         direction = rotationCorrection * boneDirection;
      }
      else
      {
         direction = GetForwardDirection();
      }

      return direction;
   }

   osg::Vec3 PoseController::GetDirection(const std::string& poseMeshName) const
   {
      osg::Vec3 result;

      const PoseInfo* poseInfo = GetPoseControl(poseMeshName);
      if (poseInfo != NULL)
      {
         result = GetDirection(poseInfo->mPoseMesh.get());
      }

      return result;
   }

   osg::Vec3 PoseController::GetPosition(const PoseMesh* poseMesh) const
   {
      osg::Vec3 bonePosition;

      if (poseMesh != NULL && mModelWrapper.valid() && mDrawable.valid())
      {
         dtCore::Transform transform;
         mDrawable->GetTransform(transform);

         osg::Matrix rotation;
         osg::Vec3 translation;
         transform.GetRotation(rotation);
         transform.GetTranslation(translation);

         // Get the bone position relative to its root
         bonePosition = mModelWrapper->GetBoneAbsoluteTranslation(poseMesh->GetEffectorID());

         // Get the gun position in the world
         bonePosition  = bonePosition * rotation;
         bonePosition += translation;
      }

      return bonePosition;
   }

   osg::Vec3 PoseController::GetPosition(const std::string& poseMeshName) const
   {
      osg::Vec3 result;

      const PoseInfo* poseInfo = GetPoseControl(poseMeshName);
      if (poseInfo != NULL)
      {
         result = GetPosition(poseInfo->mPoseMesh.get());
      }

      return result;
   }

   void PoseController::Update(float timeDelta)
   {
      // Avoid the base functionality because it will prune pose animations.
      //BaseClass::Update(timeDelta);

      if ( ! mTarget.valid() || ! mModelWrapper.valid())
      {
         return;
      }

      dtCore::Transform targetTransform;
      mTarget->GetTransform(targetTransform);

      // The 2 unit offset here is a crude approximation for this
      osg::Vec3 ownPosition = GetHeadPosition();

      // We might want to get a point slightly offset from the base position so add it here
      osg::Vec3 targetPosition = targetTransform.GetTranslation() + mTargetOffset;

      // This is the direction from us to the target
      osg::Vec3 lookDirection = targetPosition - ownPosition;
      lookDirection.normalize();

      osg::Vec3 actorForward = GetForwardDirection();

      //if (mMode == MODE_WATCH)
      {
         float remainingAzimuth   = 0.0f;
         float remainingElevation = 0.0f;

         // Get the relative azimuth and elevation
         dtAnim::GetCelestialCoordinates(lookDirection, actorForward, remainingAzimuth, remainingElevation);

         PoseInfo* curInfo = NULL;
         int lastPriority = -1;
         int curPriority = 0;
         dtAnim::PoseMesh* curPoseMesh = NULL;
         dtAnim::PoseMesh::TargetTriangle* curPoseTri = NULL;
         PoseInfoList::iterator curIter = mPoseInfoList.begin();
         PoseInfoList::iterator endIter = mPoseInfoList.end();
         for ( ; curIter != endIter; )
         {
            curInfo = curIter->get();
            curPoseMesh = curInfo->mPoseMesh.get();
            curPoseTri = &curInfo->mPoseTri;
            curPriority = curInfo->mPriority;

            if (curPoseMesh == NULL)
            {
               // Ensure this entry is deleted since the pose mesh is no longer available.
               PoseInfoList::iterator eraseIter = curIter;
               ++curIter;
               mPoseInfoList.erase(eraseIter);

               // Ensure the end iterator is proper, then continue.
               endIter = mPoseInfoList.end();
               continue;
            }
            else
            {
               curPoseMesh->GetTargetTriangleData(remainingAzimuth, remainingElevation, *curPoseTri);

               if (lastPriority < curPriority)
               {
                  remainingAzimuth   -= curPoseTri->mAzimuth;
                  remainingElevation -= curPoseTri->mElevation;
                  lastPriority = curPriority;
               }

               mPoseMeshUtil->BlendPoses(curPoseMesh, mModelWrapper.get(), *curPoseTri, mBlendTime);
            }
            
            // Incrementing loop here in case an invalid entry
            // was removed and current iterator was updated.
            ++curIter;
         }
      }
      /*else // Aim - over time
      {
         if ( ! curPoseTri.mIsInside)
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



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   PoseSequence::PoseSequence()
      : BaseClass()
   {
      mPoseController = new PoseController(*this);
      SetController(mPoseController);
   }

   /////////////////////////////////////////////////////////////////////////////
   /// No one should be calling the copy canstructor except the clone method.
   PoseSequence::PoseSequence(const PoseSequence& other, Cal3DModelWrapper* wrapper)
      : BaseClass(other, wrapper)
   {
      // NOTE: Base class will clone controller and animations.

      mPoseController = dynamic_cast<PoseController*>(GetController());
   }

   /////////////////////////////////////////////////////////////////////////////
   PoseSequence::~PoseSequence()
   {}

   /////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtAnim::Animatable> PoseSequence::Clone(dtAnim::Cal3DModelWrapper* modelWrapper) const
   {
      return new PoseSequence(*this, modelWrapper);
   }

   /////////////////////////////////////////////////////////////////////////////
   PoseController* PoseSequence::GetPoseController()
   {
      return mPoseController.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   const PoseController* PoseSequence::GetPoseController() const
   {
      return mPoseController.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   void PoseSequence::Update(float timeDelta)
   {
      // NOTE: Intentionally replacing base class version of Update
      // to prevent this sequence from being pruned.

      SetActive(true);

      PoseController* controller = GetPoseController();
      if (controller != NULL)
      {
         controller->Update(timeDelta);
      }
   }

} // END - namespace dtAnim
