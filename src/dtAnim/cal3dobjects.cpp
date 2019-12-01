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

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
// DELTA3D
#include <dtAnim/cal3dobjects.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/cal3danimator.h>
// CAL3D
#include <cal3d/animation_action.h>
#include <cal3d/animation_cycle.h>
#include <cal3d/corekeyframe.h>
#include <cal3d/coretrack.h>
#include <cal3d/mixer.h>
#include <cal3d/submesh.h>



namespace dtAnim
{
   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   Cal3dBone::Cal3dBone(Cal3DModelWrapper& model, CalBone& bone)
      : mModel(&model)
      , mBone(&bone)
      , mID(-1)
   {
      mID = mModel->GetCalModel()->getCoreModel()->getCoreSkeleton()->getCoreBoneId(
         mBone->getCoreBone()->getName());
   }
   
   Cal3dBone::~Cal3dBone()
   {}

   int Cal3dBone::GetID() const
   {
      return mID;
   }

   const std::string& Cal3dBone::GetName() const
   {
      return mBone->getCoreBone()->getName();
   }

   BoneInterface* Cal3dBone::GetParentBone() const
   {
      int parentID = mBone->getCoreBone()->getParentId();
      return mModel->GetBoneByIndex(parentID);
   }

   int Cal3dBone::GetChildBones(dtAnim::BoneArray& outBones) const
   {
      int count = 0;

      std::list<int> childIdList = mBone->getCoreBone()->getListChildId();

      dtAnim::BoneInterface* curBone = NULL;
      std::list<int>::iterator listEnd=childIdList.end();
      for (std::list<int>::iterator listIndex=childIdList.begin(); listIndex!=listEnd; ++listIndex)
      {
         curBone = mModel->GetBoneByIndex(*listIndex);

         if (curBone != NULL)
         {
            outBones.push_back(curBone);
            ++count;
         }
      }

      return count;
   }
   
   void Cal3dBone::GetAbsoluteMatrix(osg::Matrix& outMatrix) const
   {
      outMatrix.setRotate(GetAbsoluteRotation());
      outMatrix.setTrans(GetAbsoluteTranslation());
   }

   osg::Quat Cal3dBone::GetAbsoluteRotation() const
   {
      const CalQuaternion& calQuat = mBone->getRotationAbsolute();
      osg::Quat FixedQuat(-calQuat.x, -calQuat.y, -calQuat.z, calQuat.w);
      return FixedQuat;
   }

   osg::Vec3 Cal3dBone::GetAbsoluteTranslation() const
   {
      const CalVector& calvec = mBone->getTranslationAbsolute();
      return osg::Vec3(calvec.x, calvec.y, calvec.z);
   }

   osg::Quat Cal3dBone::GetRelativeRotation() const
   {
      const CalQuaternion& calQuat = mBone->getRotation();
      return osg::Quat(-calQuat.x, -calQuat.y, -calQuat.z, calQuat.w);
   }

   osg::Quat Cal3dBone::GetAbsoluteRotationForKeyframe(const dtAnim::AnimationInterface& anim, dtAnim::Keyframe keyframe) const
   {
      osg::Quat accumulatedRotation;
      osg::Quat relativeRotation;

      const dtAnim::BoneInterface* curBone = this;
      do
      {
         const dtAnim::BoneInterface* parentBone = curBone->GetParentBone();

         // If the animation has altered this bone rotation,
         // use it, otherwise use the normal bone rotation
         if (anim.HasTrackForBone(*curBone))
         {
            // Make sure this is a valid keyframe
            dtAnim::Keyframe keyCount = anim.GetKeyframeCountForBone(*curBone);
            
            if (keyframe >= keyCount)
            {
               keyframe = keyCount - 1;
            }

            // Get the rotation for the bone at the desired keyframe
            relativeRotation = anim.GetKeyframeQuat(*curBone, keyframe);
         }
         else
         {
           relativeRotation = curBone->GetRelativeRotation();
         }

         // Accumulate the rotation
         accumulatedRotation = accumulatedRotation * relativeRotation;

         // Move on to the next bone
         curBone = parentBone;

      } while (curBone != NULL);

      return accumulatedRotation;
   }

   CalBone* Cal3dBone::GetCalBone()
   {
      return mBone;
   }

   const CalBone* Cal3dBone::GetCalBone() const
   {
      return mBone;
   }


   
   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   Cal3dSkeleton::Cal3dSkeleton(Cal3DModelWrapper& model, CalSkeleton& skeleton)
      : mModel(&model)
      , mSkeleton(&skeleton)
   {}

   Cal3dSkeleton::~Cal3dSkeleton()
   {}

   dtCore::RefPtr<dtAnim::BoneInterface> Cal3dSkeleton::GetBone(const std::string& name)
   {
      return mModel->GetBone(name);
   }
   
