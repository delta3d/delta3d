#ifndef RTS_CAMERA_PLUGIN
#define RTS_CAMERA_PLUGIN

#include "export.h"
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/plugininterface.h>

#include "stagertscameramotionmodel.h"

using namespace dtEditQt;

namespace dtEditQt
{
   class Viewport;
}

/**
 * The RTSCameraPlugin is a plugin that is used as a tool
 * to place a LinkedPointsActor into the world.
 */
class DT_RTS_CAMERA_EXPORT RTSCameraPlugin
   : public QWidget
   , public Plugin
{
   Q_OBJECT

public:

   const static std::string PLUGIN_NAME;

   RTSCameraPlugin(MainWindow* mw);

   ~RTSCameraPlugin();

   virtual void Destroy();

public slots:

   /**
   * Handles when actors are selected.
   *
   * @param[in]  actors  The list of actors being selected.
   */
   void onActorsSelected(ActorRefPtrVector& actors);

   /**
    *	Handles when a property is changed.
    *
    * @param[in]  proxy     The proxy with the property that changed.
    * @param[in]  property  The property that was changed.
    */
   void onActorPropertyChanged(dtCore::ActorPtr proxy, ActorPropertyRefPtr property);

private:

   MainWindow* mMainWindow;
   dtCore::RefPtr<STAGERTSCameraMotionModel> mMotionModel;
};

#endif // RTS_CAMERA_PLUGIN
