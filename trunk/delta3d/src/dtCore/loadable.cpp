#include "dtcore/loadable.h"
#include "dtCore/notify.h"

#include <osgDB/ReadFile>
#include <osgDB/Registry>

using namespace dtCore;

Loadable::Loadable(void)
{
}

Loadable::~Loadable(void)
{
}


/*!
* Load a geometry from a file using any supplied data file paths set in
* dtCore::SetDataFilePathList().  Additional calls to this method will just combine
* the loaded files together.
*
* @param filename : The name of the file to be loaded
* @param useCache : If true, use OSG's object cache
*/
osg::Node* Loadable::LoadFile(std::string filename, bool useCache)
{   
   mFilename = filename;
   Notify(DEBUG_INFO, "Loadable:Loading %s...", filename.c_str());

   osg::ref_ptr <osgDB::ReaderWriter::Options> options = new osgDB::ReaderWriter::Options;

   if (useCache)
   {
      options.get()->setObjectCacheHint(osgDB::ReaderWriter::Options::CACHE_ALL);
   }
   else
   {  
      options.get()->setObjectCacheHint(osgDB::ReaderWriter::Options::CACHE_NONE);
   }

   osg::Node *model = osgDB::readNodeFile(mFilename, options.get());
   if (model != NULL)
   {
      // this crashes - prolly should be called from the Update traversal
//      if (mDrawableNode.get()->getNumChildren() != 0)
//      {
//         mDrawableNode.get()->removeChild(0,mDrawableNode.get()->getNumChildren() );
//      }

      //mDrawableNode.get()->addChild( model );
      return model;
   }
   else
   {
      Notify(WARN, "Loadable: Can't load %s", mFilename.c_str());
      return NULL;
   }
   
}
