#include <osg/MatrixTransform>
#include <osg/Node>
#include <osg/Timer>
#include <osg/Texture2D> //Cal3DLoader needs this

#include <dtAnim/submesh.h>
#include <dtAnim/chardrawable.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/cal3danimator.h>
#include <dtAnim/cal3dmodeldata.h>
#include <dtAnim/cal3ddatabase.h>
#include <dtAnim/animnodebuilder.h>
#include <dtCore/system.h>
#include <dtUtil/log.h>
#include <cassert>


#include <cal3d/corematerial.h>

#include <dtAnim/ical3ddriver.h>

using namespace dtAnim;

CharDrawable::CharDrawable(Cal3DModelWrapper* wrapper)
   : dtCore::Transformable()
   , mNode(new osg::Node())
   , mAnimator(new Cal3DAnimator(wrapper))
   , mLastMeshCount(0)
{
   AddSender(&dtCore::System::GetInstance());

   GetMatrixNode()->addChild(mNode.get());

   SetCal3DWrapper( wrapper );
}

CharDrawable::~CharDrawable()
{
   RemoveSender(&dtCore::System::GetInstance()); 
}


Cal3DModelWrapper* CharDrawable::GetCal3DWrapper()
{
   return mAnimator->GetWrapper();
}

void CharDrawable::OnMessage(dtCore::Base::MessageData *data)
{
   assert(mAnimator.get());

   // tick the animation
   if( data->message == "preframe" )
   {
      double dt = *static_cast<double*>(data->userData);
      mAnimator->Update(dt);

      Cal3DModelWrapper* wrapper = mAnimator->GetWrapper();
      if (mLastMeshCount != wrapper->GetMeshCount())
      {
         //there are a different number of meshes, better rebuild our drawables
         RebuildSubmeshes(wrapper, mNode.get());
         mLastMeshCount = wrapper->GetMeshCount();
      }

   }
}


/** Will delete all existing drawables added to the geode, then add in a whole
  * new set.
  */
void CharDrawable::RebuildSubmeshes(Cal3DModelWrapper* wrapper, osg::Node* geode)
{
   GetMatrixNode()->removeChild(geode);
   dtCore::RefPtr<osg::Node> newNode = Cal3DDatabase::GetInstance().GetNodeBuilder().CreateNode(wrapper);
   GetMatrixNode()->addChild(newNode.get());
   mNode = newNode;
}

void CharDrawable::SetCal3DWrapper(Cal3DModelWrapper* wrapper)
{
   mAnimator->SetWrapper(wrapper);
   RebuildSubmeshes(wrapper, mNode.get());
   mLastMeshCount = wrapper->GetMeshCount();
}

