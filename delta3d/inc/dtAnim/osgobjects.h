#ifndef __DELTA_OSGOBJECTS_H__
#define __DELTA_OSGOBJECTS_H__

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/export.h>
#include <dtAnim/animationinterface.h>
#include <dtAnim/boneinterface.h>
#include <dtAnim/materialinterface.h>
#include <dtAnim/meshinterface.h>
#include <dtAnim/skeletoninterface.h>
#include <dtAnim/osganimator.h>
#include <dtAnim/osgmodelwrapper.h>

#include <osg/Geometry>
#include <osg/StateSet>
#include <osgAnimation/Animation>
#include <osgAnimation/Bone>
#include <osgAnimation/MorphGeometry>
#include <osgAnimation/RigGeometry>
#include <osgAnimation/Skeleton>



namespace dtAnim
{
   typedef std::vector<dtCore::RefPtr<osgAnimation::Channel> > OsgChannelArray;



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT OsgSkeleton : public osg::Referenced, virtual public dtAnim::SkeletonInterface
   {
   public:
      DT_DECLARE_VIRTUAL_REF_INTERFACE_INLINE

      OsgSkeleton(OsgModelWrapper& model, osgAnimation::Skeleton& skel);

      virtual dtCore::RefPtr<dtAnim::BoneInterface> GetBone(const std::string& name);
      virtual int GetBones(dtAnim::BoneArray& outBones);

      virtual int GetBoneCount() const;
      
      virtual int GetRootBones(dtAnim::BoneArray& outBones);

      osgAnimation::Skeleton* GetOsgSkeleton();
      const osgAnimation::Skeleton* GetOsgSkeleton() const;

   protected:
      virtual ~OsgSkeleton();

      dtCore::RefPtr<dtAnim::OsgModelWrapper> mModel;
      dtCore::RefPtr<osgAnimation::Skeleton> mSkel;
   };



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT OsgBone : public osg::Referenced, virtual public dtAnim::BoneInterface
   {
   public:
      DT_DECLARE_VIRTUAL_REF_INTERFACE_INLINE

      OsgBone(OsgModelWrapper& model, osgAnimation::Bone& bone);

      virtual int GetID() const;

      virtual const std::string& GetName() const;

      virtual dtAnim::BoneInterface* GetParentBone() const;

      virtual int GetChildBones(dtAnim::BoneArray& outBones) const;

      virtual void GetAbsoluteMatrix(osg::Matrix& outMatrix) const;

      virtual osg::Quat GetAbsoluteRotation() const;
      virtual osg::Vec3 GetAbsoluteTranslation() const;

      virtual osg::Quat GetRelativeRotation() const;
      
      virtual osg::Quat GetAbsoluteRotationForKeyframe(const dtAnim::AnimationInterface& anim, dtAnim::Keyframe keyframe) const;

      osgAnimation::Bone* GetOsgBone();
      const osgAnimation::Bone* GetOsgBone() const;

   protected:
      virtual ~OsgBone();

      dtCore::RefPtr<dtAnim::OsgModelWrapper> mModel;
      dtCore::RefPtr<osgAnimation::Bone> mBone;
      int mID;
   };



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT OsgMaterial : public osg::Referenced, virtual public dtAnim::MaterialInterface
   {
   public:
      DT_DECLARE_VIRTUAL_REF_INTERFACE_INLINE

      OsgMaterial(OsgModelWrapper& model, osg::StateSet& stateSet);

      virtual int GetID() const;

      virtual void SetName(const std::string& name);
      virtual const std::string& GetName() const;

      virtual int GetTextureCount() const;

      virtual void SetTexture(int index, osg::Texture* textureMap);
      virtual osg::Texture* GetTexture(int index) const;

      virtual const std::string GetTextureFile(int index) const;

      osg::StateSet* GetOsgStateSet();
      const osg::StateSet* GetOsgStateSet() const;

   protected:
      virtual ~OsgMaterial();

      dtCore::RefPtr<dtAnim::OsgModelWrapper> mModel;
      dtCore::RefPtr<osg::StateSet> mStateSet;
      int mID;
   };


   
   class OsgSubmesh;

   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT OsgMorphTarget : public osg::Referenced, virtual public dtAnim::MorphTargetInterface
   {
   public:
      DT_DECLARE_VIRTUAL_REF_INTERFACE_INLINE

      OsgMorphTarget(dtAnim::OsgSubmesh& submesh, osgAnimation::MorphGeometry& morphGeom, int id);

      int GetID() const;
      
      virtual void SetName(const std::string& name);
      virtual const std::string& GetName() const;

      virtual void SetWeight(float weight);
      virtual float GetWeight() const;

      osgAnimation::MorphGeometry::MorphTarget* GetOsgMorphTarget();
      const osgAnimation::MorphGeometry::MorphTarget* GetOsgMorphTarget() const;

   protected:
      virtual ~OsgMorphTarget();

   private:
      dtCore::RefPtr<dtAnim::OsgSubmesh> mSubmesh;
      dtCore::RefPtr<osgAnimation::MorphGeometry> mMorphGeom;
      int mID;
   };