   int Cal3dSkeleton::GetBones(dtAnim::BoneArray& outBones)
   {
      return mModel->GetBones(outBones);
   }

   int Cal3dSkeleton::GetBoneCount() const
   {
      return int(mSkeleton->getVectorBone().size());
   }

   int Cal3dSkeleton::GetRootBones(dtAnim::BoneArray& outBones)
   {
      return mModel->GetRootBones(outBones);
   }

   CalSkeleton* Cal3dSkeleton::GetCalSkeleton()
   {
      return mModel->GetCalModel()->getSkeleton();
   }

   const CalSkeleton* Cal3dSkeleton::GetCalSkeleton() const
   {
      return mModel->GetCalModel()->getSkeleton();
   }

   CalCoreSkeleton* Cal3dSkeleton::GetCalCoreSkeleton()
   {
      return mModel->GetCalModel()->getSkeleton()->getCoreSkeleton();
   }

   const CalCoreSkeleton* Cal3dSkeleton::GetCalCoreSkeleton() const
   {
      return mModel->GetCalModel()->getSkeleton()->getCoreSkeleton();
   }


   
   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   Cal3dMaterial::Cal3dMaterial(Cal3DModelWrapper& model, CalCoreMaterial& material)
      : mModel(&model)
      , mMaterial(&material)
      , mID(-1)
   {
      CalCoreModel* coreModel = mModel->GetCalModel()->getCoreModel();
      const std::string& name = mMaterial->getName();
      int numMaterials = coreModel->getCoreMaterialCount();
      for (int i = 0; i < numMaterials; ++i)
      {
         if (coreModel->getCoreMaterial(i)->getName() == name)
         {
            mID = i;
            break;
         }
      }
   }

   Cal3dMaterial::~Cal3dMaterial()
   {}

   int Cal3dMaterial::GetID() const
   {
      return mID;
   }

   void Cal3dMaterial::SetName(const std::string& name)
   {
      mMaterial->setName(name);
   }

   const std::string& Cal3dMaterial::GetName() const
   {
      return mMaterial->getName();
   }

   int Cal3dMaterial::GetTextureCount() const
   {
      return mMaterial->getMapCount();
   }

   void Cal3dMaterial::SetTexture(int index, osg::Texture* textureMap)
   {
      bool added = false;
      if (index >= mMaterial->getMapCount())
      {
         mMaterial->reserve(index + 1);
         added = true;
      }

      mMaterial->setMapUserData(index, (Cal::UserData)(textureMap));

      if (added)
      {
         // make one material thread for each material
         // NOTE: this is not the right way to do it, but this viewer can't do the right
         // mapping without further information on the model etc., so this is the only
         // thing we can do here.

         // Every part of the core model (every submesh to be more exact) has a material
         // thread assigned. You can now very easily change the look of a model instance,
         // by simply select a new current material set for its parts. The Cal3D library
         // is now able to look up the material in the material grid with the given new
         // material set and the material thread stored in the core model parts.

         // create the a material thread
//         coreModel.createCoreMaterialThread(materialId);

         // initialize the material thread
//         coreModel.setCoreMaterialId(materialId, 0, materialId);
      }
   }
   
   osg::Texture* Cal3dMaterial::GetTexture(int index) const
   {
      return reinterpret_cast<osg::Texture*>(mModel->GetCalRenderer()->getMapUserData(index));
   }

   const std::string Cal3dMaterial::GetTextureFile(int index) const
   {
      return index < mMaterial->getMapCount() ? mMaterial->getMapFilename(index) : "";
   }

   osg::Vec4 Cal3dMaterial::GetAmbientColor() const
   {
      osg::Vec4 color;

      const CalCoreMaterial::Color& calColor = mMaterial->getAmbientColor();
      color.set(calColor.red, calColor.green, calColor.blue, calColor.alpha);
      color /= 255.0f;

      return color;
   }

   osg::Vec4 Cal3dMaterial::GetDiffuseColor() const
   {
      osg::Vec4 color;

      const CalCoreMaterial::Color& calColor = mMaterial->getDiffuseColor();
      color.set(calColor.red, calColor.green, calColor.blue, calColor.alpha);
      color /= 255.0f;

      return color;
   }

   osg::Vec4 Cal3dMaterial::GetSpecularColor() const
   {
      osg::Vec4 color;

      const CalCoreMaterial::Color& calColor = mMaterial->getSpecularColor();
      color.set(calColor.red, calColor.green, calColor.blue, calColor.alpha);
      color /= 255.0f;

      return color;
   }

   float Cal3dMaterial::GetShininess() const
   {
      return mMaterial->getShininess();
   }

   CalCoreMaterial* Cal3dMaterial::GetCalCoreMaterial()
   {
      return mMaterial.get();
   }

