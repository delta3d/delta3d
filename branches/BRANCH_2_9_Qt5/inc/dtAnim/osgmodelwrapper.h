#ifndef __DELTA_OSGMODELWRAPPER_H__
#define __DELTA_OSGMODELWRAPPER_H__

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/export.h>
#include <dtAnim/baseinterfaceobjectcache.h>
#include <dtAnim/basemodelwrapper.h>
#include <dtAnim/osganimator.h>
#include <dtAnim/osgmodeldata.h>
#include <dtAnim/osgobjects.h>



////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
namespace dtAnim
{
   class OsgAnimation;
   class OsgBone;
   class OsgMaterial;
   class OsgMesh;
   class OsgModelResourceFinder;
   class OsgModelWrapper;
   class OsgSkeleton;



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   typedef dtAnim::BaseInterfaceObjectCache<OsgModelWrapper, OsgAnimation, OsgBone, OsgMaterial, OsgMesh, OsgSkeleton>
      BaseOsgInterfaceObjectCache;
   class DT_ANIM_EXPORT OsgInterfaceObjectCache : public BaseOsgInterfaceObjectCache
   {
   public:
      typedef dtAnim::BaseOsgInterfaceObjectCache BaseClass;

      OsgInterfaceObjectCache(OsgModelWrapper& model);

      dtAnim::OsgModelWrapper* GetOsgModelWrapper();

      virtual void Update();
      virtual void UpdateAnimations();
      virtual void UpdateBones();
      virtual void UpdateMaterials();
      virtual void UpdateMeshes();
      virtual void UpdateSkeleton();

      virtual void Clear();

      dtAnim::OsgAnimation* GetAnimationByIndex(int index);
      dtAnim::OsgBone* GetBoneByIndex(int index);

      osgAnimation::BasicAnimationManager* GetOsgAnimationManager();
      osg::Node* GetOsgAnimationManagerNode();

   protected:
      virtual ~OsgInterfaceObjectCache();

      typedef OsgModelResourceFinder ResFinder;
      void Internal_UpdateAnimations(ResFinder& finder);
      void Internal_UpdateBones(ResFinder& finder);
      void Internal_UpdateMaterials(ResFinder& finder);
      void Internal_UpdateMeshes(ResFinder& finder);
      void Internal_UpdateSkeleton(ResFinder& finder);

   private:
      typedef std::map<int, dtAnim::OsgAnimation*> IDAnimMap;
      typedef std::map<int, dtAnim::OsgBone*> IDBoneMap;

      IDAnimMap mIDAnimMap;
      IDBoneMap mIDBoneMap;

      dtCore::RefPtr<osgAnimation::BasicAnimationManager> mAnimManager;
      dtCore::RefPtr<osg::Node> mAnimNode;

      friend OsgModelWrapper;
   };



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT OsgModelWrapper : public dtAnim::BaseModelWrapper
   {
   public:
      typedef dtAnim::BaseModelWrapper BaseClass;

      OsgModelWrapper(OsgModelData& modelData);

      virtual dtCore::RefPtr<osg::Node> CreateDrawableNode(bool immediate = true);
      
      virtual osg::Node* GetDrawableNode();

      virtual dtAnim::OsgModelData* GetOsgModelData();

      virtual dtAnim::AnimationUpdaterInterface* GetAnimator();
      dtAnim::OsgAnimator* GetOsgAnimator();

      void SetModel(osg::Node* model);

      osg::Node* GetModel();
      const osg::Node* GetModel() const;

      virtual dtAnim::AnimationInterface* GetAnimation(const std::string& name) const;
      virtual int GetAnimations(dtAnim::AnimationArray& outAnims) const;

      virtual dtAnim::BoneInterface* GetBone(const std::string& name) const;
      virtual int GetBones(dtAnim::BoneArray& outBones) const;

      virtual dtAnim::MaterialInterface* GetMaterial(const std::string& name) const;
      virtual int GetMaterials(dtAnim::MaterialArray& outMaterials) const;

      virtual dtAnim::MeshInterface* GetMesh(const std::string& name) const;
      virtual int GetMeshes(dtAnim::MeshArray& outMeshes) const;

      virtual dtAnim::SkeletonInterface* GetSkeleton();
      virtual const dtAnim::SkeletonInterface* GetSkeleton() const;

      virtual int GetAnimationCount() const;
      virtual int GetBoneCount() const;
      virtual int GetMaterialCount() const;
      virtual int GetMeshCount() const;

      virtual dtAnim::AnimationInterface* GetAnimationByIndex(int index) const;
      virtual dtAnim::BoneInterface* GetBoneByIndex(int index) const;

      /**
       * Convenience method for accessing the root bones of the model.
       */
      virtual int GetRootBones(dtAnim::BoneArray& outBones) const;

      void UpdateInterfaceObjects();
      void UpdateAnimationInterfaceObjects();
      void UpdateBoneInterfaceObjects();
      void UpdateMaterialInterfaceObjects();
      void UpdateMeshInterfaceObjects();
      void ClearInterfaceObjects();

      virtual void SetLODLevel(float level);

      virtual void UpdateAnimation(float deltaTime);
      
      virtual void ClearAllAnimations(float delay = 0.0f);

      virtual osg::BoundingBox GetBoundingBox();

      /**
       * Set the absolute scale relative to the underlying model's original scale.
       * Setting a scale of 1 should have the effect of resetting the original model scale.
       */
      virtual void SetScale(float scale);
      virtual float GetScale() const;

      void SetHardwareMode(bool hardwareMode, bool forced = false);
      bool IsHardwareMode() const;

      /**
       * Method to notify this wrapper object of external
       * changes to its associated OSG objects.
       * @param resourceType Type of resource that has changed and that needs mappings updated.
       */
      virtual void HandleModelResourceUpdate(dtAnim::ModelResourceType resourceType);
      
      /**
       * Method to nofity this object that the associated drawable model has been modified/rebuilt.
       */
      virtual void HandleModelUpdated();

   protected:
      virtual ~OsgModelWrapper();

      void Internal_UpdateAnimatorObject();

   private:
      bool mHardwareMode;
      float mScale;
      dtCore::RefPtr<dtAnim::OsgInterfaceObjectCache> mCache;
      dtCore::RefPtr<dtAnim::OsgAnimator> mAnimator;
      dtCore::RefPtr<osg::Node> mNode;
   };
}

#endif
