/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
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
 * Author:
 * Edited By: Jeff P. Houde
 */
#ifndef DIRECTORQT_PLUGIN_MANAGER
#define DIRECTORQT_PLUGIN_MANAGER

#include <dtDirectorQt/export.h>
#include <dtDirectorQt/plugininterface.h>
#include <list>
#include <map>
#include <string>
#include <QtCore/QObject>

namespace dtDirector
{
   class DirectorEditor;

   class DT_DIRECTOR_QT_EXPORT PluginManager : public QObject
   {
      Q_OBJECT

   public:

      PluginManager(DirectorEditor* editor);

      typedef std::map<std::string, PluginFactory*> PluginFactoryMap;
      typedef std::map<std::string, Plugin*> ActivePluginMap;

      /** load all libraries in dir and check for plugin factories */
      void LoadPluginsInDir(const std::string& path);

      /** start all plugins that are saved as active in the config file */
      void StartPluginsInConfigFile();

      /**
       * write the list of active plugins to config file so 
       * they can be started next time
       */
      void StoreActivePluginsToConfigFile();

      /** get names of all plugins */
      void GetAvailablePlugins(std::list<std::string>& toFill) const;

      /** get names of all currently instantiated plugins */
      void GetActivePlugins(std::list<std::string>& toFill) const;

      /** is there a factory for a plugin with this name? */
      bool FactoryExists(const std::string& name);

      /** Get PluginFactory for Plugin with this name.
       * @throw dtUtil::Exception if no PluginFactory exists with that Plugin name
       * @param name The name of the Plugin/PluginFactory to get
       */
      PluginFactory* GetPluginFactory(const std::string& name);

      /** is this plugin currently running? */
      bool IsInstantiated(const std::string& name);

      /** is this plugin a system plugin? */
      bool IsSystemPlugin(const std::string& name);

      /** return instance of plugin or NULL if not active */
      Plugin* GetPlugin(const std::string& name);

      /** returns all dependencies for a given plugin */
      std::list<std::string> GetPluginDependencies(const std::string& name);

   public slots:

      /** instantiate plugin with given name 
          @param name Name of plugin to start
          @param storeToConfig Store list of active plugins to config file?
      */
      void StartPlugin(const std::string& name, bool storeToConfig = true);

      /** stop and remove plugin with given name 
          @param name Name of plugin to stop
          @param storeToConfig Store list of active plugins to config file?
      */
      void StopPlugin(const std::string& name, bool storeToConfig = true);

   private:

      /** load plugin factory from library given by path */
      PluginFactory* LoadPluginFactory(const std::string& baseLibName);


      /** map from plugin name -> plugin factory */
      PluginFactoryMap mFactories;

      /** map from plugin name -> plugin instance */
      ActivePluginMap mActivePlugins;

      /** give plugins access to Editor */
      DirectorEditor* mEditor;
   };
}

#endif //DIRECTORQT_PLUGIN_MANAGER