   const CalCoreMaterial* Cal3dMaterial::GetCalCoreMaterial() const
   {
      return mMaterial.get();
   }


   
   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   Cal3dSubmesh::Cal3dSubmesh(Cal3DModelWrapper& model, CalSubmesh& submesh)
      : mModel(&model)
      , mSubmesh(&submesh)
   {}

   Cal3dSubmesh::~Cal3dSubmesh()
   {}

   int Cal3dSubmesh::GetVertexCount() const
   {
      return mModel.valid() ? mSubmesh->getVertexCount() : 0;
   }

   int Cal3dSubmesh::GetFaceCount() const
   {
      return mModel.valid() ? mSubmesh->getFaceCount() : 0;
   }

   int Cal3dSubmesh::GetMorphTargetWeightCount() const
   {
      return mModel.valid() ? mSubmesh->getMorphTargetWeightCount() : 0;
   }

   int Cal3dSubmesh::GetFaceData(dtAnim::IndexArray& outData)
   {
      int count = GetFaceCount();
      
      if (count > 0)
      {
         int limit = count * 3;
         outData.reserve(outData.size() + limit);

         int* buffer = new int[limit];
         mSubmesh->getFaces(buffer);

         outData.insert(outData.end(), buffer, buffer + limit);

         delete [] buffer;
      }

      return count;
   }

   int Cal3dSubmesh::GetVertexData(dtAnim::FloatArray& outData, int stride)
   {
      int vertexCount = mSubmesh->getVertexCount();

      if (vertexCount > 0)
      {
         size_t bufferLength = vertexCount * sizeof(CalVector);
         float* buffer = new float[bufferLength];
      
         GetVertices(buffer, stride);
         outData.insert(outData.end(), buffer, buffer + bufferLength);

         delete buffer;
      }

      return vertexCount;
   }

   int Cal3dSubmesh::GetNormalData(dtAnim::FloatArray& outData, int stride)
   {
      int vertexCount = mSubmesh->getVertexCount();

      if (vertexCount > 0)
      {
         size_t bufferLength = vertexCount * sizeof(CalVector);
         float* buffer = new float[bufferLength];
      
         GetNormals(buffer, stride);
         outData.insert(outData.end(), buffer, buffer + bufferLength);

         delete buffer;
      }

      return vertexCount;
   }

   int Cal3dSubmesh::GetMorphTargetWeightData(dtAnim::FloatArray& outData)
   {
      dtAnim::FloatArray& weights = mSubmesh->getVectorMorphTargetWeight();
      int count = int(weights.size());

      outData.reserve(outData.size() + count);
      outData.insert(outData.end(), weights.begin(), weights.end());

      return count;
   }

   int Cal3dSubmesh::GetMorphTargets(dtAnim::MorphTargetArray& outSubmorphTargets) const
   {
      int count = 0;

      typedef std::vector<CalCoreSubMorphTarget *> MorphTargetArray;
      MorphTargetArray& targets = mSubmesh->getCoreSubmesh()->getVectorCoreSubMorphTarget();

      dtAnim::Cal3dSubmesh* thisSubmesh = const_cast<dtAnim::Cal3dSubmesh*>(this);
      CalCoreSubMorphTarget* curTarget = NULL;
      MorphTargetArray::iterator curIter = targets.begin();
      MorphTargetArray::iterator endIter = targets.end();
      for (int id = 0; curIter != endIter; ++curIter, ++id)
      {
         curTarget = *curIter;
         outSubmorphTargets.push_back(new dtAnim::Cal3dMorphTarget(*thisSubmesh, *curTarget, id));
         ++count;
      }

      return count;
   }

   int Cal3dSubmesh::GetFaces(int* outData)
   {
      return mModel->GetCalRenderer()->getFaces(outData);
   }

   int Cal3dSubmesh::GetVertices(float* outData, int stride)
   {
      return mModel->GetCalRenderer()->getVertices(outData, stride);
   }

   int Cal3dSubmesh::GetNormals(float* outData, int stride)
   {
      return mModel->GetCalRenderer()->getNormals(outData, stride);
   }

   int Cal3dSubmesh::GetTextureCoords(int textureUnit, float* outData, int stride)
   {
      return mModel->GetCalRenderer()->getTextureCoordinates(textureUnit, outData, stride);
   }
   
   void Cal3dSubmesh::SetDrawable(dtAnim::SubmeshDrawable* drawable)
   {
      mDrawable = drawable;
   }

   dtAnim::SubmeshDrawable* Cal3dSubmesh::GetDrawable()
   {
      return mDrawable.get();
   }

   const dtAnim::SubmeshDrawable* Cal3dSubmesh::GetDrawable() const
   {
      return mDrawable.get();
   }

   CalSubmesh* Cal3dSubmesh::GetCalSubmesh()
   {
      return mSubmesh;
   }

   const CalSubmesh* Cal3dSubmesh::GetCalSubmesh() const
   {
      return mSubmesh;
   }



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   Cal3dHardwareSubmesh::Cal3dHardwareSubmesh(CalHardwareModel& model, int index)
      : mModel(&model)
      , mID(index)
      , mSubmeshArray(&model.getVectorHardwareMesh())
   {}

