#ifndef MASS_IMPORTER_PLUGIN
#define MASS_IMPORTER_PLUGIN

#include "export.h"
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/plugininterface.h>
#include <QtGui/QComboBox>
#include <QtGui/QDockWidget>
#include <QtGui/QLineEdit>

/**
   The mass importer is a STAGE plugin that can be used to import multiple meshes
   as game actors.
   The user can choose an actor type that should be instantiated and
   he can choose the name of an actor property. This property is set to the
   path of the selected mesh file.
   The name of the actor is set to the mesh file name.
*/
class DT_MASS_IMPORTER_EXPORT MassImporterPlugin 
   : public QDockWidget
   , public dtEditQt::Plugin
{
   Q_OBJECT

public:

   const static std::string PLUGIN_NAME;
   
   MassImporterPlugin(dtEditQt::MainWindow* mw);

   ~MassImporterPlugin();

   virtual void Destroy();

   /** override close event to get notified when user closes the dock */
   virtual void closeEvent(QCloseEvent* event);

public slots:
      
   /** user has chosen a number of files. Import them as actors now. */   
   void OnChooseFiles();

private:

   /** create actor, add it to mesh and set property with given name to mesh path */
   void CreateActorFromMesh(const dtDAL::ActorType& type, const std::string& propertyName, const std::string& mesh);

   dtEditQt::MainWindow* mMainWindow;

   // text line edits for chosing actor type and property name
   QComboBox* mActorType;
   QLineEdit* mPropertyName;

};

#endif //MASS_IMPORTER_PLUGIN
