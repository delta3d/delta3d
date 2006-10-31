#include <dtGUI/resourceprovider.h>
#include <dtUtil/log.h>

#include <osgDB/FileUtils>

using namespace dtGUI;

ResourceProvider::ResourceProvider():
CEGUI::DefaultResourceProvider()
{

}


/** This will load the file with the supplied filename using the search paths
 *  find the files.
 *
 * @see SetDataFilePathList()
 */
void ResourceProvider::loadRawDataContainer(const CEGUI::String& filename, CEGUI::RawDataContainer& output, const CEGUI::String& resourceGroup)
{
   //find file
   std::string foundFilename = osgDB::findDataFile(filename.c_str());
   if (foundFilename.empty())
   {
      dtUtil::Log::GetInstance().LogMessage( dtUtil::Log::LOG_WARNING, __FUNCTION__,
         "dtGUI::ResourceProvider can't find file '%s'", filename.c_str());
   }
   else
   {
      //pass to base class for reading
      CEGUI::String foundFilenameString( foundFilename );

      CEGUI::DefaultResourceProvider::loadRawDataContainer(foundFilenameString, output, resourceGroup );
   }
}
