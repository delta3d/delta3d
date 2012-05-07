////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <CEGUI/CEGUIExceptions.h>
#include <dtGUI/resourceprovider.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/log.h>
#include <dtUtil/refstring.h>



using namespace dtGUI;

////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
////////////////////////////////////////////////////////////////////////////////
ResourceProvider::ResourceProvider()
   : CEGUI::DefaultResourceProvider()
{
}

////////////////////////////////////////////////////////////////////////////////
void ResourceProvider::loadRawDataContainer(const CEGUI::String& filename, CEGUI::RawDataContainer& output, const CEGUI::String& resourceGroup)
{
   std::string strFilename(filename.c_str());
   std::string strResGroup(resourceGroup.c_str());

   // Look up resource group directory
   ResourceGroupMap::const_iterator iter =
      d_resourceGroups.find(resourceGroup.empty() ? d_defaultResourceGroup : resourceGroup);

   // If there was no entry for this group, then find the full path name
   bool found = false;
   if(iter != d_resourceGroups.end())
   {
      try
      {
         //pass to base class for reading
         CEGUI::DefaultResourceProvider::loadRawDataContainer(filename, output, resourceGroup);
         found = true;
      }
      catch(CEGUI::Exception& e)
      {
         LOG_INFO(e.getMessage().c_str());
      }
   }

   if(!found)
   {
      typedef std::pair<StrStrMultiMap::iterator, StrStrMultiMap::iterator> StrStrIterPair;
      StrStrIterPair range = mResGroupSearchSuffixMap.equal_range(strResGroup);

      std::string foundFilename(strFilename);
      StrStrMultiMap::iterator curIter = range.first;
      for(; curIter != range.second; ++curIter)
      {
         const std::string& path = curIter->second;
         const std::string combinedPath = dtUtil::FileUtils::ConcatPaths(path, strFilename);
         foundFilename = dtUtil::FindFileInPathList(combinedPath);
         if(!foundFilename.empty())
         {
            break;
         }
      }

      if(foundFilename.empty())
      {
         std::string errorStr =
            "dtGUI::ResourceProvider can't find file '" +
            std::string(filename.c_str()) + "'.";

         throw CEGUI::FileIOException(errorStr);
      }
      else
      {
         //pass to base class for reading
         CEGUI::String foundFilenameString(foundFilename);
         CEGUI::DefaultResourceProvider::loadRawDataContainer(foundFilenameString, output, resourceGroup );
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
bool ResourceProvider::AddSearchSuffix(const std::string& resourceGroup,
                                       const std::string& searchSuffix)
{
   dtUtil::RefString suffix = searchSuffix;

   size_t numElements = mResGroupSearchSuffixMap.size();
   mResGroupSearchSuffixMap.insert(std::make_pair(resourceGroup, suffix));
   return numElements < mResGroupSearchSuffixMap.size();
}

////////////////////////////////////////////////////////////////////////////////
bool ResourceProvider::RemoveSearchSuffix(const std::string& resourceGroup,
                                          const std::string& searchSuffix)
{
   bool success = false;

   StrStrMultiMap::iterator foundIter = mResGroupSearchSuffixMap.lower_bound(searchSuffix);

   if(foundIter != mResGroupSearchSuffixMap.end())
   {
      mResGroupSearchSuffixMap.erase(foundIter);
      success = true;
   }

   return success;
}

////////////////////////////////////////////////////////////////////////////////
unsigned ResourceProvider::RemoveSearchSuffixes(const std::string& resourceGroup)
{
   size_t numElements = mResGroupSearchSuffixMap.size();

   typedef std::pair<StrStrMultiMap::iterator, StrStrMultiMap::iterator> StrStrIterPair;
   StrStrIterPair range = mResGroupSearchSuffixMap.equal_range(resourceGroup);
   mResGroupSearchSuffixMap.erase(range.first, range.second);

   return unsigned(numElements - mResGroupSearchSuffixMap.size());
}

////////////////////////////////////////////////////////////////////////////////
unsigned ResourceProvider::ClearSearchSuffixes()
{
   unsigned numElements = unsigned(mResGroupSearchSuffixMap.size());
   mResGroupSearchSuffixMap.clear();
   return numElements;
}
