#ifndef __DELTA_BASEINTERFACEOBJECTCACHE_H__
#define __DELTA_BASEINTERFACEOBJECTCACHE_H__

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/export.h>
#include <dtAnim/animationinterface.h>
#include <dtAnim/boneinterface.h>
#include <dtAnim/materialinterface.h>
#include <dtAnim/meshinterface.h>
#include <dtCore/observerptr.h>
#include <dtCore/refptr.h>



namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   template <typename T_ModelWrapper,
      typename T_Anim, typename T_Bone, typename T_Material, typename T_Mesh, typename T_Skeleton>
   class DT_ANIM_EXPORT BaseInterfaceObjectCache : public osg::Referenced
   {
   public:
      BaseInterfaceObjectCache(T_ModelWrapper& model)
         : mModel(&model)
      {}

      virtual void UpdateAnimations() = 0;
      virtual void UpdateBones() = 0;
      virtual void UpdateMaterials() = 0;
      virtual void UpdateMeshes() = 0;
      virtual void UpdateSkeleton() = 0;
      
      virtual void Update()
      {
         if (!mModel.valid())
         {
            return;
         }

         UpdateSkeleton();
         UpdateAnimations();
         UpdateBones();
         UpdateMaterials();
         UpdateMeshes();
      }

      virtual void Clear()
      {
         mAnims.clear();
         mBones.clear();
         mMaterials.clear();
         mMeshes.clear();
         mSkel = NULL;
      }

      T_Anim* GetAnimation(const std::string& name)
      {
         typename AnimMap::iterator iter = mAnims.find(name);
         return iter != mAnims.end() ? iter->second.get() : NULL;
      }

      T_Bone* GetBone(const std::string& name)
      {
         typename BoneMap::iterator iter = mBones.find(name);
         return iter != mBones.end() ? iter->second.get() : NULL;
      }

      T_Material* GetMaterial(const std::string& name)
      {
         typename MaterialMap::iterator iter = mMaterials.find(name);
         return iter != mMaterials.end() ? iter->second.get() : NULL;
      }

      T_Mesh* GetMesh(const std::string& name)
      {
         typename MeshMap::iterator iter = mMeshes.find(name);
         return iter != mMeshes.end() ? iter->second.get() : NULL;
      }

      T_Skeleton* GetSkeleton()
      {
         return mSkel.get();
      }

      int GetAnimations(dtAnim::AnimationArray& outAnims)
      {
         // Count items added, not the total in the array.
         // The array may or may not contain other items.
         int results = 0;

         typename AnimMap::iterator curIter = mAnims.begin();
         typename AnimMap::iterator endIter = mAnims.end();
         for (; curIter != endIter; ++curIter)
         {
            outAnims.push_back(curIter->second.get());
            ++results;
         }

         return results;
      }
   
      int GetBones(dtAnim::BoneArray& outBones)
      {
         // Count items added, not the total in the array.
         // The array may or may not contain other items.
         int results = 0;
      
         typename BoneMap::iterator curIter = mBones.begin();
         typename BoneMap::iterator endIter = mBones.end();
         for (; curIter != endIter; ++curIter)
         {
            outBones.push_back(curIter->second.get());
            ++results;
         }

         return results;
      }
   
      int GetMaterials(dtAnim::MaterialArray& outMaterials)
      {
         // Count items added, not the total in the array.
         // The array may or may not contain other items.
         int results = 0;
      
         typename MaterialMap::iterator curIter = mMaterials.begin();
         typename MaterialMap::iterator endIter = mMaterials.end();
         for (; curIter != endIter; ++curIter)
         {
            outMaterials.push_back(curIter->second.get());
            ++results;
         }

         return results;
      }
   
      int GetMeshes(dtAnim::MeshArray& outMeshes)
      {
         // Count items added, not the total in the array.
         // The array may or may not contain other items.
         int results = 0;
      
         typename MeshMap::iterator curIter = mMeshes.begin();
         typename MeshMap::iterator endIter = mMeshes.end();
         for (; curIter != endIter; ++curIter)
         {
            outMeshes.push_back(curIter->second.get());
            ++results;
         }

         return results;
      }

      int GetAnimationCount() const
      {
         return int(mAnims.size());
      }

      int GetBoneCount() const
      {
         return int(mBones.size());
      }

      int GetMaterialCount() const
      {
         return int(mMaterials.size());
      }

      int GetMeshCount() const
      {
         return int(mMeshes.size());
      }

      typedef std::map<std::string, dtCore::RefPtr<T_Anim> > AnimMap;
      typedef std::map<std::string, dtCore::RefPtr<T_Bone> > BoneMap;
      typedef std::map<std::string, dtCore::RefPtr<T_Material> > MaterialMap;
      typedef std::map<std::string, dtCore::RefPtr<T_Mesh> > MeshMap;

   protected:
      virtual ~BaseInterfaceObjectCache() {}

      dtCore::ObserverPtr<T_ModelWrapper> mModel;
      dtCore::RefPtr<T_Skeleton> mSkel;
      AnimMap mAnims;
      BoneMap mBones;
      MaterialMap mMaterials;
      MeshMap mMeshes;
   };
}

#endif
