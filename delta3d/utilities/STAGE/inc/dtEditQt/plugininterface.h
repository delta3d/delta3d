#ifndef DELTA_PLUGIN_INTERFACE
#define DELTA_PLUGIN_INTERFACE

#include <list>
#include <string>

namespace dtEditQt
{

   class MainWindow;

   /**
      Abstract interface class for STAGE plugins
    */
   class Plugin
   {
      public:

      virtual ~Plugin() {}   

      /** Is called after instantiation */
      virtual void Create() {}

      /** Is called before destruction */
      virtual void Destroy() {}
    
   };

   /** 
     A plugin factory is used by the plugin manager to identify available
     plugins and instantiate them.
   */
   class PluginFactory
   {
   public:

      PluginFactory()
         : mIsSystemPlugin(false)
      {
      }

      virtual ~PluginFactory() {}

      /** construct the plugin and return a pointer to it */
      virtual Plugin* Create(MainWindow* mw) = 0;

      /** delete the plugin */
      virtual void Destroy() = 0;
      
      /** get the name of the plugin */
      virtual std::string GetName() = 0;

      /** get a description of the plugin */
      virtual std::string GetDescription() = 0;

      /** 
        fill list with names of all plugins this plugin depends on.
        WARNING: circular dependencies are not handled and
        will cause a crash!
      */
      virtual void GetDependencies(std::list<std::string>&) {};

      /** 
         get the version of STAGE that the plugin is compiled against 
         Only plugins compiled against the current version of STAGE are started
      */
      virtual std::string GetExpectedSTAGEVersion() 
      { 
         // should be replaced by SVN to give version number
         return "$Revision$"; 
      }

      /** Should plugin be started autmatically? */
      bool IsSystemPlugin() { return mIsSystemPlugin; }

   protected:

      // set this to true if plugin should always be started
      bool mIsSystemPlugin;

   };

}

#endif //DELTA_PLUGIN_INTERFACE
