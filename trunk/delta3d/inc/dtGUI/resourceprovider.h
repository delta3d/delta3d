#ifndef RESOURCEPROVIDER_INCLUDED
#define RESOURCEPROVIDER_INCLUDED

#include "CEGUI/CEGUIDefaultResourceProvider.h"

namespace dtGUI
{
   ///A simple CEGUI ResourceProvider based on the CEGUI::DefaultResourceProvider

   /** Add the functionality of using search paths to find the data files
     * used by CEGUI.
     *
     * @see SetDataFilePathList()
     */
   class ResourceProvider :   public CEGUI::DefaultResourceProvider
   {
   public:
      ResourceProvider();
      ~ResourceProvider(){};

      ///Load the data of the supplied filename.
      void loadRawDataContainer(const CEGUI::String& filename, CEGUI::RawDataContainer& output, const CEGUI::String& resourceGroup);

   };
}

#endif //RESOURCEPROVIDER_INCLUDED
