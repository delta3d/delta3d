#ifndef DELTA_PLUGIN_MANAGER
#define DELTA_PLUGIN_MANAGER

#include <dtEditQt/export.h>
#include <dtEditQt/plugininterface.h>
#include <list>
#include <map>
#include <string>
#include <QtCore/QObject>

namespace dtEditQt
{

   class MainWindow;

   class DT_EDITQT_EXPORT PluginManager : public QObject
   {
      Q_OBJECT

   public:

      PluginManager(MainWindow* mw);

      typedef std::map<std::string,PluginFactory*> PluginFactoryMap;
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

      /** is this plugin currently running? */
      bool IsInstantiated(const std::string& name);

      /** is this plugin a system plugin? */
      bool IsSystemPlugin(const std::string& name);

      /** return instance of plugin or NULL if not active */
      Plugin* GetPlugin(const std::string& name);

      /** returns all dependencies for a given plugin */
      std::list<std::string> GetPluginDependencies(std::string name);

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

      /** get factory for plugin with this name */
      PluginFactory* GetPluginFactory(const std::string& name);

      /** map from plugin name -> plugin factory */
      PluginFactoryMap mFactories;

      /** map from plugin name -> plugin instance */
      ActivePluginMap mActivePlugins;

      /** give plugins acces to GUI */
      MainWindow* mMainWindow;

   };
}

#endif //DELTA_PLUGIN_MANAGER
