#include "testNetwork.h"
#include "packets.h"

using namespace dtCore;
using namespace dtABC;
using namespace dtNet;

TestNetwork::TestNetwork( const std::string &hostName, 
                          const std::string& configFilename )
: Application( configFilename ),
  mHostName(hostName)
{
   mNet = new MyNetwork( GetScene() );

   std::string logFilename;

   if (mHostName.empty()) logFilename = std::string("server.log");
   else logFilename = std::string("client.log");

   //initialize the game name, version number, and a networking log file
   mNet->InitializeGame("TestNetwork", 1, logFilename);

   //register our custom packet with GNE
   //must come *after* NetMgr::InitializeGame()
   GNE::PacketParser::defaultRegisterPacket<PositionPacket>();
   GNE::PacketParser::defaultRegisterPacket<PlayerQuitPacket>();

   //if no hostname was supplied, create a server, otherwise create a client
   if (mHostName.empty())
   {
      mNet->SetupServer( 4444 );
      GetWindow()->SetWindowTitle("I'm the Host: " + GetUniqueId().ToString());
   }
   else
   {
      mNet->SetupClient( hostName, 4444 );
      GetWindow()->SetWindowTitle("I'm a Client: " + GetUniqueId().ToString());
   }
}
   
void TestNetwork::Config()
{      
   mMotion = new FlyMotionModel(GetKeyboard(), GetMouse());
   mMotion->SetTarget( GetCamera() );

   //setup scene here
   mTerrain = new Object("Terrain");
   mTerrain->LoadFile("models/dirt.ive");
   AddDrawable( mTerrain.get() );

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
            //send a "ping" packet for latency info
            GNE::PingPacket ping;
            mNet->SendPacket("all", ping);
         }
         break;

      default:
         break;
   }
}

void TestNetwork::PreFrame( const double deltaFrameTime )
{
   mNet->PreFrame( deltaFrameTime );
}

void TestNetwork::Frame( const double deltaFrameTime )
{
   //send a packet to tell the network where we're at
   SendPosition();
}

void TestNetwork::Quit()
{
   //notify everyone else we are quitting
   PlayerQuitPacket packet( GetUniqueId().ToString() );
   mNet->SendPacket( "all", packet );

   //shutdown the networking
   mNet->Shutdown();

   Application::Quit();
}

//send our position out to all connections
void TestNetwork::SendPosition()
{
   //get our new position
   Transform xform;
   GetCamera()->GetTransform( &xform );
   osg::Vec3 xyz;
   osg::Vec3 hpr;
   xform.GetTranslation(xyz);
   xform.GetRotation(hpr);

   PositionPacket packet( xyz, hpr, GetUniqueId().ToString() );
   mNet->SendPacket( "all", packet );
}