   Cal3dHardwareSubmesh::~Cal3dHardwareSubmesh()
   {
      mModel = NULL;
   }

   int Cal3dHardwareSubmesh::GetID() const
   {
      return mID;
   }

   bool Cal3dHardwareSubmesh::IsValid() const
   {
      return mID >= 0 && mID < int(mSubmeshArray->size());
   }

   int Cal3dHardwareSubmesh::GetVertexCount() const
   {
      return (*mSubmeshArray)[mID].vertexCount;
   }
   
   int Cal3dHardwareSubmesh::GetFaceCount() const
   {
      return (*mSubmeshArray)[mID].faceCount;
   }

   int Cal3dHardwareSubmesh::GetMorphTargetWeightCount() const
   {
      return 0;
   }

   int Cal3dHardwareSubmesh::GetFaceData(dtAnim::IndexArray& outData)
   {
      // TODO:
      return 0;
   }

   int Cal3dHardwareSubmesh::GetVertexData(dtAnim::FloatArray& outData, int stride)
   {
      // TODO:
      return 0;
   }

   int Cal3dHardwareSubmesh::GetNormalData(dtAnim::FloatArray& outData, int stride)
   {
      // TODO:
      return 0;
   }

   int Cal3dHardwareSubmesh::GetMorphTargetWeightData(dtAnim::FloatArray& outData)
   {
      return 0;
   }

   int Cal3dHardwareSubmesh::GetMorphTargets(dtAnim::MorphTargetArray& submorphTargets) const
   {
      return 0;
   }

   void Cal3dHardwareSubmesh::SetDrawable(dtAnim::HardwareSubmeshDrawable* drawable)
   {
      mDrawable = drawable;
   }

   dtAnim::HardwareSubmeshDrawable* Cal3dHardwareSubmesh::GetDrawable()
   {
      return mDrawable.get();
   }

   const dtAnim::HardwareSubmeshDrawable* Cal3dHardwareSubmesh::GetDrawable() const
   {
      return mDrawable.get();
   }



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   Cal3dMorphTarget::Cal3dMorphTarget(dtAnim::Cal3dSubmesh& submesh, CalCoreSubMorphTarget& morphTarget, int id)
      : mSubmesh(&submesh)
      , mMorphTarget(&morphTarget)
      , mID(id)
   {}

   Cal3dMorphTarget::~Cal3dMorphTarget()
   {
      mSubmesh = NULL;
      mMorphTarget = NULL;
   }

   int Cal3dMorphTarget::GetID() const
   {
      return mID;
   }
      
   void Cal3dMorphTarget::SetName(const std::string& name)
   {
      mName = name;
   }

   const std::string& Cal3dMorphTarget::GetName() const
   {
      return mName;
   }

   void Cal3dMorphTarget::SetWeight(float weight)
   {
      mSubmesh->GetCalSubmesh()->setMorphTargetWeight(mID, weight);
   }

   float Cal3dMorphTarget::GetWeight() const
   {
      return mSubmesh->GetCalSubmesh()->getMorphTargetWeight(mID);
   }

   CalCoreSubMorphTarget* Cal3dMorphTarget::GetCalMorphTarget()
   {
      return mMorphTarget;
   }

   const CalCoreSubMorphTarget* Cal3dMorphTarget::GetCalMorphTarget() const
   {
      return mMorphTarget;
   }


   
   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   Cal3dMesh::Cal3dMesh(Cal3DModelWrapper& model, CalMesh& mesh)
      : mModel(&model)
      , mMesh(&mesh)
      , mID(-1)
      , mVisible(true)
   {
      mID = mModel->GetCalModel()->getCoreModel()->getCoreMeshId(mMesh->getCoreMesh()->getName());
   }

   Cal3dMesh::~Cal3dMesh()
   {}

   int Cal3dMesh::GetID() const
   {
      return mID;
   }

   void Cal3dMesh::SetName(const std::string& name)
   {
      mMesh->getCoreMesh()->setName(name);
   }

   const std::string& Cal3dMesh::GetName() const
   {
      return mMesh->getCoreMesh()->getName();
   }

   void Cal3dMesh::SetVisible(bool visible)
   {
      mVisible = visible;
   }
   
   bool Cal3dMesh::IsVisible() const
   {
      return mVisible;
   }

   int Cal3dMesh::GetVertexCount() const
   {
      int count = 0;

      int limit = GetSubmeshCount();
      for (int i = 0; i < limit; ++i)
      {
         if (i == mID) // HACK:
         count += GetSubmeshVertexCount(i);
      }

      return count;
   }

