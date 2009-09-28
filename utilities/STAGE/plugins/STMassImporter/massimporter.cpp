#include "massimporter.h"
#include "ui_massimporter.h"

#include <assert.h>

#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/librarymanager.h>
#include <dtDAL/map.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/pluginmanager.h>
#include <dtEditQt/viewportmanager.h>
#include <dtUtil/fileutils.h>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>

const std::string MassImporterPlugin::PLUGIN_NAME = "Mass Importer";


MassImporterPlugin::MassImporterPlugin(dtEditQt::MainWindow* mw)
   : mMainWindow(mw)
{
   // apply layout made with QT designer
   Ui_MassImporter ui;
   ui.setupUi(this);

   // store handles to text edit lines
   mPropertyName = ui.mPropertyName;
   mActorType = ui.mActorTypes;


   // add dock widget to STAGE main window
   mw->addDockWidget(Qt::LeftDockWidgetArea, this);   

   // open file menu when user presses the button
   connect(ui.mChooseFiles, SIGNAL(clicked()), this, SLOT(OnChooseFiles()));   

   typedef std::vector<const dtDAL::ActorType*> TypeList;
   TypeList types;
   dtDAL::LibraryManager::GetInstance().GetActorTypes(types);
   int idx = 0;
   for(TypeList::const_iterator i = types.begin(); i != types.end(); ++i)
   {
      const dtDAL::ActorType* type = *i; 
      const QVariant id(type->GetUniqueId().c_str());      
      ui.mActorTypes->addItem(type->GetFullName().c_str(), id);
      if(type->GetFullName() == "dtcore.Static Mesh")
      {
         ui.mActorTypes->setCurrentIndex(idx);
      }
      ++idx;
   }
}


MassImporterPlugin::~MassImporterPlugin()
{
}


// remove our stuff from GUI
void MassImporterPlugin::Destroy()
{  
   mMainWindow->removeDockWidget(this);
}


// user has closed the dock. Stop the plugin
void MassImporterPlugin::closeEvent(QCloseEvent* event)
{
   mMainWindow->GetPluginManager()->StopPlugin(PLUGIN_NAME);
}


// open file dialog and create actors for chosen files
void MassImporterPlugin::OnChooseFiles()
{
   // if no map is loaded 
   if(dtEditQt::EditorData::GetInstance().getCurrentMap() == NULL)
   {  
      QMessageBox::critical(mMainWindow, "Error", "No map loaded", "Ok");
      return;
   }

   // get values from GUI
   QVariant t = mActorType->itemData(mActorType->currentIndex());
   std::string typeId = t.toString().toStdString();

   const dtDAL::ActorType* actorType = NULL;

   // search for actor type with given unique id.
   // would be nice if there was a method in the library manager for this
   typedef std::vector<const dtDAL::ActorType*> TypeList;
   TypeList types;
   dtDAL::LibraryManager::GetInstance().GetActorTypes(types);
   for(TypeList::const_iterator i = types.begin(); i != types.end(); ++i)
   {
      if((*i)->GetUniqueId() == typeId)
      {
         actorType = (*i);
         break;
      }
   }  

   if(actorType == NULL)
   {
      QMessageBox::critical(mMainWindow, "Error", "Cannot find actor type", "Ok");
      return;
   }

   // open file dialog at project context dir
   std::string contextName = dtEditQt::EditorData::GetInstance().getCurrentProjectContext();
   QStringList filenames = QFileDialog::getOpenFileNames(mMainWindow,
                                                   "Load Meshes",
                                                   contextName.c_str(),
                                                   "Meshes(*.osg *.ive *.*)" );

   dtEditQt::EditorData::GetInstance().getMainWindow()->startWaitCursor();
   for(QStringList::iterator i = filenames.begin(); i != filenames.end(); ++i)
   {
      CreateActorFromMesh(*actorType, mPropertyName->text().toStdString(), (*i).toStdString());
   }    
   dtEditQt::EditorData::GetInstance().getMainWindow()->endWaitCursor();

}


void MassImporterPlugin::CreateActorFromMesh(const dtDAL::ActorType& type, const std::string& propertyName, const std::string& mesh)
{
   using namespace dtEditQt;   

   // set name of actor to file name
   dtUtil::FileInfo info = dtUtil::FileUtils::GetInstance().GetFileInfo(mesh);
   std::string actorName = info.baseName;

   // make mesh path relative to project assets root dir
   std::string contextName = dtEditQt::EditorData::GetInstance().getCurrentProjectContext();
   QString relPath = info.fileName.substr(contextName.size() + 1, info.fileName.length()).c_str();

   // replace slashes with colons (why is that necessary?)
   relPath.replace("/", ":");

   // check if an actor with this name is already in map. If it is, update it.
   // If not: create a new actor.
   dtCore::RefPtr<dtDAL::ActorProxy> proxy;
   dtCore::RefPtr<dtDAL::Map> mapPtr = EditorData::GetInstance().getCurrentMap();
   std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > proxies;
   mapPtr->FindProxies(proxies, actorName, type.GetCategory(), type.GetName());

   if(proxies.empty())
   {
      // create new actor
      proxy = dtDAL::LibraryManager::GetInstance().CreateActorProxy(type);
      
      proxy->SetName(actorName);

      // add the new proxy to the map
      mapPtr->AddProxy(*(proxy.get()));      
      
      // let the world know that a new proxy exists
      EditorEvents::GetInstance().emitBeginChangeTransaction();
      EditorEvents::GetInstance().emitActorProxyCreated(proxy.get(), false);
      EditorEvents::GetInstance().emitEndChangeTransaction();
   }
   else
   {
      assert(proxies.size() == 1);
      proxy = proxies.front();
   }   

   // set mesh property to mesh path
   dtDAL::ResourceActorProperty* resourceProperty;
   proxy->GetProperty(propertyName, resourceProperty);
   
   if(resourceProperty == 0)
   {
      LOG_ERROR("Can't set property, it does not exist: " + propertyName);
   }
   else
   {
      dtDAL::ResourceDescriptor meshDescriptor(relPath.toStdString());
      resourceProperty->SetValue(&meshDescriptor);
      EditorEvents::GetInstance().emitActorPropertyChanged(proxy, resourceProperty);
   }
}


////////////////////////////////////////////////////////////

namespace MassImporter
{
class DT_MASS_IMPORTER_EXPORT PluginFactory : public dtEditQt::PluginFactory
{
public:

   PluginFactory() {}
   ~PluginFactory() {}

   /** get the name of the plugin */
   virtual std::string GetName() { return MassImporterPlugin::PLUGIN_NAME; }

   /** get a description of the plugin */
   virtual std::string GetDescription() { return "Creates a number of actors from a file selection"; }

   virtual void GetDependencies(std::list<std::string>& deps) 
   {
      // just for testing
      deps.push_back("Plugin Manager");
   }

    /** construct the plugin and return a pointer to it */
   virtual dtEditQt::Plugin* Create(dtEditQt::MainWindow* mw) 
   { 
      mPlugin = new MassImporterPlugin(mw);
      return mPlugin;
   }

   virtual void Destroy() 
   {
      delete mPlugin;
   }

private:

   dtEditQt::Plugin* mPlugin;
}; 
} //namespace MassImporter

extern "C" DT_MASS_IMPORTER_EXPORT dtEditQt::PluginFactory* CreatePluginFactory()
{
   return new MassImporter::PluginFactory;
}
