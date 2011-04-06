#ifndef RTS_CAMERA_PLUGIN
#define RTS_CAMERA_PLUGIN

#include "export.h"
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/plugininterface.h>

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

private:

   MainWindow* mMainWindow;
};

#endif // RTS_CAMERA_PLUGIN
