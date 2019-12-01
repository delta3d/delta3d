#include <dtABC/baseabc.h>
#include <dtCore/map.h>
#include <dtUtil/cullmask.h>

#include <dtCore/keyboard.h>
#include <dtCore/mouse.h>
#include <dtCore/deltawin.h>
#include <dtCore/camera.h>
#include <dtCore/view.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtCore/project.h>

#include <osgViewer/View>
#include <cassert>

using namespace dtABC;
using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(BaseABC)

/** Public methods */
/**
 * Constructors
 */
////////////////////////////////////////////////////////////////////////////////
BaseABC::BaseABC(const std::string& name /*= "BaseABC"*/)
   : Base(name)
{
   RegisterInstance(this);

   dtCore::System::GetInstance().TickSignal.connect_slot(this, &BaseABC::OnSystem);
   CreateDefaultView();
}

////////////////////////////////////////////////////////////////////////////////
BaseABC::~BaseABC()
{
   DeregisterInstance(this);
}

////////////////////////////////////////////////////////////////////////////////
void BaseABC::Config()
{
   System* sys = &dtCore::System::GetInstance();
   assert(sys);

   sys->Config();
}

////////////////////////////////////////////////////////////////////////////////
void BaseABC::Quit()
{
   System::GetInstance().Stop();
}

////////////////////////////////////////////////////////////////////////////////
void BaseABC::SetCamera(dtCore::Camera* camera)
{
   if (GetView() != NULL)
   {
      GetView()->SetCamera(camera);
   }
}

////////////////////////////////////////////////////////////////////////////////
dtCore::DeltaWin* BaseABC::GetWindow()
{
   return mWindow.get();
}

////////////////////////////////////////////////////////////////////////////////
void BaseABC::SetWindow(dtCore::DeltaWin* win)
{
   mWindow = win;
   if (GetCamera() != NULL)
   {
      GetCamera()->SetWindow(win);
   }
}

////////////////////////////////////////////////////////////////////////////////
void BaseABC::AddDrawable(DeltaDrawable* obj)
{
   if (GetScene() != NULL)
   {
      GetScene()->AddChild(obj);
   }
}

////////////////////////////////////////////////////////////////////////////////
void BaseABC::RemoveDrawable(DeltaDrawable* obj)
{
   if (GetScene() != NULL)
   {
      GetScene()->RemoveChild(obj);
   }
}

////////////////////////////////////////////////////////////////////////////////
void BaseABC::OnSystem(const dtUtil::RefString& str, double deltaSim, double)
{
   if (str == dtCore::System::MESSAGE_EVENT_TRAVERSAL)
   {
      EventTraversal(deltaSim);
   }
   else if (str == dtCore::System::MESSAGE_PRE_FRAME)
   {
      PreFrame(deltaSim);
   }
   else if (str == dtCore::System::MESSAGE_FRAME)
   {
      Frame(deltaSim);
   }
   else if (str == dtCore::System::MESSAGE_POST_FRAME)
   {
      PostFrame(deltaSim);
   }
   else if (str == dtCore::System::MESSAGE_PAUSE)
   {
      Pause(deltaSim);
   }
}

////////////////////////////////////////////////////////////////////////////////
void BaseABC::CreateInstances()
{
    // create the camera
   assert(mViewList[0].get());

   dtCore::RefPtr<dtCore::Camera> defaultCam = new dtCore::Camera("defaultCam");   
   defaultCam->GetOSGCamera()->setCullMask(dtUtil::CullMask::MAIN_CAMERA_MASK);

   mViewList[0]->SetCamera(defaultCam.get());
   mViewList[0]->SetScene(new dtCore::Scene("defaultScene"));

   if (GetKeyboard() != NULL)
   {
      GetKeyboard()->SetName("defaultKeyboard");
   }

   if (GetMouse() != NULL)
   {
      GetMouse()->SetName("defaultMouse");
   }
}

////////////////////////////////////////////////////////////////////////////////
dtCore::View* BaseABC::CreateDefaultView()
{
   mViewList.push_back(new dtCore::View("defaultView"));
   return mViewList[0].get();
}

