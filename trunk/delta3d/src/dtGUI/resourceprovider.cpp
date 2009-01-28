#include <dtGUI/resourceprovider.h>
#include <dtCore/globals.h>
#include <dtUtil/log.h>


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
   std::string foundFilename = filename.c_str();

   // Look up resource group directory
   ResourceGroupMap::const_iterator iter =
      d_resourceGroups.find(resourceGroup.empty() ? d_defaultResourceGroup : resourceGroup);

   // If there was no entry for this group, then find the full path name
   if(iter == d_resourceGroups.end())
   {
      foundFilename = dtCore::FindFileInPathList(foundFilename);
      if(foundFilename.empty())
      {
         dtUtil::Log::GetInstance().LogMessage( dtUtil::Log::LOG_WARNING, __FUNCTION__,
            "dtGUI::ResourceProvider can't find file '%s'", filename.c_str());
      }
   }

   // If we have a filename, then try and load it now
   if(!foundFilename.empty())
   {
      //pass to base class for reading
      CEGUI::String foundFilenameString( foundFilename );

      CEGUI::DefaultResourceProvider::loadRawDataContainer(foundFilenameString, output, resourceGroup );
   }
}
