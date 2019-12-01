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
 * Erik Johnson
 */

#ifndef __DELTA_CAL3DWRAPPER_H__
#define __DELTA_CAL3DWRAPPER_H__

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/export.h>
#include <dtAnim/baseinterfaceobjectcache.h>
#include <dtAnim/cal3danimator.h>
#include <dtAnim/cal3dobjects.h>
#include <dtAnim/basemodelwrapper.h>
#include <dtCore/observerptr.h>
#include <dtUtil/deprecationmgr.h>
#include <dtUtil/warningdisable.h>

DT_DISABLE_WARNING_ALL_START
#include <cal3d/model.h>
#include <cal3d/coremodel.h>
#include <cal3d/renderer.h>
   
#include <osg/Quat>                   // for return type
#include <osg/Referenced>
#include <osg/Vec4>
#include <osg/Vec3>
#include <osg/BoundingBox>

#include <cal3d/global.h>
DT_DISABLE_WARNING_END


#include <vector>                     // for param type



////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
class CalCoreMaterial;
class CalCoreModel;
class CalHardwareModel;
class CalMixer;
class CalModel;
class CalMorphTargetMixer;
class CalPhysique;
class CalRenderer;
class CalSpringSystem;



namespace dtAnim
{
   class Cal3DAnimator;
   class Cal3DModelWrapper;

   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   typedef dtAnim::BaseInterfaceObjectCache<Cal3DModelWrapper, Cal3dAnimation, Cal3dBone, Cal3dMaterial, Cal3dBaseMesh, Cal3dSkeleton>
      BaseCal3dInterfaceObjectCache;
   class DT_ANIM_EXPORT Cal3dInterfaceObjectCache : public BaseCal3dInterfaceObjectCache
   {
   public:
      typedef BaseCal3dInterfaceObjectCache BaseClass;

      Cal3dInterfaceObjectCache(Cal3DModelWrapper& model);

      //void Update();
      virtual void UpdateAnimations();
      virtual void UpdateBones();
      virtual void UpdateMaterials();
      virtual void UpdateMeshes();
      virtual void UpdateSkeleton();

      virtual void Clear();

      Cal3dAnimation* GetAnimationByID(int index);
      Cal3dBone* GetBoneByID(int index);

   protected:
      virtual ~Cal3dInterfaceObjectCache();

   private:
      typedef std::map<int, dtCore::ObserverPtr<Cal3dAnimation> > IDAnimMap;
      typedef std::map<int, dtCore::ObserverPtr<Cal3dBone> > IDBoneMap;

      IDAnimMap mIDAnimMap;
      IDBoneMap mIDBoneMap;

      friend Cal3DModelWrapper;
   };



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////

   /**
    * Wraps the Cal3D CalModel class. It is expected that users will use the
    * Cal3DModelWrapper instead of using the CalModel class directly.
    * To create a new Cal3DModelWrapper:
    * @code
    * dtCore::RefPtr<Cal3DModelWrapper> wrap = new Cal3DModelWrapper( calModel );
    * @endcode
    */
   class DT_ANIM_EXPORT Cal3DModelWrapper : public dtAnim::BaseModelWrapper
   {
   public:
      typedef dtAnim::BaseModelWrapper BaseClass;
      static const float DEFAULT_MINIMUM_BLEND_TIME;

      Cal3DModelWrapper(dtAnim::Cal3DModelData& modelData);

      virtual dtCore::RefPtr<osg::Node> CreateDrawableNode(bool immediate = false);

      virtual osg::Node* GetDrawableNode();

      virtual dtAnim::Cal3DModelData* GetCalModelData() const;

      virtual dtAnim::AnimationUpdaterInterface* GetAnimator();

      dtAnim::Cal3DAnimator* GetCalAnimator();

      void SetCalModel(CalModel* model);

      /**
       * Get a pointer to the internal CalModel.  For advanced users only!
       *
       * Warning! This violates the protective services brought to you by the wrapper.
       * Only modify the CalModel if you know how it will impact the rest of the
       * Delta3D animation system.  You have been warned.
       * @return A pointer to the internal CalModel this class operates on.
       */
      CalModel* GetCalModel();

      /**
       * Get a const pointer to the internal CalModel.  For advanced users only!
       *
       * Warning! This violates the protective services brought to you by the wrapper.
       * Only modify the CalModel if you know how it will impact the rest of the
       * Delta3D animation system.  You have been warned.
       * @return A const pointer to the internal CalModel this class operates on.
       */
      const CalModel* GetCalModel() const;

      CalRenderer* GetCalRenderer();

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
      dtAnim::Cal3dBaseMesh* GetMeshByIndex(int index) const;
      
      virtual int GetRootBones(dtAnim::BoneArray& outBones) const;

      void UpdateInterfaceObjects();
      void UpdateAnimationInterfaceObjects();
      void UpdateBoneInterfaceObjects();
      void UpdateMaterialInterfaceObjects();
      void UpdateMeshInterfaceObjects();
      void ClearInterfaceObjects();

      bool AttachMesh(int meshID);
      bool DetachMesh(int meshID);

      int GetAttachedMeshCount() const;

      void SetMaterialSet(int materialSetID);
      virtual void SetLODLevel(float level);

      /// Update the Cal3D system using the CalModel's update.
      virtual void UpdateAnimation(float deltaTime);
      
      /// Remove all existing animations from the mixer
      virtual void ClearAllAnimations(float delay = 0.0f);

      /************************************************************************/
      bool BeginRenderingQuery();
      void EndRenderingQuery();

      bool SelectMeshSubmesh(int meshID, int submeshID);
      dtCore::RefPtr<dtAnim::Cal3dSubmesh> GetSelectedSubmesh();
      dtCore::RefPtr<dtAnim::Cal3dMaterial> GetSelectedSubmeshMaterial();

      /************************************************************************/

      /// Get a bounding box the encompasses the character in its default pose
      virtual osg::BoundingBox GetBoundingBox();

      /**
       * Set the absolute scale relative to the underlying model's original scale.
       * Setting a scale of 1 should have the effect of resetting the original model scale.
       */
      virtual void SetScale(float scale);
      virtual float GetScale() const;

      /**
       * Method to notify this wrapper object of external
       * changes to its associated Cal3D objects.
       * @param resourceType Type of resource that has changed and that needs mappings updated.
       */
      virtual void HandleModelResourceUpdate(dtAnim::ModelResourceType resourceType);

      /**
       * Method to nofity this object that the associated drawable model has been modified/rebuilt.
       */
      virtual void HandleModelUpdated();

   protected:
      virtual ~Cal3DModelWrapper();

   private:
      float mScale;

      // Root Cal3D object that represents a character instance.
      CalModel*    mCalModel;

      // Updates geometry and texture buffers.
      CalRenderer* mRenderer;

      // Maintains instances and mappings to interface objects that
      // wrap their relevant Cal3D objects/functionality.
      dtCore::RefPtr<Cal3dInterfaceObjectCache> mCache;

      // Manages animation controllers.
      dtCore::RefPtr<dtAnim::Cal3DAnimator> mAnimator;

      // Drawable associated with this model.
      dtCore::RefPtr<osg::Node> mDrawable;

      // State variables that track what submesh
      // is selected during a rendering query.
      int mSelectedMeshID;
      int mSelectedSubmeshID;
   };

} // namespace dtAnim

#endif // __DELTA_CAL3DWRAPPER_H__
