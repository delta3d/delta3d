
////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/osgmodelwrapper.h>
#include <dtAnim/modeldatabase.h>
#include <dtAnim/osganimator.h>
#include <dtAnim/osgmodelresourcefinder.h>
#include <dtAnim/osgnodebuilder.h>
#include <dtUtil/log.h>
#include <osg/CopyOp>
#include <osg/Geode>
#include <sstream>


namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   OsgInterfaceObjectCache::OsgInterfaceObjectCache(OsgModelWrapper& model)
      : BaseClass(model)
   {
   }

   OsgInterfaceObjectCache::~OsgInterfaceObjectCache()
   {
      Clear();
   }

   dtAnim::OsgModelWrapper* OsgInterfaceObjectCache::GetOsgModelWrapper()
   {
      return static_cast<dtAnim::OsgModelWrapper*>(mModel.get());
   }

   void OsgInterfaceObjectCache::Update()
   {
      if (!mModel.valid() || mModel->GetDrawableNode() == NULL)
      {
         return;
      }

      Clear();

      osg::Node* node = mModel->GetDrawableNode();
      dtCore::RefPtr<ResFinder> finder = new ResFinder(ResFinder::SEARCH_ALL);
      node->accept(*finder);

      Internal_UpdateSkeleton(*finder);
      Internal_UpdateBones(*finder);
      Internal_UpdateMaterials(*finder);
      Internal_UpdateMeshes(*finder);
      Internal_UpdateAnimations(*finder);
   }

   void OsgInterfaceObjectCache::UpdateAnimations()
   {
      mAnims.clear();
      mIDAnimMap.clear();

      osg::Node* node = mModel->GetDrawableNode();

      if (node != NULL)
      {
         dtCore::RefPtr<ResFinder> finder = new ResFinder(ResFinder::SEARCH_ANIMATIONS);
         node->accept(*finder);

         Internal_UpdateAnimations(*finder);
      }
   }
   
   void OsgInterfaceObjectCache::UpdateBones()
   {
      mBones.clear();
      mIDBoneMap.clear();

      osg::Node* node = mModel->GetDrawableNode();

      if (node != NULL)
      {
         dtCore::RefPtr<ResFinder> finder = new ResFinder(ResFinder::SEARCH_BONES);
         node->accept(*finder);

         Internal_UpdateBones(*finder);
      }
   }
   
   void OsgInterfaceObjectCache::UpdateMaterials()
   {
      mMaterials.clear();

      osg::Node* node = mModel->GetDrawableNode();
      
      if (node != NULL)
      {
         dtCore::RefPtr<ResFinder> finder = new ResFinder(ResFinder::SEARCH_MATERIALS);
         node->accept(*finder);

         Internal_UpdateMaterials(*finder);
      }
   }
   
   void OsgInterfaceObjectCache::UpdateMeshes()
   {
      mMeshes.clear();

      osg::Node* node = mModel->GetDrawableNode();
      
      if (node != NULL)
      {
         dtCore::RefPtr<ResFinder> finder = new ResFinder(ResFinder::SEARCH_MESHES);
         node->accept(*finder);

         Internal_UpdateMeshes(*finder);
      }
   }

   void OsgInterfaceObjectCache::UpdateSkeleton()
   {
      mSkel = NULL;

      osg::Node* node = mModel->GetDrawableNode();
      
      if (node != NULL)
      {
         dtCore::RefPtr<ResFinder> finder = new ResFinder(ResFinder::SEARCH_SKELETON);
         node->accept(*finder);

         Internal_UpdateSkeleton(*finder);
      }
   }

   void OsgInterfaceObjectCache::Clear()
   {
      BaseClass::Clear();

      mIDAnimMap.clear();
      mIDBoneMap.clear();
   }
   
   dtAnim::OsgAnimation* OsgInterfaceObjectCache::GetAnimationByIndex(int index)
   {
      return index < 0 || index > (int)(mIDAnimMap.size())
         ? NULL : mIDAnimMap[index];
   }

   dtAnim::OsgBone* OsgInterfaceObjectCache::GetBoneByIndex(int index)
   {
      return index < 0 || index > (int)(mIDBoneMap.size())
         ? NULL : mIDBoneMap[index];
   }

   osgAnimation::BasicAnimationManager* OsgInterfaceObjectCache::GetOsgAnimationManager()
   {
      return mAnimManager.get();
   }

   osg::Node* OsgInterfaceObjectCache::GetOsgAnimationManagerNode()
   {
      return mAnimNode.get();
   }

   void OsgInterfaceObjectCache::Internal_UpdateAnimations(ResFinder& finder)
   {
      typedef ResFinder::OsgAnimManagerArray OsgAnimManagerArray;
      typedef osgAnimation::AnimationList OsgAnimList;

      dtAnim::OsgModelData* modelData = mModel->GetOsgModelData();

      OsgAnimManagerArray::iterator curManager = finder.mAnimManagers.begin();
      OsgAnimManagerArray::iterator endManager = finder.mAnimManagers.end();
      for ( ; curManager != endManager; ++curManager)
      {
         const OsgAnimList& animList = (*curManager)->getAnimationList();

         osgAnimation::Animation* curAnim = NULL;
         OsgAnimList::const_iterator curIter = animList.begin();
         OsgAnimList::const_iterator endIter = animList.end();
         for ( ; curIter != endIter; ++curIter)
         {
            curAnim = *curIter;
            const std::string& animName = curAnim->getName();
            
            dtCore::RefPtr<OsgAnimation> anim = new OsgAnimation(*mModel, *curAnim);

            const osgAnimation::Animation* coreAnim = modelData->GetCoreAnimation(animName);
            if (coreAnim == NULL)
            {
               LOG_ERROR("Could not access core animation for \"" + animName + "\"");
            }
            else
            {
               anim->SetOriginalDuration(coreAnim->getDuration());
               anim->SetOriginalWeight(coreAnim->getWeight());
            }

            mAnims.insert(std::make_pair(animName, anim));
         }
      }

      if ( ! finder.mAnimManagers.empty())
      {
         mAnimManager = finder.mAnimManagers.front();
         mAnimNode = finder.mAnimNodes.front();

         // Warn if there is more than one animation manager in the model.
         if (finder.mAnimManagers.size() > 1)
         {
            std::ostringstream oss;
            oss << "Character model \"" << mModel->GetModelData()->GetModelName()
               << "\" has more than one AnimationManager, " << finder.mAnimManagers.size()
               << " in total." << std::endl;
            LOG_WARNING(oss.str());
         }
      }
      else // Warn that there are no animation managers detected.
      {
         std::ostringstream oss;
         oss << "Character model \"" << mModel->GetModelData()->GetModelName()
            << "\" does not have an AnimationManager." << std::endl;
         LOG_ERROR(oss.str());
      }

      // Create an index map of the animations.
      AnimMap::iterator curAnimIter = mAnims.begin();
      AnimMap::iterator endAnimIter = mAnims.end();
      for (int i = 0; curAnimIter != endAnimIter; ++curAnimIter, ++i)
      {
         mIDAnimMap.insert(std::make_pair(i, curAnimIter->second.get()));
      }
   }

   void OsgInterfaceObjectCache::Internal_UpdateBones(ResFinder& finder)
   {
      typedef ResFinder::OsgBoneArray OsgBoneArray;

      osgAnimation::Bone* bone = NULL;
      OsgBoneArray::iterator curIter = finder.mBones.begin();
      OsgBoneArray::iterator endIter = finder.mBones.end();
      for (; curIter != endIter; ++curIter)
      {
         bone = curIter->get();
         mBones.insert(std::make_pair(bone->getName(),
            new dtAnim::OsgBone(*GetOsgModelWrapper(), *bone)));
      }

      // Create an index map of the bones.
      BoneMap::iterator curBoneIter = mBones.begin();
      BoneMap::iterator endBoneIter = mBones.end();
      for (int i = 0; curBoneIter != endBoneIter; ++curBoneIter, ++i)
      {
         mIDBoneMap.insert(std::make_pair(i, curBoneIter->second.get()));
      }
   }

   void OsgInterfaceObjectCache::Internal_UpdateMaterials(ResFinder& finder)
   {
      typedef ResFinder::OsgMaterialArray OsgMaterialArray;

      dtAnim::OsgModelWrapper* wrapper = GetOsgModelWrapper();

      osg::StateSet* stateSet = NULL;
      OsgMaterialArray::iterator curIter = finder.mMaterials.begin();
      OsgMaterialArray::iterator endIter = finder.mMaterials.end();
      for (; curIter != endIter; ++curIter)
      {
         stateSet = curIter->get();
         mMaterials.insert(std::make_pair(stateSet->getName(),
            new dtAnim::OsgMaterial(*wrapper, *stateSet)));
      }

      const osg::CopyOp::CopyFlags MATERIAL_COPY_FLAGS
         = osg::CopyOp::DEEP_COPY_STATESETS
         || osg::CopyOp::DEEP_COPY_UNIFORMS
         || osg::CopyOp::DEEP_COPY_CALLBACKS;

      // Other parts of the character might not be loaded.
      // Collect all materials that may have been registered
      // in the model data that would not otherwise be available
      // without the presence of drawables.
      OsgModelData* modelData = wrapper->GetOsgModelData();
      osg::StateSet* coreMaterial = NULL;
      const OsgModelData::OsgMaterialMap& coreMaterials = modelData->GetCoreMaterials();
      OsgModelData::OsgMaterialMap::const_iterator curCoreIter = coreMaterials.begin();
      OsgModelData::OsgMaterialMap::const_iterator endCoreIter = coreMaterials.end();
      for (; curCoreIter != endCoreIter; ++curCoreIter)
      {
         coreMaterial = curCoreIter->second.get();
         const std::string& name = curCoreIter->first;
         BaseClass::MaterialMap::iterator foundIter = mMaterials.find(name);
         if (foundIter == mMaterials.end())
         {
            dtCore::RefPtr<osg::StateSet> materialCopy
               = static_cast<osg::StateSet*>(coreMaterial->clone(MATERIAL_COPY_FLAGS));
            
            if (materialCopy.valid())
            {
               mMaterials.insert(std::make_pair(name,
                  new dtAnim::OsgMaterial(*wrapper, *materialCopy)));
            }
            else
            {
               LOG_ERROR("Could not clone material \"" + name + "\"");
            }
         }
      }
   }

   void OsgInterfaceObjectCache::Internal_UpdateMeshes(ResFinder& finder)
   {
      typedef ResFinder::OsgGeodeArray OsgMeshArray;

      osg::Geode* mesh = NULL;
      OsgMeshArray::iterator curIter = finder.mMeshes.begin();
      OsgMeshArray::iterator endIter = finder.mMeshes.end();
      for (int id = 0; curIter != endIter; ++curIter, ++id)
      {
         mesh = curIter->get();

         // Ensure that the mesh objects have valid unique names.
         // TODO: Move this to place in the load phase.
         std::string name(mesh->getName());
         if (name.empty())
         {
            osg::Drawable* geom = mesh->getDrawable(0);
            if (geom != NULL)
            {
               name = geom->getName();

               if (name.empty())
               {
                  osgAnimation::RigGeometry* rigGeom
                     = dynamic_cast<osgAnimation::RigGeometry*>(geom);

                  if (rigGeom != NULL)
                  {
                     geom = rigGeom->getSourceGeometry();
                     if (geom != NULL)
                     {
                        name = geom->getName();
                     }
                  }
               }
            }
            
            if (name.empty())
            {
               std::ostringstream oss;
               oss << name << id;
               name = oss.str();
            }

            mesh->setName(name);
         }

         dtCore::RefPtr<OsgMesh> osgMesh = new dtAnim::OsgMesh(*GetOsgModelWrapper(), *mesh);
         osgMesh->SetID(id);
         mMeshes.insert(std::make_pair(mesh->getName(), osgMesh.get()));
      }
   }

   void OsgInterfaceObjectCache::Internal_UpdateSkeleton(ResFinder& finder)
   {
      if (finder.mSkeleton.valid())
      {
         mSkel = new dtAnim::OsgSkeleton(*GetOsgModelWrapper(), *finder.mSkeleton);
      }
   }



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   OsgModelWrapper::OsgModelWrapper(OsgModelData& modelData)
      : BaseClass(modelData)
      , mHardwareMode(false)
      , mScale(1.0f)
   {
      mCache = new dtAnim::OsgInterfaceObjectCache(*this);
      mCache->Update();

      mNode = CreateDrawableNode();

      Internal_UpdateAnimatorObject();
   }

   OsgModelWrapper::~OsgModelWrapper()
   {
      mCache->Clear();
      mCache = NULL;

      mAnimator = NULL;
      mNode = NULL;
   }

   dtCore::RefPtr<osg::Node> OsgModelWrapper::CreateDrawableNode(bool immediate)
   {
      mNode = dtAnim::ModelDatabase::GetInstance().CreateNode(*this, immediate);
      
      if (immediate)
      {
         mCache->Update();
      }

      return mNode.get();
   }

   osg::Node* OsgModelWrapper::GetDrawableNode()
   {
      return mNode.get();
   }

   dtAnim::OsgModelData* OsgModelWrapper::GetOsgModelData()
   {
      return static_cast<dtAnim::OsgModelData*>(GetModelData());
   }

   dtAnim::AnimationUpdaterInterface* OsgModelWrapper::GetAnimator()
   {
      return mAnimator.get();
   }

   dtAnim::OsgAnimator* OsgModelWrapper::GetOsgAnimator()
   {
      return mAnimator.get();
   }

   void OsgModelWrapper::SetModel(osg::Node* model)
   {
      if (mNode != model)
      {
         mNode = model;
         mCache->Clear();

         if (mNode.valid())
         {
            mCache->Update();
         }
      }
   }

   osg::Node* OsgModelWrapper::GetModel()
   {
      return mNode.get();
   }

   const osg::Node* OsgModelWrapper::GetModel() const
   {
      return mNode.get();
   }

   dtAnim::AnimationInterface* OsgModelWrapper::GetAnimation(const std::string& name) const
   {
      return mCache->GetAnimation(name);
   }

   int OsgModelWrapper::GetAnimations(dtAnim::AnimationArray& outAnims) const
   {
      return mCache->GetAnimations(outAnims);
   }

   dtAnim::BoneInterface* OsgModelWrapper::GetBone(const std::string& name) const
   {
      return mCache->GetBone(name);
   }

   int OsgModelWrapper::GetBones(dtAnim::BoneArray& outBones) const
   {
      return mCache->GetBones(outBones);
   }

   dtAnim::MaterialInterface* OsgModelWrapper::GetMaterial(const std::string& name) const
   {
      return mCache->GetMaterial(name);
   }

   int OsgModelWrapper::GetMaterials(dtAnim::MaterialArray& outMaterials) const
   {
      return mCache->GetMaterials(outMaterials);
   }

   dtAnim::MeshInterface* OsgModelWrapper::GetMesh(const std::string& name) const
   {
      return mCache->GetMesh(name);
   }
      
   int OsgModelWrapper::GetMeshes(dtAnim::MeshArray& outMeshes) const
   {
      return mCache->GetMeshes(outMeshes);
   }

   dtAnim::SkeletonInterface* OsgModelWrapper::GetSkeleton()
   {
      return mCache->GetSkeleton();
   }
      
   const dtAnim::SkeletonInterface* OsgModelWrapper::GetSkeleton() const
   {
      return mCache->GetSkeleton();
   }

   int OsgModelWrapper::GetAnimationCount() const
   {
      return mCache->GetAnimationCount();
   }
      
   int OsgModelWrapper::GetBoneCount() const
   {
      return mCache->GetBoneCount();
   }
      
   int OsgModelWrapper::GetMaterialCount() const
   {
      return mCache->GetMaterialCount();
   }
      
   int OsgModelWrapper::GetMeshCount() const
   {
      return mCache->GetMeshCount();
   }

   dtAnim::AnimationInterface* OsgModelWrapper::GetAnimationByIndex(int index) const
   {
      return mCache->GetAnimationByIndex(index);
   }
      
   dtAnim::BoneInterface* OsgModelWrapper::GetBoneByIndex(int index) const
   {
      return mCache->GetBoneByIndex(index);
   }

   int OsgModelWrapper::GetRootBones(dtAnim::BoneArray& outBones) const
   {
      int results = 0;

      typedef OsgInterfaceObjectCache::BoneMap BoneMap;

      dtAnim::BoneInterface* bone = NULL;
      BoneMap::iterator curIter = mCache->mBones.begin();
      BoneMap::iterator endIter = mCache->mBones.end();
      for (; curIter != endIter; ++curIter)
      {
         bone = curIter->second.get();
         if (bone->GetParentBone() == NULL)
         {
            outBones.push_back(bone);
            ++results;
         }
      }

      return results;
   }

   void OsgModelWrapper::UpdateInterfaceObjects()
   {
      mCache->Update();

      Internal_UpdateAnimatorObject();
   }

   void OsgModelWrapper::UpdateAnimationInterfaceObjects()
   {
      mCache->UpdateAnimations();

      Internal_UpdateAnimatorObject();
   }

   void OsgModelWrapper::UpdateBoneInterfaceObjects()
   {
      mCache->UpdateSkeleton();
      mCache->UpdateBones();
   }

   void OsgModelWrapper::UpdateMaterialInterfaceObjects()
   {
      mCache->UpdateMaterials();
   }

   void OsgModelWrapper::UpdateMeshInterfaceObjects()
   {
      mCache->UpdateMeshes();
   }

   void OsgModelWrapper::ClearInterfaceObjects()
   {
      mCache->Clear();
   }

   void OsgModelWrapper::SetLODLevel(float level)
   {
      // TODO:
   }

   void OsgModelWrapper::UpdateAnimation(float deltaTime)
   {
      if (mAnimator.valid())
      {
         mAnimator->Update(deltaTime);
      }
   }
      
   void OsgModelWrapper::ClearAllAnimations(float delay)
   {
      if (mAnimator.valid())
      {
         mAnimator->ClearAll(delay);
      }
   }

   osg::BoundingBox OsgModelWrapper::GetBoundingBox()
   {
      osg::BoundingBox box;

      if (mNode.valid())
      {
         typedef OsgInterfaceObjectCache::BaseClass::MeshMap MeshMap;
         OsgMesh* curMesh = NULL;
         MeshMap::iterator curIter = mCache->mMeshes.begin();
         MeshMap::iterator endIter = mCache->mMeshes.end();
         for (; curIter != endIter; ++curIter)
         {
            curMesh = curIter->second.get();
            if (curMesh->IsVisible())
            {
               box.expandBy(curMesh->GetBoundingBox());
            }
         }
      }
      
      return box;
   }

   void OsgModelWrapper::SetScale(float scale)
   {
      // Ensure scale never goes to 0, to prevent the NAN plague.
      if(scale <= 0.0f)
      {
         scale = 0.001f;
      }

      float prevScale = mScale;
      mScale = scale;

      // If the previous scale was not 1...
      if(prevScale != 1.0f)
      {
         // ...reverse its effect by 1/prevScale and then apply the new scale.
         scale = 1.0f/prevScale * scale;
      }

      OsgSkeleton* skel = mCache->mSkel.get();
      if (skel != NULL)
      {
         osg::Vec3 scale(mScale, mScale, mScale);
         skel->GetOsgSkeleton()->setMatrix(skel->GetOsgSkeleton()->getMatrix().scale(scale));
      }
   }

   float OsgModelWrapper::GetScale() const
   {
      return mScale;
   }

   void OsgModelWrapper::SetHardwareMode(bool hardwareMode, bool forced)
   {
      if (forced || mHardwareMode != hardwareMode)
      {
         dtCore::RefPtr<dtAnim::OsgNodeBuilder> nodeBuilder = new dtAnim::OsgNodeBuilder;
         if (nodeBuilder->EnsureMode(*this, *mNode, hardwareMode))
         {
            mHardwareMode = hardwareMode;
         }
      }
   }

   bool OsgModelWrapper::IsHardwareMode() const
   {
      return mHardwareMode;
   }

   void OsgModelWrapper::HandleModelResourceUpdate(dtAnim::ModelResourceType resourceType)
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

   void OsgModelWrapper::HandleModelUpdated()
   {
      UpdateInterfaceObjects();
   }

   void OsgModelWrapper::Internal_UpdateAnimatorObject()
   {
      if (mCache->GetOsgAnimationManager() != NULL)
      {
         mAnimator = new dtAnim::OsgAnimator(
            *this, *mCache->GetOsgAnimationManager(), *mCache->GetOsgAnimationManagerNode());
      }
   }

}
