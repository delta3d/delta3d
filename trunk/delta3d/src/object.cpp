// object.cpp: implementation of the Object class.
//
//////////////////////////////////////////////////////////////////////

#include "object.h"
#include <osgDB/ReadFile>
#include <osgDB/Registry>
#include "notify.h"
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
      mObject.get()->GetTransform(&xform);
      sgMat4 mat;
      xform.Get(mat);

      xformNode->setMatrix( osg::Matrix((float*)mat) );

      traverse(node, nv);
   }
private:
   osg::ref_ptr<dtCore::Object> mObject;
};


Object::Object(string name)
{
   RegisterInstance(this);

   SetName(name);
   mNode = new osg::MatrixTransform;
   osg::StateSet *stateSet = mNode->getOrCreateStateSet();
   stateSet->setMode(GL_NORMALIZE, osg::StateAttribute::ON);

   //hookup an update callback on this node
   mNode.get()->setUpdateCallback(new _updateCallback(this));
}

Object::~Object()
{
   Notify(DEBUG_INFO, "Object: Deleting '%s'", this->GetName().c_str());
   DeregisterInstance(this);
   mNode = NULL;
}


/*!
 * Load a geometry from a file using any supplied data file paths set in
 * dtCore::SetDataFilePathList().  Additional calls to this method will just combine
 * the loaded files together.
 *
 * @param filename : The name of the file to be loaded
 * @param useCache : If true, use OSG's object cache
 */
bool Object::LoadFile(string filename, bool useCache)
{
   bool retVal = false;

   mFilename = filename;
   Notify(DEBUG_INFO, "Object:Loading %s...", mFilename.c_str());

   osgDB::Registry::CacheHintOptions cache = osgDB::Registry::CACHE_NONE;
   if (useCache) cache = osgDB::Registry::CACHE_ALL;

   osg::Node *model = osgDB::readNodeFile(mFilename, cache);
   if (model != NULL)
   {
      // this crashes - prolly should be called from the Update traversal
      if (mNode.get()->getNumChildren() != 0)
      {
         mNode.get()->removeChild(0,mNode.get()->getNumChildren() );
      }
      mNode.get()->addChild( model );
      retVal = true;
   }
   else
   {
      Notify(WARN, "Object: Can't load %s", mFilename.c_str());
      retVal = false;
   }
   return retVal;
}
