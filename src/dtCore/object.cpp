// object.cpp: implementation of the Object class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix.h>
#include <dtCore/object.h>
#include <dtCore/collisioncategorydefaults.h>
#include <dtCore/transform.h>

#include <dtUtil/boundingshapeutils.h>

#include <osg/MatrixTransform>
#include <osg/Matrix>

namespace dtCore
{

   IMPLEMENT_MANAGEMENT_LAYER(Object)

   /////////////////////////////////////////////////////////////////////////////
   Object::Object(const std::string& name)
      : Physical(name)
      , mModel(new Model)
      , mRecenterGeometry(false)
   {
      Ctor();
   }

   /////////////////////////////////////////////////////////////////////////////
   Object::Object(TransformableNode& node, const std::string& name)
      : Physical(node, name)
      , mModel(new Model)
      , mRecenterGeometry(false)
   {
      Ctor();
   }

   /////////////////////////////////////////////////////////////////////////////
   void Object::Ctor()
   {
      RegisterInstance(this);

      osg::StateSet* stateSet = GetOSGNode()->getOrCreateStateSet();
      stateSet->setMode(GL_NORMALIZE, osg::StateAttribute::ON);

      GetMatrixNode()->addChild(&mModel->GetMatrixTransform());

      SetCollisionCategoryBits(COLLISION_CATEGORY_MASK_OBJECT);
   }

   /////////////////////////////////////////////////////////////////////////////
   Object::~Object()
   {
      DeregisterInstance(this);
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Node* Object::LoadFile(const std::string& filename, bool useCache)
   {
      osg::Node* node = NULL;
      node = Loadable::LoadFile(filename, useCache);

      //We should always clear the geometry.  If LoadFile fails, we should have no geometry.
      if (mModel->GetMatrixTransform().getNumChildren() != 0)
      {
         mModel->GetMatrixTransform().removeChild(0,GetMatrixNode()->getNumChildren());
      }

      //attach our geometry node to the matrix node
      if (node != NULL)
      {
         //recenter the geometry about the origin by finding the center of it's
         //bounding box and adding a transform between the loaded group node
         //and the top transform which undo's any offsets
         if (mRecenterGeometry)
         {
            dtUtil::BoundingBoxVisitor bbv;
            node->accept(bbv);

            osg::Matrix tempMat;
            tempMat.makeTranslate(-bbv.mBoundingBox.center());
            mModel->GetMatrixTransform().setMatrix(tempMat);
            mModel->SetDirty();
         }

         mModel->GetMatrixTransform().addChild(node);

         return node;
      }
      else
      {
         return NULL;
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void Object::SetScale(const osg::Vec3& xyz)
   {
      mModel->SetScale(xyz);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Object::SetModelRotation(const osg::Vec3& v3)
   {
      dtCore::Transform ourTransform;
      mModel->GetTransform(ourTransform);
      ourTransform.SetRotation(v3);
      mModel->SetTransform(ourTransform);
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Vec3 Object::GetModelRotation()
   {
      osg::Vec3 v3;
      dtCore::Transform ourTransform;
      mModel->GetTransform(ourTransform);
      ourTransform.GetRotation(v3);
      return v3;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Object::SetModelTranslation(const osg::Vec3& v3)
   {
      dtCore::Transform ourTransform;
      mModel->GetTransform(ourTransform);
      ourTransform.SetTranslation(v3);
      mModel->SetTransform(ourTransform);
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Vec3 Object::GetModelTranslation()
   {
      osg::Vec3 v3;
      dtCore::Transform ourTransform;
      mModel->GetTransform(ourTransform);
      ourTransform.GetTranslation(v3);
      return v3;
   }

   //////////////////////////////////////////////////////////////////////////////
   osg::Vec3 Object::GetScale() const
   {
      osg::Vec3 scale;
      mModel->GetScale(scale);
      return scale;
   }

   //////////////////////////////////////////////////////////////////////////////
   osg::MatrixTransform& Object::GetMatrixTransform()
   {
      return mModel->GetMatrixTransform();
   }

   //////////////////////////////////////////////////////////////////////////////
   const osg::MatrixTransform& Object::GetMatrixTransform() const
   {
      return mModel->GetMatrixTransform();
   }
}
