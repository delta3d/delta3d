#include <dtAnim/cal3dmodelwrapper.h>
#include <cal3d/cal3d.h>
#include <cal3d/coretrack.h>
#include <cal3d/corekeyframe.h>

#include <cassert>
#include <algorithm>

using namespace dtAnim;

Cal3DModelWrapper::Cal3DModelWrapper(CalModel *model):
mCalModel(model),
mRenderer(NULL),
mMixer(NULL)
{
   assert(mCalModel != NULL);

   mRenderer = mCalModel->getRenderer();
   mMixer    = mCalModel->getMixer();

   if (model)
   {
      CalCoreModel *coreModel = model->getCoreModel();

      // attach all meshes to the model
      if (coreModel)
      {
         for(int meshId = 0; meshId < coreModel->getCoreMeshCount(); meshId++)
         {
            AttachMesh(meshId);
         }
      }      
   }     
}

Cal3DModelWrapper::~Cal3DModelWrapper()
{
   delete mCalModel;
}

bool Cal3DModelWrapper::AttachMesh( int meshID )
{
   bool success = mCalModel->attachMesh(meshID); 
   mCalModel->setMaterialSet(0);
  
   return success;
}

bool Cal3DModelWrapper::DetachMesh( int meshID )
{
   return mCalModel->detachMesh(meshID); 
}


void Cal3DModelWrapper::SetCalModel( CalModel *model )
{
   assert(model != NULL);

   mCalModel = model;   
   mRenderer = mCalModel->getRenderer();
   mMixer    = mCalModel->getMixer();
}

/** 
 * @param id : a valid ID of an animation (0 based)
 * @param weight : the strength of this animation in relation to the other
 *                 animations already being blended.
 * @param delay : how long it takes for this animation to become full strength (seconds)
 * @return true if successful, false if an error happened.
 */
bool Cal3DModelWrapper::BlendCycle( int id, float weight, float delay )
{
   return mMixer->blendCycle(id, weight, delay);
}

/** 
 * @param id : a valid ID of an animation already being blended (0 based)
 * @param delay : how long it takes to fade this animation out (seconds)
 * @return true if successful, false if an error happened.
 */
bool Cal3DModelWrapper::ClearCycle( int id, float delay )
{
   return mMixer->clearCycle(id, delay);
}

/** 
 * @param id : a valid ID of a animation to perform one-time (0 based)
 * @param delayIn : how long it takes to fade in this animation to full strength (seconds)
 * @param delayOut: how long it takes to fade out this animation (seconds)
 * @param weightTgt : the strength of this animation
 * @param autoLock : true prevents the action from being reset and removed on the last
 *                   key frame
 * @return true if successful, false if an error happened.
 */
bool Cal3DModelWrapper::ExecuteAction( int id, float delayIn, float delayOut, 
                                       float weightTgt/*=1.f*/, bool autoLock/*=false*/ )
{
   return mMixer->executeAction(id, delayIn, delayOut, weightTgt, autoLock);
}

/** 
 * @param id : a valid ID of a one-time animation already playing (0 based)
 * @return true if successful, false if an error happened or animation doesn't exist.
 */
bool Cal3DModelWrapper::RemoveAction( int id )
{
   return mMixer->removeAction(id);
}

/** Warning! This violates the protective services brought to you by the wrapper.
  * Only modify the CalModel if you know how it will impact the rest of the 
  * Delta3D animation system.  You have been warned.
  * @return A pointer to the internal CalModel this class operates on.
  */
CalModel* Cal3DModelWrapper::GetCalModel() 
{
   return mCalModel;
}

/** Warning! This violates the protective services brought to you by the wrapper.
  * Only modify the CalModel if you know how it will impact the rest of the 
  * Delta3D animation system.  You have been warned.
  * @return A const pointer to the internal CalModel this class operates on.
  */
const CalModel* Cal3DModelWrapper::GetCalModel() const
{
   return mCalModel;
}

int Cal3DModelWrapper::GetCoreAnimationCount() const
{
   return mCalModel->getCoreModel()->getCoreAnimationCount();
}

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

osg::Quat Cal3DModelWrapper::GetBoneAbsoluteRotation(unsigned int boneID) const
{
   CalBone *bone = mCalModel->getSkeleton()->getBone(boneID);
   assert(bone);

   if (bone)
   {
      const CalQuaternion& calQuat = bone->getRotationAbsolute();
      osg::Quat FixedQuat(calQuat.x, calQuat.y, calQuat.z, calQuat.w);
      return FixedQuat.inverse();
   }

   return osg::Quat();
}

osg::Vec3 Cal3DModelWrapper::GetBoneAbsoluteTranslation(unsigned int boneID) const
{
   CalBone *bone = mCalModel->getSkeleton()->getBone(boneID);
   assert(bone);

   if (bone)
   {
      const CalVector& calvec = bone->getTranslationAbsolute();
      return osg::Vec3(calvec.x, calvec.y, calvec.z);
   }

   return osg::Vec3();
}

osg::Quat Cal3DModelWrapper::GetBoneRelativeRotation(unsigned int boneID) const
{
   CalBone *bone = mCalModel->getSkeleton()->getBone(boneID);
   assert(bone);

   if (bone)
   {
      const CalQuaternion& calQuat = bone->getRotation();
      return osg::Quat(calQuat.x, calQuat.y, calQuat.z, -calQuat.w);     
   }

   return osg::Quat();
}

