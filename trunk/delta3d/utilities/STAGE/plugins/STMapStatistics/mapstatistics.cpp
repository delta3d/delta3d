#include "mapstatistics.h"
#include "ui_mapstatistics.h"

#include <dtEditQt/pluginmanager.h>
#include <dtEditQt/editordata.h>

#include <dtCore/deltadrawable.h>
#include <dtCore/map.h>

#include <dtUtil/nodecollector.h>
#include <dtUtil/stringutils.h>

#include <QtGui/QFileDialog>

#include <osg/Node>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Group>
#include <osgUtil/Statistics>

const std::string MapStatisticsPlugin::PLUGIN_NAME = "Map Statistics";


////////////////////////////////////////////////////////////////////////////////
MapStatisticsPlugin::MapStatisticsPlugin(MainWindow* mw)
   : mMainWindow(mw)
   , mCalculateButton(NULL)
   , mStatTable(NULL)
{
   // apply layout made with QT designer
   Ui_MapStatistics ui;
   ui.setupUi(this);

   mMainWindow->addDockWidget(Qt::RightDockWidgetArea, this);

   mCalculateButton = ui.calculateButton;
   mStatTable = ui.statTable;

   connect(mCalculateButton, SIGNAL(clicked()), this, SLOT(onCalculateButtonPressed()));
}

////////////////////////////////////////////////////////////////////////////////
MapStatisticsPlugin::~MapStatisticsPlugin()
{
}

////////////////////////////////////////////////////////////////////////////////
void MapStatisticsPlugin::Destroy()
{
   mCalculateButton = NULL;
}

////////////////////////////////////////////////////////////////////////////////
void MapStatisticsPlugin::closeEvent(QCloseEvent* event)
{
   mMainWindow->GetPluginManager()->StopPlugin(PLUGIN_NAME);
}

////////////////////////////////////////////////////////////////////////////////
void MapStatisticsPlugin::onCalculateButtonPressed()
{
   mStatTable->clear();

   // Add the starting items
   QTreeWidgetItem* selectedItem = new QTreeWidgetItem;
   QTreeWidgetItem* sceneItem = new QTreeWidgetItem;

   selectedItem->setText(0, "Selected");
   sceneItem->setText(0, "Scene");

   mStatTable->addTopLevelItem(selectedItem);
   mStatTable->addTopLevelItem(sceneItem);

   // Only calculate if there is a map loaded
   dtCore::Map* map = EditorData::GetInstance().getCurrentMap();
   if (map)
   {
      osg::ref_ptr<osgUtil::StatsVisitor> statVisitor = new osgUtil::StatsVisitor;

      // Calculate for the current selection
      {
         std::vector<dtCore::BaseActorObject*> selectedList;
         EditorData::GetInstance().GetSelectedActors(selectedList);

         for (size_t selectIndex = 0; selectIndex < selectedList.size(); ++selectIndex)
         {
            selectedList[selectIndex]->GetActor()->GetOSGNode()->accept(*statVisitor);
         }

         // Update "Selected" widgets
         {
            QTreeWidgetItem* vertTotalWidget = new QTreeWidgetItem(selectedItem, QStringList("vertices"));
            QTreeWidgetItem* primTotalWidget = new QTreeWidgetItem(selectedItem, QStringList("primitives"));
            vertTotalWidget->setText(1,dtUtil::ToString(statVisitor->_instancedStats._vertexCount).c_str());
            primTotalWidget->setText(1,dtUtil::ToString(GetTotalPrimitivesFromStatVisitor(*statVisitor)).c_str());
         }
      }

      // Calculate for the entire map
      {
         statVisitor->reset();

         std::vector<dtCore::RefPtr<dtCore::BaseActorObject> > proxies;
         map->GetAllProxies(proxies);

         for (int proxyIndex = 0; proxyIndex < (int)proxies.size(); proxyIndex++)
         {
             proxies[proxyIndex]->GetActor()->GetOSGNode()->accept(*statVisitor);
         }

         // Update "Scene" widgets
         {
            QTreeWidgetItem* vertTotalWidget = new QTreeWidgetItem(sceneItem, QStringList("vertices"));
            QTreeWidgetItem* primTotalWidget = new QTreeWidgetItem(sceneItem, QStringList("primitives"));
            vertTotalWidget->setText(1,dtUtil::ToString(statVisitor->_instancedStats._vertexCount).c_str());
            primTotalWidget->setText(1,dtUtil::ToString(GetTotalPrimitivesFromStatVisitor(*statVisitor)).c_str());
         }
      }

      //statVisitor->print(std::cout);
   }
}

