#ifndef _DTDIS_PLUGIN_MANAGER_H_
#define _DTDIS_PLUGIN_MANAGER_H_

#include <string>                        // for parameter
#include <map>                           // for member
#include <dtDIS/createdestroypolicy.h>  // for host template
#include <dtDIS/idisplugin.h>            // for template parameter
#include <dtCore/sigslot.h>              // typedef, members
#include <dtDIS/dtdisexport.h>           // for export symbols

namespace dtDIS
{
   /// \todo does this duplicate the dtUtil::LibraryManager code.
   ///\brief manages a container of available plugins.
   /// instantiates a GMComponent from the plugin.
   /// notifies observers when plugins are added or removed to the container.
   class DT_DIS_EXPORT PluginManager
   {
   public:
      typedef dtDIS::details::CreateDestroyPolicy<IDISPlugin> LibLoaderT;
      typedef LibLoaderT::LibraryInterface RegistryEntry;

      /// specifies the required function signature for slots observing plugin events.
      typedef sigslot::signal2<const std::string&, RegistryEntry&> PluginSignal;

      /// a mapping from the library path loaded to an object with defined entry points.
      typedef std::map<std::string,RegistryEntry> LibraryRegistry;

      /// searches for the file, notifies observers of the new plugin.
      /// @param path the file path for the library to be loaded.
      /// @return true if the plugin was successfully added to the registry
      bool LoadPlugin(const std::string& path);

      /// notifies observers that the memory will be released if the path is a loaded library,
      /// releases the memory for library.
      /// @param path the file path for the library to be loaded.
      /// @return true if the plugin path was found in the registry.
      bool UnloadPlugin(const std::string& path);

      /// get the signal instance in order to connect or disconnet to the loaded event.
      PluginSignal& GetLoadedSignal();

      /// get the signal instance in order to connect or disconnet to the unloaded event.
      PluginSignal& GetUnloadedSignal();

      /// get the plugin container
      const LibraryRegistry& GetRegistry() const;

      /// get the plugin container
      LibraryRegistry& GetRegistry();

      /// will call this->UnloadPlugin for all of the known plugins.
      void UnloadAllPlugins();

   private:
      typedef LibraryRegistry::iterator LibRegIter;
      void UnloadImplementation(LibRegIter& iter);

      /// a registry of all currently loaded plugins
      LibraryRegistry mPlugins;

      LibLoaderT mLoadStrategy;

      PluginSignal mLoaded;
      PluginSignal mUnloading;
   };
}

#endif  // _DTDIS_PLUGIN_MANAGER_H_
