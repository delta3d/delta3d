// TestAI.cpp : defines the implementation of the application

#include "testai.h"
#include <osgDB/FileUtils>
#include <dtCore/camera.h>
#include <dtCore/globals.h>
#include <dtDAL/map.h>
#include <dtDAL/project.h>
#include <dtDAL/exceptionenum.h>
#include <dtCore/macros.h>

#include <dtAI/waypoint.h>
#include <dtAI/waypointmanager.h>

#include <dtCore/generickeyboardlistener.h>
#include <dtGUI/ceguikeyboardlistener.h>

#include <dtUtil/mathdefines.h>
#include <dtUtil/log.h>

using namespace dtCore;
using namespace dtABC;
using namespace dtDAL;
using namespace dtAI;


TestAI::TestAI(const std::string& configFilename)
   : Application(configFilename)
   , mCharacter(0)
{
   //Generating a default config file if there isn't one already
   if( !osgDB::fileExists( configFilename ) ) 
   {
      GenerateDefaultConfigFile();
   }
}


TestAI::~TestAI()
{
  
}
   
void TestAI::Config()
{
   Application::Config();

   //initialize the Waypoint Helper
   WaypointManager::GetInstance();   
   GetScene()->AddDrawable(WaypointManager::GetInstance());

   LoadDemoMap("TesttownLt");

   //create our navigation mesh
   WaypointManager::GetInstance()->CreateNavMesh(GetScene());

   //turn on viewing of waypoints
   WaypointManager::GetInstance()->SetDrawWaypoints(true);

   //set camera offset
   dtCore::Transform trans;
   trans.SetTranslation(-1.0f, 5.5f, 1.5f);
   trans.SetRotation(180.0f, -2.0f, 0.0f);
   GetCamera()->SetTransform(trans);

   //create overhead camera and disable it by default   
   mOverheadCamera = new dtCore::Camera();
   mOverheadCamera->SetScene(GetScene());
   mOverheadCamera->SetWindow(GetWindow());
   mOverheadCamera->SetEnabled(false);
   //set overhead camera offset
   trans.SetTranslation(-1.0f, 20.0f, 100.0f);
   trans.SetRotation(90.0f, 270.0f, 0.0f);
   mOverheadCamera->SetTransform(trans);
   GetCamera()->AddChild(mOverheadCamera.get());


   //get the first waypoint to spawn the character at
   const WaypointManager::WaypointMap& pContainer = WaypointManager::GetInstance()->GetWaypoints();
   WaypointManager::WaypointMap::const_iterator iter = pContainer.begin();
   const Waypoint* pWaypoint = (*iter).second;

   //spawn our character
   mCharacter = new dtAI::AICharacter(GetScene(), GetCamera(), pWaypoint, "marine/marine.rbody", 10);    
   GoToWaypoint(1);

   //seed the random generator
   srand(420);
}

bool TestAI::KeyPressed(const dtCore::Keyboard* keyboard, Producer::KeyboardKey key, Producer::KeyCharacter character)
{
  
   switch( key )
   {
      case Producer::Key_space:
      {
         if(GetCamera()->GetEnabled())
         {
            GetCamera()->SetEnabled(false);
            mOverheadCamera->SetEnabled(true);
         }
         else
         {
            mOverheadCamera->SetEnabled(false);
            GetCamera()->SetEnabled(true);
         }
         return true;
      }

      case Producer::Key_Escape:
         {            
            Quit();
            return true;
         }

      default:
         break;
   }
   return false;
}

void TestAI::PreFrame( const double deltaFrameTime )
{
   mCharacter->Update(deltaFrameTime);

   if(mCharacter->GetCurrentWaypoint() == mCurrentWaypoint)
   {
      //send the character to a random waypoint
      WaypointManager::WaypointMap::size_type pNumWaypoints = WaypointManager::GetInstance()->GetWaypoints().size() - 1;
      unsigned pWaypointNum = dtUtil::RandRange(0U, unsigned(pNumWaypoints));      
      GoToWaypoint(pWaypointNum);           
   }
}


void TestAI::LoadDemoMap(const std::string& pStr)
{
   try
   {
      std::string contextName =  dtCore::GetDeltaRootPath()+"/data/demoMap";
      Project::GetInstance().SetContext(contextName);
      std::string pContext = Project::GetInstance().GetContext();
      Map &myMap = Project::GetInstance().GetMap(pStr);

      //Since we are in an Application we can simply call...
      LoadMap(myMap);
   }
   catch(const dtUtil::Exception& )
   {
      LOG_ERROR("ERROR: Map Not Found");
   }
}



bool TestAI::GoToWaypoint(int pWaypointNum)
{
   //loop through the waypoints and send our character to the one
   //whose index is pWaypointNum in the WaypointMap contained within WaypointManager
   const WaypointManager::WaypointMap& pWaypoints = WaypointManager::GetInstance()->GetWaypoints(); 
   WaypointManager::WaypointMap::const_iterator iter = pWaypoints.begin();
   WaypointManager::WaypointMap::const_iterator endOfMap = pWaypoints.end();

   bool pHasPath = false;

   int i = 0;
   while(iter != endOfMap)
   {
      if(i == pWaypointNum)
      {
         pHasPath = mCharacter->FindPathAndGoToWaypoint((*iter).second);
         if(pHasPath)
         {
            mCurrentWaypoint = (*iter).second;
            return true;
         }
         break;
      }

      ++i;   
      ++iter;
   }
   return false;
}



