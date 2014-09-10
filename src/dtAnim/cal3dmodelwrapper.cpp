/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology
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

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/macros.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <cal3d/cal3d.h>
#include <cal3d/coretrack.h>
#include <cal3d/corekeyframe.h>

#include <cassert>
#include <algorithm>

//#include <dtUtil/stringutils.h>
//#include <dtUtil/log.h>

namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   /////////////////////////////////////////////////////////////////////////////
   const float Cal3DModelWrapper::DEFAULT_MINIMUM_BLEND_TIME = 0.1f;



   /////////////////////////////////////////////////////////////////////////////
   // CLASS VARIABLES
   /////////////////////////////////////////////////////////////////////////////
   bool Cal3DModelWrapper::sAllowBindPose = false;



   /////////////////////////////////////////////////////////////////////////////
   // CLASS METHODS
   /////////////////////////////////////////////////////////////////////////////
   void Cal3DModelWrapper::SetAllowBindPose(bool allow)
   {
      sAllowBindPose = allow;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Cal3DModelWrapper::GetAllowBindPose()
   {
      return sAllowBindPose;
   }



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   Cal3DModelWrapper::Cal3DModelWrapper(CalModel* model)
      : mScale(1.0f)
      , mMinBlendTime(DEFAULT_MINIMUM_BLEND_TIME)
      , mCalModel(model)
      , mRenderer(NULL)
      , mMixer(NULL)
      , mHardwareModel(NULL)
   {
      assert(mCalModel != NULL);

      mRenderer = mCalModel->getRenderer();
      mMixer    = mCalModel->getMixer();

      if (model)
      {
         CalCoreModel* coreModel = model->getCoreModel();

         // attach all meshes to the model
         if (coreModel)
         {
            for (int meshId = 0; meshId < coreModel->getCoreMeshCount(); ++meshId)
            {
               // If the mesh is currently loaded
               if (coreModel->getCoreMesh(meshId))
               {
                  AttachMesh(meshId);
                  ShowMesh(meshId);
               }
            }
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   Cal3DModelWrapper::~Cal3DModelWrapper()
   {
      delete mCalModel;
      delete mHardwareModel;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Cal3DModelWrapper::AttachMesh(int meshID)
   {
      bool success = mCalModel->attachMesh(meshID);
      mCalModel->setMaterialSet(0);

      return success;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Cal3DModelWrapper::DetachMesh(int meshID)
   {
      return mCalModel->detachMesh(meshID);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DModelWrapper::ShowMesh(int meshID)
   {
      if (meshID >= GetMeshCount())
      {
         return;
      }

      mMeshVisibility[meshID] = true;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DModelWrapper::HideMesh(int meshID)
   {
      if (meshID >= GetMeshCount())
      {
         return;
      }

      mMeshVisibility[meshID] = false;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Cal3DModelWrapper::IsMeshVisible(int meshID)
   {
      if (meshID >= GetMeshCount())
      {
         return false;
      }

      return mMeshVisibility[meshID];
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DModelWrapper::SetCalModel(CalModel* model)
   {
      assert(model != NULL);

      mCalModel = model;
      mRenderer = mCalModel->getRenderer();
      mMixer    = mCalModel->getMixer();
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Cal3DModelWrapper::BlendCycle(int id, float weight, float delay)
   {
      float fadeTime = delay < mMinBlendTime ? mMinBlendTime : delay;
      return mMixer->blendCycle(id, weight, fadeTime);
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Cal3DModelWrapper::BlendPose(int id, float weight, float delay)
   {
#ifdef CAL3D_VERSION_DEVELOPMENT
      return mMixer->blendPose(id, weight, delay);
#else
      return mMixer->blendCycle(id, weight, delay);
#endif
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DModelWrapper::SetSpeedFactor(int id, float speedFactor)
   {
      if ((int)(mMixer->getAnimationVector().size()) > id && mMixer->getAnimationVector()[id] != NULL)
      {
         CalAnimationCycle* cac = dynamic_cast<CalAnimationCycle*>(mMixer->getAnimationVector()[id]);
         if (cac != NULL)
         {
            if (cac->getState() != CalAnimation::STATE_ASYNC)
            {
               float duration = cac->getCoreAnimation()->getDuration() / speedFactor;
               cac->setAsync(cac->getTime(), duration);
            }
            else
            {
               cac->setTimeFactor(speedFactor);
            }
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Cal3DModelWrapper::ClearCycle(int id, float delay)
   {
      return mMixer->clearCycle(id, delay);
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Cal3DModelWrapper::ClearPose(int id, float delay)
   {
#ifdef CAL3D_VERSION_DEVELOPMENT
      return mMixer->clearPose(id, delay);
#else
      return mMixer->clearCycle(id, delay);
#endif
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DModelWrapper::ClearAll(float delay)
   {
      std::vector<CalAnimation*>& animList = mMixer->getAnimationVector();
      for (size_t animIndex = 0; animIndex < animList.size(); ++animIndex)
      {
         CalAnimation* currentAnim = animList[animIndex];
         if (currentAnim)
         {
            if (currentAnim->getType() == CalAnimation::TYPE_CYCLE)
            {
               ClearCycle(animIndex, delay);
            }
            else if (currentAnim->getType() == CalAnimation::TYPE_POSE)
            {
               ClearPose(animIndex, delay);
            }
            else if (currentAnim->getType() == CalAnimation::TYPE_ACTION)
            {
               RemoveAction(animIndex);
            }
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Cal3DModelWrapper::ExecuteAction(int id, float delayIn, float delayOut,
         float weightTgt/*=1.f*/, bool autoLock/*=false*/)
   {
      return mMixer->executeAction(id, delayIn, delayOut, weightTgt, autoLock);
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Cal3DModelWrapper::RemoveAction(int id)
   {
      return mMixer->removeAction(id);
   }

   /////////////////////////////////////////////////////////////////////////////
   CalModel* Cal3DModelWrapper::GetCalModel()
   {
      return mCalModel;
   }

   /////////////////////////////////////////////////////////////////////////////
   const CalModel* Cal3DModelWrapper::GetCalModel() const
   {
      return mCalModel;
   }

   /////////////////////////////////////////////////////////////////////////////
   int Cal3DModelWrapper::GetCoreAnimationCount() const
   {
      return mCalModel->getCoreModel()->getCoreAnimationCount();
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Quat Cal3DModelWrapper::GetCoreTrackKeyFrameQuat(unsigned int animid, unsigned int boneid, unsigned int keyframeindex) const
   {
      CalCoreTrack* cct = mCalModel->getCoreModel()->getCoreAnimation(animid)->getCoreTrack(boneid);
      assert(cct);

      if (cct)
      {
         CalCoreKeyframe* calKeyFrame = cct->getCoreKeyframe(keyframeindex);
         const CalQuaternion& calQuat = calKeyFrame->getRotation();
         return osg::Quat(calQuat.x, calQuat.y, calQuat.z, -calQuat.w);
      }

      return osg::Quat();
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Quat Cal3DModelWrapper::GetBoneAbsoluteRotationForKeyFrame(int animID, int boneID, unsigned int keyframeindex) const
   {
      osg::Quat accumulatedRotation;
      osg::Quat relativeRotation;

      do
      {
         int parentBoneID = GetParentBoneID(boneID);

         // If the animation has altered this bone rotation,
         // use it, otherwise use the normal bone rotation
         if (HasTrackForBone(animID, boneID))
         {
            // Make sure this is a valid keyframe
            unsigned int keyframecount = GetCoreAnimationKeyframeCountForTrack(animID, boneID);
            assert(keyframeindex < keyframecount);

            // Get the rotation for the bone at the desired keyframe
            relativeRotation = GetCoreTrackKeyFrameQuat(animID, boneID, keyframeindex);
         }
         else
         {
           relativeRotation = GetBoneRelativeRotation(boneID);
         }

         // Accumulate the rotation
         accumulatedRotation = accumulatedRotation * relativeRotation;

         // Move on to the next bone
         boneID = parentBoneID;

      } while (boneID != dtAnim::Cal3DModelWrapper::NULL_BONE);

      return accumulatedRotation;
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Quat Cal3DModelWrapper::GetBoneAbsoluteRotation(unsigned int boneID) const
   {
      CalBone* bone = mCalModel->getSkeleton()->getBone(boneID);
      assert(bone);

      if (bone)
      {
         const CalQuaternion& calQuat = bone->getRotationAbsolute();
         osg::Quat FixedQuat(-calQuat.x, -calQuat.y, -calQuat.z, calQuat.w);
         return FixedQuat;
      }

      return osg::Quat();
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Vec3 Cal3DModelWrapper::GetBoneAbsoluteTranslation(unsigned int boneID) const
   {
      CalBone* bone = mCalModel->getSkeleton()->getBone(boneID);
      assert(bone);

      if (bone)
      {
         const CalVector& calvec = bone->getTranslationAbsolute();
         return osg::Vec3(calvec.x, calvec.y, calvec.z);
      }

      return osg::Vec3();
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Quat Cal3DModelWrapper::GetBoneRelativeRotation(unsigned int boneID) const
   {
      CalBone* bone = mCalModel->getSkeleton()->getBone(boneID);
      assert(bone);

      if (bone)
      {
         const CalQuaternion& calQuat = bone->getRotation();
         return osg::Quat(-calQuat.x, -calQuat.y, -calQuat.z, calQuat.w);
      }

      return osg::Quat();
   }

   /////////////////////////////////////////////////////////////////////////////
   int Cal3DModelWrapper::GetCoreBoneID(const std::string& name) const
   {
      CalCoreBone* corebone = mCalModel->getCoreModel()->getCoreSkeleton()->getCoreBone(name);
      if (corebone == NULL)
      {
         return Cal3DModelWrapper::NULL_BONE;
      }

      return mCalModel->getCoreModel()->getCoreSkeleton()->getCoreBoneId(name);
   }

   ///@cond DOXYGEN_SHOULD_SKIP_THIS
   namespace details
   {
      struct ReturnBoneName
      {
         std::string operator ()(CalCoreBone* bone) const
         {
            return bone->getName();
         }
      };
   }
   ///@endcond

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DModelWrapper::GetCoreBoneNames(std::vector<std::string>& toFill) const
   {
      CalCoreSkeleton* skel = mCalModel->getCoreModel()->getCoreSkeleton();
      std::vector<CalCoreBone*> boneVec = skel->getVectorCoreBone();

      if (boneVec.empty()) { return; }

      toFill.resize(boneVec.size());
      std::transform(boneVec.begin(), boneVec.end(), toFill.begin(), details::ReturnBoneName());
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Cal3DModelWrapper::HasTrackForBone(unsigned int animID, int boneID) const
   {
      return (mCalModel->getCoreModel()->getCoreAnimation(animID)->getCoreTrack(boneID) != NULL);
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Cal3DModelWrapper::HasBone(int boneID) const
   {
      return(mCalModel->getSkeleton()->getBone(boneID) != NULL);
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Cal3DModelWrapper::HasAnimation(int animID) const
   {
      std::vector<CalAnimation*>& animationList = mCalModel->getMixer()->getAnimationVector();
      return animID < (int)animationList.size() && animationList[animID];
   }

   /////////////////////////////////////////////////////////////////////////////
   const std::string& Cal3DModelWrapper::GetCoreAnimationName(int animID) const
   {
      return mCalModel->getCoreModel()->getCoreAnimation(animID)->getName();
   }

   /////////////////////////////////////////////////////////////////////////////
   int Cal3DModelWrapper::GetCoreAnimationIDByName(const std::string& name) const
   {
      return mCalModel->getCoreModel()->getCoreAnimationId(name);
   }

   /////////////////////////////////////////////////////////////////////////////
   unsigned int Cal3DModelWrapper::GetCoreAnimationTrackCount(int animID) const
   {
      return mCalModel->getCoreModel()->getCoreAnimation(animID)->getTrackCount();
   }

   /////////////////////////////////////////////////////////////////////////////
   int Cal3DModelWrapper::GetParentBoneID(unsigned int boneID) const
   {
      //CalBone* currentBone = const_cast<CalBone*>(mCalModel->getSkeleton()->getBone(boneID));
      CalCoreBone* coreBone = const_cast<CalCoreBone*>(mCalModel->getCoreModel()->getCoreSkeleton()->getCoreBone(boneID));

      if (coreBone)
      {
         //CalCoreBone* coreBone = currentBone->getCoreBone();
         return coreBone->getParentId();
      }

      return NULL_BONE;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DModelWrapper::GetCoreBoneChildrenIDs(int parentCoreBoneID, std::vector<int>& toFill) const
   {
      std::list<int> childIdList = mCalModel->getCoreModel()->getCoreSkeleton()->getCoreBone(parentCoreBoneID)->getListChildId();

      // prefer a container that is contiguous
      toFill.clear();
      std::list<int>::iterator listEnd=childIdList.end();
      for (std::list<int>::iterator listIndex=childIdList.begin(); listIndex!=listEnd; ++listIndex)
      {
         toFill.push_back(*listIndex);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DModelWrapper::GetRootBoneIDs(std::vector<int>& toFill) const
   {
      toFill = mCalModel->getCoreModel()->getCoreSkeleton()->getVectorRootCoreBoneId();
   }

   /////////////////////////////////////////////////////////////////////////////
   unsigned int Cal3DModelWrapper::GetCoreAnimationKeyframeCount(int animID) const
   {
      return mCalModel->getCoreModel()->getCoreAnimation(animID)->getTotalNumberOfKeyframes();
   }

   /////////////////////////////////////////////////////////////////////////////
   unsigned int Cal3DModelWrapper::GetCoreAnimationKeyframeCountForTrack(int animID, int boneID) const
   {
      CalCoreAnimation* pAnimation = mCalModel->getCoreModel()->getCoreAnimation(animID);
      assert(pAnimation);

      CalCoreTrack* pTrack = pAnimation->getCoreTrack(boneID);
      assert(pTrack);

      return pTrack->getCoreKeyframeCount();
   }

   /////////////////////////////////////////////////////////////////////////////
   float Cal3DModelWrapper::GetCoreAnimationDuration(int animID) const
   {
      return mCalModel->getCoreModel()->getCoreAnimation(animID)->getDuration();
   }

   /////////////////////////////////////////////////////////////////////////////
   const std::string& Cal3DModelWrapper::GetCoreMeshName(int meshID) const
   {
      return mCalModel->getCoreModel()->getCoreMesh(meshID)->getName();
   }

   ////////////////////////////////////////////////////////////////////////////////
   int Cal3DModelWrapper::GetCoreMeshID(const std::string& name) const
   {
      return mCalModel->getCoreModel()->getCoreMeshId(name);
   }

   /////////////////////////////////////////////////////////////////////////////
   int Cal3DModelWrapper::GetCoreMaterialCount() const
   {
      return mCalModel->getCoreModel()->getCoreMaterialCount();
   }

   /////////////////////////////////////////////////////////////////////////////
   CalCoreMaterial* Cal3DModelWrapper::GetCoreMaterial(int matID)
   {
      return mCalModel->getCoreModel()->getCoreMaterial(matID);
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Vec4 Cal3DModelWrapper::GetCoreMaterialDiffuse(int matID) const
   {
      osg::Vec4 retColor;

      CalCoreMaterial* mat = mCalModel->getCoreModel()->getCoreMaterial(matID);
      if (mat != NULL)
      {
         CalCoreMaterial::Color color = mat->getDiffuseColor();
         retColor.set(color.red, color.green, color.blue, color.alpha);
      }

      return retColor;
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Vec4 Cal3DModelWrapper::GetCoreMaterialAmbient(int matID) const
   {
      osg::Vec4 retColor;

      CalCoreMaterial* mat = mCalModel->getCoreModel()->getCoreMaterial(matID);

      if (mat != NULL)
      {
         CalCoreMaterial::Color color = mat->getAmbientColor();
         retColor.set(color.red, color.green, color.blue, color.alpha);
      }

      return retColor;
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Vec4 Cal3DModelWrapper::GetCoreMaterialSpecular(int matID) const
   {
      osg::Vec4 retColor;

      CalCoreMaterial* mat = mCalModel->getCoreModel()->getCoreMaterial(matID);

      if (mat != NULL)
      {
         CalCoreMaterial::Color color = mat->getSpecularColor();
         retColor.set(color.red, color.green, color.blue, color.alpha);
      }

      return retColor;
   }

   /////////////////////////////////////////////////////////////////////////////
   float Cal3DModelWrapper::GetCoreMaterialShininess(int matID) const
   {
      CalCoreMaterial* mat = mCalModel->getCoreModel()->getCoreMaterial(matID);

      if (mat != NULL)
      {
         return mat->getShininess();
      }
      else
      {
         return 0.f;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::BoundingBox Cal3DModelWrapper::GetBoundingBox()
   {
      // Compute the bounding box which will be used for culling
      CalBoundingBox& calBBox = GetCalModel()->getBoundingBox(false);

      return osg::BoundingBox(-calBBox.plane[0].d, -calBBox.plane[2].d, -calBBox.plane[4].d,
                              calBBox.plane[1].d, calBBox.plane[3].d, calBBox.plane[5].d);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DModelWrapper::SetScale(float scale)
   {
//      // Ensure scale never goes to 0, to prevent the NAN plague.
//      if(scale <= 0.0f)
//      {
//         scale = 0.001f;
//      }
//
//      float prevScale = mScale;

      // DG - The core model scale is problematic for reusing core models.
      mScale = scale;

//      // If the previous scale was not 1...
//      if(prevScale != 1.0f)
//      {
//         // ...reverse its effect by 1/prevScale and then apply the new scale.
//         scale = 1.0f/prevScale * scale;
//      }
//
//      mCalModel->getCoreModel()->scale(scale);
   }

   /////////////////////////////////////////////////////////////////////////////
   float Cal3DModelWrapper::GetScale() const
   {
      return mScale;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DModelWrapper::SetAnimationTime(float time)
   {
      mMixer->setAnimationTime(time);
   }

   /////////////////////////////////////////////////////////////////////////////
   float Cal3DModelWrapper::GetAnimationTime()
   {
      return mMixer->getAnimationTime();
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DModelWrapper::SetMaterialSet(int materialSetID)
   {
      mCalModel->setMaterialSet(materialSetID);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DModelWrapper::SetLODLevel(float level)
   {
      mCalModel->setLodLevel(level);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DModelWrapper::Update(float deltaTime)
   {
      mCalModel->update(deltaTime);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DModelWrapper::UpdateAnimation(float deltaTime)
   {
      mCalModel->getMixer()->updateAnimation(deltaTime);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DModelWrapper::UpdateMorphTargetMixer(float deltaTime)
   {
      mCalModel->getMorphTargetMixer()->update(deltaTime);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DModelWrapper::UpdatePhysique()
   {
      mCalModel->getPhysique()->update();
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DModelWrapper::UpdateSpringSystem(float deltaTime)
   {
      mCalModel->getSpringSystem()->update(deltaTime);
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Cal3DModelWrapper::BeginRenderingQuery()
   {
      return mRenderer->beginRendering();
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DModelWrapper::EndRenderingQuery()
   {
      mRenderer->endRendering();
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Cal3DModelWrapper::SelectMeshSubmesh(int meshID, int submeshID)
   {
      return mRenderer->selectMeshSubmesh(meshID, submeshID);
   }

   /////////////////////////////////////////////////////////////////////////////
   int Cal3DModelWrapper::GetVertexCount()
   {
      return mRenderer->getVertexCount();
   }

   /////////////////////////////////////////////////////////////////////////////
   int Cal3DModelWrapper::GetFaceCount()
   {
      return mRenderer->getFaceCount();
   }

   /////////////////////////////////////////////////////////////////////////////
   int Cal3DModelWrapper::GetMapCount()
   {
      return mRenderer->getMapCount();
   }

   /////////////////////////////////////////////////////////////////////////////
   int Cal3DModelWrapper::GetMeshCount() const
   {
      return mRenderer->getMeshCount();
   }

   /////////////////////////////////////////////////////////////////////////////
   int Cal3DModelWrapper::GetSubmeshCount(int submeshID)
   {
      return mRenderer->getSubmeshCount(submeshID);
   }

   /////////////////////////////////////////////////////////////////////////////
   int Cal3DModelWrapper::GetCoreMeshCount() const
   {
      return mCalModel->getCoreModel()->getCoreMeshCount();
   }

   /////////////////////////////////////////////////////////////////////////////
   int Cal3DModelWrapper::GetFaces(CalIndex* faces)
   {
      return mRenderer->getFaces((CalIndex*)faces);
   }

   /////////////////////////////////////////////////////////////////////////////
   int Cal3DModelWrapper::GetNormals(float* normals, int stride)
   {
      return mRenderer->getNormals(normals, stride);
   }

   /////////////////////////////////////////////////////////////////////////////
   int Cal3DModelWrapper::GetTextureCoords(int mapID, float* coords, int stride)
   {
      return mRenderer->getTextureCoordinates(mapID, coords, stride);
   }

   /////////////////////////////////////////////////////////////////////////////
   int Cal3DModelWrapper::GetVertices(float* vertBuffer, int stride)
   {
      return mRenderer->getVertices(vertBuffer, stride);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DModelWrapper::GetAmbientColor(unsigned char* colorBuffer)
   {
      mRenderer->getAmbientColor(colorBuffer);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DModelWrapper::GetDiffuseColor(unsigned char* colorBuffer)
   {
      mRenderer->getDiffuseColor(colorBuffer);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DModelWrapper::GetSpecularColor(unsigned char* colorBuffer)
   {
      mRenderer->getSpecularColor(colorBuffer);
   }

   /////////////////////////////////////////////////////////////////////////////
   float Cal3DModelWrapper::GetShininess()
   {
      return mRenderer->getShininess();
   }

   /////////////////////////////////////////////////////////////////////////////
   void* Cal3DModelWrapper::GetMapUserData(int mapID)
   {
      return mRenderer->getMapUserData(mapID);
   }

   /////////////////////////////////////////////////////////////////////////////
   const std::string& Cal3DModelWrapper::GetCoreMaterialName(int matID) const
   {
      return mCalModel->getCoreModel()->getCoreMaterial(matID)->getName();
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DModelWrapper::UpdateSkeleton()
   {
      if(CanUpdate())
      {
         mCalModel->getMixer()->updateSkeleton();
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Cal3DModelWrapper::CanUpdate() const
   {
      CalMixer* mixer = mCalModel->getMixer();
      return ! mixer->getAnimationActionList().empty()
         || ! mixer->getAnimationCycle().empty()
         || GetAllowBindPose();
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DModelWrapper::SetMinimumBlendTime(float seconds)
   {
      mMinBlendTime = seconds;
   }
   
   /////////////////////////////////////////////////////////////////////////////
   float Cal3DModelWrapper::GetMinimumBlendTime() const
   {
      return mMinBlendTime;
   }

   /////////////////////////////////////////////////////////////////////////////
} // namespace dtAnim
