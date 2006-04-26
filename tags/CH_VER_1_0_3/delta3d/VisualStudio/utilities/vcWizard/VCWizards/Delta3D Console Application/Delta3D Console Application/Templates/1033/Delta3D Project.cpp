// [!output PROJECT_NAME].cpp : defines the implementation of the application

#include "[!output PROJECT_NAME].h"

using namespace dtCore;
using namespace dtABC;

IMPLEMENT_MANAGEMENT_LAYER( [!output PROJECT_NAME] )

[!output PROJECT_NAME]::[!output PROJECT_NAME]( const std::string& configFilename )
: Application( configFilename )
{
   RegisterInstance( this );

   //Generating a default config file if there isn't one already
   if( !osgDB::fileExists( configFilename ) ) 
   {
      GenerateDefaultConfigFile();
   }
}


[!output PROJECT_NAME]::~[!output PROJECT_NAME]()
{
   DeregisterInstance( this );
}
   
void [!output PROJECT_NAME]::Config()
{   
   GetWindow()->SetWindowTitle("[!output PROJECT_NAME]");
   
   //setup scene here

   Application::Config();
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