////////////////////////////////////////////////////////////////////////////////
void BaseABC::LoadMap(dtCore::Map& map, bool addBillBoards)
{
   typedef std::vector< dtCore::RefPtr<dtCore::BaseActorObject> > ActorProxyVector;
   ActorProxyVector proxies;
   map.FindProxies(proxies, "*", "dtcore", "Camera");

   bool atLeastOneEnabled(false);
   for (ActorProxyVector::iterator iter = proxies.begin();
        iter != proxies.end();
        iter++)
   {
      if (dtCore::Camera* camera = dynamic_cast<dtCore::Camera*>((*iter)->GetDrawable()))
      {
         camera->SetWindow(GetWindow());

         atLeastOneEnabled = camera->GetEnabled() || atLeastOneEnabled;
      }
   }

   map.FindProxies(proxies, "*", "dtcore", "View");
   for (ActorProxyVector::iterator iter = proxies.begin();
        iter != proxies.end();
        iter++)
   {
      if (dtCore::View* view = dynamic_cast<dtCore::View*>((*iter)->GetDrawable()))
      {
          view->SetScene(GetScene());
      }
   }

   if (atLeastOneEnabled)
   {
      //At least one Camera from the map is enabled,
      //therefore let's disable our default BaseABC Camera
      GetCamera()->SetEnabled(false);

      LOG_INFO("At least one Camera is our map is enabled, so the default Camera in BaseABC has been disabled.")
   }

   dtCore::Project::GetInstance().LoadMapIntoScene(map, *GetScene(), addBillBoards);
}

////////////////////////////////////////////////////////////////////////////////
dtCore::Map& BaseABC::LoadMap(const std::string& name, bool addBillBoards)
{
   dtCore::Map& map = dtCore::Project::GetInstance().GetMap(name);
   LoadMap(map, addBillBoards);
   return map;
}

//////////////////////////////////////////////////////////////////////////
dtCore::View* dtABC::BaseABC::GetView()
{
   if (!mViewList.empty())
   {
      return mViewList[0].get();
   }

   return NULL;
}

//////////////////////////////////////////////////////////////////////////
const dtCore::View* dtABC::BaseABC::GetView() const
{
   if (!mViewList.empty())
   {
      return mViewList[0].get();
   }

   return NULL;
}

////////////////////////////////////////////////////////////////////////////////
dtCore::View* dtABC::BaseABC::GetView(unsigned int idx)
{
   if (idx >= mViewList.size())
   {
      return NULL;
   }
   else
   {
      return mViewList[idx].get();
   }
}

////////////////////////////////////////////////////////////////////////////////
const dtCore::View* dtABC::BaseABC::GetView(unsigned int idx) const
{
   if (idx > mViewList.size())
   {
      return NULL;
   }
   else
   {
      return mViewList[idx].get();
   }
}

//////////////////////////////////////////////////////////////////////////
void dtABC::BaseABC::SetView(dtCore::View* view)
{
   mViewList[0] = view;
}

//////////////////////////////////////////////////////////////////////////
dtCore::Camera* dtABC::BaseABC::GetCamera()
{
   if (GetView() != NULL)
   {
      return GetView()->GetCamera();
   }

   return NULL;
}

//////////////////////////////////////////////////////////////////////////
const dtCore::Camera* dtABC::BaseABC::GetCamera() const
{
   if (GetView() != NULL)
   {
      return GetView()->GetCamera();
   }

   return NULL;
}

//////////////////////////////////////////////////////////////////////////
dtCore::Scene* dtABC::BaseABC::GetScene()
{
   if (GetView() != NULL)
   {
      return GetView()->GetScene();
   }

   return NULL;
}

//////////////////////////////////////////////////////////////////////////
void dtABC::BaseABC::SetScene(dtCore::Scene* scene)
{
   if (GetView() != NULL)
   {
      GetView()->SetScene(scene);
   }
}

//////////////////////////////////////////////////////////////////////////
dtCore::Keyboard* dtABC::BaseABC::GetKeyboard()
{
   if (GetView() != NULL)
   {
      return GetView()->GetKeyboard();
   }

   return NULL;
}

//////////////////////////////////////////////////////////////////////////
void dtABC::BaseABC::SetKeyboard(dtCore::Keyboard* keyboard)
{
   if (GetView() != NULL)
   {
      GetView()->SetKeyboard(keyboard);
   }
}

//////////////////////////////////////////////////////////////////////////
dtCore::Mouse* dtABC::BaseABC::GetMouse()
{
   if (GetView() != NULL)
   {
      return GetView()->GetMouse();
   }

   return NULL;
}

//////////////////////////////////////////////////////////////////////////
void dtABC::BaseABC::SetMouse(dtCore::Mouse* mouse)
{
   if (GetView() != NULL)
   {
      GetView()->SetMouse(mouse);
   }
}

////////////////////////////////////////////////////////////////////////////////
unsigned int dtABC::BaseABC::GetNumberOfViews() const
{
   return mViewList.size();
}
