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

   class ModelMatrixUpdateCallback : public osg::NodeCallback
   {
      public:
         ModelMatrixUpdateCallback(osg::Vec3& scale):
            mScale(scale)
         {
         }

         /** Callback method called by the NodeVisitor when visiting a node.*/
         virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
         {
            osg::MatrixTransform* modelTransform = static_cast<osg::MatrixTransform*>(node);
            dtCore::Transform xform;
            osg::Matrix m;
            xform.Set(modelTransform->getMatrix());
            xform.Rescale(mScale);
            xform.Get(m);
            modelTransform->setMatrix(m);
            traverse(node,nv);
         }
      private:
         osg::Vec3& mScale;
   };

   /////////////////////////////////////////////////////////////////////////////
   Object::Object(const std::string& name)
   :  Physical(name),
      mRecenterGeometry( false )
   {
      Ctor();
   }

   /////////////////////////////////////////////////////////////////////////////
   Object::Object( TransformableNode &node, const std::string &name )
   :  Physical(node, name),
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

      mScale.set(1.0, 1.0, 1.0);
      mUpdateCallback = new ModelMatrixUpdateCallback(mScale);
      mModelTransform = new osg::MatrixTransform;
      GetMatrixNode()->addChild(mModelTransform.get());

      SetDirty();

      // Default collision category = 5
      SetCollisionCategoryBits( UNSIGNED_BIT(5) );
   }


   /////////////////////////////////////////////////////////////////////////////
   Object::~Object()
   {
      DeregisterInstance(this);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Object::SetDirty()
   {
      mModelTransform->setUpdateCallback(mUpdateCallback.get());
   }

   /////////////////////////////////////////////////////////////////////////////
   void Object::SetModelScale(const osg::Vec3& modelScale)
   {
      mScale = modelScale;
      SetDirty();
   }

   /////////////////////////////////////////////////////////////////////////////
   void Object::GetModelScale(osg::Vec3& modelScale) const
   {
      modelScale = mScale;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Object::SetModelTransform(const dtCore::Transform& xform)
   {
      //two copies?  Not good.
      osg::Matrix m;
      xform.Get(m);
      mModelTransform->setMatrix(m);
      //dirty so we can re-apply the scale.
      SetDirty();
   }
   
   /////////////////////////////////////////////////////////////////////////////
   void Object::GetModelTransform(dtCore::Transform& xform) const
   {
      xform.Set(mModelTransform->getMatrix());
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Node* Object::LoadFile(const std::string& filename, bool useCache)
   {
      osg::Node *node = NULL;
      node = Loadable::LoadFile(filename, useCache);
   
      //We should always clear the geometry.  If LoadFile fails, we should have no geometry.
      if (mModelTransform->getNumChildren() != 0)
      {
         mModelTransform->removeChild(0,GetMatrixNode()->getNumChildren() );
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
            mModelTransform->setMatrix( tempMat );
            SetDirty();
         }

         mModelTransform->addChild(node);

         return node;
      }
      else
      {
         return NULL;
      }
   }

}
