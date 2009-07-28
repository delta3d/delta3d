#ifndef FENCE_TOOL_PLUGIN
#define FENCE_TOOL_PLUGIN

#include "export.h"
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/plugininterface.h>
#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include <dtActors/linkedpointsactorproxy.h>
#include <dtActors/buildingactor.h>
#include <QtGui/QAction>

using namespace dtEditQt;

namespace dtEditQt
{
   class Viewport;
}

/**
 * The BuildingToolPlugin is a plugin that is used as a tool
 * to place a LinkedPointsActor into the world.
 */
class DT_BUILDING_TOOL_EXPORT BuildingToolPlugin 
   : public QWidget
   , public Plugin
{
   Q_OBJECT

public:

   const static std::string PLUGIN_NAME;
   
   BuildingToolPlugin(MainWindow* mw);

   ~BuildingToolPlugin();

   virtual void Destroy();

public slots:

   /**
   * Handles when a new actor proxy is created.
   *
   * @param[in]  proxy               The proxy that was created.
   * @param[in]  forceNoAdjustments  If true, all slots catching this event should not change the proxy.
   */
   void onActorProxyCreated(ActorProxyRefPtr proxy, bool forceNoAdjustments);

   /**
   * Handles when actors are selected.
   *
   * @param[in]  actors  The list of actors being selected.
   */
   void onActorsSelected(ActorProxyRefPtrVector& actors);

   /**
   * Handles when to select actors.
   *
   * @param[in]   vp               The viewport triggering this event.
   * @param[in]   e                The mouse event.
   * @param[out]  overrideDefault  Should be set true if you don't want the default behavior to handle this.
   */
   void onSelectActors(Viewport* vp, QMouseEvent* e, bool* overrideDefault);

   /**
   * User has pressed the mode button.
   */
   void onModeButtonPressed();

private:

   /**
   * Initializes the tool.
   */
   void initialize(dtActors::BuildingActorProxy* activeProxy);

   /**
   * Shuts down the tool.
   */
   void shutdown();

   MainWindow* mMainWindow;

   dtCore::RefPtr<dtActors::BuildingActorProxy> mActiveProxy;
   dtActors::BuildingActor*                     mActiveActor;

   QAction* mModeButton;
};

#endif // BUILDING_TOOL_PLUGIN
