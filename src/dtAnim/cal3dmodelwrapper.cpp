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
// DELTA3D
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/animnodebuilder.h>
#include <dtAnim/cal3danimator.h>
#include <dtAnim/cal3dobjects.h>
#include <dtAnim/modeldatabase.h>
#include <dtUtil/log.h>
// CAL3D
#include <cal3d/cal3d.h>
#include <cal3d/coretrack.h>
#include <cal3d/corekeyframe.h>

#include <cassert>
#include <algorithm>



namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   Cal3dInterfaceObjectCache::Cal3dInterfaceObjectCache(Cal3DModelWrapper& model)
      : BaseClass(model)
   {}

   Cal3dInterfaceObjectCache::~Cal3dInterfaceObjectCache()
   {
      Clear();
   }

   void Cal3dInterfaceObjectCache::UpdateAnimations()
   {
      mAnims.clear();
      mIDAnimMap.clear();

      CalCoreModel* model = mModel->GetCalModel()->getCoreModel();

      std::string name;
      CalCoreAnimation* anim = NULL;
      int limit = model->getCoreAnimationCount();
      for (int id = 0; id < limit; ++id)
      {
         anim = model->getCoreAnimation(id);
         name = anim->getName();

         dtCore::RefPtr<dtAnim::Cal3dAnimation> newAnim = new dtAnim::Cal3dAnimation(*mModel, *anim);
         mAnims.insert(std::make_pair(name, newAnim));

         mIDAnimMap.insert(std::make_pair(id, newAnim.get()));
      }
   }

   void Cal3dInterfaceObjectCache::UpdateBones()
   {
      mBones.clear();
      mIDBoneMap.clear();

      CalSkeleton* skel = mModel->GetCalModel()->getSkeleton();

      std::string name;
      CalBone* bone = NULL;
      int limit = int(skel->getCoreSkeleton()->getVectorCoreBone().size());
      for (int id = 0; id < limit; ++id)
      {
         bone = skel->getBone(id);
         name = bone->getCoreBone()->getName();

         dtCore::RefPtr<dtAnim::Cal3dBone> newBone = new dtAnim::Cal3dBone(*mModel, *bone);
         mBones.insert(std::make_pair(name, newBone));

         mIDBoneMap.insert(std::make_pair(id, newBone.get()));
      }
   }

   void Cal3dInterfaceObjectCache::UpdateMaterials()
   {
      mMaterials.clear();

      CalCoreModel* model = mModel->GetCalModel()->getCoreModel();

      std::string name;
      CalCoreMaterial* material = NULL;
      int limit = model->getCoreMaterialCount();
      for (int id = 0; id < limit; ++id)
      {
         material = model->getCoreMaterial(id);
         name = material->getName();
         mMaterials.insert(std::make_pair(name, new dtAnim::Cal3dMaterial(*mModel, *material)));
      }
   }

   void Cal3dInterfaceObjectCache::UpdateMeshes()
   {
      mMeshes.clear();

      CalModel* model = mModel->GetCalModel();
      Cal3DModelData* modelData = mModel->GetCalModelData();
      CalHardwareModel* hardwareModel = modelData->GetCalHardwareModel();

      std::string name;
      CalMesh* mesh = NULL;
      int limit = model->getCoreModel()->getCoreMeshCount();

      if (hardwareModel != NULL)
      {
         limit = hardwareModel->getHardwareMeshCount();
      }

      for (int id = 0; id < limit; ++id)
      {
         mesh = model->getMesh(id);
         if (hardwareModel != NULL && model->getCoreModel()->getCoreMesh(id) != NULL)
         {
            name = model->getCoreModel()->getCoreMesh(id)->getName();
            dtCore::RefPtr<dtAnim::Cal3dHardwareMesh> newMesh = new dtAnim::Cal3dHardwareMesh(*mModel, *hardwareModel, id);
            newMesh->SetName(name);
            mMeshes.insert(std::make_pair(name, newMesh));
         }
         else if (mesh != NULL)
         {
            name = mesh->getCoreMesh()->getName();
            mMeshes.insert(std::make_pair(name, new dtAnim::Cal3dMesh(*mModel, *mesh)));
         }
      }
   }

   void Cal3dInterfaceObjectCache::UpdateSkeleton()
   {
      CalSkeleton* skel = mModel->GetCalModel()->getSkeleton();
      mSkel = new dtAnim::Cal3dSkeleton(*mModel, *skel);
   }

   void Cal3dInterfaceObjectCache::Clear()
   {
      mIDAnimMap.clear();
      mIDBoneMap.clear();

      mAnims.clear();
      mBones.clear();
      mMaterials.clear();
      mMeshes.clear();
      mSkel = NULL;
   }

   dtAnim::Cal3dAnimation* Cal3dInterfaceObjectCache::GetAnimationByID(int id)
   {
      dtAnim::Cal3dAnimation* anim = NULL;
      
      IDAnimMap::iterator foundIter = mIDAnimMap.find(id);
      if (foundIter != mIDAnimMap.end())
      {
         anim = foundIter->second.get();
      }

      return anim;
   }

   dtAnim::Cal3dBone* Cal3dInterfaceObjectCache::GetBoneByID(int id)
   {
      dtAnim::Cal3dBone* bone = NULL;
      
      IDBoneMap::iterator foundIter = mIDBoneMap.find(id);
      if (foundIter != mIDBoneMap.end())
      {
         bone = foundIter->second.get();
      }

      return bone;
   }



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   Cal3DModelWrapper::Cal3DModelWrapper(dtAnim::Cal3DModelData& modelData)
      : BaseClass(modelData)
      , mScale(1.0f)
      , mCalModel(NULL)
      , mRenderer(NULL)
      , mSelectedMeshID(-1)
      , mSelectedSubmeshID(-1)
   {
      CalCoreModel* coreModel = modelData.GetCoreModel();
      mCalModel = new CalModel(coreModel);

      mRenderer = mCalModel->getRenderer();
      mCache = new dtAnim::Cal3dInterfaceObjectCache(*this);

      // attach all meshes to the model
      if (coreModel)
      {
         // Update the interface object cache.
         // This can be done since it has access to the
         // passed-in model object.
         mCache->Update();

         for (int meshId = 0; meshId < coreModel->getCoreMeshCount(); ++meshId)
         {
            // If the mesh is currently loaded
            CalCoreMesh* coreMesh = coreModel->getCoreMesh(meshId);
            if (coreMesh != NULL)
            {
               AttachMesh(meshId);

               dtAnim::MeshInterface* mesh = GetMesh(coreMesh->getName());
               if (mesh != NULL)
               {
                  mesh->SetVisible(true);
               }
            }
         }
      }
      SetScale(modelData.GetScale());
      UpdateInterfaceObjects();
      mAnimator = new Cal3DAnimator(this);
   }

   /////////////////////////////////////////////////////////////////////////////
   Cal3DModelWrapper::~Cal3DModelWrapper()
   {
      // Clear the cache to prevent circular references.
      mCache->Clear();
      mCache = NULL;

      Cal3DAnimator* calAnimator = static_cast<Cal3DAnimator*>(mAnimator.get());
      calAnimator->SetWrapper(NULL);
      calAnimator = NULL;
      mAnimator = NULL;
      mDrawable = NULL;

      delete mCalModel;
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<osg::Node> Cal3DModelWrapper::CreateDrawableNode(bool immediate)
   {
      mDrawable = dtAnim::ModelDatabase::GetInstance().CreateNode(*this, immediate);
      
      // Now that the drawable node is accessible from this wrapper, call the
      // base functionality to commit the scale to the character model.
      UpdateScale();

      osg::MatrixTransform* scaleNode = GetScaleTransform();
      if (scaleNode != NULL)
      {
         mDrawable = scaleNode;
      }

      return mDrawable.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Node* Cal3DModelWrapper::GetDrawableNode()
   {
      return mDrawable.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   dtAnim::Cal3DModelData* Cal3DModelWrapper::GetCalModelData() const
   {
      return dynamic_cast<Cal3DModelData*>(GetModelData());
   }

   /////////////////////////////////////////////////////////////////////////////
   dtAnim::AnimationUpdaterInterface* Cal3DModelWrapper::GetAnimator()
   {
      return mAnimator.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   dtAnim::Cal3DAnimator* Cal3DModelWrapper::GetCalAnimator()
   {
      return mAnimator.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   dtAnim::AnimationInterface* Cal3DModelWrapper::GetAnimation(const std::string& name) const
   {
      return mCache->GetAnimation(name);
   }

   /////////////////////////////////////////////////////////////////////////////
   int Cal3DModelWrapper::GetAnimations(dtAnim::AnimationArray& outAnims) const
   {
      return mCache->GetAnimations(outAnims);
   }

   /////////////////////////////////////////////////////////////////////////////
   dtAnim::BoneInterface* Cal3DModelWrapper::GetBone(const std::string& name) const
   {
      return mCache->GetBone(name);
   }
   
   /////////////////////////////////////////////////////////////////////////////
   int Cal3DModelWrapper::GetBones(dtAnim::BoneArray& outBones) const
   {
      return mCache->GetBones(outBones);
   }

   /////////////////////////////////////////////////////////////////////////////
   dtAnim::MaterialInterface* Cal3DModelWrapper::GetMaterial(const std::string& name) const
   {
      return mCache->GetMaterial(name);
   }
   
   /////////////////////////////////////////////////////////////////////////////
   int Cal3DModelWrapper::GetMaterials(dtAnim::MaterialArray& outMaterials) const
   {
      return mCache->GetMaterials(outMaterials);
   }

   /////////////////////////////////////////////////////////////////////////////
   dtAnim::MeshInterface* Cal3DModelWrapper::GetMesh(const std::string& name) const
   {
      return mCache->GetMesh(name);
   }
   
   /////////////////////////////////////////////////////////////////////////////
   int Cal3DModelWrapper::GetMeshes(dtAnim::MeshArray& outMeshes) const
   {
      return mCache->GetMeshes(outMeshes);
   }

   /////////////////////////////////////////////////////////////////////////////
   dtAnim::SkeletonInterface* Cal3DModelWrapper::GetSkeleton()
   {
      return mCache->GetSkeleton();
   }

   /////////////////////////////////////////////////////////////////////////////
   const dtAnim::SkeletonInterface* Cal3DModelWrapper::GetSkeleton() const
   {
      return mCache->GetSkeleton();
   }

   /////////////////////////////////////////////////////////////////////////////
   int Cal3DModelWrapper::GetAnimationCount() const
   {
      return (int) mCache->mAnims.size();
   }

   /////////////////////////////////////////////////////////////////////////////
   int Cal3DModelWrapper::GetBoneCount() const
   {
      return (int) mCache->mBones.size();
   }

   /////////////////////////////////////////////////////////////////////////////
   int Cal3DModelWrapper::GetMaterialCount() const
   {
      return (int) mCache->mMaterials.size();
   }

   /////////////////////////////////////////////////////////////////////////////
   int Cal3DModelWrapper::GetMeshCount() const
   {
      return (int) mCache->mMeshes.size();
   }

   /////////////////////////////////////////////////////////////////////////////
   dtAnim::AnimationInterface* Cal3DModelWrapper::GetAnimationByIndex(int index) const
   {
      return mCache->GetAnimationByID(index);
   }

   /////////////////////////////////////////////////////////////////////////////
   dtAnim::BoneInterface* Cal3DModelWrapper::GetBoneByIndex(int id) const
   {
      return mCache->GetBoneByID(id);
   }
   
   /////////////////////////////////////////////////////////////////////////////
   dtAnim::Cal3dBaseMesh* Cal3DModelWrapper::GetMeshByIndex(int id) const
   {
      CalMesh* mesh = mCalModel->getMesh(id);
      return mesh == NULL ? NULL : mCache->GetMesh(mesh->getCoreMesh()->getName());
   }

   /////////////////////////////////////////////////////////////////////////////
   int Cal3DModelWrapper::GetRootBones(dtAnim::BoneArray& outBones) const
   {
      int count = 0;

      if (mCalModel != NULL)
      {
         typedef std::vector<int> IntArray;
         IntArray ids = mCalModel->getCoreModel()->getCoreSkeleton()->getVectorRootCoreBoneId();

         dtAnim::BoneInterface* bone = NULL;
         int limit = int(ids.size());
         for (int i = 0; i < limit; ++i)
         {
            bone = GetBoneByIndex(i);
            if (bone != NULL)
            {
               outBones.push_back(bone);
               ++count;
            }
         }
      }

      return count;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DModelWrapper::UpdateInterfaceObjects()
   {
      UpdateBoneInterfaceObjects();
      UpdateMaterialInterfaceObjects();
      UpdateMeshInterfaceObjects();
      UpdateAnimationInterfaceObjects();
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DModelWrapper::UpdateAnimationInterfaceObjects()
   {
      mCache->UpdateAnimations();
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DModelWrapper::UpdateBoneInterfaceObjects()
   {
      mCache->UpdateSkeleton();
      mCache->UpdateBones();
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DModelWrapper::UpdateMaterialInterfaceObjects()
   {
      mCache->UpdateMaterials();
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DModelWrapper::UpdateMeshInterfaceObjects()
   {
      mCache->UpdateMeshes();
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DModelWrapper::ClearInterfaceObjects()
   {
      mCache->Clear();
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Cal3DModelWrapper::AttachMesh(int meshID)
   {
      bool success = mCalModel->attachMesh(meshID);

      if (success)
      {
         mCalModel->setMaterialSet(0);
         UpdateMeshInterfaceObjects();
      }

      return success;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Cal3DModelWrapper::DetachMesh(int meshID)
   {
      bool success = mCalModel->detachMesh(meshID);

      if (success)
      {
         UpdateMeshInterfaceObjects();
      }

      return success;
   }

   /////////////////////////////////////////////////////////////////////////////
   int Cal3DModelWrapper::GetAttachedMeshCount() const
   {
      return mRenderer->getMeshCount();
   }
   
   /////////////////////////////////////////////////////////////////////////////
   void Cal3DModelWrapper::SetCalModel(CalModel* model)
   {
      assert(model != NULL);

      mCalModel = model;
      mRenderer = mCalModel->getRenderer();

      mSelectedMeshID = -1;
      mSelectedSubmeshID = -1;

      Cal3DAnimator* calAnimator = dynamic_cast<Cal3DAnimator*>(mAnimator.get());
      calAnimator->SetWrapper(this);
      mCache->Update();
   }
   
   /////////////////////////////////////////////////////////////////////////////
   void Cal3DModelWrapper::ClearAllAnimations(float delay)
   {
      mAnimator->ClearAll(delay);
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
   CalRenderer* Cal3DModelWrapper::GetCalRenderer()
   {
      return mRenderer;
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
      // DG - Setting core model scale is problematic for reusing core models because it changes to base vertex data.
      mScale = scale;

      UpdateScale();
   }

   /////////////////////////////////////////////////////////////////////////////
   float Cal3DModelWrapper::GetScale() const
   {
      return mScale;
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
   void Cal3DModelWrapper::UpdateAnimation(float deltaTime)
   {
      mAnimator->Update(deltaTime);
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
      mSelectedMeshID = meshID;
      mSelectedSubmeshID = submeshID;
      return mRenderer->selectMeshSubmesh(meshID, submeshID);
   }
   
   /////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtAnim::Cal3dSubmesh> Cal3DModelWrapper::GetSelectedSubmesh()
   {
      dtCore::RefPtr<dtAnim::Cal3dSubmesh> submesh;
      CalSubmesh* calSubmesh = NULL;

      if (mSelectedMeshID >= 0 && mSelectedSubmeshID >= 0)
      {
         CalMesh* mesh = mCalModel->getMesh(mSelectedMeshID);
         calSubmesh = mesh == NULL ? NULL : mesh->getSubmesh(mSelectedSubmeshID);

         if (calSubmesh != NULL)
         {
            submesh = new dtAnim::Cal3dSubmesh(*this, *calSubmesh);
         }
      }

      return submesh;
   }
   
   /////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtAnim::Cal3dMaterial> Cal3DModelWrapper::GetSelectedSubmeshMaterial()
   {
      dtAnim::Cal3dMaterial* material = NULL;

      if (mSelectedMeshID >= 0)
      {
         CalMesh* mesh = mCalModel->getMesh(mSelectedMeshID);
         if (mesh != NULL)
         {
            CalSubmesh* calSubmesh = mesh->getSubmesh(mSelectedSubmeshID);
            if (calSubmesh != NULL)
            {
               int materialID = calSubmesh->getCoreMaterialId();
               dtAnim::MaterialArray materials;
               mCache->GetMaterials(materials);

               if (materialID >= 0 && materialID < int(materials.size()))
               {
                  material = dynamic_cast<dtAnim::Cal3dMaterial*>(materials[materialID].get());
               }
            }
         }
      }

      return material;
   }

   void Cal3DModelWrapper::HandleModelResourceUpdate(dtAnim::ModelResourceType resourceType)
   {
      switch (resourceType)
      {
      case dtAnim::SKEL_FILE:
         UpdateBoneInterfaceObjects();
         break;

      case dtAnim::MAT_FILE:
         UpdateMaterialInterfaceObjects();
         break;

      case dtAnim::MESH_FILE:
         UpdateMeshInterfaceObjects();
         break;

      case dtAnim::ANIM_FILE:
         UpdateAnimationInterfaceObjects();
         break;

      default:
         break;
      }
   }

   void Cal3DModelWrapper::HandleModelUpdated()
   {
      UpdateInterfaceObjects();
   }

   /////////////////////////////////////////////////////////////////////////////
} // namespace dtAnim
