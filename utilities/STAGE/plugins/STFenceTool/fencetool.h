#ifndef FENCE_TOOL_PLUGIN
#define FENCE_TOOL_PLUGIN

#include "export.h"
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/plugininterface.h>
#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include <dtActors/linkedpointsactorproxy.h>
#include <dtActors/fenceactor.h>
#include <QtGui/QAction>

using namespace dtEditQt;

namespace dtEditQt
{
   class Viewport;
}

/**
 * The FenceToolPlugin is a plugin that is used as a tool
 * to place a LinkedPointsActor into the world.
 */
class DT_FENCE_TOOL_EXPORT FenceToolPlugin 
   : public QWidget
   , public Plugin
{
   Q_OBJECT

public:

   const static std::string PLUGIN_NAME;
   
   FenceToolPlugin(MainWindow* mw);

   ~FenceToolPlugin();

   virtual void Destroy();

public slots:

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
   void initialize(dtActors::FenceActorProxy* activeProxy);

   /**
   * Shuts down the tool.
   */
   void shutdown();

   MainWindow* mMainWindow;

   dtCore::RefPtr<dtActors::FenceActorProxy> mActiveProxy;
   dtActors::FenceActor*                     mActiveActor;

   QAction*     mModeButton;
};

#endif // FENCE_TOOL_PLUGIN
