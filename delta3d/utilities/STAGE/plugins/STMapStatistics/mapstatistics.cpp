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
#include <osg/Version>
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
         dtCore::ActorPtrVector selectedList;
         EditorData::GetInstance().GetSelectedActors(selectedList);

         for (size_t selectIndex = 0; selectIndex < selectedList.size(); ++selectIndex)
         {
            selectedList[selectIndex]->GetDrawable()->GetOSGNode()->accept(*statVisitor);
         }

         AddStatsToItem(*statVisitor, *selectedItem);
      }

      // Calculate for the entire map
      {
         statVisitor->reset();

         dtCore::ActorRefPtrVector proxies;
         map->GetAllProxies(proxies);

         for (int proxyIndex = 0; proxyIndex < (int)proxies.size(); proxyIndex++)
         {
            if (proxies[proxyIndex]->GetDrawable())
            {
               osg::Node* node =  proxies[proxyIndex]->GetDrawable()->GetOSGNode();
               if (node)
               {
                  node->accept(*statVisitor);
               }
            }            
         }

         AddStatsToItem(*statVisitor, *sceneItem);
      }

      //statVisitor->print(std::cout);
   }
}

////////////////////////////////////////////////////////////////////////////////
void MapStatisticsPlugin::GetGeometryMetrics(dtCore::BaseActorObject* actor, unsigned int& outVerts, unsigned int& outPrimitives)
{
   if (actor)
   {
      dtCore::DeltaDrawable* drawable = actor->GetDrawable();
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
      int numDrawables = iter->second->getNumDrawables();
      for (int geomIndex = 0; geomIndex < numDrawables; ++geomIndex)
      {
          osg::Geometry* geom = iter->second->getDrawable(geomIndex)->asGeometry();

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
void MapStatisticsPlugin::AddStatsToItem(osgUtil::StatsVisitor& statsVisitor, QTreeWidgetItem& item)
{
   QTreeWidgetItem* vertTotalWidget = new QTreeWidgetItem(&item, QStringList("vertices"));
   QTreeWidgetItem* primTotalWidget = new QTreeWidgetItem(&item, QStringList("primitives"));

   vertTotalWidget->setText(1,dtUtil::ToString(statsVisitor._instancedStats._vertexCount).c_str());
   primTotalWidget->setText(1,dtUtil::ToString(GetTotalPrimitivesFromStatVisitor(statsVisitor)).c_str());

   QTreeWidgetItem* uniqueWidget = new QTreeWidgetItem(&item, QStringList("unique"));

   QTreeWidgetItem* uniqueStateWidget = new QTreeWidgetItem(uniqueWidget, QStringList("StateSets"));
   QTreeWidgetItem* uniqueGroupWidget = new QTreeWidgetItem(uniqueWidget, QStringList("Groups"));
   QTreeWidgetItem* uniqueTransformWidget = new QTreeWidgetItem(uniqueWidget, QStringList("Transforms"));
   QTreeWidgetItem* uniqueLODWidget = new QTreeWidgetItem(uniqueWidget, QStringList("LOD"));
   QTreeWidgetItem* uniqueSwitchWidget = new QTreeWidgetItem(uniqueWidget, QStringList("Switch"));
   QTreeWidgetItem* uniqueGeodeWidget = new QTreeWidgetItem(uniqueWidget, QStringList("Geode"));
   QTreeWidgetItem* uniqueDrawableWidget = new QTreeWidgetItem(uniqueWidget, QStringList("Drawable"));
   QTreeWidgetItem* uniqueGeometryWidget = new QTreeWidgetItem(uniqueWidget, QStringList("Geometry"));

   uniqueStateWidget->setText(1,dtUtil::ToString(statsVisitor._statesetSet.size()).c_str());
   uniqueGroupWidget->setText(1,dtUtil::ToString(statsVisitor._groupSet.size()).c_str());
   uniqueTransformWidget->setText(1,dtUtil::ToString(statsVisitor._transformSet.size()).c_str());
   uniqueLODWidget->setText(1,dtUtil::ToString(statsVisitor._lodSet.size()).c_str());
   uniqueSwitchWidget->setText(1,dtUtil::ToString(statsVisitor._switchSet.size()).c_str());
   uniqueGeodeWidget->setText(1,dtUtil::ToString(statsVisitor._geodeSet.size()).c_str());
   uniqueDrawableWidget->setText(1,dtUtil::ToString(statsVisitor._drawableSet.size()).c_str());
   uniqueGeometryWidget->setText(1,dtUtil::ToString(statsVisitor._geometrySet.size()).c_str());

#if defined(OSG_VERSION_MAJOR) && OSG_VERSION_MAJOR >= 3
   QTreeWidgetItem* uniqueFastGeometryWidget = new QTreeWidgetItem(uniqueWidget, QStringList("Fast Geometry"));
   uniqueFastGeometryWidget->setText(1,dtUtil::ToString(statsVisitor._fastGeometrySet.size()).c_str());
#endif

   QTreeWidgetItem* instanceWidget = new QTreeWidgetItem(&item, QStringList("instance"));

   //QTreeWidgetItem* instanceStateWidget =
   new QTreeWidgetItem(instanceWidget, QStringList("StateSets"));
   //QTreeWidgetItem* instanceGroupWidget =
   new QTreeWidgetItem(instanceWidget, QStringList("Groups"));
   //QTreeWidgetItem* instanceTransformWidget =
   new QTreeWidgetItem(instanceWidget, QStringList("Transforms"));
   //QTreeWidgetItem* instanceLODWidget =
   new QTreeWidgetItem(instanceWidget, QStringList("LOD"));
   //QTreeWidgetItem* instanceSwitchWidget =
   new QTreeWidgetItem(instanceWidget, QStringList("Switch"));
   //QTreeWidgetItem* instanceeGeodeWidget =
   new QTreeWidgetItem(instanceWidget, QStringList("Geode"));
   QTreeWidgetItem* instanceeDrawableWidget = new QTreeWidgetItem(instanceWidget, QStringList("Drawable"));
   //QTreeWidgetItem* instanceeGeometryWidget =
   new QTreeWidgetItem(instanceWidget, QStringList("Geometry"));

   uniqueStateWidget->setText(1, dtUtil::ToString(statsVisitor._numInstancedStateSet).c_str());
   uniqueGroupWidget->setText(1, dtUtil::ToString(statsVisitor._numInstancedGroup).c_str());
   uniqueTransformWidget->setText(1, dtUtil::ToString(statsVisitor._numInstancedTransform).c_str());
   uniqueLODWidget->setText(1, dtUtil::ToString(statsVisitor._numInstancedLOD).c_str());
   uniqueSwitchWidget->setText(1, dtUtil::ToString(statsVisitor._numInstancedSwitch).c_str());
   uniqueGeodeWidget->setText(1, dtUtil::ToString(statsVisitor._numInstancedGeode).c_str());
   instanceeDrawableWidget->setText(1, dtUtil::ToString(statsVisitor._numInstancedDrawable).c_str());
   uniqueGeometryWidget->setText(1, dtUtil::ToString(statsVisitor._numInstancedDrawable).c_str());

#if defined(OSG_VERSION_MAJOR) && OSG_VERSION_MAJOR >= 3
   //QTreeWidgetItem* instanceFastGeometryWidget =
   new QTreeWidgetItem(instanceWidget, QStringList("Fast Geometry"));
   uniqueFastGeometryWidget->setText(1, dtUtil::ToString(statsVisitor._numInstancedFastGeometry).c_str());
#endif
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
      PluginFactory(): mPlugin(NULL) {}
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
