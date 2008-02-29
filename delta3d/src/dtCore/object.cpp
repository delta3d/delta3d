// object.cpp: implementation of the Object class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix-src.h>
#include <dtCore/object.h>
#include <dtCore/scene.h>
#include <dtCore/camera.h>//due to include of scene.h
#include <dtCore/keyboardmousehandler.h> //due to include of scene.h
#include <dtCore/keyboard.h>//due to include of scene.h
#include <dtCore/boundingboxvisitor.h>

#include <dtUtil/polardecomp.h>

#include <osgDB/ReadFile>
#include <osgDB/Registry>
#include <osg/MatrixTransform>
#include <osg/Matrix>
#include <osg/NodeCallback>
#include <osg/NodeVisitor>

namespace dtCore
{
   
   IMPLEMENT_MANAGEMENT_LAYER(Object)

   /////////////////////////////////////////////////////////////////////////////
   Object::Object(const std::string& name)
   :  Physical(name),
      mModel(new Model), 
      mRecenterGeometry( false )
   {
      Ctor();
   }

   /////////////////////////////////////////////////////////////////////////////
   Object::Object( TransformableNode &node, const std::string &name )
   :  Physical(node, name),
      mModel(new Model), 
      mRecenterGeometry( false )
   {
      Ctor();
   }

   /////////////////////////////////////////////////////////////////////////////
   void Object::Ctor()
   {
      RegisterInstance(this);

      osg::StateSet *stateSet = GetOSGNode()->getOrCreateStateSet();
      stateSet->setMode(GL_NORMALIZE, osg::StateAttribute::ON);

      GetMatrixNode()->addChild(&mModel->GetMatrixTransform());

      // Default collision category = 5
      SetCollisionCategoryBits( UNSIGNED_BIT(5) );
   }


   /////////////////////////////////////////////////////////////////////////////
   Object::~Object()
   {
      DeregisterInstance(this);
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Node* Object::LoadFile(const std::string& filename, bool useCache)
   {
      osg::Node *node = NULL;
      node = Loadable::LoadFile(filename, useCache);
   
      //We should always clear the geometry.  If LoadFile fails, we should have no geometry.
      if (mModel->GetMatrixTransform().getNumChildren() != 0)
      {
         mModel->GetMatrixTransform().removeChild(0,GetMatrixNode()->getNumChildren() );
      }

      //attach our geometry node to the matrix node
      if (node!=NULL)
      {
         //recenter the geometry about the origin by finding the center of it's
         //bounding box and adding a transform between the loaded group node
         //and the top transform which undo's any offsets
         if( mRecenterGeometry )
         {
            BoundingBoxVisitor bbv;
            node->accept(bbv);

            osg::Matrix tempMat;
            tempMat.makeTranslate( -bbv.mBoundingBox.center() );
            mModel->GetMatrixTransform().setMatrix( tempMat );
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
   void Object::SetScale(const osg::Vec3 &xyz)
   {  
      mModel->SetScale(xyz);
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
