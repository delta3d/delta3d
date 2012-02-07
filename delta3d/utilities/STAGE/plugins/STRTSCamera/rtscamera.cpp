#include "rtscamera.h"

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
#include <dtCore/transform.h>


const std::string RTSCameraPlugin::PLUGIN_NAME = "RTS Camera";


////////////////////////////////////////////////////////////////////////////////
RTSCameraPlugin::RTSCameraPlugin(MainWindow* mw)
   : mMainWindow(mw)
{
   dtEditQt::PerspectiveViewport* viewport = mMainWindow->GetPerspView();

   mMotionModel = new STAGERTSCameraMotionModel(viewport->GetView()->GetMouse());
   mMotionModel->SetCamera(viewport->getCamera());
   mMotionModel->SetViewport(viewport);

   mMainWindow->GetPerspView()->setCameraMotionModel(mMotionModel);

   connect(&EditorEvents::GetInstance(), SIGNAL(selectedActors(ActorProxyRefPtrVector &)),
      this, SLOT(onActorsSelected(ActorProxyRefPtrVector &)));
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

////////////////////////////////////////////////////////////////////////////////
void RTSCameraPlugin::onActorsSelected(ActorProxyRefPtrVector& actors)
{
   int count = (int)actors.size();
   if (count > 0)
   {
      // Modify the minimum ground height to the lowest height of the currently selected actor(s).
      float lowestHeight = 0;

      bool first = true;

      for (int index = 0; index < count; ++index)
      {
         dtCore::ActorProxy* proxy = actors[index];
         if (proxy)
         {
            dtCore::Transformable* actor = NULL;
            proxy->GetActor(actor);
            if (actor)
            {
               dtCore::Transform transform;
               actor->GetTransform(transform);
               float height = transform.GetTranslation().z();
               if (height < lowestHeight || first)
               {
                  first = false;
                  lowestHeight = height;
               }
            }
         }
      }

      if (mMotionModel.valid())
      {
         mMotionModel->SetGroundHeight(lowestHeight - 5);
      }
   }
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
