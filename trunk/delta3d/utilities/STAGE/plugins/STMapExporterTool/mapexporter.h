#ifndef MAP_EXPORTER_PLUGIN
#define MAP_EXPORTER_PLUGIN

#include "export.h"
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/plugininterface.h>
#include <QtGui/QDockWidget>
#include <QtGui/QPushButton>

#include <dtUtil/packager.h>

using namespace dtEditQt;

namespace dtEditQt
{
   class Viewport;
}

namespace dtDAL
{
   class ActorProperty;
}

namespace osg
{
   class Node;
}

/**
 * The MapExporterPlugin is a plugin that is used as a tool
 * to place a LinkedPointsActor into the world.
 */
class DT_MAP_EXPORTER_EXPORT MapExporterPlugin 
   : public QDockWidget
   , public Plugin
{
   Q_OBJECT

public:

   const static std::string PLUGIN_NAME;
   
   MapExporterPlugin(MainWindow* mw);

   ~MapExporterPlugin();

   virtual void Destroy();

   /** override close event to get notified when user closes the dock */
   virtual void closeEvent(QCloseEvent* event);

public slots:

   /**
   * User has pressed the export button.
   */
   void onExportButtonPressed();

   /**
   * Adds any resources referenced by the given property.
   *
   * @param[in]  prop      The property to look search.
   * @param[in]  packager  The packager to add resources to.
   *
   * @return     Returns true if it was a valid resource property.
   */
   bool AddResourcesFromProperty(const dtDAL::ActorProperty* prop, dtUtil::Packager& packager);

   /**
   * Find any resources referenced by the given OSG node and all its children.
   *
   * @param[in]  node      The OSG node to search.
   * @param[in]  packager  The packager to add resources to.
   *
   * @return     Returns true if any shaders were found.
   */
   bool AddResourcesFromNode(osg::Node* node, dtUtil::Packager& packager);

private:


   MainWindow*    mMainWindow;
   QPushButton*   mExportButton;
};

#endif // MAP_EXPORTER_PLUGIN