   int Cal3dMesh::GetFaceCount() const
   {
      int count = 0;

      int limit = GetSubmeshCount();
      for (int i = 0; i < limit; ++i)
      {
         if (i == mID) // HACK:
         count += GetSubmeshFaceCount(i);
      }

      return count;
   }

   int Cal3dMesh::GetSubmeshCount() const
   {
      return 1;// HACK: mMesh->getSubmeshCount();
   }
   
   int Cal3dMesh::GetSubmeshes(dtAnim::SubmeshArray& outSubmeshes) const
   {
      int results = 0;

      CalSubmesh* submesh = NULL;
      int limit = GetSubmeshCount();
      for (int i = 0; i < limit; ++i)
      {
         if (i == mID) // HACK:
         {
         submesh = mMesh->getSubmesh(i);
         outSubmeshes.push_back(new dtAnim::Cal3dSubmesh(*mModel, *submesh));

         ++results;
         }
      }

      return results;
   }

   int Cal3dMesh::GetSubmeshVertexCount(int submeshIndex) const
   {
      return submeshIndex < mMesh->getSubmeshCount()
         ? mMesh->getSubmesh(submeshIndex)->getVertexCount()
         : 0;
   }

   int Cal3dMesh::GetSubmeshFaceCount(int submeshIndex) const
   {
      return submeshIndex < mMesh->getSubmeshCount()
         ? mMesh->getSubmesh(submeshIndex)->getFaceCount()
         : 0;
   }

   CalMesh* Cal3dMesh::GetCalMesh()
   {
      return mID >= 0 ? mModel->GetCalModel()->getMesh(mID) : NULL;
   }

   const CalMesh* Cal3dMesh::GetCalMesh() const
   {
      return mID >= 0 ? mModel->GetCalModel()->getMesh(mID) : NULL;
   }

   CalCoreMesh* Cal3dMesh::GetCalCoreMesh()
   {
      return mMesh->getCoreMesh();
   }

   const CalCoreMesh* Cal3dMesh::GetCalCoreMesh() const
   {
      return mMesh->getCoreMesh();
   }


   
   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   Cal3dHardwareMesh::Cal3dHardwareMesh(Cal3DModelWrapper& model, CalHardwareModel& hardwareModel, int id)
      : mModel(&model)
      , mHardwareModel(&hardwareModel)
      , mID(id)
      , mVisible(true)
   {}

   Cal3dHardwareMesh::~Cal3dHardwareMesh()
   {}

   int Cal3dHardwareMesh::GetID() const
   {
      return mID;
   }
      
   void Cal3dHardwareMesh::SetName(const std::string& name)
   {
      mName = name;
   }
   
   const std::string& Cal3dHardwareMesh::GetName() const
   {
      return mName;
   }

   void Cal3dHardwareMesh::SetVisible(bool visible)
   {
      mVisible = visible;
   }

   bool Cal3dHardwareMesh::IsVisible() const
   {
      return mVisible;
   }

   int Cal3dHardwareMesh::GetVertexCount() const
   {
      int count = 0;

      int submeshCount = GetSubmeshCount();
      for (int i = 0; i < submeshCount; ++i)
      {
         count += GetSubmeshVertexCount(i);
      }

      return count;
   }

   int Cal3dHardwareMesh::GetFaceCount() const
   {
      int count = 0;

      int submeshCount = GetSubmeshCount();
      for (int i = 0; i < submeshCount; ++i)
      {
         count += GetSubmeshFaceCount(i);
      }

      return count;
   }

   int Cal3dHardwareMesh::GetSubmeshCount() const
   {
      typedef std::vector<CalHardwareModel::CalHardwareMesh> HarwareSubmeshArray;
      
      int count = 0;

      const CalHardwareModel::CalHardwareMesh* curMesh = NULL;
      const HarwareSubmeshArray& meshes = mHardwareModel->getVectorHardwareMesh();
      HarwareSubmeshArray::const_iterator curIter = meshes.begin();
      HarwareSubmeshArray::const_iterator endIter = meshes.end();
      for ( ; curIter != endIter; ++curIter)
      {
         curMesh = &(*curIter);
         if (mID == curMesh->meshId)
         {
            ++count;
         }
      }

      return count;
   }

   int Cal3dHardwareMesh::GetSubmeshes(dtAnim::SubmeshArray& outSubmeshes) const
   {
      int count = 0;

      typedef std::vector<CalHardwareModel::CalHardwareMesh> HarwareSubmeshArray;
      
      const CalHardwareModel::CalHardwareMesh* curMesh = NULL;
      const HarwareSubmeshArray& meshes = mHardwareModel->getVectorHardwareMesh();
      HarwareSubmeshArray::const_iterator curIter = meshes.begin();
      HarwareSubmeshArray::const_iterator endIter = meshes.end();
      for ( ; curIter != endIter; ++curIter)
      {
         curMesh = &(*curIter);
         if (mID == curMesh->meshId)
         {
            outSubmeshes.push_back(new dtAnim::Cal3dHardwareSubmesh(*mHardwareModel, curMesh->submeshId));
            ++count;
         }
      }

      return count;
   }

