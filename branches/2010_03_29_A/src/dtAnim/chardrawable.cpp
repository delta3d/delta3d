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

////////////////////////////////////////////////////////////////////////////////
CharDrawable::CharDrawable(Cal3DModelWrapper* wrapper)
   : dtCore::Transformable()
   , mAnimator(new Cal3DAnimator(wrapper))
   , mNode(new osg::Node())
   , mLastMeshCount(0)
{
   AddSender(&dtCore::System::GetInstance());

   GetMatrixNode()->addChild(mNode.get());

   SetCal3DWrapper(wrapper);
}

////////////////////////////////////////////////////////////////////////////////
CharDrawable::~CharDrawable()
{
   RemoveSender(&dtCore::System::GetInstance());
}

////////////////////////////////////////////////////////////////////////////////
Cal3DModelWrapper* CharDrawable::GetCal3DWrapper()
{
   return mAnimator->GetWrapper();
}

////////////////////////////////////////////////////////////////////////////////
void CharDrawable::OnMessage(dtCore::Base::MessageData* data)
{
   assert(mAnimator.get());

   // tick the animation
   if (data->message == dtCore::System::MESSAGE_PRE_FRAME)
   {
      double dt = *static_cast<double*>(data->userData);
      mAnimator->Update(dt);

      Cal3DModelWrapper* wrapper = mAnimator->GetWrapper();
      if (mLastMeshCount != wrapper->GetMeshCount())
      {
         //there are a different number of meshes, better rebuild our drawables
         RebuildSubmeshes();
         mLastMeshCount = wrapper->GetMeshCount();
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
osg::Node* CharDrawable::RebuildSubmeshes()
{
   GetMatrixNode()->removeChild(mNode.get());
   mNode = Cal3DDatabase::GetInstance().GetNodeBuilder().CreateNode(mAnimator->GetWrapper());
   GetMatrixNode()->addChild(mNode.get());
   return mNode.get();
}

////////////////////////////////////////////////////////////////////////////////
void CharDrawable::SetCal3DWrapper(Cal3DModelWrapper* wrapper)
{
   mAnimator->SetWrapper(wrapper);
   RebuildSubmeshes();
   mLastMeshCount = wrapper->GetMeshCount();
}

