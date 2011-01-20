#include "rtscamera.h"

#include "stagertscameramotionmodel.h"

#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/pluginmanager.h>
#include <dtEditQt/viewportmanager.h>
#include <dtEditQt/viewportoverlay.h>
#include <dtEditQt/viewport.h>
#include <dtEditQt/editorviewport.h>
#include <dtEditQt/editorviewportcontainer.h>
#include <dtEditQt/perspectiveviewport.h>
#include <dtEditQt/stageobjectmotionmodel.h>

#include <dtCore/keyboard.h>
#include <dtCore/mouse.h>


const std::string RTSCameraPlugin::PLUGIN_NAME = "RTS Camera";


////////////////////////////////////////////////////////////////////////////////
RTSCameraPlugin::RTSCameraPlugin(MainWindow* mw)
   : mMainWindow(mw)
{
   dtEditQt::PerspectiveViewport* viewport = mMainWindow->GetPerspView();

   STAGERTSCameraMotionModel* motionModel = new STAGERTSCameraMotionModel(viewport->GetView()->GetMouse());
   motionModel->SetCamera(viewport->getCamera());
   motionModel->SetViewport(viewport);

   mMainWindow->GetPerspView()->setCameraMotionModel(motionModel);
}

////////////////////////////////////////////////////////////////////////////////
RTSCameraPlugin::~RTSCameraPlugin()
{
}

////////////////////////////////////////////////////////////////////////////////
void RTSCameraPlugin::Destroy()
{
   mMainWindow->GetPerspView()->setCameraMotionModel(NULL);
}

////////////////////////////////////////////////////////////

namespace RTSCamera
{
class DT_RTS_CAMERA_EXPORT PluginFactory : public dtEditQt::PluginFactory
{
public:

   PluginFactory() {}
   ~PluginFactory() {}

   /** get the name of the plugin */
   virtual std::string GetName() { return RTSCameraPlugin::PLUGIN_NAME; }

   /** get a description of the plugin */
   virtual std::string GetDescription() { return "An RTS Camera for the Perspective viewport."; }

   virtual void GetDependencies(std::list<std::string>& deps) 
   {
   }

    /** construct the plugin and return a pointer to it */
   virtual Plugin* Create(MainWindow* mw) 
   {
      mPlugin = new RTSCameraPlugin(mw);
      return mPlugin;
   }

   virtual void Destroy() 
   {
      delete mPlugin;
   }

private:

   Plugin* mPlugin;
}; 
} //namespace RTSCameraPlugin

extern "C" DT_RTS_CAMERA_EXPORT dtEditQt::PluginFactory* CreatePluginFactory()
{
   return new RTSCamera::PluginFactory;
}
