// testNetwork.cpp : defines the implementation of the application

#include "testNetwork.h"

using namespace dtCore;
using namespace dtABC;
using namespace dtNet;

IMPLEMENT_MANAGEMENT_LAYER( TestNetwork )

TestNetwork::TestNetwork( const std::string &hostName, 
                          const std::string& configFilename )
: Application( configFilename ),
  mHostName(hostName)
{
   RegisterInstance( this );


   mNet = new NetMgr();

   std::string logFilename;

   if (mHostName.empty()) logFilename = std::string("server.log");
   else logFilename = std::string("client.log");

   mNet->InitializeGame("TestNetwork", 1, logFilename);


   if (mHostName.empty())
   {
      mNet->SetupServer(4444);
      GetWindow()->SetWindowTitle("I'm the Host");
   }
   else
   {
      mNet->SetupClient( hostName, 4444 );
      GetWindow()->SetWindowTitle("I'm a Client");
   }
}


TestNetwork::~TestNetwork()
{
   DeregisterInstance( this );
}
   
void TestNetwork::Config()
{      
   //setup scene here

   Application::Config();
}

void TestNetwork::KeyPressed(   Keyboard*      keyboard, 
                                    Producer::KeyboardKey  key,
                                    Producer::KeyCharacter character )
{
   switch( key )
   {
      case Producer::Key_Escape:
         Quit();
         break;
      case Producer::Key_P:
         {
            GNE::PingPacket ping;
            mNet->SendPacketToAll(ping);
         }

         break;
      default:
         break;
   }
}

void TestNetwork::PreFrame( const double deltaFrameTime )
{
   //called prior to rendering of frame, do you scene updates here
}

void TestNetwork::Frame( const double deltaFrameTime )
{
   //called during frame render
}

void TestNetwork::PostFrame( const double deltaFrameTime )
{
   //called after frame has been rendering, collect information about results from scene interaction here
}

void TestNetwork::Quit()
{
   {   
      mNet->Shutdown();
   }

   Application::Quit();
}
