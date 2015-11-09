
////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/osgobjects.h>
#include <dtAnim/osganimator.h>
#include <osg/Geode>
#include <osg/Texture2D>
#include <osgAnimation/RigTransformHardware>
#include <osgAnimation/RigTransformSoftware>
#include <algorithm>



namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // HELPER FUNCTIONS
   /////////////////////////////////////////////////////////////////////////////
   int CopyOsgArrayToFloatArray(const osg::Array& sourceArray, int stride, dtAnim::FloatArray& outData)
   {
      int count = sourceArray.getNumElements();
      int numCoords = sourceArray.getDataSize();

      if (stride < 0)
      {
         stride = 0;
      }

      outData.reserve(outData.size() + (count * numCoords) + (count * stride));

      GLfloat* rawArray = (GLfloat*)(sourceArray.getDataPointer());
      int step = stride + numCoords;
      for (int i = 0, v = 0; v < count; ++v, i += step)
      {
         for (int j = 0; j < numCoords; ++j)
         {
            outData[i+j] = rawArray[v+j];
         }
      }

      return count;
   }

   /////////////////////////////////////////////////////////////////////////////
   int GetTriangleCount(osg::Geometry& geom)
   {
      int count = 0;

      typedef const osg::Geometry::PrimitiveSetList PrimitiveList;
      const PrimitiveList& primList = geom.getPrimitiveSetList();

      osg::PrimitiveSet* curPrimSet = NULL;
      PrimitiveList::const_iterator curPrimIter = primList.begin();
      PrimitiveList::const_iterator endPrimIter = primList.end();
      for (; curPrimIter != endPrimIter; ++curPrimIter)
      {
         curPrimSet = curPrimIter->get();

         int curCount = 0;
         int numIndices = curPrimSet->getNumIndices();
         switch (curPrimSet->getMode())
         {
         case(osg::PrimitiveSet::TRIANGLES):
            curCount = numIndices/3;
            break;
         case(osg::PrimitiveSet::QUADS):
            curCount = numIndices/2; // ex: 8 points, 2 quads, 4 triangles
            break;
         case(osg::PrimitiveSet::TRIANGLE_STRIP):
         case(osg::PrimitiveSet::TRIANGLE_FAN):
         case(osg::PrimitiveSet::QUAD_STRIP):
            curCount = numIndices-2;
            break;
         default:
            curCount = 0;
            break;
         }

         count += curCount;
      }

      return count;
   }



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   OsgSkeleton::OsgSkeleton(OsgModelWrapper& model, osgAnimation::Skeleton& skel)
      : mModel(&model)
      , mSkel(&skel)
   {}

   OsgSkeleton::~OsgSkeleton()
   {
      mModel = NULL;
      mSkel = NULL;
   }

   dtCore::RefPtr<dtAnim::BoneInterface> OsgSkeleton::GetBone(const std::string& name)
   {
      return mModel->GetBone(name);
   }
   
   int OsgSkeleton::GetBones(dtAnim::BoneArray& outBones)
   {
      return mModel->GetBones(outBones);
   }

   int OsgSkeleton::GetBoneCount() const
   {
      return mModel->GetBoneCount();
   }
   
   int OsgSkeleton::GetRootBones(dtAnim::BoneArray& outBones)
   {
      return mModel->GetRootBones(outBones);
   }

   osgAnimation::Skeleton* OsgSkeleton::GetOsgSkeleton()
   {
      return mSkel.get();
   }

   const osgAnimation::Skeleton* OsgSkeleton::GetOsgSkeleton() const
   {
      return mSkel;
   }



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   OsgBone::OsgBone(OsgModelWrapper& model, osgAnimation::Bone& bone)
      : mModel(&model)
      , mBone(&bone)
      , mID(-1)
   {
      // TODO: Set ID
   }

   OsgBone::~OsgBone()
   {
      mModel = NULL;
      mBone = NULL;
   }

   int OsgBone::GetID() const
   {
      return mID;
   }

   const std::string& OsgBone::GetName() const
   {
      return mBone->getName();
   }

   dtAnim::BoneInterface* OsgBone::GetParentBone() const
   {
      osgAnimation::Bone* parent = mBone->getBoneParent();

      return parent == NULL ? NULL : mModel->GetBone(parent->getName());
   }

   int OsgBone::GetChildBones(dtAnim::BoneArray& outBones) const
   {
      int count = 0;

      dtAnim::BoneInterface* curBone = NULL;
      osgAnimation::Bone* curOsgBone = NULL;
      int numChildren = mBone->getNumChildren();
      for (int i = 0; i < numChildren; ++i)
      {
         curOsgBone = static_cast<osgAnimation::Bone*>(mBone->getChild(i));

         if (curOsgBone != NULL)
         {
            curBone = mModel->GetBone(curOsgBone->getName());

            if (curBone != NULL)
            {
               outBones.push_back(curBone);
               ++count;
            }
         }
      }

      return count;
   }

   void OsgBone::GetAbsoluteMatrix(osg::Matrix& outMatrix) const
   {
      outMatrix = mBone->getMatrixInSkeletonSpace();
   }

   osg::Quat OsgBone::GetAbsoluteRotation() const
   {
      osg::Matrix resultBoneMatrix;
      GetAbsoluteMatrix(resultBoneMatrix);

      return resultBoneMatrix.getRotate();
   }

   osg::Vec3 OsgBone::GetAbsoluteTranslation() const
   {
      osg::Matrix resultBoneMatrix;
      GetAbsoluteMatrix(resultBoneMatrix);

      return resultBoneMatrix.getTrans();
   }

   osg::Quat OsgBone::GetRelativeRotation() const
   {
      return mBone->getMatrixInBoneSpace().getRotate();
   }
   
   osg::Quat OsgBone::GetAbsoluteRotationForKeyframe(const dtAnim::AnimationInterface& anim, dtAnim::Keyframe keyframe) const
   {
      return anim.GetKeyframeQuat(*this, keyframe);
   }

   osgAnimation::Bone* OsgBone::GetOsgBone()
   {
      return mBone.get();
   }

   const osgAnimation::Bone* OsgBone::GetOsgBone() const
   {
      return mBone.get();
   }


   
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   OsgMaterial::OsgMaterial(OsgModelWrapper& model, osg::StateSet& stateSet)
      : mModel(&model)
      , mStateSet(&stateSet)
      , mID(-1)
   {
      // TODO: Set ID
   }

   OsgMaterial::~OsgMaterial()
   {
      mModel = NULL;
      mStateSet = NULL;
   }

   int OsgMaterial::GetID() const
   {
      return mID;
   }

   void OsgMaterial::SetName(const std::string& name)
   {
      mStateSet->setName(name);
   }
   
   const std::string& OsgMaterial::GetName() const
   {
      return mStateSet->getName();
   }

   int OsgMaterial::GetTextureCount() const
   {
      return (int)(mStateSet->getTextureAttributeList().size());
   }

   void OsgMaterial::SetTexture(int index, osg::Texture* textureMap)
   {
      mStateSet->setTextureAttribute(index, textureMap);
   }

   osg::Texture* OsgMaterial::GetTexture(int index) const
   {
      return static_cast<osg::Texture*>(mStateSet->getTextureAttribute(index, osg::StateAttribute::TEXTURE));
   }

   const std::string OsgMaterial::GetTextureFile(int index) const
   {
      osg::Texture* tex = GetTexture(index);
      osg::Image* img = NULL;

      if(tex != NULL)
      {
         osg::Texture2D* tex2 = dynamic_cast<osg::Texture2D*>(tex);
         if (tex2 != NULL)
         {
            img = tex2->getImage();
         }
      }

      return img == NULL ? "" : img->getFileName();
   }

   osg::StateSet* OsgMaterial::GetOsgStateSet()
   {
      return mStateSet.get();
   }

   const osg::StateSet* OsgMaterial::GetOsgStateSet() const
   {
      return mStateSet.get();
   }


   
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   OsgMorphTarget::OsgMorphTarget(dtAnim::OsgSubmesh& submesh,
      osgAnimation::MorphGeometry& morphGeom, int id)
      : mSubmesh(&submesh)
      , mMorphGeom(&morphGeom)
      , mID(id)
   {}

   OsgMorphTarget::~OsgMorphTarget()
   {
      mSubmesh = NULL;
      mMorphGeom = NULL;
   }

   int OsgMorphTarget::GetID() const
   {
      return mID;
   }

   void OsgMorphTarget::SetName(const std::string& name)
   {
      mMorphGeom->setName(name);
   }

   const std::string& OsgMorphTarget::GetName() const
   {
      return mMorphGeom->getName();
   }

   void OsgMorphTarget::SetWeight(float weight)
   {
      osgAnimation::MorphGeometry::MorphTarget* target = GetOsgMorphTarget();
      if (target != NULL)
      {
         target->setWeight(weight);
      }
   }
   
   float OsgMorphTarget::GetWeight() const
   {
      const osgAnimation::MorphGeometry::MorphTarget* target = GetOsgMorphTarget();
      return target == NULL ? 0.0f : target->getWeight();
   }

   osgAnimation::MorphGeometry::MorphTarget* OsgMorphTarget::GetOsgMorphTarget()
   {
      osgAnimation::MorphGeometry::MorphTarget* target = NULL;
      if (mID < int(mMorphGeom->getMorphTargetList().size()))
      {
         target = &mMorphGeom->getMorphTarget(mID);
      }
      return target;
   }
   
   const osgAnimation::MorphGeometry::MorphTarget* OsgMorphTarget::GetOsgMorphTarget() const
   {
      osgAnimation::MorphGeometry::MorphTarget* target = NULL;
      if (mID < int(mMorphGeom->getMorphTargetList().size()))
      {
         target = &mMorphGeom->getMorphTarget(mID);
      }
      return target;
   }




   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   OsgSubmesh::OsgSubmesh(OsgMesh& parentMesh, osg::Geometry& submesh)
      : mParentMesh(&parentMesh)
      , mSubmesh(&submesh)
   {
   }
   
   OsgSubmesh::~OsgSubmesh()
   {
      mParentMesh = NULL;
   }

   int OsgSubmesh::GetID() const
   {
      osg::Geode* geode = mParentMesh->GetOsgGeode();
      return geode->getDrawableIndex(mSubmesh.get());
   }

   std::string OsgSubmesh::GetName() const
   {
      return mSubmesh->getName();
   }

   int OsgSubmesh::GetVertexCount() const
   {
      return mSubmesh->getVertexArray()->getNumElements();
   }
   
   int OsgSubmesh::GetFaceCount() const
   {
      return GetTriangleCount(*mSubmesh);
   }
   
   int OsgSubmesh::GetMorphTargetWeightCount() const
   {
      // TODO:
      return 0;
   }

   int OsgSubmesh::GetFaceData(dtAnim::IndexArray& outData)
   {
      // TODO:
      return 0;
   }
   
   int OsgSubmesh::GetVertexData(dtAnim::FloatArray& outData, int stride)
   {
      osg::Array& verts = *mSubmesh->getVertexArray();
      return CopyOsgArrayToFloatArray(verts, stride, outData);
   }
   
   int OsgSubmesh::GetNormalData(dtAnim::FloatArray& outData, int stride)
   {
      osg::Array& normals = *mSubmesh->getNormalArray();
      return CopyOsgArrayToFloatArray(normals, stride, outData);
   }
   
   int OsgSubmesh::GetMorphTargetWeightData(dtAnim::FloatArray& outData)
   {
      // TODO:
      return 0;
   }

   int OsgSubmesh::GetMorphTargets(dtAnim::MorphTargetArray& submorphTargets) const
   {
      // TODO:
      return 0;
   }

   osgAnimation::RigGeometry* OsgSubmesh::GetOsgRigGeometry()
   {
      return dynamic_cast<osgAnimation::RigGeometry*>(mSubmesh.get());
   }

   const osgAnimation::RigGeometry* OsgSubmesh::GetOsgRigGeometry() const
   {
      return dynamic_cast<osgAnimation::RigGeometry*>(mSubmesh.get());
   }

   osgAnimation::MorphGeometry* OsgSubmesh::GetOsgMorphGeometry()
   {
      return dynamic_cast<osgAnimation::MorphGeometry*>(mSubmesh.get());
   }

   const osgAnimation::MorphGeometry* OsgSubmesh::GetOsgMorphGeometry() const
   {
      return dynamic_cast<osgAnimation::MorphGeometry*>(mSubmesh.get());
   }


   
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   OsgMesh::OsgMesh(OsgModelWrapper& model, osg::Geode& geode)
      : mModel(&model)
      , mGeode(&geode)
      , mID(-1)
   {
      mOriginalMask = geode.getNodeMask();
   }

   OsgMesh::~OsgMesh()
   {
      mModel = NULL;
      mGeode = NULL;
   }

   void OsgMesh::SetID(int id)
   {
      mID = id;
   }

   int OsgMesh::GetID() const
   {
      return mID;
   }

   void OsgMesh::SetName(const std::string& name)
   {
      mGeode->setName(name);

      mModel->UpdateMeshInterfaceObjects();
   }

   const std::string& OsgMesh::GetName() const
   {
      return mGeode->getName();
   }

   void OsgMesh::SetVisible(bool visible)
   {
      if (visible) // show
      {
         mGeode->setNodeMask(mOriginalMask == 0 ? 1 : mOriginalMask);
      }
      else // hide
      {
         // Remember the current mask if it has bits set for visibility.
         osg::Node::NodeMask currentMask = mGeode->getNodeMask();
         if (currentMask != 0)
         {
            mOriginalMask = currentMask;
         }

         mGeode->setNodeMask(0);
      }
   }

   bool OsgMesh::IsVisible() const
   {
      return mGeode->getNodeMask() != 0;
   }

   int OsgMesh::GetVertexCount() const
   {
      int count = 0;

      osg::Array* verts = NULL;

      int numDrawables = mGeode->getNumDrawables();
      for (int i = 0; i < numDrawables; ++i)
      {
         osg::Geometry* geom = mGeode->getDrawable(i)->asGeometry();
         if (geom != NULL)
         {
            verts = geom->getVertexArray();
            if (verts != NULL)
            {
               count += verts->getNumElements();
            }
         }
      }

      return count;
   }

   int OsgMesh::GetFaceCount() const
   {
      int count = 0;

      osg::Geometry* geom = NULL;
      int numDrawables = mGeode->getNumDrawables();
      for (int i = 0; i < numDrawables; ++i)
      {
         geom = mGeode->getDrawable(i)->asGeometry();
         if (geom != NULL)
         {
            count += GetTriangleCount(*geom);
         }
      }

      return count;
   }

   int OsgMesh::GetSubmeshCount() const
   {
       return mGeode->getNumDrawables();
   }

   // HELPER TEMPLATE FUNCTION
   template<typename T_Array, typename T_Geom, typename T_OsgInterface>
   int OsgMesh_GetGeometryByType(OsgMesh& caller, osg::Geode& geode,
      const std::string& geomClassName,
      T_Array& outArray)
   {
      int results = 0;
      
      osg::Drawable* drawable = NULL;
      T_Geom* geom = NULL;
      int numDrawables = geode.getNumDrawables();
      for (int i = 0; i < numDrawables; ++i)
      {
         drawable = geode.getDrawable(i);

         if (strcmp(drawable->className(), geomClassName.c_str()) == 0)
         {
            geom = static_cast<T_Geom*>(drawable);

            outArray.push_back(new T_OsgInterface(caller, *geom));
            ++results;
         }
      }
      
      return results;
   }

   int OsgMesh::GetSubmeshes(dtAnim::SubmeshArray& outSubmeshes) const
   {
      return OsgMesh_GetGeometryByType
         <dtAnim::SubmeshArray, osgAnimation::RigGeometry, dtAnim::OsgSubmesh>
         (*const_cast<dtAnim::OsgMesh*>(this), *mGeode, "RigGeometry", outSubmeshes);
   }

   int OsgMesh::GetMorphTargets(dtAnim::MorphTargetArray& morphTargets) const
   {
      // TODO:
      return 0;
   }

   osg::BoundingBox OsgMesh::GetBoundingBox() const
   {
      return mGeode->getBoundingBox();
   }

   bool OsgMesh::IsHardwareMode() const
   {
      bool result = false;

      if (mGeode->getNumDrawables() > 0)
      {
         osgAnimation::RigGeometry* geom
            = dynamic_cast<osgAnimation::RigGeometry*>(mGeode->getDrawable(0));

         result = geom != NULL && dynamic_cast<osgAnimation::RigTransformHardware*>
            (geom->getRigTransformImplementation()) != NULL;
      }

      return result;
   }

   osg::Geode* OsgMesh::GetOsgGeode()
   {
      return mGeode.get();
   }

   const osg::Geode* OsgMesh::GetOsgGeode() const
   {
      return mGeode.get();
   }
   
   void OsgMesh::ApplyMaterial(OsgMaterial& material)
   {
      mGeode->setStateSet(material.GetOsgStateSet());
   }



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   OsgTrack::OsgTrack(OsgAnimation& anim, osgAnimation::Target& target, const std::string& targetName)
      : mParentAnim(&anim)
      , mTarget(&target)
      , mTargetName(targetName)
   {
   }

   OsgTrack::~OsgTrack()
   {
      mTarget = NULL;
      mParentAnim = NULL;
   }

   osgAnimation::Target* OsgTrack::GetOsgTarget()
   {
      return mTarget.get();
   }

   const osgAnimation::Target* OsgTrack::GetOsgTarget() const
   {
      return mTarget.get();
   }

   const std::string& OsgTrack::GetOsgTargetName() const
   {
      return mTargetName;
   }

   int OsgTrack::GetOsgChannels(OsgChannelArray& outChannels) const
   {
      int results = 0;

      osgAnimation::Animation* anim = mParentAnim->GetOsgAnimation();

      osgAnimation::Channel* curChannel = NULL;
      osgAnimation::ChannelList& channels = anim->getChannels();
      osgAnimation::ChannelList::iterator curIter = channels.begin();
      osgAnimation::ChannelList::iterator endIter = channels.end();
      for (; curIter != endIter; ++curIter)
      {
         curChannel = curIter->get();
         if (curChannel->getTarget() == mTarget.get())
         {
            outChannels.push_back(curChannel);
            ++results;
         }
      }

      return results;
   }

   dtAnim::BoneInterface* OsgTrack::GetBone() const
   {
      return mParentAnim->GetModel()->GetBone(mTargetName);
   }

   bool OsgTrack::GetTransformAtTime(float time, dtAnim::Location& outLocation, dtAnim::Rotation& outRotation) const
   {
      // TODO:
      return false;
   }



   /////////////////////////////////////////////////////////////////////////////
   // HELPER PREDICATE
   /////////////////////////////////////////////////////////////////////////////
   struct IsChannelForTargetPred
   {
      IsChannelForTargetPred(const std::string& targetName)
         : mTargetName(targetName)
      {}

      bool operator() (osgAnimation::Channel* channel)
      {
         return channel->getTargetName() == mTargetName;
      }

      std::string mTargetName;
   };

   struct FindTransformChannelPred
   {
      FindTransformChannelPred(const std::string& targetName)
         : mTargetCheck(!targetName.empty())
         , mTargetName(targetName)
         , mTarget(NULL)
      {
      }

      bool operator() (osgAnimation::Channel* channel)
      {
         bool result = false;

         if (mTargetCheck)
         {
            if (mTargetName == channel->getTargetName())
            {
               osgAnimation::Target* target = channel->getTarget();
               bool determineTargetType = false;

               if (mTarget == NULL)
               {
                  mTarget = target;
                  determineTargetType = true;
               }

               if (mTarget != target)
               {
                  // TODO:
                  // Target changed!
                  determineTargetType = true;
               }

               if (determineTargetType)
               {
                  mTargetIsMatrix = NULL != dynamic_cast<osgAnimation::MatrixTarget*>(target);
                  mTargetIsQuat = NULL != dynamic_cast<osgAnimation::QuatTarget*>(target);
               }

               result = mTargetIsMatrix || mTargetIsQuat;
            }
         }
         else // Check target type every time.
         {
            osgAnimation::Target* target = channel->getTarget();
            result = NULL != dynamic_cast<osgAnimation::QuatTarget*>(target)
               || NULL != dynamic_cast<osgAnimation::MatrixTarget*>(target);
         }

         if (result)
         {
            mFoundChannels.push_back(channel);
         }

         return result;
      }

      bool mTargetIsMatrix;
      bool mTargetIsQuat;
      bool mTargetCheck;
      std::string mTargetName;
      osgAnimation::Target* mTarget;

      OsgChannelArray mFoundChannels;
   };
   
   
   
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   OsgAnimation::OsgAnimation(OsgModelWrapper& model, osgAnimation::Animation& anim)
      : mModel(&model)
      , mAnim(&anim)
      , mID(-1)
      , mSynchronous(true)
      , mOriginalWeight(1.0f)
      , mOriginalDuration(0.0f)
   {
      // TODO: Set ID
   }

   OsgAnimation::~OsgAnimation()
   {
      mAnim = NULL;
      mModel = NULL;
   }

   int OsgAnimation::GetID() const
   {
      return mID;
   }

   void OsgAnimation::SetName(const std::string& name)
   {
      mAnim->setName(name);
   }
   
   const std::string& OsgAnimation::GetName() const
   {
      return mAnim->getName();
   }

   void OsgAnimation::SetPlayMode(const dtAnim::PlayModeEnum& playMode)
   {
      typedef dtAnim::PlayModeEnum PlayMode;
      typedef osgAnimation::Animation OsgAnim;
      typedef osgAnimation::Animation::PlayMode OsgPlayMode;

      OsgPlayMode mode = OsgAnim::ONCE;

      if (&playMode == &PlayMode::LOOP)
      {
         mode = OsgAnim::LOOP;
      }
      else if (&playMode == &PlayMode::SWING)
      {
         mode = OsgAnim::PPONG;
      }
      else if (&playMode == &PlayMode::POSE)
      {
         mode = OsgAnim::STAY;
      }
      
      mAnim->setPlayMode(mode);
   }
   
   const dtAnim::PlayModeEnum& OsgAnimation::GetPlayMode() const
   {
      typedef dtAnim::PlayModeEnum PlayMode;
      typedef osgAnimation::Animation OsgAnim;
      typedef osgAnimation::Animation::PlayMode OsgPlayMode;

      const PlayMode* playMode = &PlayMode::NONE;

      OsgPlayMode osgMode = mAnim->getPlayMode();
      switch (osgMode)
      {
      case OsgAnim::ONCE:
         playMode = &PlayMode::ONCE;
         break;
      case OsgAnim::LOOP:
         playMode = &PlayMode::LOOP;
         break;
      case OsgAnim::PPONG:
         playMode = &PlayMode::SWING;
         break;
      case OsgAnim::STAY:
         playMode = &PlayMode::POSE;
         break;
      default:
         break;
      }

      return *playMode;
   }

   const dtAnim::AnimationStateEnum& OsgAnimation::GetState() const
   {
      const OsgAnimator* animator = GetAnimator();

      return animator == NULL ? dtAnim::AnimationStateEnum::NONE : animator->GetAnimationState(*this);
   }
   
   bool OsgAnimation::PlayAction(float fadeInTime, float fadeOutTime,
      float weight, bool autoLock)
   {
      // TODO: Autolock???

      return GetAnimator()->PlayAction(*this, fadeInTime, fadeOutTime, weight);
   }
      
   bool OsgAnimation::PlayCycle(float weight, float fadeInTime)
   {
      return GetAnimator()->PlayCycle(*this, weight, fadeInTime);
   }
   
   bool OsgAnimation::ClearAction()
   {
      return GetAnimator()->ClearAction(*this);
   }
   
   bool OsgAnimation::ClearCycle(float fadeOutTime)
   {
      return GetAnimator()->ClearCycle(*this, fadeOutTime);
   }

   bool OsgAnimation::Clear(float fadeOutTime)
   {
      // TODO:
      return false;
   }

   float OsgAnimation::GetDuration() const
   {
      return mAnim->getDuration();
   }

   float OsgAnimation::GetWeight() const
   {
      return mAnim->getWeight();
   }

   float OsgAnimation::GetTime() const
   {
      float timelineTime = GetAnimator()->GetAnimationTime();

      float animTime = 0.0f;

      float duration = mAnim->getDuration();
      if (duration != 0.0f)
      {
         float startTime = mAnim->getStartTime();

         // Animation time should be relative to the animation and not
         // the overall timeline. Animation range should fall within
         // 0 to loop duration.
         animTime = (timelineTime - startTime) / duration;

         // Clamp the lower time limit. 
         if (animTime < 0.0f)
         {
            animTime = 0.0f;
         }

         // Clamp to the duration or adjust the time
         // relative to another loop.
         if (animTime > duration)
         {
            const dtAnim::PlayModeEnum& playMode = GetPlayMode();
            if (playMode == dtAnim::PlayModeEnum::LOOP)
            {
               float ratio = animTime / duration;
               ratio -= std::floorf(ratio); // Loop count
               animTime = duration * ratio;
            }
            else if (playMode == dtAnim::PlayModeEnum::SWING)
            {
               float ratio = (animTime / duration) * 0.5; // Twice the anim time is one full swing loop.
               ratio -= std::floorf(ratio); // Loop count
               animTime = duration * ratio;
            }
            else
            {
               animTime = duration;
            }
         }
      }

      return animTime;
   }
   
   int OsgAnimation::GetKeyframeCount() const
   {
      // TODO:
      // Determine true FPS per animation resource.

      // Use an action to determine the default frames per second.
      dtCore::RefPtr<osgAnimation::Action> action = new osgAnimation::Action;
      action->setDuration(mOriginalDuration);
      int keyframeCount = action->getNumFrames();

      return keyframeCount;
   }

   int OsgAnimation::GetKeyframeCountForBone(const dtAnim::BoneInterface& bone) const
   {
      int keyframeCount = 0;

      const std::string& boneName = bone.GetName();
      osgAnimation::Channel* curChannel = NULL;
      osgAnimation::ChannelList& channels = mAnim->getChannels();
      osgAnimation::ChannelList::iterator curIter = channels.begin();
      osgAnimation::ChannelList::iterator endIter = channels.end();
      for (; curIter != endIter; ++curIter)
      {
         curChannel = curIter->get();
         if (curChannel->getTargetName() == boneName)
         {
            int keyCount = (int)(curChannel->getSampler()->getKeyframeContainer()->size());
            if (keyframeCount < keyCount)
            {
               keyframeCount = keyCount;
            }
         }
      }

      return keyframeCount;
   }

   int OsgAnimation::GetTrackCount() const
   {
      return int(mAnim->getChannels().size());
   }

   int OsgAnimation::GetTracks(dtAnim::TrackArray& outTracks)
   {
      // TODO:
      return 0;
   }

   bool OsgAnimation::HasTrackForBone(const dtAnim::BoneInterface& bone) const
   {
      OsgChannelArray channels;
      return 0 < GetOsgChannelsForBone(bone, channels);
   }
   
   osg::Quat OsgAnimation::GetKeyframeQuat(const dtAnim::BoneInterface& bone, dtAnim::Keyframe keyframe) const
   {
      osg::Quat result;

      // TODO:

      return result;
   }

   void OsgAnimation::SetSynchronous(bool synchronous)
   {
      mSynchronous = synchronous;
   }

   bool OsgAnimation::IsSynchronous() const
   {
      return mSynchronous;
   }

   void OsgAnimation::SetOriginalDuration(float duration)
   {
      mOriginalDuration = duration;
   }

   float OsgAnimation::GetOriginalDuration() const
   {
      return mOriginalDuration;
   }
      
   void OsgAnimation::SetOriginalWeight(float weight)
   {
      mOriginalWeight = weight;
   }

   float OsgAnimation::GetOriginalWeight() const
   {
      return mOriginalWeight;
   }

   osgAnimation::Animation* OsgAnimation::GetOsgAnimation()
   {
      return mAnim.get();
   }

   const osgAnimation::Animation* OsgAnimation::GetOsgAnimation() const
   {
      return mAnim.get();
   }
   
   int OsgAnimation::GetOsgChannels(OsgChannelArray& outChannels) const
   {
      osgAnimation::ChannelList& channels = mAnim->getChannels();
      int results = (int)(channels.size());

      if (results > 0)
      {
         outChannels.insert(outChannels.end(), channels.begin(), channels.end());
      }

      return results;
   }

   int OsgAnimation::GetOsgChannelsForBone(
      const dtAnim::BoneInterface& bone, OsgChannelArray& outChannels) const
   {
      int results = 0;
      
      const std::string& boneName = bone.GetName();
      osgAnimation::Channel* curChannel = NULL;
      osgAnimation::ChannelList& channels = mAnim->getChannels();
      osgAnimation::ChannelList::iterator curIter = channels.begin();
      osgAnimation::ChannelList::iterator endIter = channels.end();
      for (; curIter != endIter; ++curIter)
      {
         curChannel = curIter->get();
         if (curChannel->getTargetName() == boneName)
         {
            outChannels.push_back(curChannel);
            ++results;
         }
      }

      return results;
   }

   int OsgAnimation::GetOsgChannelsForBoneTransforms(const dtAnim::BoneInterface& bone, OsgChannelArray& outChannels) const
   {
      osgAnimation::ChannelList& channels = mAnim->getChannels();
      FindTransformChannelPred pred(bone.GetName());
      int results = std::count_if(channels.begin(), channels.end(), pred);

      if (results > 0)
      {
         outChannels.insert(outChannels.end(), pred.mFoundChannels.begin(), pred.mFoundChannels.end());
      }

      return results;
   }

   dtAnim::OsgAnimator* OsgAnimation::GetAnimator()
   {
      return mModel->GetOsgAnimator();
   }

   const dtAnim::OsgAnimator* OsgAnimation::GetAnimator() const
   {
      return mModel->GetOsgAnimator();
   }

   dtAnim::OsgModelWrapper* OsgAnimation::GetModel()
   {
      return mModel.get();
   }

   const dtAnim::OsgModelWrapper* OsgAnimation::GetModel() const
   {
      return mModel.get();
   }
   
   void OsgAnimation::Reset()
   {
      mAnim->setDuration(mOriginalDuration);
      mAnim->setWeight(mOriginalWeight);
   }

}