   class OsgMesh;

   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT OsgSubmesh : public osg::Referenced, virtual public dtAnim::SubmeshInterface
   {
   public:
      DT_DECLARE_VIRTUAL_REF_INTERFACE_INLINE

      OsgSubmesh(OsgMesh& parentMesh, osg::Geometry& submesh);

      int GetID() const;

      std::string GetName() const;

      virtual int GetVertexCount() const;
      virtual int GetFaceCount() const;
      virtual int GetMorphTargetWeightCount() const;

      virtual int GetFaceData(dtAnim::IndexArray& outData);
      virtual int GetVertexData(dtAnim::FloatArray& outData, int stride);
      virtual int GetNormalData(dtAnim::FloatArray& outData, int stride);
      virtual int GetMorphTargetWeightData(dtAnim::FloatArray& outData);

      virtual int GetMorphTargets(dtAnim::MorphTargetArray& submorphTargets) const;
      
      osgAnimation::RigGeometry* GetOsgRigGeometry();
      const osgAnimation::RigGeometry* GetOsgRigGeometry() const;

      osgAnimation::MorphGeometry* GetOsgMorphGeometry();
      const osgAnimation::MorphGeometry* GetOsgMorphGeometry() const;

   protected:
      virtual ~OsgSubmesh();
      
      dtCore::RefPtr<dtAnim::OsgMesh> mParentMesh;
      dtCore::RefPtr<osg::Geometry> mSubmesh;
   };


   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT OsgMesh : public osg::Referenced, virtual public dtAnim::MeshInterface
   {
   public:
      DT_DECLARE_VIRTUAL_REF_INTERFACE_INLINE

      OsgMesh(OsgModelWrapper& model, osg::Geode& geode);

      void SetID(int id);
      virtual int GetID() const;

      virtual void SetName(const std::string& name);
      virtual const std::string& GetName() const;

      virtual void SetVisible(bool visible);
      virtual bool IsVisible() const;

      virtual int GetVertexCount() const;
      virtual int GetFaceCount() const;

      virtual int GetSubmeshCount() const;
      virtual int GetSubmeshes(dtAnim::SubmeshArray& outSubmeshes) const;
   
      virtual int GetMorphTargets(dtAnim::MorphTargetArray& morphTargets) const;

      osg::BoundingBox GetBoundingBox() const;

      bool IsHardwareMode() const;

      osg::Geode* GetOsgGeode();
      const osg::Geode* GetOsgGeode() const;

      void ApplyMaterial(OsgMaterial& material);

   protected:
      virtual ~OsgMesh();

      dtCore::RefPtr<dtAnim::OsgModelWrapper> mModel;
      dtCore::RefPtr<osg::Geode> mGeode;
      osg::Node::NodeMask mOriginalMask;
      int mID;
   };



