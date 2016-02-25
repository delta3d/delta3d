#ifndef MAP_STATISTICS_PLUGIN
#define MAP_STATISTICS_PLUGIN

#include "export.h"
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/plugininterface.h>
#include <QtGui/QDockWidget>
#include <QtGui/QTreeWidget>
#include <QtGui/QPushButton>

using namespace dtEditQt;

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace dtEditQt
{
   class Viewport;
}

namespace dtCore
{
   class ActorProperty;
}

namespace osg
{
   class Node;
   class Geode;
}

namespace osgUtil
{
   class StatsVisitor;
}
/// @endcond

/**
 * The MapStatisticsPlugin is a plugin that is used calculate metrics for currently loaded map
 */
class DT_MAP_STATISTICS_EXPORT MapStatisticsPlugin : public QDockWidget, public Plugin
{
   Q_OBJECT

public:
   const static std::string PLUGIN_NAME;

   MapStatisticsPlugin(MainWindow* mw);

   ~MapStatisticsPlugin();

   virtual void Destroy();

   /** override close event to get notified when user closes the dock */
   virtual void closeEvent(QCloseEvent* event);

public slots:
   /**
    * User has pressed the "Calculate" button.
    */
   void onCalculateButtonPressed();

private:
   MainWindow*   mMainWindow;
   QPushButton*  mCalculateButton;
   QTreeWidget* mStatTable;

   typedef std::map<std::string, dtCore::ObserverPtr<osg::Geode> > GeodeNodeMap;

   void GetGeometryMetrics(dtCore::BaseActorObject* actor, unsigned int& outVerts, unsigned int& outPrimitives);
   void GetGeometryMetrics(GeodeNodeMap& nodeMap, unsigned int& outVerts, unsigned int& outPrimitives);

   void AddStatsToItem(osgUtil::StatsVisitor&, QTreeWidgetItem& item);
   unsigned int GetTotalPrimitivesFromStatVisitor(osgUtil::StatsVisitor& visitor);
};

#endif // MAP_STATISTICS_PLUGIN
