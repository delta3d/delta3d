#include "dtCore/loadable.h"
#include "dtCore/refptr.h"
#include <dtUtil/log.h>

#include <osg/Node>
#include <osgDB/ReadFile>
#include <osgDB/Registry>

using namespace dtCore;
using namespace dtUtil;

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
osg::Node* Loadable::LoadFile(const std::string& filename, bool useCache)
{   
   mFilename = filename;
   Log::GetInstance().LogMessage(Log::LOG_DEBUG, __FUNCTION__,
                                 "Loading '%s'", filename.c_str());

   RefPtr<osgDB::ReaderWriter::Options> options = new osgDB::ReaderWriter::Options;

   if (useCache)
   {
      options.get()->setObjectCacheHint(osgDB::ReaderWriter::Options::CACHE_ALL);
   }
   else
   {  
      options.get()->setObjectCacheHint(osgDB::ReaderWriter::Options::CACHE_NONE);
   }

   osg::Node *model = osgDB::readNodeFile(mFilename, options.get());
   if (model != 0)
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
      Log::GetInstance().LogMessage(Log::LOG_WARNING, __FUNCTION__, 
               "Can't load '%s'", mFilename.c_str() );
      return NULL;
   }
   
}
