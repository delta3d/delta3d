/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007 John K. Grant
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * John K. Grant, April 2007.
 */

#ifndef __DELTA_DTDIS_PLUGIN_MANAGER_H__
#define __DELTA_DTDIS_PLUGIN_MANAGER_H__

#include <string>                        // for parameter
#include <map>                           // for member
#include <dtDIS/createdestroypolicy.h>  // for host template
#include <dtDIS/idisplugin.h>            // for template parameter
#include <dtCore/sigslot.h>              // typedef, members
#include <dtDIS/dtdisexport.h>           // for export symbols

namespace dtDIS
{
   ///\brief manages a container of available plugins.
   /// notifies observers when plugins are added or removed to the container.
   class DT_DIS_EXPORT PluginManager
   {
   public:
      /// convenience typedef.
      typedef dtDIS::details::CreateDestroyPolicy<IDISPlugin> LibLoaderT;

      /// the stored type, also passed to observers upon notification.
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

      bool LoadLibraryHandle(const std::string& path, dtUtil::LibrarySharingManager::LibraryHandle* lh);

      /// a registry of all currently loaded plugins
      LibraryRegistry mPlugins;

      LibLoaderT mLoadStrategy;

      PluginSignal mLoaded;
      PluginSignal mUnloading;
   };
}

#endif  // __DELTA_DTDIS_PLUGIN_MANAGER_H__
