#include "spplugindialog.h"
#include <dtEditQt/pluginmanager.h>
#include <iostream>

const std::string PluginManagerPlugin::PLUGIN_NAME = "Plugin Manager";

PluginManagerPlugin::PluginManagerPlugin(dtEditQt::MainWindow* mw)
: mMainWindow(mw)
, mDialog(NULL)
, mPluginList(NULL)
{
   // add entry to edit menu that opens up the plugin manager
   mMainWindow->GetEditMenu()->addSeparator();
   QAction* action = mw->GetEditMenu()->addAction("Manage Plugins...");   
   connect(action, SIGNAL(triggered()), this, SLOT(onOpenDialog()));
}


PluginManagerPlugin::~PluginManagerPlugin()
{
}


void PluginManagerPlugin::onOpenDialog()
{
   
   // create the plugin list
   mDialog = new QDialog(mMainWindow);
   
   // load user interface from QT designer file
   Ui_PluginDialog ui;
   ui.setupUi(mDialog);
   mPluginList = ui.mPluginList;

   // connect cancel and accept buttons
   connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(onCloseDialog()));
   connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(onApplyChanges()));

   // show the dialog
   mDialog->setVisible(true);

   dtEditQt::PluginManager* manager = mMainWindow->GetPluginManager();

   // get a list of plugin factories
   std::list<std::string> plugins;
   manager->GetAvailablePlugins(plugins);

   for(std::list<std::string>::iterator i = plugins.begin(); i != plugins.end(); ++i)
   {
      // create list entry for this plugin
      QListWidgetItem* item = new QListWidgetItem((*i).c_str());
      
      // set checkbox to checked if plugin is active
      if(manager->IsInstantiated(*i))
      {
         item->setCheckState(Qt::Checked);
      }
      else
      {
         item->setCheckState(Qt::Unchecked);
      }

      // can't disable system plugins
      if(!manager->IsSystemPlugin(*i))
      {
         item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
      }
      else
      {
         item->setFlags(Qt::ItemIsUserCheckable);
      }
      // add to list
      ui.mPluginList->addItem(item);
   }
}


// user has closed the dialog
void PluginManagerPlugin::onCloseDialog()
{
   mDialog->close();
   mDialog->deleteLater();
   mPluginList = NULL;
   mDialog = NULL;
}


// user has pressed OK button. Start and stop plugins
void PluginManagerPlugin::onApplyChanges()
{
   dtEditQt::PluginManager* manager = mMainWindow->GetPluginManager();

   for(int i = 0; i < mPluginList->count(); ++i)
   {
      QListWidgetItem* item = mPluginList->item(i);
      std::string name = item->text().toStdString();
      bool enabled = (item->checkState() == Qt::Checked);
            
      // can't load/unload system plugins
      if(manager->IsSystemPlugin(name))
      {
         continue;
      }

      if(!enabled && manager->IsInstantiated(name))
      {
         manager->StopPlugin(name);        
      }
      else if(enabled && !manager->IsInstantiated(name))
      {
         manager->StartPlugin(name);
      }
   }

   onCloseDialog();
}


/////////////////////////////////////////////////

namespace PluginDialog
{
class DT_PLUGIN_MANAGER_EXPORT PluginFactory : public dtEditQt::PluginFactory
{
public:

   PluginFactory() 
   {
      // always start this at STAGE start
      mIsSystemPlugin = true;
   }

   ~PluginFactory() {}

   /** get the name of the plugin */
   virtual std::string GetName() { return PluginManagerPlugin::PLUGIN_NAME; }

   /** get a description of the plugin */
   virtual std::string GetDescription() { return "A simple exaple of a STAGE plugin"; }

    /** construct the plugin and return a pointer to it */
   virtual dtEditQt::Plugin* Create(dtEditQt::MainWindow* mw) 
   { 
      mPlugin = new PluginManagerPlugin(mw);
      return mPlugin;
   }

   virtual void Destroy() { delete mPlugin; }

private:

   dtEditQt::Plugin* mPlugin;

}; 
} //namespace PluginDialog

// library entry point
extern "C" DT_PLUGIN_MANAGER_EXPORT dtEditQt::PluginFactory* CreatePluginFactory()
{
   return new PluginDialog::PluginFactory;
}