int Cal3DModelWrapper::GetCoreBoneID(const std::string& name) const
{
   CalCoreBone* corebone = mCalModel->getCoreModel()->getCoreSkeleton()->getCoreBone(name);
   if( corebone == NULL )
   {
      return Cal3DModelWrapper::NULL_BONE;
   }

   return mCalModel->getCoreModel()->getCoreSkeleton()->getCoreBoneId(name);
}

namespace dtAnim
{
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
}

void Cal3DModelWrapper::GetCoreBoneNames(std::vector<std::string>& toFill) const
{
   CalCoreSkeleton *skel = mCalModel->getCoreModel()->getCoreSkeleton();
   std::vector<CalCoreBone*> boneVec = skel->getVectorCoreBone();

   if( boneVec.empty() ) return;

   toFill.resize( boneVec.size() );
   std::transform( boneVec.begin(), boneVec.end(), toFill.begin(), details::ReturnBoneName() );
}

bool Cal3DModelWrapper::HasTrackForBone(unsigned int animID, int boneID) const
{
   return (mCalModel->getCoreModel()->getCoreAnimation(animID)->getCoreTrack(boneID) != NULL);
}

bool Cal3DModelWrapper::HasBone(int boneID) const
{
   return( mCalModel->getSkeleton()->getBone(boneID) != NULL );
}

const std::string& Cal3DModelWrapper::GetCoreAnimationName( int animID ) const
{
   return mCalModel->getCoreModel()->getCoreAnimation(animID)->getName();
}

unsigned int Cal3DModelWrapper::GetCoreAnimationTrackCount( int animID ) const
{
   return mCalModel->getCoreModel()->getCoreAnimation(animID)->getTrackCount();
}

int Cal3DModelWrapper::GetParentBoneID(unsigned int boneID) const
{
   //CalBone *currentBone = const_cast<CalBone*>(mCalModel->getSkeleton()->getBone(boneID));
   CalCoreBone *coreBone = const_cast<CalCoreBone*>(mCalModel->getCoreModel()->getCoreSkeleton()->getCoreBone(boneID));
   
   if (coreBone)
   {
      //CalCoreBone *coreBone = currentBone->getCoreBone();     
      return coreBone->getParentId();      
   }

   return NULL_BONE;
}

void Cal3DModelWrapper::GetCoreBoneChildrenIDs(int parentCoreBoneID, std::vector<int>& toFill) const
{
   std::list<int> childIdList = mCalModel->getCoreModel()->getCoreSkeleton()->getCoreBone(parentCoreBoneID)->getListChildId();

   // prefer a container that is contiguous
   toFill.clear();
   std::list<int>::iterator listEnd=childIdList.end();
   for(std::list<int>::iterator listIndex=childIdList.begin(); listIndex!=listEnd; ++listIndex)
   {
      toFill.push_back( *listIndex );
   }
}

void Cal3DModelWrapper::GetRootBoneIDs(std::vector<int>& toFill) const
{
   toFill = mCalModel->getCoreModel()->getCoreSkeleton()->getVectorRootCoreBoneId();
}

unsigned int Cal3DModelWrapper::GetCoreAnimationKeyframeCount( int animID ) const
{
   return mCalModel->getCoreModel()->getCoreAnimation(animID)->getTotalNumberOfKeyframes();
}

unsigned int Cal3DModelWrapper::GetCoreAnimationKeyframeCountForTrack(int animID, int boneID) const
{
   CalCoreAnimation *pAnimation = mCalModel->getCoreModel()->getCoreAnimation(animID);
   assert(pAnimation);

   CalCoreTrack *pTrack = pAnimation->getCoreTrack(boneID);
   assert(pTrack);

   return pTrack->getCoreKeyframeCount();
}

float Cal3DModelWrapper::GetCoreAnimationDuration( int animID ) const
{
   return mCalModel->getCoreModel()->getCoreAnimation(animID)->getDuration();
}

const std::string& Cal3DModelWrapper::GetCoreMeshName( int meshID ) const
{
   return mCalModel->getCoreModel()->getCoreMesh(meshID)->getName();
}

osg::Vec4 Cal3DModelWrapper::GetCoreMaterialDiffuse( int matID ) const
{
   osg::Vec4 retColor;

   CalCoreMaterial *mat = mCalModel->getCoreModel()->getCoreMaterial(matID);
   if (mat != NULL)
   {
      CalCoreMaterial::Color color = mat->getDiffuseColor();
      retColor.set(color.red, color.green, color.blue, color.alpha);
   }

   return retColor;
}

osg::Vec4 Cal3DModelWrapper::GetCoreMaterialAmbient( int matID ) const 
{
   osg::Vec4 retColor;

   CalCoreMaterial *mat = mCalModel->getCoreModel()->getCoreMaterial(matID);
   if (mat != NULL)
   {
      CalCoreMaterial::Color color = mat->getAmbientColor();
      retColor.set(color.red, color.green, color.blue, color.alpha);
   }

   return retColor;
}

osg::Vec4 Cal3DModelWrapper::GetCoreMaterialSpecular( int matID ) const
{
   osg::Vec4 retColor;

   CalCoreMaterial *mat = mCalModel->getCoreModel()->getCoreMaterial(matID);
   if (mat != NULL)
   {
      CalCoreMaterial::Color color = mat->getSpecularColor();
      retColor.set(color.red, color.green, color.blue, color.alpha);
   }

   return retColor;
}

float Cal3DModelWrapper::GetCoreMaterialShininess( int matID ) const
{
   CalCoreMaterial *mat = mCalModel->getCoreModel()->getCoreMaterial(matID);
   if (mat != NULL)
   {
      return mat->getShininess();
   }
   else
   {
      return 0.f;
   }
}

