// [!output PROJECT_NAME].cpp : defines the implementation of the application

#include "[!output PROJECT_NAME].h"

using namespace dtCore;
using namespace dtABC;
using namespace std;

IMPLEMENT_MANAGEMENT_LAYER( [!output PROJECT_NAME] )

[!output PROJECT_NAME]::[!output PROJECT_NAME]( string configFilename )
: Application( configFilename )
{
   RegisterInstance( this );
}


[!output PROJECT_NAME]::~[!output PROJECT_NAME]()
{
   DeregisterInstance( this );
}
   
void [!output PROJECT_NAME]::Config()
{
   Application::Config();
   
   GetWindow()->SetWindowTitle("[!output PROJECT_NAME]");
   
   //setup scene here
}

void [!output PROJECT_NAME]::KeyPressed(   Keyboard*      keyboard, 
                                    Producer::KeyboardKey  key,
                                    Producer::KeyCharacter character )
{
   switch( key )
   {
      case Producer::Key_Escape:
         Quit();
         break;
      //make cases for other keys
      default:
         break;
   }
}

void [!output PROJECT_NAME]::PreFrame( const double deltaFrameTime )
{
   //called prior to rendering of frame, do you scene updates here
}

void [!output PROJECT_NAME]::Frame( const double deltaFrameTime )
{
   //called during frame render
}

void [!output PROJECT_NAME]::PostFrame( const double deltaFrameTime )
{
   //called after frame has been rendering, collect information about results from scene interaction here
}
