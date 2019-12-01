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

#ifndef __DELTA_CAL3DOBJECTS_H__
#define __DELTA_CAL3DOBJECTS_H__

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
// DELTA3D
#include <dtAnim/export.h>
#include <dtUtil/referencedinterface.h>
#include <dtCore/refptr.h>
#include <dtCore/observerptr.h>
#include <dtAnim/animationinterface.h>
#include <dtAnim/boneinterface.h>
#include <dtAnim/hardwaresubmesh.h>
#include <dtAnim/materialinterface.h>
#include <dtAnim/meshinterface.h>
#include <dtAnim/morphinterface.h>
#include <dtAnim/skeletoninterface.h>
#include <dtAnim/submesh.h>
// OSG
#include <osg/Vec4>
// CAL3D
DT_DISABLE_WARNING_ALL_START
#include <cal3d/animation.h>
#include <cal3d/bone.h>
#include <cal3d/corematerial.h>
#include <cal3d/coremesh.h>
#include <cal3d/coreskeleton.h>
#include <cal3d/coresubmorphtarget.h>
#include <cal3d/coretrack.h>
#include <cal3d/hardwaremodel.h>
#include <cal3d/mesh.h>
#include <cal3d/skeleton.h>
DT_DISABLE_WARNING_END



namespace dtAnim
{
   class Cal3DModelWrapper;



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT Cal3dBone : public osg::Referenced, public dtAnim::BoneInterface
   {
   public:
      DT_DECLARE_VIRTUAL_REF_INTERFACE_INLINE

      Cal3dBone(Cal3DModelWrapper& model, CalBone& bone);

      virtual int GetID() const;

      virtual const std::string& GetName() const;

      virtual BoneInterface* GetParentBone() const;

      virtual int GetChildBones(dtAnim::BoneArray& outBones) const;

      virtual void GetAbsoluteMatrix(osg::Matrix& outMatrix) const;

      virtual osg::Quat GetAbsoluteRotation() const;
      virtual osg::Vec3 GetAbsoluteTranslation() const;

      virtual osg::Quat GetRelativeRotation() const;

      virtual osg::Quat GetAbsoluteRotationForKeyframe(const dtAnim::AnimationInterface& anim, dtAnim::Keyframe keyframe) const;

      CalBone* GetCalBone();
      const CalBone* GetCalBone() const;

   protected:
      virtual ~Cal3dBone();

   private:
      dtCore::ObserverPtr<Cal3DModelWrapper> mModel;
      CalBone* mBone;
      int mID;
   };



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT Cal3dSkeleton : public osg::Referenced, virtual public dtAnim::SkeletonInterface
   {
   public:
      DT_DECLARE_VIRTUAL_REF_INTERFACE_INLINE

      Cal3dSkeleton(Cal3DModelWrapper& model, CalSkeleton& skeleton);

      virtual dtCore::RefPtr<dtAnim::BoneInterface> GetBone(const std::string& name);
      virtual int GetBones(dtAnim::BoneArray& outBones);

      virtual int GetBoneCount() const;

      virtual int GetRootBones(dtAnim::BoneArray& outBones);

      CalSkeleton* GetCalSkeleton();
      const CalSkeleton* GetCalSkeleton() const;

      CalCoreSkeleton* GetCalCoreSkeleton();
      const CalCoreSkeleton* GetCalCoreSkeleton() const;

   protected:
      virtual ~Cal3dSkeleton();

   private:
      dtCore::ObserverPtr<Cal3DModelWrapper> mModel;
      CalSkeleton* mSkeleton;
   };


   
   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT Cal3dMaterial : public osg::Referenced, virtual public dtAnim::MaterialInterface
   {
   public:
      DT_DECLARE_VIRTUAL_REF_INTERFACE_INLINE

      Cal3dMaterial(Cal3DModelWrapper& model, CalCoreMaterial& material);

      virtual int GetID() const;

      virtual void SetName(const std::string& name);
      virtual const std::string& GetName() const;

      virtual int GetTextureCount() const;
      
      virtual void SetTexture(int index, osg::Texture* textureMap);
      virtual osg::Texture* GetTexture(int index) const;

      virtual const std::string GetTextureFile(int index) const;
      
      osg::Vec4 GetAmbientColor() const;
      osg::Vec4 GetDiffuseColor() const;
      osg::Vec4 GetSpecularColor() const;
      float GetShininess() const;

      CalCoreMaterial* GetCalCoreMaterial();
      const CalCoreMaterial* GetCalCoreMaterial() const;

   protected:
      virtual ~Cal3dMaterial();

   private:
      dtCore::ObserverPtr<Cal3DModelWrapper> mModel;
      CalCoreMaterialPtr mMaterial;
      int mID;
   };