   class OsgAnimation;

   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   /**
    * This class wraps a collection of animation channels associated with
    * a target object.
    */
   class OsgTrack : public osg::Referenced, virtual TrackInterface
   {
   public:
      OsgTrack(OsgAnimation& anim, osgAnimation::Target& target, const std::string& targetName);

      osgAnimation::Target* GetOsgTarget();
      const osgAnimation::Target* GetOsgTarget() const;

      const std::string& GetOsgTargetName() const;

      /**
       * Access all the channels related to the track target.
       */
      int GetOsgChannels(OsgChannelArray& outChannels) const;

      virtual dtAnim::BoneInterface* GetBone() const;

      /**
       * Returns the location and rotaion information of a bone at a specified time.
       * @param time The time in seconds of the animation between 0.0 and animation duration.
       * @param outLocation Variable to receive the translation information.
       * @param outRotation Variable to receive the rotational information.
       * @return TRUE if 
       */
      virtual bool GetTransformAtTime(float time, dtAnim::Location& outLocation, dtAnim::Rotation& outRotation) const;

   protected:
      virtual ~OsgTrack();

   private:
      dtCore::RefPtr<OsgAnimation> mParentAnim;
      dtCore::RefPtr<osgAnimation::Target> mTarget;
      std::string mTargetName;
   };



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT OsgAnimation : public osg::Referenced, virtual public dtAnim::AnimationInterface
   {
   public:
      DT_DECLARE_VIRTUAL_REF_INTERFACE_INLINE

      OsgAnimation(OsgModelWrapper& model, osgAnimation::Animation& anim);

      virtual int GetID() const;

      virtual void SetName(const std::string& name);
      virtual const std::string& GetName() const;

      virtual void SetPlayMode(const dtAnim::PlayModeEnum& playMode);
      virtual const dtAnim::PlayModeEnum& GetPlayMode() const;

      virtual const dtAnim::AnimationStateEnum& GetState() const;
      
      virtual bool PlayAction(float delayIn, float delayOut,
         float weight = 1.0f, bool autoLock = false);
      
      virtual bool PlayCycle(float weight, float delay);
      
      virtual bool ClearAction();
      
      virtual bool ClearCycle(float delay);

      virtual bool Clear(float delay);

      virtual float GetDuration() const;

      virtual float GetWeight() const;

      virtual float GetTime() const;
      
      virtual int GetKeyframeCount() const;

      virtual int GetKeyframeCountForBone(const dtAnim::BoneInterface& bone) const;

      virtual int GetTrackCount() const;

      virtual int GetTracks(dtAnim::TrackArray& outTracks);

      virtual bool HasTrackForBone(const dtAnim::BoneInterface& bone) const;
      
      virtual osg::Quat GetKeyframeQuat(const dtAnim::BoneInterface& bone, dtAnim::Keyframe keyframe) const;

      void SetSynchronous(bool synchronous);
      bool IsSynchronous() const;

      void SetOriginalDuration(float duration);
      float GetOriginalDuration() const;
      
      void SetOriginalWeight(float weight);
      float GetOriginalWeight() const;

      osgAnimation::Animation* GetOsgAnimation();
      const osgAnimation::Animation* GetOsgAnimation() const;

      int GetOsgChannels(OsgChannelArray& outChannels) const;

      int GetOsgChannelsForBone(const dtAnim::BoneInterface& bone, OsgChannelArray& outChannels) const;
      
      int GetOsgChannelsForBoneTransforms(const dtAnim::BoneInterface& bone, OsgChannelArray& outChannels) const;

      OsgAnimator* GetAnimator();
      const OsgAnimator* GetAnimator() const;

      OsgModelWrapper* GetModel();
      const OsgModelWrapper* GetModel() const;

      void Reset();

   protected:
      virtual ~OsgAnimation();

      dtCore::RefPtr<dtAnim::OsgModelWrapper> mModel;
      dtCore::RefPtr<osgAnimation::Animation> mAnim;
      int mID;
      bool mSynchronous;
      float mOriginalWeight;
      float mOriginalDuration;
   };

}

#endif
