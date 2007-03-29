#include <dtAnim/cal3dmodelwrapper.h>
#include <cal3d/cal3d.h>
#include <assert.h>

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

   SetMaterialSet(0);
}

Cal3DModelWrapper::~Cal3DModelWrapper()
{
   delete mCalModel;
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

const std::string& Cal3DModelWrapper::GetCoreAnimationName( int animID )
{
   return mCalModel->getCoreModel()->getCoreAnimation(animID)->getName();
}

unsigned int Cal3DModelWrapper::GetCoreAnimationTrackCount( int animID )
{
   return mCalModel->getCoreModel()->getCoreAnimation(animID)->getTrackCount();
}

unsigned int Cal3DModelWrapper::GetCoreAnimationKeyframeCount( int animID )
{
   return mCalModel->getCoreModel()->getCoreAnimation(animID)->getTotalNumberOfKeyframes();
}

float Cal3DModelWrapper::GetCoreAnimationDuration( int animID )
{
   return mCalModel->getCoreModel()->getCoreAnimation(animID)->getDuration();
}
