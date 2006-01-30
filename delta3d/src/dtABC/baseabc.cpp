#include <dtABC/baseabc.h>
#include <dtDAL/map.h>
#include <cassert>

using namespace dtABC;
using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(BaseABC)

/** Public methods */
/**
 * Constructors
 */
BaseABC::BaseABC( const std::string& name /*= "BaseABC"*/ )
:  Base(name),
   KeyboardListener()
{
   RegisterInstance(this);

   System*  sys   = System::Instance();
   assert( sys );
   AddSender( sys );
}



/**
 * Destructors
 */
BaseABC::~BaseABC()
{
   DeregisterInstance(this);
   RemoveSender( System::Instance() );
}



/**
 * Configure the system and messages.
 */
void BaseABC::Config()
{
   System*  sys   = System::Instance();
   assert( sys );

   sys->Config();
}



/**
 * Stop the system render loop.
 */
void BaseABC::Quit()
{
   System::Instance()->Stop();
}



/**
 * Add a visual object to the Scene.
 *
 * @param obj the object to add to the scene
 */
void BaseABC::AddDrawable( DeltaDrawable* obj )
{
   assert( obj );
   mScene->AddDrawable( obj );
}


/**
 * Remove a visual object from the Scene.
 *
 * @param obj the object to remove from the scene
 */
void BaseABC::RemoveDrawable( DeltaDrawable* obj )
{
   assert( obj );
   mScene->RemoveDrawable( obj );
}


/** Protected methods */
/**
 * Process the render loop messages.
 *
 * @param data the render event message
 */
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



/**
 * Process a keyboard key-press messages.
 *
 * @param keyboard pointer to the input device
 * @param key the producer key-code
 * @param character producer character
 */
void BaseABC::KeyPressed(  dtCore::Keyboard*       keyboard, 
                           Producer::KeyboardKey   key,
                           Producer::KeyCharacter  character )
{
   switch (key)
   {
      case Producer::Key_Escape:
         Quit();
         break;
      default:
         break;
   }
}



/**
 * Process a keyboard key-release messages.
 *
 * @param keyboard pointer to the input device
 * @param key the producer key-code
 * @param character producer character
 */
void
BaseABC::KeyReleased(   dtCore::Keyboard*          keyboard, 
                        Producer::KeyboardKey   key,
                        Producer::KeyCharacter  character )
{
}



/**
 * Create the basic instances
 */
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
   typedef std::vector< osg::ref_ptr< dtDAL::ActorProxy > > ActorProxyVector;
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