   int Cal3dHardwareMesh::GetSubmeshVertexCount(int submeshIndex) const
   {
      typedef std::vector<CalHardwareModel::CalHardwareMesh> HarwareSubmeshArray;

      int count = 0;
      
      const CalHardwareModel::CalHardwareMesh* curMesh = NULL;
      const HarwareSubmeshArray& meshes = mHardwareModel->getVectorHardwareMesh();
      HarwareSubmeshArray::const_iterator curIter = meshes.begin();
      HarwareSubmeshArray::const_iterator endIter = meshes.end();
      for ( ; curIter != endIter; ++curIter)
      {
         curMesh = &(*curIter);
         if (mID == curMesh->meshId && curMesh->submeshId == submeshIndex)
         {
            count += curMesh->vertexCount;
         }
      }

      return count;
   }

   int Cal3dHardwareMesh::GetSubmeshFaceCount(int submeshIndex) const
   {
      typedef std::vector<CalHardwareModel::CalHardwareMesh> HarwareSubmeshArray;

      int count = 0;
      
      const CalHardwareModel::CalHardwareMesh* curMesh = NULL;
      const HarwareSubmeshArray& meshes = mHardwareModel->getVectorHardwareMesh();
      HarwareSubmeshArray::const_iterator curIter = meshes.begin();
      HarwareSubmeshArray::const_iterator endIter = meshes.end();
      for ( ; curIter != endIter; ++curIter)
      {
         curMesh = &(*curIter);
         if (mID == curMesh->meshId && curMesh->submeshId == submeshIndex)
         {
            count += curMesh->faceCount;
         }
      }

      return count;
   }

   CalHardwareModel* Cal3dHardwareMesh::GetCalHardwareModel()
   {
      return mHardwareModel;
   }

   const CalHardwareModel* Cal3dHardwareMesh::GetCalHardwareModel() const
   {
      return mHardwareModel;
   }

   void Cal3dHardwareMesh::SetDrawable(dtAnim::HardwareSubmeshDrawable* drawable)
   {
      mDrawable = drawable;
   }

   dtAnim::HardwareSubmeshDrawable* Cal3dHardwareMesh::GetDrawable()
   {
      return mDrawable.get();
   }

   const dtAnim::HardwareSubmeshDrawable* Cal3dHardwareMesh::GetDrawable() const
   {
      return mDrawable.get();
   }


   
   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   Cal3dAnimation::Cal3dAnimation(Cal3DModelWrapper& model, CalCoreAnimation& animation)
      : mModel(&model)
      , mAnim(&animation)
      , mID(-1)
   {
      mID = mModel->GetCalModel()->getCoreModel()->getCoreAnimationId(mAnim->getName());
   }
   
   Cal3dAnimation::~Cal3dAnimation()
   {}

   Cal3DModelWrapper* Cal3dAnimation::GetModelWrapper() const
   {
      return mModel.get();
   }

   int Cal3dAnimation::GetID() const
   {
      return mID;
   }
   
   void Cal3dAnimation::SetName(const std::string& name)
   {
      mAnim->setName(name);
   }

   const std::string& Cal3dAnimation::GetName() const
   {
      return mAnim->getName();
   }
   
   void Cal3dAnimation::SetPlayMode(const dtAnim::PlayModeEnum& playMode)
   {
      // Do nothing currently.
      // Play mode is represented by different objects in Cal3D.
   }
   
   const dtAnim::PlayModeEnum& Cal3dAnimation::GetPlayMode() const
   {
      const dtAnim::PlayModeEnum* mode = &dtAnim::PlayModeEnum::NONE;
      CalAnimation* anim = GetAnimationInstance();
      if (anim != NULL)
      {
         switch (anim->getType())
         {
         case CalAnimation::TYPE_ACTION:
            mode = &dtAnim::PlayModeEnum::ONCE;
            break;

         case CalAnimation::TYPE_CYCLE:
            mode = &dtAnim::PlayModeEnum::LOOP;
            break;
            
         case CalAnimation::TYPE_POSE:
            mode = &dtAnim::PlayModeEnum::POSE;
            break;

         default:
            break;
         }
      }

      return *mode;
   }

   const dtAnim::AnimationStateEnum& Cal3dAnimation::GetState() const
   {
      const dtAnim::AnimationStateEnum* state = &dtAnim::AnimationStateEnum::NONE;

      CalAnimation* anim = GetAnimationInstance();
      if (anim != NULL)
      {
         switch (anim->getState())
         {
         case CalAnimation::STATE_IN:
            state = &dtAnim::AnimationStateEnum::FADE_IN;
            break;

         case CalAnimation::STATE_OUT:
            state = &dtAnim::AnimationStateEnum::FADE_OUT;
            break;
            
         case CalAnimation::STATE_STEADY:
            state = &dtAnim::AnimationStateEnum::STEADY;
            break;

         case CalAnimation::STATE_STOPPED:
            state = &dtAnim::AnimationStateEnum::STOPPED;
            break;

         default:
            break;
         }
      }

      return *state;
   }

