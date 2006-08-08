#include <dtABC/baseabc.h>
#include <dtDAL/map.h>

#include <dtCore/keyboard.h>
#include <dtCore/mouse.h>
#include <dtCore/deltawin.h>
#include <dtCore/camera.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtDAL/project.h>

#include <cassert>

using namespace dtABC;
using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(BaseABC)

/** Public methods */
/**
 * Constructors
 */
BaseABC::BaseABC( const std::string& name /*= "BaseABC"*/ ) :  Base(name),
   mWindow(0),
   mCamera(0),
   mScene(0),
   mKeyboard(0),
   mMouse(0)
{
   RegisterInstance(this);

   System*  sys   = System::Instance();
   assert( sys );
   AddSender( sys );
}

BaseABC::~BaseABC()
{
   DeregisterInstance(this);
   RemoveSender( System::Instance() );
}

void BaseABC::Config()
{
   System*  sys   = System::Instance();
   assert( sys );

   sys->Config();
}

void BaseABC::Quit()
{
   System::Instance()->Stop();
}

void BaseABC::AddDrawable( DeltaDrawable* obj )
{
   assert( obj );
   mScene->AddDrawable( obj );
}

void BaseABC::RemoveDrawable( DeltaDrawable* obj )
{
   assert( obj );
   mScene->RemoveDrawable( obj );
}

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

void BaseABC::CreateInstances()
{
   // create the scene
   mScene   = new dtCore::Scene("defaultScene");
   assert( mScene.get() );

   // create the camera
   mCamera  = new dtCore::Camera("defaultCam");
   assert( mCamera.get() );


   // attach camera to the scene
   mCamera->SetScene( mScene.get() );
}

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
         camera->SetScene( GetScene() );
         camera->SetWindow( GetWindow() );

         atLeastOneEnabled = camera->GetEnabled() || atLeastOneEnabled;
        
      }
   }

   if( atLeastOneEnabled )
   {
      //At least one Camera from the map is enabled,
      //therefore let's disable our default BaseABC Camera
      mCamera->SetEnabled(false);

      LOG_INFO( "At least one Camera is our map is enabled, so the default Camera in BaseABC has been disabled." )
   }
   
   dtDAL::Project::GetInstance().LoadMapIntoScene( map, *GetScene(), addBillBoards );
}

dtDAL::Map& BaseABC::LoadMap( const std::string& name, bool addBillBoards)
{
   dtDAL::Map& map = dtDAL::Project::GetInstance().GetMap(name);
   LoadMap( map, addBillBoards );
   return map;
}