   class Cal3dSubmesh;
   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT Cal3dMorphTarget : public osg::Referenced, virtual public dtAnim::MorphTargetInterface
   {
   public:
      DT_DECLARE_VIRTUAL_REF_INTERFACE_INLINE

      Cal3dMorphTarget(dtAnim::Cal3dSubmesh& submesh, CalCoreSubMorphTarget& morphTarget, int id);

      int GetID() const;

      virtual void SetName(const std::string& name);
      virtual const std::string& GetName() const;

      virtual void SetWeight(float weight);
      virtual float GetWeight() const;

      CalCoreSubMorphTarget* GetCalMorphTarget();
      const CalCoreSubMorphTarget* GetCalMorphTarget() const;

   protected:
      virtual ~Cal3dMorphTarget();

   private:
      dtCore::ObserverPtr<dtAnim::Cal3dSubmesh> mSubmesh;
      CalCoreSubMorphTarget* mMorphTarget;
      int mID;
      std::string mName;
   };

   

   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT Cal3dBaseSubmesh : public osg::Referenced, virtual public dtAnim::SubmeshInterface
   {
   public:
      DT_DECLARE_VIRTUAL_REF_INTERFACE_INLINE

      Cal3dBaseSubmesh() {}

   protected:
      virtual ~Cal3dBaseSubmesh() {}
   };



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT Cal3dSubmesh : public dtAnim::Cal3dBaseSubmesh
   {
   public:
      typedef dtAnim::Cal3dBaseSubmesh BaseClass;

      Cal3dSubmesh(Cal3DModelWrapper& model, CalSubmesh& submesh);

      virtual int GetVertexCount() const;
      virtual int GetFaceCount() const;
      virtual int GetMorphTargetWeightCount() const;

      // Returns number of vertices (num floats / 3)
      virtual int GetFaceData(dtAnim::IndexArray& outData);
      virtual int GetVertexData(dtAnim::FloatArray& outData, int stride);
      virtual int GetNormalData(dtAnim::FloatArray& outData, int stride);
      virtual int GetMorphTargetWeightData(dtAnim::FloatArray& outData);

      virtual int GetMorphTargets(dtAnim::MorphTargetArray& submorphTargets) const;

      // Primitve array related methods
      int GetFaces(int* outData);
      int GetVertices(float* outData, int stride = 0);
      int GetNormals(float* outData, int stride = 0);
      int GetTextureCoords(int textureUnit, float* outData, int stride = 0);

      void SetDrawable(dtAnim::SubmeshDrawable* drawable);
      dtAnim::SubmeshDrawable* GetDrawable();
      const dtAnim::SubmeshDrawable* GetDrawable() const;

      CalSubmesh* GetCalSubmesh();
      const CalSubmesh* GetCalSubmesh() const;

   protected:
      virtual ~Cal3dSubmesh();

   private:
      dtCore::ObserverPtr<Cal3DModelWrapper> mModel;
      dtCore::ObserverPtr<dtAnim::SubmeshDrawable> mDrawable;
      CalSubmesh* mSubmesh;
   };



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT Cal3dHardwareSubmesh : public dtAnim::Cal3dBaseSubmesh
   {
   public:
      typedef dtAnim::Cal3dBaseSubmesh BaseClass;

      Cal3dHardwareSubmesh(CalHardwareModel& model, int index);

      int GetID() const;

      bool IsValid() const;

      virtual int GetVertexCount() const;
      virtual int GetFaceCount() const;
      virtual int GetMorphTargetWeightCount() const;

      // Returns number of vertices (num floats / 3)
      virtual int GetFaceData(dtAnim::IndexArray& outData);
      virtual int GetVertexData(dtAnim::FloatArray& outData, int stride);
      virtual int GetNormalData(dtAnim::FloatArray& outData, int stride);
      virtual int GetMorphTargetWeightData(dtAnim::FloatArray& outData);

      virtual int GetMorphTargets(dtAnim::MorphTargetArray& submorphTargets) const;

      void SetDrawable(dtAnim::HardwareSubmeshDrawable* drawable);
      dtAnim::HardwareSubmeshDrawable* GetDrawable();
      const dtAnim::HardwareSubmeshDrawable* GetDrawable() const;

   protected:
      virtual ~Cal3dHardwareSubmesh();

   private:
      CalHardwareModel* mModel;
      int mID;

      typedef std::vector<CalHardwareModel::CalHardwareMesh> CalHardwareMeshArray;
      CalHardwareMeshArray* mSubmeshArray;

      dtCore::ObserverPtr<dtAnim::HardwareSubmeshDrawable> mDrawable;
   };
   


   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT Cal3dBaseMesh : public osg::Referenced, virtual public dtAnim::MeshInterface
   {
   public:
      DT_DECLARE_VIRTUAL_REF_INTERFACE_INLINE

         Cal3dBaseMesh() {}

   protected:
      virtual ~Cal3dBaseMesh() {}
   };


   
   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT Cal3dMesh : public dtAnim::Cal3dBaseMesh
   {
   public:
      typedef dtAnim::Cal3dBaseMesh BaseClass;

      Cal3dMesh(Cal3DModelWrapper& model, CalMesh& mesh);

      virtual int GetID() const;
      
      virtual void SetName(const std::string& name);
      virtual const std::string& GetName() const;

      virtual void SetVisible(bool visible);
      virtual bool IsVisible() const;

      virtual int GetVertexCount() const;
      virtual int GetFaceCount() const;

      virtual int GetSubmeshCount() const;
      virtual int GetSubmeshes(dtAnim::SubmeshArray& outSubmeshes) const;

      int GetSubmeshVertexCount(int submeshIndex) const;
      int GetSubmeshFaceCount(int submeshIndex) const;

      CalMesh* GetCalMesh();
      const CalMesh* GetCalMesh() const;

      CalCoreMesh* GetCalCoreMesh();
      const CalCoreMesh* GetCalCoreMesh() const;

   protected:
      virtual ~Cal3dMesh();

   private:
      dtCore::ObserverPtr<Cal3DModelWrapper> mModel;
      CalMesh* mMesh;
      int mID;
      bool mVisible;
   };


   
   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT Cal3dHardwareMesh : public dtAnim::Cal3dBaseMesh
   {
   public:
      typedef dtAnim::Cal3dBaseMesh BaseClass;

      Cal3dHardwareMesh(Cal3DModelWrapper& model, CalHardwareModel& hardwareModel, int id);

      virtual int GetID() const;
      
      virtual void SetName(const std::string& name);
      virtual const std::string& GetName() const;

      virtual void SetVisible(bool visible);
      virtual bool IsVisible() const;

      virtual int GetVertexCount() const;
      virtual int GetFaceCount() const;

      virtual int GetSubmeshCount() const;
      virtual int GetSubmeshes(dtAnim::SubmeshArray& outSubmeshes) const;

      int GetSubmeshVertexCount(int submeshIndex) const;
      int GetSubmeshFaceCount(int submeshIndex) const;

      CalHardwareModel* GetCalHardwareModel();
      const CalHardwareModel* GetCalHardwareModel() const;

      void SetDrawable(dtAnim::HardwareSubmeshDrawable* drawable);
      dtAnim::HardwareSubmeshDrawable* GetDrawable();
      const dtAnim::HardwareSubmeshDrawable* GetDrawable() const;

   protected:
      virtual ~Cal3dHardwareMesh();

   private:
      dtCore::ObserverPtr<Cal3DModelWrapper> mModel;
      CalHardwareModel* mHardwareModel;
      int mID;
      bool mVisible;
      std::string mName;

      dtCore::ObserverPtr<dtAnim::HardwareSubmeshDrawable> mDrawable;
   };


   
   class Cal3dTrack;
   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT Cal3dAnimation : public osg::Referenced, virtual public dtAnim::AnimationInterface
   {
   public:
      DT_DECLARE_VIRTUAL_REF_INTERFACE_INLINE

      Cal3dAnimation(Cal3DModelWrapper& model, CalCoreAnimation& animation);

      Cal3DModelWrapper* GetModelWrapper() const;

      virtual int GetID() const;

      virtual void SetName(const std::string& name);
      virtual const std::string& GetName() const;

      virtual void SetPlayMode(const dtAnim::PlayModeEnum& playMode);
      virtual const dtAnim::PlayModeEnum& GetPlayMode() const;

      virtual const dtAnim::AnimationStateEnum& GetState() const;

      CalAnimation* GetCalAnimation();
      const CalAnimation* GetCalAnimation() const;

      CalCoreAnimation* GetCalCoreAnimation();
      const CalCoreAnimation* GetCalCoreAnimation() const;

      virtual bool PlayAction(float delayIn, float delayOut,
         float weight = 1.0f, bool autoLock = false);

      virtual bool PlayCycle(float weight, float delay);
      
      virtual bool ClearAction();

      virtual bool ClearCycle(float delay);

      virtual bool Clear(float delay);

      void SetSpeedFactor(float speedFactor);
      float GetSpeedFactor() const;

      virtual float GetWeight() const;

      virtual float GetTime() const;

      virtual float GetDuration() const;
      
      virtual int GetKeyframeCount() const;

      virtual int GetKeyframeCountForBone(const dtAnim::BoneInterface& bone) const;

      virtual int GetTrackCount() const;

      virtual int GetTracks(dtAnim::TrackArray& outTracks);

      virtual bool HasTrackForBone(const dtAnim::BoneInterface& bone) const;
   
      virtual osg::Quat GetKeyframeQuat(const dtAnim::BoneInterface& bone, dtAnim::Keyframe keyframe) const;

      CalAnimation* GetAnimationInstance() const;

   protected:
      virtual ~Cal3dAnimation();

   private:
      dtCore::ObserverPtr<Cal3DModelWrapper> mModel;
      CalCoreAnimation* mAnim;
      int mID;

      dtAnim::TrackArray mTracks;
   };



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT Cal3dTrack : public osg::Referenced, public dtAnim::TrackInterface
   {
   public:
      DT_DECLARE_VIRTUAL_REF_INTERFACE_INLINE

      Cal3dTrack(dtAnim::Cal3dAnimation& anim, CalCoreTrack& track);

      virtual dtAnim::BoneInterface* GetBone() const;

      virtual bool GetTransformAtTime(float time, dtAnim::Location& outLocation, dtAnim::Rotation& outRotation) const;

   protected:
      virtual ~Cal3dTrack();

   private:
      dtCore::ObserverPtr<dtAnim::Cal3dAnimation> mAnim;
      CalCoreTrack* mTrack;
   };

}

#endif
