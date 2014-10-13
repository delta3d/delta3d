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
 *
 */

#ifndef __DELTA_BASEMODELWRAPPER_H__
#define __DELTA_BASEMODELWRAPPER_H__

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
// DELTA3D
#include <dtAnim/export.h>
#include <dtAnim/animationupdaterinterface.h>
#include <dtAnim/animationinterface.h>
#include <dtAnim/basemodeldata.h>
#include <dtAnim/boneinterface.h>
#include <dtAnim/materialinterface.h>
#include <dtAnim/meshinterface.h>
#include <dtCore/refptr.h>
#include <dtAnim/skeletoninterface.h>
// OSG
#include <osg/BoundingBox>
#include <osg/MatrixTransform>
#include <osg/Node>
#include <osg/Quat>
#include <osg/Vec3>
// STD
#include <vector>



namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT BaseModelWrapper : public osg::Referenced
   {
   public:
      BaseModelWrapper(dtAnim::BaseModelData& modelData);

      virtual dtCore::RefPtr<osg::Node> CreateDrawableNode(bool immediate) = 0;
      virtual osg::Node* GetDrawableNode() = 0;

      virtual dtAnim::BaseModelData* GetModelData() const;

      virtual dtAnim::AnimationUpdaterInterface* GetAnimator() = 0;

      virtual dtAnim::AnimationInterface* GetAnimationByIndex(int index) const = 0;
      virtual dtAnim::BoneInterface* GetBoneByIndex(int index) const = 0;

      virtual dtAnim::AnimationInterface* GetAnimation(const std::string& name) const = 0;
      virtual int GetAnimations(dtAnim::AnimationArray& outAnims) const = 0;

      virtual dtAnim::BoneInterface* GetBone(const std::string& name) const = 0;
      virtual int GetBones(dtAnim::BoneArray& outBones) const = 0;

      virtual dtAnim::MaterialInterface* GetMaterial(const std::string& name) const = 0;
      virtual int GetMaterials(MaterialArray& outMaterials) const = 0;

      virtual dtAnim::MeshInterface* GetMesh(const std::string& name) const = 0;
      virtual int GetMeshes(dtAnim::MeshArray& outMaterials) const = 0;

      virtual dtAnim::SkeletonInterface* GetSkeleton() = 0;
      virtual const dtAnim::SkeletonInterface* GetSkeleton() const = 0;

      virtual int GetAnimationCount() const = 0;
      virtual int GetBoneCount() const = 0;
      virtual int GetMaterialCount() const = 0;
      virtual int GetMeshCount() const = 0;

      virtual int GetRootBones(dtAnim::BoneArray& outBones) const = 0;
      
      virtual void SetLODLevel(float level) = 0;

      /// Get a bounding box the encompasses the character in its default pose
      virtual osg::BoundingBox GetBoundingBox() = 0;

      /**
       * Set the absolute scale relative to the underlying model's original scale.
       * Setting a scale of 1 should have the effect of resetting the original model scale.
       */
      virtual void SetScale(float scale) = 0;
      virtual float GetScale() const = 0;
      
      virtual void UpdateAnimation(float deltaTime) = 0;

      virtual void ClearAllAnimations(float delay = 0.0f) = 0;

      virtual void HandleModelResourceUpdate(dtAnim::ModelResourceType resourceType) = 0;
      
      /**
       * Method to notify this object that the associated drawable model has been modified/rebuilt.
       */
      virtual void HandleModelUpdated();

      virtual void UpdateScale();

   protected:
      virtual ~BaseModelWrapper();

      osg::MatrixTransform* GetScaleTransform() const;

   private:
      dtCore::RefPtr<dtAnim::BaseModelData> mModelData;
      dtCore::RefPtr<osg::MatrixTransform> mScaleTransform;
   };

} // namespace dtAnim

#endif // __DELTA_MODELWRAPPERINTERFACE_H__
