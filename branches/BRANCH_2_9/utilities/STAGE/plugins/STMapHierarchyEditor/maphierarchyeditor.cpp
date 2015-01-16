#include "maphierarchyeditor.h"

#include "nodebutton.h"

#include <dtCore/environment.h>
#include <dtCore/transformable.h>

#include <dtCore/actorhierarchynode.h>
#include <dtCore/baseactor.h>
#include <dtCore/map.h>

#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/pluginmanager.h>
#include <dtEditQt/viewportmanager.h>

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

   BuildTreeFromMap();
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
// user has closed the dock widget. Stop the plugin
void MapHierarchyEditorPlugin::closeEvent(QCloseEvent* event)
{
   mMainWindow->GetPluginManager()->StopPlugin(PLUGIN_NAME);
}

////////////////////////////////////////////////////////////////////////////////
void MapHierarchyEditorPlugin::BuildTreeFromMap()
{  
   dtCore::Map* m = dtEditQt::EditorData::GetInstance().getCurrentMap();
   if (m != NULL)   
   {      
      dtCore::BaseActor* curActor = m->GetDrawableActorHierarchy();      
      
      // This is the old way to create a scene hierarchy.
      // Do this if there is no hierarchy defined in the map.
      if(curActor->GetChildCount() == 0)
      {
         // TODO: Remove the environment actor dependent code.
         // Do not assume everyone wants an environment actor in every map.

         dtCore::BaseActor* envActor = m->GetEnvironmentActor();
         dtCore::UniqueId envId;

         if (envActor != NULL)
         {
            envId = envActor->GetId();

            curActor->AddChild(*envActor);
            curActor = dynamic_cast<dtCore::BaseActor*>(envActor);
         }      

         // Typedef to make code a bit more readable.
         typedef std::map<dtCore::UniqueId, dtCore::RefPtr<dtCore::BaseActorObject> > IdActorMap;

         const IdActorMap& allProxies = m->GetAllProxies();

         IdActorMap::const_iterator curIter = allProxies.begin();
         IdActorMap::const_iterator endIter = allProxies.end();
         for (; curIter != endIter; ++curIter)
         {
            if (envActor != NULL && curIter->first == envId)
            {
               // Do not include environment actor.
               // It has already been included.
               continue;
            }            
           
            // Only adding drawable actors to the scene hierarchy.
            dtCore::BaseActor* baseActor = dynamic_cast<dtCore::BaseActor*>(curIter->second.get());      
            if (curActor != NULL && baseActor != NULL && baseActor->GetDrawable() != NULL)
            {
               curActor->AddChild(*baseActor);
            }
         }
      }
      else //There is a hierarchy defined in this map
      {
         //Do nothing: the hierarchy was defined when the map was loaded.         
      }      
   }

   BuildTreeGUI();   
}

////////////////////////////////////////////////////////////////////////////////
void MapHierarchyEditorPlugin::BuildTreeGUI()
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

   NodeButton* root = new NodeButton(NULL, this); 
   root->Place(0, 0);

   dtCore::Map* m = dtEditQt::EditorData::GetInstance().getCurrentMap();
   if (m != NULL)   
   {
      dtCore::BaseActor* currentHierNode = m->GetDrawableActorHierarchy();
     
      for (int i = 0; i < currentHierNode->GetChildCount(); ++i)
      {
         BuildGUIBranch(root, currentHierNode->GetChild(i));
      }
   }

   //rightmost column needs a horizontal spacer 
   QSpacerItem* hs = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
   lo->addItem(hs, 0, lo->columnCount());
}

////////////////////////////////////////////////////////////////////////////////
void MapHierarchyEditorPlugin::BuildGUIBranch(NodeButton* GUIparent,
                                              dtCore::BaseActor* branch)
{
   //build a node, place it as a child of parent
   NodeButton* nextGUINode = new NodeButton(branch, this);
   GUIparent->AddChild(nextGUINode);

   //run through all branch's children   
   for (int i = 0; i < branch->GetChildCount(); ++i)
   {
      BuildGUIBranch(nextGUINode, branch->GetChild(i));
   }
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
