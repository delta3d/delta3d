#include <prefix/dtcoreprefix.h>
#include <dtCore/loadable.h>
#include <dtCore/refptr.h>
#include <dtUtil/log.h>

#include <osg/Node>
#include <osgDB/ReadFile>
#include <osgDB/Registry>
#include <osgDB/FileNameUtils>
#include <sstream>
#include <strstream>

using namespace dtCore;
using namespace dtUtil;

DataFilter *Loadable::smDataFilter = NULL;

Loadable::Loadable(void)
{
}

Loadable::~Loadable(void)
{
}

void Loadable::FlushObjectCache()
{
   osgDB::Registry::instance()->releaseGLObjects();
   osgDB::Registry::instance()->clearObjectCache();
}

void Loadable::SetFilter(DataFilter *filter)
{
   smDataFilter = filter;
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
      options->setObjectCacheHint(osgDB::ReaderWriter::Options::CACHE_ALL);
   }
   else
   {
      options->setObjectCacheHint(osgDB::ReaderWriter::Options::CACHE_NONE);
   }

   if(smDataFilter == NULL)
   {
      // No filter, so just do it the old way: let OSG do the load
      return osgDB::readNodeFile(mFilename, options.get());
   }

   osg::Node *model = NULL;

   // Find a ReaderWriter for the stream based on filename extension
   osgDB::ReaderWriter *rw = osgDB::Registry::instance()->getReaderWriterForExtension(osgDB::getLowerCaseFileExtension(mFilename));
   if(rw != NULL)
   {
      std::ifstream file(mFilename.c_str(), std::ios::in|std::ios::binary|std::ios::ate);
      if(file.is_open())
      {
         std::istream &filteredStream = smDataFilter->FilterData(file);
         file.close();
         osgDB::ReaderWriter::ReadResult rr = rw->readNode(filteredStream, options.get());
         model = rr.takeNode();
         smDataFilter->ShutdownFilter(file);
      }
   }

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
