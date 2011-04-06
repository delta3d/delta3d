// [!output PROJECT_NAME].cpp : defines the implementation of the application

#include "[!output PROJECT_NAME].h"
#include <osgDB/FileUtils>
#include <dtCore/deltawin.h>

using namespace dtCore;
using namespace dtABC;

[!output PROJECT_NAME]::[!output PROJECT_NAME](const std::string& configFilename)
: Application(configFilename)
{
   //Generating a default config file if there isn't one already
   if(!osgDB::fileExists(configFilename)) 
   {
      GenerateDefaultConfigFile();
   }
}


[!output PROJECT_NAME]::~[!output PROJECT_NAME]()
{
}
   
void [!output PROJECT_NAME]::Config()
{   
   GetWindow()->SetWindowTitle("[!output PROJECT_NAME]");
   
   //setup scene here

   Application::Config();
}

bool [!output PROJECT_NAME]::KeyPressed(const dtCore::Keyboard* keyboard, int kc)
{
   bool handled(false);
   switch(kc)
   {
      case osgGA::GUIEventAdapter::KEY_Escape:
         Quit();
         handled = true;
         break;
      //make cases for other keys
      default:
         break;
   }
   
   return handled;
}

void [!output PROJECT_NAME]::PreFrame(const double deltaFrameTime)
{
   //called prior to rendering of frame, do you scene updates here
}

void [!output PROJECT_NAME]::PostFrame(const double deltaFrameTime)
{
   //called after frame has been rendering, collect information about results from scene interaction here
}
