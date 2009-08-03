#include <dtEditQt/pluginmanager.h>

#include <dtEditQt/configurationmanager.h>
#include <dtEditQt/mainwindow.h>
#include <dtUtil/exception.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/librarysharingmanager.h>
#include <dtUtil/log.h>
#include <dtUtil/stringutils.h>
#include <iostream>
#include <QtCore/QDir>
#include <QtGui/QMessageBox>
#include <sstream>

using namespace dtUtil;

namespace dtEditQt
{

   PluginManager::PluginManager(MainWindow* mw)
      : mMainWindow(mw)
   {
   }


   /** load all dlls in dir and check for plugin factories */
   void PluginManager::LoadPluginsInDir(const std::string& path)
   {
      // find out library extension for this system
      // take care of debug/release library stuff on windows
      #if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
         std::string libExtension = ".dll";
      #else
         std::string libExtension = ".so";
      #endif

      // get libs from directory
      FileExtensionList ext;
      ext.push_back(libExtension);
      DirectoryContents files;

      try
      {
         files = FileUtils::GetInstance().DirGetFiles(path, ext);
      }
      catch (const dtUtil::Exception& e)
      {
         //in case the path is bogus
         DTUNREFERENCED_PARAMETER(e);
      }


      if (!files.empty())
      {
         //add the path to the list of paths to search for libraries.
         LibrarySharingManager::GetInstance().AddToSearchPath(path);
      }

      // for each library in dir
      DirectoryContents::const_iterator i;
      for(i = files.begin(); i != files.end(); ++i)
      {

         std::string fileName = *i;

         try
         {
            // load the plugin library
            const std::string basePluginName = LibrarySharingManager::GetPlatformIndependentLibraryName(fileName);
            PluginFactory* factory = LoadPluginFactory(basePluginName);
            std::string name = factory->GetName();

            // check if a plugin with this name already exists
            if(mActivePlugins.find(name) != mActivePlugins.end())
            {
               std::ostringstream msg;
               msg << "Unable to load plugin " << name <<": A plugin with that name was already loaded!";
               throw Exception(msg.str(), __FILE__, __LINE__);
            }

            // insert factory into factory list
            mFactories[name] = factory;

            // factory exists, but plugin is not instantiated yet
            mActivePlugins[name] = NULL;


            // start system plugins immediately
            if(factory->IsSystemPlugin())
            {
               StartPlugin(name, false);
            }
         }
         catch(Exception&)
         {
            LOG_ERROR("Can't load plugin " + (*i));
         }
      }
   }


   PluginFactory* PluginManager::LoadPluginFactory(const std::string& baseLibName)
   {
      // use library sharing manager to do the actual library loading
      LibrarySharingManager& lsm = LibrarySharingManager::GetInstance();

      dtCore::RefPtr<LibrarySharingManager::LibraryHandle> libHandle;
      try
      {
         libHandle = lsm.LoadSharedLibrary(baseLibName, true);
      }
      catch (Exception)
      {
         std::ostringstream msg;
         msg << "Unable to load plugin " << baseLibName;
         throw Exception(msg.str(), __FILE__, __LINE__);
      }

      LibrarySharingManager::LibraryHandle::SYMBOL_ADDRESS createFn;
      createFn = libHandle->FindSymbol("CreatePluginFactory");

      //Make sure the plugin actually implemented these functions and they
      //have been exported.
      if (!createFn)
      {
         std::ostringstream msg;
         msg << "Plugin " << baseLibName << " does not contain symbol 'CreatePluginFactory'";
         throw Exception(msg.str(), __FILE__, __LINE__);
      }

      // typedef for function pointer to get factory from library
      typedef PluginFactory* (*CreatePluginFactoryFn)();

      // instantiate factory
      CreatePluginFactoryFn fn = (CreatePluginFactoryFn) createFn;
      PluginFactory* factory = fn();

      // check if the library was compiled against this revision of STAGE
      if(factory->GetExpectedSTAGEVersion() != "$Revision$")
      {
         std::ostringstream msg;
         msg << "Can't load plugin " << baseLibName << ": Built against wrong version";
         throw Exception(msg.str(), __FILE__, __LINE__);
      }
      else
      {
         return factory;
      }
   }


   /** get the list of plugins to start from config file and start them */
   void PluginManager::StartPluginsInConfigFile()
   {
      // get config string from manager      
      std::string activated = 
         ConfigurationManager::GetInstance().GetVariable(ConfigurationManager::PLUGINS, CONF_MGR_PLUGINS_ACTIVATED);

      if(activated == "")
      {
         return;
      }

      // split config string by slashes
      std::vector<std::string> tokens;
      StringTokenizer<IsSlash>::tokenize(tokens, activated);

      // for each token
      std::vector<std::string>::iterator iter;
      for(iter = tokens.begin(); iter != tokens.end(); ++iter)
      {
         std::string name = *iter;
         //if the plugin can be started and was not yet started
         if(FactoryExists(name) && !IsInstantiated(name))
         {
            // start it!
            StartPlugin(name, false);
         }
      }
   }


