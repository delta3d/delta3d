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

const std::string MapStatisticsPlugin::PLUGIN_NAME = "Map Statistics";


////////////////////////////////////////////////////////////////////////////////
MapStatisticsPlugin::MapStatisticsPlugin(MainWindow* mw)
   : mMainWindow(mw)
   , mCalculateButton(NULL)
   , mSceneVertsEdit(NULL)
{
   // apply layout made with QT designer
   Ui_MapStatistics ui;
   ui.setupUi(this);

   mMainWindow->addDockWidget(Qt::RightDockWidgetArea, this);

   mCalculateButton         = ui.calculateButton;
   mSceneVertsEdit          = ui.sceneVertices;
   mScenePrimitivesEdit     = ui.scenePrimitives;
   mSelectionVertsEdit      = ui.actorVertices;
   mSelectionPrimitivesEdit = ui.actorPrimitives;

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
   unsigned int vertTotal = 0;
   unsigned int primitiveTotal = 0;

   unsigned int tempVertCount = 0;
   unsigned int tempPrimitiveCount = 0;

   std::vector<dtCore::BaseActorObject*> selectedList;
   EditorData::GetInstance().GetSelectedActors(selectedList);

   for (size_t selectIndex = 0; selectIndex < selectedList.size(); ++selectIndex)
   {
      GetGeometryMetrics(selectedList[selectIndex], tempVertCount, tempPrimitiveCount);

      vertTotal += tempVertCount;
      primitiveTotal += tempPrimitiveCount;
   }

   mSelectionVertsEdit->setText(dtUtil::ToString(vertTotal).c_str());
   mSelectionPrimitivesEdit->setText(dtUtil::ToString(primitiveTotal).c_str());

   // Reset stat counts so we can use these again
   vertTotal = primitiveTotal = 0;

   // Grab data from every proxy in the map
   dtCore::Map* map = EditorData::GetInstance().getCurrentMap();
   if (map)
   {
      std::vector<dtCore::RefPtr<dtCore::BaseActorObject> > proxies;
      map->GetAllProxies(proxies);

      for (int proxyIndex = 0; proxyIndex < (int)proxies.size(); proxyIndex++)
      {
         tempVertCount = tempPrimitiveCount = 0;
         GetGeometryMetrics(proxies[proxyIndex].get(), tempVertCount, tempPrimitiveCount);

         // Accumulate the totals
         vertTotal += tempVertCount;
         primitiveTotal += tempPrimitiveCount;
      }

      mSceneVertsEdit->setText(dtUtil::ToString(vertTotal).c_str());
      mScenePrimitivesEdit->setText(dtUtil::ToString(primitiveTotal).c_str());
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