////////////////////////////////////////////////////////////////////////////////
void MapStatisticsPlugin::GetGeometryMetrics(dtCore::BaseActorObject* actor, unsigned int& outVerts, unsigned int& outPrimitives)
{
   if (actor)
   {
      dtCore::DeltaDrawable* drawable = actor->GetActor();
      if (drawable)
      {
         dtCore::RefPtr<dtUtil::NodeCollector> nc = new dtUtil::NodeCollector;
         nc->CollectNodes(drawable->GetOSGNode(), dtUtil::NodeCollector::GeodeFlag);

         unsigned int numVerts = 0;
         unsigned int numPrimitives = 0;

         GetGeometryMetrics(nc->GetGeodeNodeMap(), numVerts, numPrimitives);

         outVerts += numVerts;
         outPrimitives += numPrimitives;
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void MapStatisticsPlugin::GetGeometryMetrics(GeodeNodeMap& nodeMap, unsigned int& outVerts, unsigned int& outPrimitives)
{
   outVerts = 0;
   outPrimitives = 0;

   GeodeNodeMap::iterator iter = nodeMap.begin();
   while (iter != nodeMap.end())
   {
      const osg::Geode::DrawableList& drawables = iter->second->getDrawableList();

      for (size_t geomIndex = 0; geomIndex < drawables.size(); ++geomIndex)
      {
         osg::Geometry* geom = drawables[geomIndex]->asGeometry();

         if (geom)
         {
            osg::Array* array = geom->getVertexArray();
            if (array)
            {
               outVerts += array->getNumElements();
            }

            osg::Geometry::PrimitiveSetList& setList = geom->getPrimitiveSetList();
            for (size_t setIndex = 0; setIndex < setList.size(); ++setIndex)
            {
               outPrimitives += setList[setIndex]->getNumPrimitives();
            }
         }
      }

      ++iter;
   }
}

////////////////////////////////////////////////////////////////////////////////
unsigned int MapStatisticsPlugin::GetTotalPrimitivesFromStatVisitor(osgUtil::StatsVisitor& visitor)
{
   unsigned int instancedPrimitives = 0;
   osgUtil::Statistics::PrimitiveCountMap::iterator iter;
   for(iter = visitor._instancedStats.GetPrimitivesBegin();
       iter != visitor._instancedStats.GetPrimitivesEnd();
       ++iter)
   {
      instancedPrimitives += iter->second;
   }

   return instancedPrimitives;
}

////////////////////////////////////////////////////////////

namespace MapStatistics
{
   class DT_MAP_STATISTICS_EXPORT PluginFactory : public dtEditQt::PluginFactory
   {
   public:
      PluginFactory() {}
      ~PluginFactory() {}

      /** get the name of the plugin */
      virtual std::string GetName() { return MapStatisticsPlugin::PLUGIN_NAME; }

      /** get a description of the plugin */
      virtual std::string GetDescription() { return "Generates statistics about the current selection and the scene as a whole."; }

      virtual void GetDependencies(std::list<std::string>& deps)
      {
      }

      /** construct the plugin and return a pointer to it */
      virtual Plugin* Create(MainWindow* mw)
      {
         mPlugin = new MapStatisticsPlugin(mw);
         return mPlugin;
      }

      virtual void Destroy()
      {
         delete mPlugin;
      }

   private:
      Plugin* mPlugin;
   };
} //namespace MapExporterPlugin

extern "C" DT_MAP_STATISTICS_EXPORT dtEditQt::PluginFactory* CreatePluginFactory()
{
   return new MapStatistics::PluginFactory;
}
