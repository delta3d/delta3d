// testNetwork.cpp : defines the implementation of the application

#include "testNetwork.h"
#include "packets.h"

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

   mNet = new MyNetwork();

   std::string logFilename;

   if (mHostName.empty()) logFilename = std::string("server.log");
   else logFilename = std::string("client.log");

   mNet->InitializeGame("TestNetwork", 1, logFilename);

   //must come *after* MgrInit;
   GNE::PacketParser::defaultRegisterPacket<PositionPacket>();

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
            GNE::PingPacket ping;
            mNet->SendPacketToAll(ping);
         }
         break;

      case Producer::Key_F:
         {
            SendPosition();
         }
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
   //send a packet to tell the network where we're at
   SendPosition();
}

void TestNetwork::PostFrame( const double deltaFrameTime )
{
   //called after frame has been rendering, collect information about results from scene interaction here
}

void TestNetwork::Quit()
{
   mNet->Shutdown();

   Application::Quit();
}

void TestNetwork::SendPosition()
{
   //get our new position
   Transform xform;
   GetCamera()->GetTransform( &xform );
   osg::Vec3 xyz;
   osg::Vec3 hpr;
   xform.GetTranslation(xyz);
   xform.GetRotation(hpr);

   PositionPacket packet(xyz, hpr);
   mNet->SendPacketToAll( packet );
}