   /**
    * write the list of active plugins to config file so
    * they can be started next time
    */
   void PluginManager::StoreActivePluginsToConfigFile()
   {
      // create string with names of active plugins separated by slash
      std::ostringstream os;
      std::list<std::string> plugins;
      GetActivePlugins(plugins);
      for(std::list<std::string>::iterator i = plugins.begin(); i != plugins.end(); ++i)
      {
         // system plugins are always started, so no need to include them
         if(!IsSystemPlugin(*i))
         {
            os << *i << "/";
         }
      }
      
      ConfigurationManager::GetInstance().SetVariable(ConfigurationManager::PLUGINS, CONF_MGR_PLUGINS_ACTIVATED, os.str());
   }


   PluginFactory* PluginManager::GetPluginFactory(const std::string& name)
   {
      PluginFactoryMap::iterator i = mFactories.find(name);
      if(i == mFactories.end())
      {
         throw Exception("Plugin not found with name " + name , __FILE__, __LINE__);
      }
      return i->second;
   }


   bool PluginManager::FactoryExists(const std::string& name)
   {
      return (mFactories.find(name) != mFactories.end());
   }


   void PluginManager::GetAvailablePlugins(std::list<std::string>& toFill) const
   {
      PluginFactoryMap::const_iterator iter;
      for(iter = mFactories.begin(); iter != mFactories.end(); ++iter)
      {
         PluginFactory* factory = (*iter).second;
         toFill.push_back(factory->GetName());
      }
   }


   void PluginManager::GetActivePlugins(std::list<std::string>& toFill) const
   {
      ActivePluginMap::const_iterator iter;
      for(iter = mActivePlugins.begin(); iter != mActivePlugins.end(); ++iter)
      {
         if((*iter).second != NULL)
         {
            toFill.push_back((*iter).first);
         }
      }
   }


   std::list<std::string> PluginManager::GetPluginDependencies(std::string name)
   {
      std::list<std::string> deps;
      PluginFactory* factory = GetPluginFactory(name);

      if (factory)
      {
         factory->GetDependencies(deps);
      }

      return deps;
   }


   void PluginManager::StartPlugin(const std::string& name, bool storeToConfig)
   {
      LOG_ALWAYS("Starting plugin " + name);
      PluginFactory* factory = GetPluginFactory(name);

      // start all plugins this plugin depends on
      std::list<std::string> deps;
      factory->GetDependencies(deps);

      while(!deps.empty())
      {
         std::string dependency = deps.front();
         deps.pop_front();

         // check if dependency can be fulfilled
         if(!FactoryExists(dependency))
         {
            std::ostringstream os;
            os << "Cannot start plugin " << name << ": It depends on plugin ";
            os << dependency << " which was not found.";
            QMessageBox::critical(mMainWindow, "Error", os.str().c_str(), "Ok");
            return;
         }

         // only start dependency if it is not running now
         if(!IsInstantiated(dependency))
         {
            StartPlugin(dependency, false);
         }
      }

      // use factory to create the plugin
      mActivePlugins[name] = factory->Create(mMainWindow);

      // call Create() callback of plugin
      mActivePlugins[name]->Create();

      // remember change
      if(storeToConfig)
      {
         StoreActivePluginsToConfigFile();
      }
   }


   void PluginManager::StopPlugin(const std::string& name, bool storeToConfig)
   {
      // first check if plugin is actually running
      if(!IsInstantiated(name))
      {
         return;
      }

      // Check if any other plugins depend on this one, and stop them as well.
      std::list<std::string> activePlugins;
      GetActivePlugins(activePlugins);
      while(!activePlugins.empty())
      {
         std::string plugin = activePlugins.front();
         activePlugins.pop_front();

         PluginFactory* factory = GetPluginFactory(plugin);

         // start all plugins this plugin depends on
         std::list<std::string> deps;
         factory->GetDependencies(deps);
         while(!deps.empty())
         {
            std::string dependency = deps.front();
            deps.pop_front();

            // If the active plugin depends on this plugin, then we need to stop that one too.
            if (dependency == name)
            {
               StopPlugin(plugin);
               break;
            }
         }
      }

      LOG_ALWAYS("Stopping plugin " + name);

      Plugin* plugin = GetPlugin(name);

      // call Destroy() callback of plugin
      plugin->Destroy();

      // tell factory to delete the plugin
      GetPluginFactory(name)->Destroy();

      // erase plugin from list of active plugins
      mActivePlugins.erase(mActivePlugins.find(name));

      // remember change
      if(storeToConfig)
      {
         StoreActivePluginsToConfigFile();
      }
   }


   bool PluginManager::IsInstantiated(const std::string& name)
   {
      // all plugin names are in mActivePlugins, but names of non-active plugins
      // are mapped to NULL
      return mActivePlugins[name] != NULL;
   }


   /** is the plugin a system plugin? */
   bool PluginManager::IsSystemPlugin(const std::string& name)
   {
      return GetPluginFactory(name)->IsSystemPlugin();
   }


   Plugin* PluginManager::GetPlugin(const std::string& name)
   {
      // first check if plugin exists. return NULL if it doesn't
      ActivePluginMap::iterator i = mActivePlugins.find(name);
      if(i == mActivePlugins.end())
      {
         return NULL;
      }
      return i->second;
   }
}
