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


Object::Object(string name)
{
   RegisterInstance(this);

   SetName(name);

   osg::StateSet *stateSet = mNode->getOrCreateStateSet();
   stateSet->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
}

Object::~Object()
{
   Notify(DEBUG_INFO, "Object: Deleting '%s'", this->GetName().c_str());
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
osg::Node* Object::LoadFile(string filename, bool useCache)
{
   osg::Node *node = NULL;
   node = Loadable::LoadFile(filename, useCache);

   //attach our geometry node to the matrix node
   if (node!=NULL)
   {
      if (GetMatrixNode()->getNumChildren() != 0)
      {
         GetMatrixNode()->removeChild(0,GetMatrixNode()->getNumChildren() );
      }

      GetMatrixNode()->addChild(node);
      return node;
   }
   else
   {
      return NULL;
   }
}
