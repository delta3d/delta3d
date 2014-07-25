#include "testNetwork.h"
#include "packets.h"

#include <dtCore/deltawin.h>
#include <dtCore/transform.h>

using namespace dtCore;
using namespace dtABC;
using namespace dtNet;

////////////////////////////////////////////////////////////////////////////////
TestNetwork::TestNetwork(const std::string& hostName,
                         const std::string& configFilename)
   : Application(configFilename)
   , mHostName(hostName)
   , mQuitRequested(false)
{
   mNet = new MyNetwork(GetScene());

   std::string logFilename;

   if (mHostName.empty())
   {
      logFilename = std::string("server.log");
   }
   else
   {
      logFilename = std::string("client.log");
   }

   // initialize the game name, version number, and a networking log file
   mNet->InitializeGame("TestNetwork", 1, logFilename);

   // register our custom packet with GNE
   // must come *after* NetMgr::InitializeGame()
   GNE::PacketParser::defaultRegisterPacket<PositionPacket>();
   GNE::PacketParser::defaultRegisterPacket<PlayerQuitPacket>();

   // if no hostname was supplied, create a server, otherwise create a client
   if (mHostName.empty())
   {
      mNet->SetupServer(4444);
      GetWindow()->SetWindowTitle("I'm the Host: " + GetUniqueId().ToString());
   }
   else
   {
      mNet->SetupClient(hostName, 4444);
      GetWindow()->SetWindowTitle("I'm a Client: " + GetUniqueId().ToString());
   }

   CreateHelpLabel();
}

////////////////////////////////////////////////////////////////////////////////
void TestNetwork::Config()
{
   mMotion = new FlyMotionModel(GetKeyboard(), GetMouse());
   mMotion->SetTarget(GetCamera());

   Transform transform(0.0f, 0.0f, 5.0f);
   GetCamera()->SetTransform(transform);

   //setup scene here
   mTerrain = new Object("Terrain");
   mTerrain->LoadFile("StaticMeshes/terrain_simple.ive");
   AddDrawable(mTerrain.get());

   Application::Config();
}

////////////////////////////////////////////////////////////////////////////////
bool TestNetwork::KeyPressed(const dtCore::Keyboard* keyboard, int key)
{
   bool verdict(false);
   switch (key)
   {
   case osgGA::GUIEventAdapter::KEY_Escape:
      {
         mQuitRequested = true;
         verdict = true;
      } break;

   case 'P':
      {
         //send a "ping" packet for latency info
         GNE::PingPacket ping;
         mNet->SendPacket("all", ping);
         verdict = true;
      } break;

   case osgGA::GUIEventAdapter::KEY_F1:
      {
         mLabel->SetActive(!mLabel->GetActive());
      } break;

   default:
      {
         verdict = false;
      } break;
   }

   return verdict;
}

////////////////////////////////////////////////////////////////////////////////
void TestNetwork::PreFrame(const double deltaFrameTime)
{
   mNet->PreFrame(deltaFrameTime);
}

////////////////////////////////////////////////////////////////////////////////
void TestNetwork::Frame(const double deltaFrameTime)
{
   // send a packet to tell the network where we're at
   Application::Frame(deltaFrameTime);
   SendPosition();
}

////////////////////////////////////////////////////////////////////////////////
void TestNetwork::PostFrame(const double deltaFrameTime)
{
   // Since the system will continue through its stage cycle after
   // shutdown, we do this at the last stage
   if (mQuitRequested)
   {
      Quit();
   }
}

////////////////////////////////////////////////////////////////////////////////
void TestNetwork::Quit()
{
   // notify everyone else we are quitting
   PlayerQuitPacket packet(GetUniqueId().ToString());
   mNet->SendPacket("all", packet);

   // shutdown the networking
   mNet->Shutdown();

   Application::Quit();
}

////////////////////////////////////////////////////////////////////////////////
void TestNetwork::SendPosition()
{
   // get our new position
   Transform xform;
   GetCamera()->GetTransform(xform);
   osg::Vec3 xyz;
   osg::Vec3 hpr;
   xform.GetTranslation(xyz);
   xform.GetRotation(hpr);

   PositionPacket packet(xyz, hpr, GetUniqueId().ToString());
   mNet->SendPacket("all", packet);
}

////////////////////////////////////////////////////////////////////////////////
void TestNetwork::CreateHelpLabel()
{
   mLabel = new dtABC::LabelActor();
   osg::Vec2 testSize(21.5f, 3.0f);
   mLabel->SetBackSize(testSize);
   mLabel->SetFontSize(0.8f);
   mLabel->SetTextAlignment(dtABC::LabelActor::AlignmentEnum::LEFT_CENTER);
   mLabel->SetText(CreateHelpLabelText());
   mLabel->SetEnableDepthTesting(false);
   mLabel->SetEnableLighting(false);

   GetCamera()->AddChild(mLabel.get());
   dtCore::Transform labelOffset(-17.0f, 50.0f, 11.5f, 0.0f, 90.0f, 0.0f);
   mLabel->SetTransform(labelOffset, dtCore::Transformable::REL_CS);
   AddDrawable(GetCamera());
}

////////////////////////////////////////////////////////////////////////////////
std::string TestNetwork::CreateHelpLabelText()
{
   std::string testString("");
   testString += "F1: Toggle Help Screen\n";
   testString += "\n";
   testString += "P: Ping network with a packet\n";

   return testString;
}

