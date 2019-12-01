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

#ifndef POSE_SEQUENCE_H
#define POSE_SEQUENCE_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtCore/refptr.h>
#include <dtAnim/animationsequence.h>
#include <dtAnim/posemesh.h>
#include <dtAnim/posemeshdatabase.h>
#include <dtCore/transformable.h>
#include <map>



////////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS
////////////////////////////////////////////////////////////////////////////////
namespace dtCore
{
   class Transformable;
}

namespace dtAnim
{
   class BaseModelWrapper;
   class PoseMeshDatabase;
   class PoseMeshUtility;
}



namespace dtAnim
{
   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT PoseController : public dtAnim::AnimationSequence::AnimationController
   {
   public:
      typedef dtAnim::AnimationSequence::AnimationController BaseClass;

      static const float DEFAULT_BLEND_TIME;

      class PoseInfo : public osg::Referenced
      {
      public:
         bool mIsHead;
         int mPriority;
         dtCore::ObserverPtr<dtAnim::PoseMesh> mPoseMesh;
         dtAnim::PoseMesh::TargetTriangle mPoseTri;

         PoseInfo();
         PoseInfo(const PoseInfo& other);
         const PoseInfo& operator= (const PoseInfo& other);

         virtual void Reset();

      protected:
         virtual ~PoseInfo() {}
      };

      typedef std::list<dtCore::RefPtr<PoseInfo> > PoseInfoList;

      PoseController(AnimationSequence& parent);

      PoseController(const PoseController& other);

      const PoseController& operator=(const PoseController& other);

      virtual dtCore::RefPtr<BaseClass> Clone() const;

      void SetBlendTime(float blendTime);
      float GetBlendTime() const;

      void SetTarget(dtCore::Transformable* target);
      dtCore::Transformable* GetTarget() const;

      void SetTargetOffset(const osg::Vec3& offset);
      const osg::Vec3& GetTargetOffset() const;

      void SetPoseDrawable(dtCore::Transformable* drawable);
      dtCore::Transformable* GetPoseDrawable() const;

      void SetModelWrapper(dtAnim::BaseModelWrapper* model);
      dtAnim::BaseModelWrapper* GetModelWrapper() const;

      void SetPoseMeshDatabase(dtAnim::PoseMeshDatabase* poseDatabase);
      dtAnim::PoseMeshDatabase* GetPoseMeshDatabase() const;

      PoseMesh* GetPoseMeshByName(const std::string& poseMeshName) const;

      bool SetHeadPoseMesh(const std::string& poseMeshName);

      bool AddPoseControl(const std::string& poseMeshName, int priority, bool isHead = false);

      bool RemovePoseControl(const std::string& poseMeshName);

      const PoseInfo* GetPoseControl(const std::string& poseMeshName) const;

      int GetPoseControlCount() const;

      int ClearPoseControls();

      osg::Vec3 GetForwardDirection() const;

      /**
       * Returns the world position of the head bone.  The transform is
       * the transform to use for the world to mesh.
       */
      osg::Vec3 GetHeadPosition(const dtCore::Transform& transform) const;

      osg::Vec3 GetHeadDirection() const;
      
      osg::Vec3 GetDirection(const PoseMesh* poseMesh) const;
      osg::Vec3 GetDirection(const std::string& poseMeshName) const;

      osg::Vec3 GetPosition(const PoseMesh* poseMesh) const;
      osg::Vec3 GetPosition(const std::string& poseMeshName) const;

      virtual void Update(float timeDelta);

      void ClearAllPoses();

   protected:
      virtual ~PoseController();

      float mBlendTime;
      dtCore::RefPtr<dtAnim::PoseMeshUtility> mPoseMeshUtil;
      dtCore::ObserverPtr<dtCore::Transformable> mTarget;
      dtCore::ObserverPtr<dtCore::Transformable> mDrawable;
      dtCore::ObserverPtr<dtAnim::BaseModelWrapper> mModelWrapper;
      dtCore::RefPtr<dtAnim::PoseMeshDatabase> mPoseDatabase;
      dtCore::RefPtr<PoseInfo> mHeadPoseInfo;
      osg::Vec3 mTargetOffset;

      // for stability
      osg::Vec3 mLastHead;
      float mLastAzimuth;
      float mLastElevation;

      PoseInfoList mPoseInfoList;
   };



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT PoseSequence : public dtAnim::AnimationSequence
   {
   public:
      typedef dtAnim::AnimationSequence BaseClass;
      typedef dtCore::RefPtr<dtAnim::PoseMeshDatabase> PoseDatabase;

      PoseSequence();

      dtCore::RefPtr<dtAnim::Animatable> Clone(dtAnim::BaseModelWrapper* modelWrapper) const;

      PoseController* GetPoseController();
      const PoseController* GetPoseController() const;

      void Update(float timeDelta);

      /*override*/ void ForceFadeOut(float time);

   protected:
      virtual ~PoseSequence();

      /// No one should be calling the copy canstructor except the clone method.
      PoseSequence(const PoseSequence& other, BaseModelWrapper* wrapper);

      //not implemented
      PoseSequence& operator=(const PoseSequence&);

      dtCore::RefPtr<PoseController> mPoseController;
   };
}

#endif
