// testNetwork.cpp : defines the implementation of the application

#include "testNetwork.h"

using namespace dtCore;
using namespace dtABC;
using namespace dtNet;

IMPLEMENT_MANAGEMENT_LAYER( testNetwork )

testNetwork::testNetwork( const std::string &hostName, 
                          const std::string& configFilename )
: Application( configFilename ),
  mHostName(hostName)
{
   RegisterInstance( this );


   mNet = new NetMgr();

   std::string logFilename;

   if (mHostName.empty()) logFilename = std::string("server.log");
   else logFilename = std::string("client.log");

   mNet->InitializeGame("testNetwork", 1.0, logFilename);


   if (mHostName.empty())
   {
      mNet->SetupServer(4625);
      GetWindow()->SetWindowTitle("I'm the Host");
   }
   else
   {
      mNet->SetupClient( hostName, 4625 );
      GetWindow()->SetWindowTitle("I'm a Client");
   }
}


testNetwork::~testNetwork()
{
   DeregisterInstance( this );
}
   
void testNetwork::Config()
{   

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

void testNetwork::Quit()
{
   {   
      mNet->Shutdown();
   }

   Application::Quit();
}