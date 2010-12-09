#include "maphierarchyeditor.h"

#include "nodebutton.h"

#include <dtCore/environment.h>
#include <dtCore/transformable.h>

#include <dtDAL/map.h>

#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/pluginmanager.h>

const std::string MapHierarchyEditorPlugin::PLUGIN_NAME = "Map Hierarchy Editor";

////////////////////////////////////////////////////////////////////////////////
MapHierarchyEditorPlugin::MapHierarchyEditorPlugin(dtEditQt::MainWindow* mw)
   : mUI()
   , mMainWindow(mw)
{
   //apply layout made with QtDesigner
   mUI.setupUi(this);

   //add dock widget to STAGE main window
   mw->addDockWidget(Qt::BottomDockWidgetArea, this);
   
   connect(&dtEditQt::EditorEvents::GetInstance(), SIGNAL(currentMapChanged()),
      this, SLOT(onCurrentMapChanged()));
}

////////////////////////////////////////////////////////////////////////////////
MapHierarchyEditorPlugin::~MapHierarchyEditorPlugin()
{
}

////////////////////////////////////////////////////////////////////////////////
// remove our stuff from GUI
void MapHierarchyEditorPlugin::Destroy()
{
   mMainWindow->removeDockWidget(this);
}


////////////////////////////////////////////////////////////////////////////////
// user has closed the dock. Stop the plugin
void MapHierarchyEditorPlugin::closeEvent(QCloseEvent* event)
{
   mMainWindow->GetPluginManager()->StopPlugin(PLUGIN_NAME);
}

////////////////////////////////////////////////////////////////////////////////
void MapHierarchyEditorPlugin::BuildTreeFromMap()
{  
   QGridLayout* lo =
      dynamic_cast<QGridLayout*>(mUI.scrollAreaWidgetContents->layout());

   //cleanup any elements from previous building of the GUI tree
   //(another map may have been loaded)
   for (int r = 0; r < lo->rowCount(); ++r)
   {
      for (int c = 0; c < lo->columnCount(); ++c)
      {
         QLayoutItem* item = lo->itemAtPosition(r, c);
         if (item != NULL)
         {
            if (item->widget())
            {
               QWidget* w = item->widget();
               lo->removeWidget(w);
               delete w;               
            }
            else 
            {
               lo->removeItem(item);
               delete item;
            }
            /*
            else if (item->layout())
            {
               QLayout* l = item->layout()->item;
               lo->removeItem(l);
               delete l;
            }
            */
         }
      }
   }

   /*
   //const QObjectList& qol = lo->children();
   //printf ("Num children: %d \n", qol.count());
   printf ("Row count: %d , Col count: %d\n", lo->rowCount(), lo->columnCount());   
   */

   //TODO: make a Scene drawable and use it instead of a Transformable here
   dtCore::Transformable *sceneRoot = new dtCore::Transformable("Scene");
      
   NodeButton* root = new NodeButton(NodeButton::SCENE, NULL, this); 
   root->Place(0, 0);

   NodeButton* currentNode = root;
   dtCore::DeltaDrawable* currentDrawableNode = sceneRoot;
   
   dtDAL::Map* m = dtEditQt::EditorData::GetInstance().getCurrentMap();
   if (m != NULL)   
   {
      if (m->GetEnvironmentActor() != NULL)
      {
         dtCore::Environment* envDrawable;
         m->GetEnvironmentActor()->GetDrawable<dtCore::Environment*>(envDrawable);

         printf ("Env num children: %d \n", envDrawable->GetNumChildren());
         printf ("Child's num children: %d \n", envDrawable->GetChild(0)->GetNumChildren());

         sceneRoot->AddChild(envDrawable);         

         NodeButton *env = new NodeButton(NodeButton::ENVIRONMENT, m->GetEnvironmentActor(), this);
         currentNode->AddChild(env);

         currentNode = env;
         currentDrawableNode = envDrawable;
      }      

      const std::map<dtCore::UniqueId, dtCore::RefPtr<dtDAL::BaseActorObject> >& allProxies =
         m->GetAllProxies();

      std::map<dtCore::UniqueId, dtCore::RefPtr<dtDAL::BaseActorObject> >::const_iterator it;
      for (it = allProxies.begin(); it != allProxies.end(); ++it)
      {
         if (m->GetEnvironmentActor() != NULL &&
             it->first == m->GetEnvironmentActor()->GetId())
         {
            //don't include env actor (already been included)
            continue;
         }
        
         dtDAL::BaseActorObject* baseActor = it->second;
         dtCore::DeltaDrawable* nextDrawable = baseActor->GetDrawable();
         if (nextDrawable != NULL)
         {
            currentDrawableNode->AddChild(nextDrawable);

            NodeButton* nextProxyNode = new NodeButton(NodeButton::UNSPECIFIED,
                                                       /*baseActor->GetClassName().c_str()*/
                                                       baseActor, this);
            currentNode->AddChild(nextProxyNode);
         }
      }

   } //if (map != NULL)    

   //rightmost column needs a horizontal spacer 
   QSpacerItem* hs = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
   lo->addItem(hs, 0, lo->columnCount());
}

////////////////////////////////////////////////////////////////////////////////
void MapHierarchyEditorPlugin::RebuildTree()
{
   //TODO
}

////////////////////////////////////////////////////////////////////////////////
QGridLayout* MapHierarchyEditorPlugin::GetGridLayout()
{
   return dynamic_cast<QGridLayout*>(mUI.scrollAreaWidgetContents->layout());
}

////////////////////////////////////////////////////////////////////////////////
void MapHierarchyEditorPlugin::onCurrentMapChanged()
{
   BuildTreeFromMap();
}

namespace MapHierarchyEditor
{
class STAGE_MAP_HIERARCHY_EDITOR_EXPORT PluginFactory : public dtEditQt::PluginFactory
{
public:
   PluginFactory() {}
   ~PluginFactory() {}

   /** get the name of the plugin */
   virtual std::string GetName() { return MapHierarchyEditorPlugin::PLUGIN_NAME; }

   /** get a description of the plugin */
   virtual std::string GetDescription() { return "Map Hierarchy Editor"; }

   virtual void GetDependencies(std::list<std::string>& deps)
   {
      // just for testing
      deps.push_back("Plugin Manager");
   }

    /** construct the plugin and return a pointer to it */
   virtual dtEditQt::Plugin* Create(dtEditQt::MainWindow* mw)
   {
      mPlugin = new MapHierarchyEditorPlugin(mw);
      return mPlugin;
   }

   virtual void Destroy()
   {
      delete mPlugin;
   }

private:

   dtEditQt::Plugin* mPlugin;
};
} //namespace MapHierarchyEditor
           
extern "C" STAGE_MAP_HIERARCHY_EDITOR_EXPORT dtEditQt::PluginFactory* CreatePluginFactory()
{
   return new MapHierarchyEditor::PluginFactory;
}
