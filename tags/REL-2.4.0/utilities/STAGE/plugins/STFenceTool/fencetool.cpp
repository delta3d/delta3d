#include "fencetool.h"

#include <dtCore/deltadrawable.h>
#include <dtCore/transformable.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/librarymanager.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/pluginmanager.h>
#include <dtEditQt/viewportmanager.h>
#include <dtEditQt/viewportoverlay.h>
#include <dtEditQt/viewport.h>
#include <dtEditQt/editorviewport.h>
#include <dtEditQt/editorviewportcontainer.h>
#include <dtEditQt/perspectiveviewport.h>
#include <dtEditQt/orthoviewport.h>
#include <dtEditQt/stageobjectmotionmodel.h>
#include <dtEditQt/stagecamera.h>
#include <dtUtil/fileutils.h>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include <QtGui/QToolBar>


const std::string FenceToolPlugin::PLUGIN_NAME = "Fence Tool";


////////////////////////////////////////////////////////////////////////////////
FenceToolPlugin::FenceToolPlugin(MainWindow* mw)
   : mMainWindow(mw)
   , mActiveProxy(NULL)
   , mActiveActor(NULL)
   , mModeButton(NULL)
{
   // Find the mode button for the Linked Points Actor Tool Plugin.
   mModeButton = mMainWindow->FindExclusiveToolMode("Linked Points Actor Tool");

   // Setup our signal slots.
   connect(&EditorEvents::GetInstance(), SIGNAL(selectedActors(ActorProxyRefPtrVector &)),
      this, SLOT(onActorsSelected(ActorProxyRefPtrVector &)));

   connect(&ViewportManager::GetInstance(), SIGNAL(selectActors(Viewport*, QMouseEvent*, bool*)),
      this, SLOT(onSelectActors(Viewport*, QMouseEvent*, bool*)));

   if (mModeButton)
   {
      connect(mModeButton, SIGNAL(changed()), this, SLOT(onModeButtonPressed()));
   }
}

////////////////////////////////////////////////////////////////////////////////
FenceToolPlugin::~FenceToolPlugin()
{
}

////////////////////////////////////////////////////////////////////////////////
void FenceToolPlugin::Destroy()
{
   mModeButton = NULL;
   mActiveProxy = NULL;
   mActiveActor = NULL;
}

////////////////////////////////////////////////////////////////////////////////
void FenceToolPlugin::onActorsSelected(ActorProxyRefPtrVector& actors)
{
   // We can only use this editor if the linked points actor is the only one selected.
   if (actors.size() == 1)
   {
      // Only allow linked points actor.
      dtActors::FenceActorProxy* fenceProxy = dynamic_cast<dtActors::FenceActorProxy*>(actors[0].get());
      if (fenceProxy)
      {
         initialize(fenceProxy);
         return;
      }
   }

   shutdown();
   mActiveProxy = NULL;
   mActiveActor = NULL;
}

////////////////////////////////////////////////////////////////////////////////
void FenceToolPlugin::onSelectActors(Viewport* vp, QMouseEvent* e, bool* overrideDefault)
{
   // Only do something if the tool is active.
   if (!mModeButton || !mModeButton->isChecked())
   {
      return;
   }

   // Bail if we don't have an editor view.
   EditorViewport* editorView = dynamic_cast<EditorViewport*>(vp);
   if (!editorView)
   {
      return;
   }

   // If we don't have a valid actor selected, this tool is not needed.
   if (!mActiveActor)
   {
      return;
   }

   // Make sure we are right clicking.
   if (editorView->GetMouseButton() == Qt::RightButton)
   {
      // Make sure we are clicking on our active actor.
      dtCore::DeltaDrawable* drawable = editorView->getPickDrawable(e->pos().x(), e->pos().y());

      if (drawable)
      {
         osg::Vec3 position;
         editorView->getPickPosition(position);

         // Check if the drawable is a valid point on the actor.
         int pointIndex = 0;
         int subIndex = 0;
         dtActors::FenceActor::DrawableType type = mActiveActor->GetDrawableType(drawable, position, pointIndex, subIndex);
         if (type != dtActors::FenceActor::DRAWABLE_TYPE_NONE)
         {
            *overrideDefault = true;

            // Now determine if the picked object was a post or segment.
            switch (type)
            {
            case dtActors::FenceActor::DRAWABLE_TYPE_POST:
               mActiveActor->IncrementPostMesh(pointIndex, subIndex);
               break;

            case dtActors::FenceActor::DRAWABLE_TYPE_SEGMENT:
               mActiveActor->IncrementSegmentMesh(pointIndex, subIndex);
               break;
            
            case dtActors::FenceActor::DRAWABLE_TYPE_NONE:
            default:
            break;
            }
         }

         ViewportManager::GetInstance().refreshAllViewports();
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void FenceToolPlugin::onModeButtonPressed()
{
   if (mModeButton->isChecked())
   {
      if (mActiveProxy.valid())
      {
         initialize(mActiveProxy.get());
      }
   }
   else
   {
      shutdown();
   }
}

////////////////////////////////////////////////////////////////////////////////
void FenceToolPlugin::initialize(dtActors::FenceActorProxy* activeProxy)
{
   mActiveProxy = activeProxy;

   if (mActiveProxy.valid())
   {
      mActiveProxy->GetActor(mActiveActor);
   }
}

////////////////////////////////////////////////////////////////////////////////
void FenceToolPlugin::shutdown()
{
}


////////////////////////////////////////////////////////////

namespace FenceTool
{
class DT_FENCE_TOOL_EXPORT PluginFactory : public dtEditQt::PluginFactory
{
public:

   PluginFactory() {}
   ~PluginFactory() {}

   /** get the name of the plugin */
   virtual std::string GetName() { return FenceToolPlugin::PLUGIN_NAME; }

   /** get a description of the plugin */
   virtual std::string GetDescription() { return "Handles Post meshes and Segment textures."; }

   virtual void GetDependencies(std::list<std::string>& deps) 
   {
      deps.push_back("Linked Points Actor Tool");
   }

    /** construct the plugin and return a pointer to it */
   virtual Plugin* Create(MainWindow* mw) 
   {
      mPlugin = new FenceToolPlugin(mw);
      return mPlugin;
   }

   virtual void Destroy() 
   {
      delete mPlugin;
   }

private:

   Plugin* mPlugin;
}; 
} //namespace FenceToolPlugin

extern "C" DT_FENCE_TOOL_EXPORT dtEditQt::PluginFactory* CreatePluginFactory()
{
   return new FenceTool::PluginFactory;
}