   CalAnimation* Cal3dAnimation::GetCalAnimation()
   {
      CalAnimation* animInstance = NULL;
      
      CalMixer* mixer = mModel->GetCalModel()->getMixer();
      typedef std::vector<CalAnimation*> CalAnimArray;
      CalAnimArray& anims = mixer->getAnimationVector();

      CalAnimation* curAnim = NULL;
      CalAnimArray::iterator curIter = anims.begin();
      CalAnimArray::iterator endIter = anims.end();
      for ( ; curIter != endIter; ++curIter)
      {
         curAnim = *curIter;
         if (curAnim->getCoreAnimation() == mAnim)
         {
            animInstance = curAnim;
            break;
         }
      }

      return animInstance;
   }

   const CalAnimation* Cal3dAnimation::GetCalAnimation() const
   {
      CalAnimation* animInstance = NULL;
      
      CalMixer* mixer = mModel->GetCalModel()->getMixer();
      typedef std::vector<CalAnimation*> CalAnimArray;
      CalAnimArray& anims = mixer->getAnimationVector();

      CalAnimation* curAnim = NULL;
      CalAnimArray::const_iterator curIter = anims.begin();
      CalAnimArray::const_iterator endIter = anims.end();
      for ( ; curIter != endIter; ++curIter)
      {
         curAnim = *curIter;
         if (curAnim->getCoreAnimation() == mAnim)
         {
            animInstance = curAnim;
            break;
         }
      }

      return animInstance;
   }

   CalCoreAnimation* Cal3dAnimation::GetCalCoreAnimation()
   {
      return mAnim;
   }

   const CalCoreAnimation* Cal3dAnimation::GetCalCoreAnimation() const
   {
      return mAnim;
   }

   bool Cal3dAnimation::PlayAction(float delayIn, float delayOut,
      float weight, bool autoLock)
   {
      bool success = false;
      CalMixer* mixer = mModel->GetCalModel()->getMixer();
      if (mixer != NULL)
      {
         int id = GetID();
         success = mixer->executeAction(id, delayIn, delayOut, weight, autoLock);
      }

      return success;
   }

   bool Cal3dAnimation::PlayCycle(float weight, float delay)
   {
      bool success = false;
      CalMixer* mixer = mModel->GetCalModel()->getMixer();
      if (mixer != NULL)
      {
         float minBlendTime = mModel->GetCalAnimator()->GetMinimumBlendTime();
         float fadeTime = delay < minBlendTime ? minBlendTime : delay;

         success = mixer->blendCycle(mID, weight, fadeTime);
      }

      return success;
   }

   bool Cal3dAnimation::ClearAction()
   {
      bool success = false;
      CalMixer* mixer = mModel->GetCalModel()->getMixer();
      if (mixer != NULL)
      {
         success = mixer->removeAction(mID);
      }

      return success;
   }

   bool Cal3dAnimation::ClearCycle(float delay)
   {
      bool success = false;
      CalMixer* mixer = mModel->GetCalModel()->getMixer();
      if (mixer != NULL)
      {
         success = mixer->clearCycle(mID, delay);
      }

      return success;
   }

   bool Cal3dAnimation::Clear(float delay)
   {
      bool success = false;

      CalAnimation* anim = GetAnimationInstance();
      if (anim != NULL)
      {
         CalAnimation::Type animType = anim->getType();
         if (animType == CalAnimation::TYPE_CYCLE)
         {
            success = ClearCycle(delay);
         }
         else if (animType == CalAnimation::TYPE_ACTION)
         {
            success = ClearAction();
         }
      }

      return success;
   }

