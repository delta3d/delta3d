#include "baseabc.h"



using namespace   dtABC;
using namespace   dtCore;



IMPLEMENT_MANAGEMENT_LAYER(BaseABC)


/** Public methods */
/**
 * Constructors
 */
BaseABC::BaseABC( std::string name /*= "BaseABC"*/ )
:  Base(name),
   KeyboardListener()
{
   RegisterInstance(this);
}



/**
 * Destructors
 */
BaseABC::~BaseABC()
{
   DeregisterInstance(this);
   Notify(DEBUG_INFO, "destroying baseABC ref:%d", referenceCount());
}



/**
 * Configure the system and messages.
 */
void
BaseABC::Config( void )
{
   System*  sys   = System::GetSystem();
   assert( sys );

   sys->Config();

   AddSender( sys );
}



/**
 * Stop the system render loop.
 */
void
BaseABC::Quit( void )
{
   System::GetSystem()->Stop();
}



/**
 * Add a visual object to the Scene.
 *
 * @Param obj the object to add to the scene
 */
void
BaseABC::AddDrawable( Drawable* obj )
{
   assert( obj );
   mScene->AddDrawable( obj );
}



/** Protected methods */
/**
 * Process the render loop messages.
 *
 * @param data the render event message
 */
void
BaseABC::OnMessage( MessageData* data )
{
   if( data->message == "preframe" )
   {
      PreFrame( *(double*)data->userData );
   }
   else if( data->message == "frame" )
   {
      Frame( *(double*)data->userData );
   }
   else if( data->message == "postframe" )
   {
      PostFrame( *(double*)data->userData );
   }
}



/**
 * Process a keyboard key-press messages.
 *
 * @param keyboard pointer to the input device
 * @param key the producer key-code
 * @param character producer character
 */
void
BaseABC::KeyPressed( dtCore::Keyboard*          keyboard, 
                     Producer::KeyboardKey   key,
                     Producer::KeyCharacter  character )
{
   switch (key)
   {
      case Producer::Key_Escape:
         Quit();
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
void
BaseABC::CreateInstances( void )
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
