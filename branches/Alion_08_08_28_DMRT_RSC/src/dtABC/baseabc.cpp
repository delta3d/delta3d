#include <dtABC/baseabc.h>
#include <dtDAL/map.h>

#include <dtCore/keyboard.h>
#include <dtCore/mouse.h>
#include <dtCore/deltawin.h>
#include <dtCore/camera.h>
#include <dtCore/view.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtDAL/project.h>

#include <osgViewer/View>
#include <cassert>

using namespace dtABC;
using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(BaseABC)

/** Public methods */
/**
 * Constructors
 */
////////////////////////////////////////////////
BaseABC::BaseABC( const std::string& name /*= "BaseABC"*/ ) :
   Base(name)
{
   RegisterInstance(this);

   System*  sys   = &dtCore::System::GetInstance();
   assert( sys );
   AddSender( sys );
   CreateDefaultView();
}

////////////////////////////////////////////////
BaseABC::~BaseABC()
{
   DeregisterInstance(this);
   RemoveSender( &dtCore::System::GetInstance() );
}

////////////////////////////////////////////////
void BaseABC::Config()
{
   System*  sys   = &dtCore::System::GetInstance();
   assert( sys );

   sys->Config();
}

////////////////////////////////////////////////
void BaseABC::Quit()
{
   System::GetInstance().Stop();
}

////////////////////////////////////////////////
void BaseABC::SetCamera(dtCore::Camera * camera)
{
   GetView()->SetCamera(camera);
}

////////////////////////////////////////////////
dtCore::DeltaWin* BaseABC::GetWindow()
{
   return mWindow.get();
}

////////////////////////////////////////////////
void BaseABC::SetWindow(dtCore::DeltaWin * win)
{
   mWindow = win;
   if (GetCamera() != NULL)
      GetCamera()->SetWindow(win);
}

////////////////////////////////////////////////
void BaseABC::AddDrawable( DeltaDrawable* obj )
{
   assert( obj );
   GetScene()->AddDrawable( obj );
}

////////////////////////////////////////////////
void BaseABC::RemoveDrawable( DeltaDrawable* obj )
{
   assert( obj );
   GetScene()->RemoveDrawable( obj );
}

////////////////////////////////////////////////
void BaseABC::OnMessage( MessageData* data )
{
   if( data->message == "preframe" )
   {
      PreFrame( *static_cast<const double*>(data->userData) );
   }
   else if( data->message == "frame" )
   {
      Frame( *static_cast<const double*>(data->userData) );
   }
   else if( data->message == "postframe" )
   {
      PostFrame( *static_cast<const double*>(data->userData) );
   }
}

////////////////////////////////////////////////
void BaseABC::CreateInstances()
{
    // create the camera
   assert( mViewList[0].get() );

   mViewList[0]->SetCamera( new dtCore::Camera("defaultCam") );
   mViewList[0]->SetScene( new dtCore::Scene("defaultScene") );

   GetKeyboard()->SetName("defaultKeyboard");
   GetMouse()->SetName("defaultMouse");
}

////////////////////////////////////////////////
dtCore::View * BaseABC::CreateDefaultView()
{
   dtCore::RefPtr<dtCore::View> view = new dtCore::View("defaultView");
   mViewList.reserve(1);
   mViewList.push_back(view);
   return mViewList[0].get();
}

////////////////////////////////////////////////
void BaseABC::LoadMap( dtDAL::Map& map, bool addBillBoards )
{
   typedef std::vector< dtCore::RefPtr< dtDAL::ActorProxy > > ActorProxyVector;
   ActorProxyVector proxies;
   map.FindProxies(proxies, "*", "dtcore", "Camera");

   bool atLeastOneEnabled(false);
   for(  ActorProxyVector::iterator iter = proxies.begin();
         iter != proxies.end();
         iter++ )
   {
      if( dtCore::Camera* camera = dynamic_cast< dtCore::Camera* >( (*iter)->GetActor() ) )
      {
         camera->SetWindow( GetWindow() );

         atLeastOneEnabled = camera->GetEnabled() || atLeastOneEnabled;
      }
   }

   map.FindProxies(proxies, "*", "dtcore", "View");
   for(  ActorProxyVector::iterator iter = proxies.begin();
         iter != proxies.end();
         iter++ )
   {
      if( dtCore::View* view = dynamic_cast< dtCore::View* >( (*iter)->GetActor() ) )
      {
          view->SetScene( GetScene() );
      }
   }

   if( atLeastOneEnabled )
   {
      //At least one Camera from the map is enabled,
      //therefore let's disable our default BaseABC Camera
      GetCamera()->SetEnabled(false);

      LOG_INFO( "At least one Camera is our map is enabled, so the default Camera in BaseABC has been disabled." )
   }

   dtDAL::Project::GetInstance().LoadMapIntoScene( map, *GetScene(), addBillBoards );
}

////////////////////////////////////////////////
dtDAL::Map& BaseABC::LoadMap( const std::string& name, bool addBillBoards)
{
   dtDAL::Map& map = dtDAL::Project::GetInstance().GetMap(name);
   LoadMap( map, addBillBoards );
   return map;
}
