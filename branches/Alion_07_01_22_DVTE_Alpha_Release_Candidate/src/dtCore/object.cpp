// object.cpp: implementation of the Object class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix-src.h>
#include <dtCore/object.h>
#include <dtCore/scene.h>
#include <dtCore/boundingboxvisitor.h>
#include <osgDB/ReadFile>
#include <osgDB/Registry>
#include <osg/MatrixTransform>

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(Object)

Object::Object(const std::string& name)
:  Physical(name),
   mRecenterGeometry( false )
{
   RegisterInstance(this);

   osg::StateSet *stateSet = GetOSGNode()->getOrCreateStateSet();
   stateSet->setMode(GL_NORMALIZE, osg::StateAttribute::ON);

   // Default collision category = 5
   SetCollisionCategoryBits( UNSIGNED_BIT(5) );
}

Object::~Object()
{
   DeregisterInstance(this);
}


/*!
 * Load a geometry from a file using any supplied data file paths set in
 * dtCore::SetDataFilePathList().  Additional calls to this method will replace
 * the first geometry for the next.
 *
 * @param filename : The name of the file to be loaded
 * @param useCache : If true, use OSG's file cache
 */
osg::Node* Object::LoadFile(const std::string& filename, bool useCache)
{
   osg::Node *node = NULL;
   node = Loadable::LoadFile(filename, useCache);

   //We should always clear the geometry.  If LoadFile fails, we should have no geometry.
   if (GetMatrixNode()->getNumChildren() != 0)
   {
      GetMatrixNode()->removeChild(0,GetMatrixNode()->getNumChildren() );
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

         RefPtr<osg::MatrixTransform> offset = new osg::MatrixTransform();

         osg::Matrix tempMat;
         tempMat.makeTranslate( -bbv.mBoundingBox.center() );
         offset->setMatrix( tempMat );

         GetMatrixNode()->addChild(offset.get());
         offset->addChild(node);
      }
      else
      {
         GetMatrixNode()->addChild(node);
      }

      return node;
   }
   else
   {
      return NULL;
   }
}
