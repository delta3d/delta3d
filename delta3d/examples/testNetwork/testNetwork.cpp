// testNetwork.cpp : defines the implementation of the application

#include "testNetwork.h"
#include  <dtNet/dtnet.h>

using namespace dtCore;
using namespace dtABC;
using namespace dtNet;

IMPLEMENT_MANAGEMENT_LAYER( testNetwork )

testNetwork::testNetwork( const std::string& configFilename )
: Application( configFilename )
{
   RegisterInstance( this );

   //Generating a default config file if there isn't one already
   std::string foundPath = osgDB::findDataFile(configFilename);

   if( foundPath.empty() )
   {
      GenerateDefaultConfigFile();
   }
}


testNetwork::~testNetwork()
{
   DeregisterInstance( this );
}
   
void testNetwork::Config()
{   
   RefPtr<dtNet::NetMgr> net = new dtNet::NetMgr();

   GetWindow()->SetWindowTitle("testNetwork");
   
   //setup scene here

   Application::Config();
}

void testNetwork::KeyPressed(   Keyboard*      keyboard, 
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

void testNetwork::PreFrame( const double deltaFrameTime )
{
   //called prior to rendering of frame, do you scene updates here
}

void testNetwork::Frame( const double deltaFrameTime )
{
   //called during frame render
}

void testNetwork::PostFrame( const double deltaFrameTime )
{
   //called after frame has been rendering, collect information about results from scene interaction here
}
