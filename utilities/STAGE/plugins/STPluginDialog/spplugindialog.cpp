#include "spplugindialog.h"
#include <dtEditQt/pluginmanager.h>
#include <QtGui/QMenu>
#include <iostream>

const std::string PluginManagerPlugin::PLUGIN_NAME = "Plugin Manager";

//////////////////////////////////////////////////////////////////////////
PluginManagerPlugin::PluginManagerPlugin(dtEditQt::MainWindow* mw)
: mMainWindow(mw)
{
   // add entry to edit menu that opens up the plugin manager
   mMainWindow->GetEditMenu()->addSeparator();
   QAction* action = mw->GetEditMenu()->addAction(tr("Manage Plugins..."));   
   connect(action, SIGNAL(triggered()), this, SLOT(OnOpenDialog()));
}


//////////////////////////////////////////////////////////////////////////
PluginManagerPlugin::~PluginManagerPlugin()
{
}

//////////////////////////////////////////////////////////////////////////
void PluginManagerPlugin::OnOpenDialog()
{   
   // create the plugin list
   QDialog dialog(mMainWindow);
   
   // load user interface from QT designer file
   Ui_PluginDialog ui;
   ui.setupUi(&dialog);
   
   connect(ui.mPluginList, SIGNAL(itemChanged(QListWidgetItem*)),
           this, SLOT(OnPluginChanged(QListWidgetItem*)));


   dtEditQt::PluginManager* manager = mMainWindow->GetPluginManager();
   if (manager == NULL) { return; }

   // get a list of plugin factories
   std::list<std::string> plugins;
   manager->GetAvailablePlugins(plugins);

   for (std::list<std::string>::iterator i = plugins.begin(); i != plugins.end(); ++i)
   {
      // create list entry for this plugin
      QListWidgetItem* item = new QListWidgetItem((*i).c_str());
      std::string pluginDesc;
      try
      {
         dtEditQt::PluginFactory* factory = manager->GetPluginFactory((*i));
         pluginDesc = factory->GetDescription();
      }
      catch (...)
      {
         //factory doesn't exist?
         pluginDesc = "No plugin description";
      }

      item->setToolTip(QString::fromStdString(pluginDesc));
      
      // set checkbox to checked if plugin is active
      if (manager->IsInstantiated(*i))
      {
         item->setCheckState(Qt::Checked);
      }
      else
      {
         item->setCheckState(Qt::Unchecked);
      }

      // can't disable system plugins
      if (!manager->IsSystemPlugin(*i))
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

   // show the dialog
   if (dialog.exec() == QDialog::Accepted)
   {
      OnApplyChanges(ui.mPluginList);
   }
}


//////////////////////////////////////////////////////////////////////////
// user has checked/unchecked a plugin.
void PluginManagerPlugin::OnPluginChanged(QListWidgetItem* pluginItem)
{
   dtEditQt::PluginManager* manager = mMainWindow->GetPluginManager();
   if (!manager) return;

   const std::string pluginName = pluginItem->text().toStdString();

   // When activating a plugin, make sure we also activate any
   // plugins that it depends on.
   if (pluginItem->checkState() == Qt::Checked)
   {
      std::list<std::string> deps = manager->GetPluginDependencies(pluginName);
      while(!deps.empty())
      {
         std::string dependency = deps.front();
         deps.pop_front();

         QList<QListWidgetItem*> pluginItems = pluginItem->listWidget()->findItems(QString(dependency.c_str()), Qt::MatchExactly);

         // check if dependency can be fulfilled
         if (pluginItems.empty())
         {
            pluginItem->setCheckState(Qt::Unchecked);
            return;
         }

         for (int pluginIndex = 0; pluginIndex < pluginItems.size(); pluginIndex++)
         {
            QListWidgetItem* pluginItem = pluginItems[pluginIndex];
            if (pluginItem)
            {
               pluginItem->setCheckState(Qt::Checked);

               // now double check to see if those dependencies were turned on properly.
               if (pluginItem->checkState() == Qt::Unchecked)
               {
                  pluginItem->setCheckState(Qt::Unchecked);
                  return;
               }
            }
         }
      }
   }
   // When deactivating a plugin, make sure to also deactivate any
   // plugins that depend on this one.
   else
   {
      std::list<std::string> allPlugins;
      manager->GetAvailablePlugins(allPlugins);
      while (!allPlugins.empty())
      {
         std::string plugin = allPlugins.front();
         allPlugins.pop_front();

         std::list<std::string> deps = manager->GetPluginDependencies(plugin);
         while (!deps.empty())
         {
            std::string dependency = deps.front();
            deps.pop_front();

            // If the active plugin depends on this plugin, then we need to stop that one too.
            if (dependency == pluginName)
            {
               QList<QListWidgetItem*> pluginItems = pluginItem->listWidget()->findItems(QString(plugin.c_str()), Qt::MatchExactly);

               // check if dependency can be fulfilled
               if (pluginItems.empty())
               {
                  break;
               }

               for (int pluginIndex = 0; pluginIndex < pluginItems.size(); pluginIndex++)
               {
                  QListWidgetItem* pluginItem = pluginItems[pluginIndex];
                  if (pluginItem)
                  {
                     pluginItem->setCheckState(Qt::Unchecked);
                  }
               }
               break;
            }
         }
      }
   }
}

//////////////////////////////////////////////////////////////////////////
// user has pressed OK button. Start and stop plugins
void PluginManagerPlugin::OnApplyChanges(QListWidget* listWidget)
{
   dtEditQt::PluginManager* manager = mMainWindow->GetPluginManager();

   for (int i = 0; i < listWidget->count(); ++i)
   {
      QListWidgetItem* item = listWidget->item(i);
      std::string pluginName = item->text().toStdString();
      bool enabled = (item->checkState() == Qt::Checked);
            
      // can't load/unload system plugins
      if (manager->IsSystemPlugin(pluginName))
      {
         continue;
      }

      if (!enabled && manager->IsInstantiated(pluginName))
      {
         manager->StopPlugin(pluginName);
      }
      else if (enabled && !manager->IsInstantiated(pluginName))
      {
         manager->StartPlugin(pluginName);
      }
   }
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

   virtual ~PluginFactory() {}

   /** get the pluginName of the plugin */
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