   void Cal3dAnimation::SetSpeedFactor(float speedFactor)
   {
      CalMixer* mixer = mModel->GetCalModel()->getMixer();
      if (mID >= 0 && (int)(mixer->getAnimationVector().size()) > mID)
      {
         CalAnimation* anim = mixer->getAnimationVector()[mID];
         if (anim != NULL)
         {
            CalAnimation::Type animType = anim->getType();
            if (animType == CalAnimation::TYPE_CYCLE)
            {
               CalAnimationCycle* cac = static_cast<CalAnimationCycle*>(anim);
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
            else if (animType == CalAnimation::TYPE_ACTION)
            {
               CalAnimationAction* action = static_cast<CalAnimationAction*>(anim);
               action->setTimeFactor(speedFactor);
            }
         }
      }
   }

   float Cal3dAnimation::GetWeight() const
   {
      CalAnimation* anim = GetAnimationInstance();
      return anim == NULL ? 0.0 : anim->getWeight();
   }

   float Cal3dAnimation::GetTime() const
   {
      CalAnimation* anim = GetAnimationInstance();
      return anim == NULL ? 0.0 : anim->getTime();
   }

   float Cal3dAnimation::GetDuration() const
   {
      return GetCalCoreAnimation()->getDuration();
   }
      
   int Cal3dAnimation::GetKeyframeCount() const
   {
      return GetCalCoreAnimation()->getTotalNumberOfKeyframes();
   }

   int Cal3dAnimation::GetKeyframeCountForBone(const dtAnim::BoneInterface& bone) const
   {
      const Cal3dBone* calBone = dynamic_cast<const Cal3dBone*>(&bone);
      CalCoreTrack* track = const_cast<CalCoreAnimation*>(GetCalCoreAnimation())->getCoreTrack(calBone->GetID());
      return track == NULL ? 0 : track->getCoreKeyframeCount();
   }

   int Cal3dAnimation::GetTrackCount() const
   {
      return GetCalCoreAnimation()->getTrackCount();
   }

   int Cal3dAnimation::GetTracks(dtAnim::TrackArray& outTracks)
   {
      int count = 0;

      if (mTracks.size() != mAnim->getTrackCount())
      {
         typedef std::list<CalCoreTrack*> CalTrackList;
         CalTrackList& trackList = mAnim->getListCoreTrack();
         CalTrackList::iterator curIter = trackList.begin();
         CalTrackList::iterator endIter = trackList.end();
         
         mTracks.clear();
         mTracks.reserve(trackList.size());
         for ( ; curIter != endIter; ++curIter)
         {
            mTracks.push_back(new dtAnim::Cal3dTrack(*this, *(*curIter)));
         }
      }

      outTracks.reserve(outTracks.size() + mTracks.size());
      outTracks.insert(outTracks.end(), mTracks.begin(), mTracks.end());

      return count;
   }

   bool Cal3dAnimation::HasTrackForBone(const dtAnim::BoneInterface& bone) const
   {
      const Cal3dBone* calBone = dynamic_cast<const Cal3dBone*>(&bone);
      return (mModel->GetCalModel()->getCoreModel()->getCoreAnimation(mID)->getCoreTrack(calBone->GetID()) != NULL);
   }
   
   osg::Quat Cal3dAnimation::GetKeyframeQuat(const dtAnim::BoneInterface& bone, dtAnim::Keyframe keyframe) const
   {
      osg::Quat rotation;

      const dtAnim::Cal3dBone* calBone = static_cast<const dtAnim::Cal3dBone*>(&bone);
      int id = calBone->GetID();
      CalCoreTrack* cct = mAnim->getCoreTrack(id);
      assert(cct);

      if (cct)
      {
         CalCoreKeyframe* calKeyframe = cct->getCoreKeyframe(keyframe);
         const CalQuaternion& calQuat = calKeyframe->getRotation();
         rotation.set(calQuat.x, calQuat.y, calQuat.z, -calQuat.w);
      }

      return rotation;
   }

   CalAnimation* Cal3dAnimation::GetAnimationInstance() const
   {
      CalAnimation* result = NULL;

      CalMixer* mixer = mModel->GetCalModel()->getMixer();
      if (mixer != NULL)
      {
         typedef std::vector<CalAnimation*> CalAnimArray;
         CalAnimArray& anims = mixer->getAnimationVector();

         CalAnimation* curAnim = NULL;
         CalAnimArray::iterator curIter = anims.begin();
         CalAnimArray::iterator endIter = anims.end();
         for ( ; curIter != endIter; ++curIter)
         {
            curAnim = *curIter;
            if (curAnim != NULL && curAnim->getCoreAnimation() == mAnim)
            {
               result = curAnim;
               break;
            }
         }
      }

      return result;
   }


   
   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   Cal3dTrack::Cal3dTrack(dtAnim::Cal3dAnimation& anim, CalCoreTrack& track)
      : mAnim(&anim)
      , mTrack(&track)
   {}

   Cal3dTrack::~Cal3dTrack()
   {}

   dtAnim::BoneInterface* Cal3dTrack::GetBone() const
   {
      dtAnim::BoneInterface* bone = NULL;

      if (mAnim.valid())
      {
         bone = mAnim->GetModelWrapper()->GetBoneByIndex(mTrack->getCoreBoneId());
      }

      return bone;
   }

   bool Cal3dTrack::GetTransformAtTime(float time, dtAnim::Location& outLocation, dtAnim::Rotation& outRotation) const
   {
      CalVector calLocation;
      CalQuaternion calQuat;
      bool success = mTrack->getState(time, calLocation, calQuat); 
      
      outLocation.set(calLocation.x, calLocation.y, calLocation.z);
      outRotation.set(calQuat.x, calQuat.y, calQuat.z, -calQuat.w);

      return success;
   }

}

