#include <prefix/dtcoreprefix.h>
#include <dtCore/loadable.h>
#include <dtCore/refptr.h>
#include <dtUtil/log.h>
#include <dtUtil/fileutils.h>

#include <osg/Node>
#include <osgDB/ReadFile>
#include <osgDB/Registry>

using namespace dtCore;
using namespace dtUtil;

////////////////////////////////////////////////////////////////////////////////
Loadable::Loadable(void)
{
}

////////////////////////////////////////////////////////////////////////////////
Loadable::~Loadable(void)
{
}

////////////////////////////////////////////////////////////////////////////////
void Loadable::FlushObjectCache()
{
   osgDB::Registry::instance()->releaseGLObjects();
   osgDB::Registry::instance()->clearObjectCache();
}

////////////////////////////////////////////////////////////////////////////////
osg::Node* Loadable::LoadFile(const std::string& filename, bool useCache)
{
   mFilename = filename;
   Log::GetInstance().LogMessage(Log::LOG_DEBUG, __FUNCTION__,
                                 "Loading '%s'", filename.c_str());

   // Attempt to clone our options if possible, otherwise options cannot be passed onto the plugins
   osgDB::Registry* reg = osgDB::Registry::instance();

   dtCore::RefPtr<osgDB::ReaderWriter::Options> options = reg->getOptions() ?
      static_cast<osgDB::ReaderWriter::Options*>(reg->getOptions()->clone(osg::CopyOp::SHALLOW_COPY)) :
   new osgDB::ReaderWriter::Options;

   if (useCache)
   {
      options->setObjectCacheHint(osgDB::ReaderWriter::Options::CACHE_ALL);
   }
   else
   {
      options->setObjectCacheHint(osgDB::ReaderWriter::Options::CACHE_NONE);
   }

   osg::Node* model = dtUtil::FileUtils::GetInstance().ReadNode(mFilename, options.get());
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
