// object.cpp: implementation of the Object class.
//
//////////////////////////////////////////////////////////////////////

#include "dtCore/object.h"
#include "dtCore/scene.h"
#include <osgDB/ReadFile>
#include <osgDB/Registry>
#include "dtCore/notify.h"
#include "dtCore/loadable.h"

using namespace dtCore;
using namespace std;

IMPLEMENT_MANAGEMENT_LAYER(Object)

///An update callback for a visual Object
class _updateCallback : public osg::NodeCallback
{
public:
   _updateCallback(dtCore::Object *object)
      :  mObject(NULL)
   {
      mObject = object;
   }

   virtual void operator()(osg::Node* node, osg::NodeVisitor *nv)
   {
      osg::MatrixTransform *xformNode = (osg::MatrixTransform*)node;

      Transform xform;
      mObject->GetTransform(&xform);
      sgMat4 mat;
      xform.Get(mat);

      xformNode->setMatrix( osg::Matrix((float*)mat) );

      traverse(node, nv);
   }
private:
   dtCore::Object* mObject;
};


Object::Object(string name)
{
   RegisterInstance(this);

   SetName(name);
   //mNode = new osg::MatrixTransform;
   osg::StateSet *stateSet = mNode->getOrCreateStateSet();
   stateSet->setMode(GL_NORMALIZE, osg::StateAttribute::ON);

   //hookup an update callback on this node
   //mNode.get()->setUpdateCallback(new _updateCallback(this));
}

Object::~Object()
{
   Notify(DEBUG_INFO, "Object: Deleting '%s'", this->GetName().c_str());
   DeregisterInstance(this);
   //mNode = NULL;
}


/*!
 * Load a geometry from a file using any supplied data file paths set in
 * dtCore::SetDataFilePathList().  Additional calls to this method will just combine
 * the loaded files together.
 *
 * @param filename : The name of the file to be loaded
 * @param useCache : If true, use OSG's object cache
 */
osg::Node* Object::LoadFile(string filename, bool useCache)
{
   osg::Node *node = NULL;
   node = Loadable::LoadFile(filename, useCache);

   //attach our geometry node to the matrix node
   if (node!=NULL)
   {
      //mMatrixNode->addChild(mDrawableNode.get());
      GetMatrixNode()->addChild(node);
      return node;
   }
   else
   {
      return NULL;
   }
}
