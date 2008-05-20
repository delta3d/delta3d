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

#include <osg/LineWidth>
#include <osg/Geode>

osg::Geometry* CreateDebugLine(const osg::Vec3& beginsegment,
                               const osg::Vec3& endsegment,
                               const osg::Vec4& color,
                               const float size)
{
   osg::Vec3Array *vArray = new osg::Vec3Array(2);
   (*vArray)[0] = beginsegment;
   (*vArray)[1] = endsegment;

   osg::Vec4Array *colors = new osg::Vec4Array(1);
   (*colors)[0] = color;

   osg::Geometry* geometry = new osg::Geometry();
   geometry->setVertexArray(vArray);
   geometry->setColorArray(colors);
   geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
   geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 2));

   osg::StateSet *dstate = new osg::StateSet();
   dstate->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
   dstate->setMode(GL_CULL_FACE, osg::StateAttribute::ON);

   osg::LineWidth *lineWidth = new osg::LineWidth(size);
   dstate->setAttribute(lineWidth);

   geometry->setStateSet(dstate);
   return geometry;
}



CharDrawable::CharDrawable(Cal3DModelWrapper* wrapper)
   : dtCore::Transformable()
   , mNode(new osg::Node())
   , mAnimator(new Cal3DAnimator(wrapper))
   , mLastMeshCount(0)
{
   AddSender(&dtCore::System::GetInstance());

   GetMatrixNode()->addChild(mNode.get());

   osg::Geode *test = new osg::Geode;
   test->addDrawable(CreateDebugLine(osg::Vec3(), osg::Z_AXIS * 100.0f, osg::Vec4(1.0, 0, 0, 1), 10.0f));

   GetMatrixNode()->addChild(test);

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

