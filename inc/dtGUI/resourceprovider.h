#ifndef RESOURCEPROVIDER_INCLUDED
#define RESOURCEPROVIDER_INCLUDED

#include <dtUtil/refstring.h>
#include <dtGUI/export.h>
#include <CEGUI/CEGUIDefaultResourceProvider.h>

namespace dtUtil
{
   class RefString;
}

namespace dtGUI
{

   ///A simple CEGUI ResourceProvider based on the CEGUI::DefaultResourceProvider

   /** Add the functionality of using search paths to find the data files
     * used by CEGUI.
     *
     * @see SetDataFilePathList()
     */
   class DT_GUI_EXPORT ResourceProvider : public CEGUI::DefaultResourceProvider
   {
   public:
      ResourceProvider();
      ~ResourceProvider(){};

      ///Load the data of the supplied filename.
      void loadRawDataContainer(const CEGUI::String& filename, CEGUI::RawDataContainer& output, const CEGUI::String& resourceGroup);

      /**
       * Add an alternate sub-directory (relative to any project context)
       * to search when trying to find a file in a specified resource group.
       * @param resourceGroup The resource group for the alternate search suffix.
       * @param searchSuffix The sub-directory to append to existing project search
       *        paths. This suffix will be used in case a file is not found for the
       *        specified resource group.
       * @return TRUE if the suffix was successfully added.
       */
      bool AddSearchSuffix(const std::string& resourceGroup, const std::string& searchSuffix);

      /**
       * Remove a previously added search suffix for a specified resource group.
       * @param resourceGroup The resource group for the alternate search suffix.
       * @param searchSuffix The sub-directory search suffix that was previously added
       *        for the specified resource group. This should match exactly as it was added.
       * @return TRUE if the suffix was successfully found and removed.
       */
      bool RemoveSearchSuffix(const std::string& resourceGroup, const std::string& searchSuffix);

      /**
       * Remove all previously added search suffixes for a specified resource group.
       * @param resourceGroup The resource group for the alternate search suffixes.
       * @return The number of suffixes that were successfully removed.
       */
      unsigned RemoveSearchSuffixes(const std::string& resourceGroup);

      /**
       * Remove all previously added search suffixes for all resource groups.
       * @return The number of suffixes that were successfully removed.
       */
      unsigned ClearSearchSuffixes();

   private:
      typedef std::multimap<dtUtil::RefString, dtUtil::RefString> StrStrMultiMap;
      StrStrMultiMap mResGroupSearchSuffixMap;
   };
}

#endif //RESOURCEPROVIDER_INCLUDED
