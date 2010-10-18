#ifndef PLUGIN_DIALOG_PLUGIN
#define PLUGIN_DIALOG_PLUGIN

#include "export.h"
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/plugininterface.h>
#include "ui_spplugin.h"

/**
   This STAGE plugin adds a plugin manager to STAGE.
   The manager can be accessed over the menu point "edit" -> "Manage Plugins...".
   It shows a list of all available STAGE plugins and can be used to start and stop
   plugins during run time.
*/
class DT_PLUGIN_MANAGER_EXPORT PluginManagerPlugin 
   : public QObject
   , public dtEditQt::Plugin
{
   Q_OBJECT

public:

   PluginManagerPlugin(dtEditQt::MainWindow* mw);
   virtual ~PluginManagerPlugin();

   // used to identify plugin in plugin manager
   const static std::string PLUGIN_NAME;

public slots:

   /** when user chooses plugin manager in menu */
   void OnOpenDialog();

   /** when user checks/unchecks a plugin in the list */
   void OnPluginChanged(QListWidgetItem* item);

private:
   dtEditQt::MainWindow* mMainWindow;

   /** when user presses OK button */
   void OnApplyChanges(QListWidget* listWidget);
   
};

#endif //PLUGIN_DIALOG_PLUGIN
