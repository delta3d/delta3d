// TestAI.cpp : defines the implementation of the application
#if defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
   #pragma warning(push)
   #pragma warning(disable : 4005)
#endif

#include "testai.h"
#include <dtCore/camera.h>
#include <dtCore/globals.h>
#include <dtDAL/map.h>
#include <dtDAL/project.h>
#include <dtDAL/exceptionenum.h>
#include <dtUtil/macros.h>

#include <dtAI/waypoint.h>
#include <dtAI/waypointmanager.h>


#include <dtUtil/mathdefines.h>
#include <dtUtil/log.h>
#include <dtUtil/fileutils.h>
#include <osg/MatrixTransform>

#if defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
   #pragma warning(pop)
#endif

using namespace dtCore;
using namespace dtABC;
using namespace dtDAL;
using namespace dtAI;


TestAI::TestAI(const std::string& pMapFilename, const std::string& configFilename)
   : Application(configFilename)
   , mMapFilename(pMapFilename)
   , mCharacter(0)
{
   //Generating a default config file if there isn't one already
   if( !dtUtil::FileUtils::GetInstance().FileExists( configFilename ) ) 
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
   GetScene()->AddDrawable(&WaypointManager::GetInstance());

   LoadDemoMap(mMapFilename);

   //turn on viewing of waypoints
   WaypointManager::GetInstance().SetDrawWaypoints(true);

   //by default we wont draw the navmesh
   mDrawNavMesh = false;
   WaypointManager::GetInstance().SetDrawNavMesh(mDrawNavMesh, true);         

   //set camera offset
   dtCore::Transform trans;
   trans.SetTranslation(-1.0f, 5.5f, 1.5f);
   trans.SetRotation(180.0f, -2.0f, 0.0f);
   GetCamera()->SetTransform(trans);

   //this is needed so OSG knows which camera can generate events
   GetCamera()->GetOSGCamera()->setAllowEventFocus(true);

   //create overhead camera and disable it by default   
   dtCore::View *overheadView = new dtCore::View("overhead view");
   overheadView->SetScene( GetScene() );
   AddView( *overheadView );

   mOverheadCamera = new dtCore::Camera();
   mOverheadCamera->SetWindow(GetWindow());
   mOverheadCamera->SetEnabled(false);
   overheadView->SetCamera( mOverheadCamera.get() );

   //this is needed so OSG knows which camera can generate events
   mOverheadCamera->GetOSGCamera()->setAllowEventFocus(false);

   //set overhead camera offset
   trans.SetTranslation(0.0f, -5.0f, 70.0f);
   trans.SetRotation(0.0f, -90.f, 0.0f);
   trans.Get(mCameraOffset);
   mOverheadCamera->GetMatrixNode()->setMatrix(mCameraOffset);

   //get the first waypoint to spawn the character at
   const WaypointManager::WaypointMap& pContainer = WaypointManager::GetInstance().GetWaypoints();
   if(pContainer.empty())
   {
      LOG_ERROR("Map '" + mMapFilename + "' does not have any valid waypoints");
      exit(1);
   }
   WaypointManager::WaypointMap::const_iterator iter = pContainer.begin();
   const Waypoint* pWaypoint = (*iter).second;

   //spawn our character
	mCharacter = new dtAI::AICharacter(GetScene(), pWaypoint, "demoMap/SkeletalMeshes/marine.xml", 3);    

   //add the two Cameras as children so they get moved along with the character
   mCharacter->GetCharacter()->AddChild( GetCamera() );
   mCharacter->GetCharacter()->AddChild( mOverheadCamera.get() );

   GoToWaypoint(1);

   //seed the random generator
   srand(4);
}

bool TestAI::KeyPressed(const dtCore::Keyboard* keyboard, int key)
{
  
   switch( key )
   {
      case ' ':
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

      case 'n':
         {            
            mDrawNavMesh = !mDrawNavMesh;
            WaypointManager::GetInstance().SetDrawNavMesh(mDrawNavMesh, true);            
            return true;
         }

      case 'a':
         {
            if(mOverheadCamera->GetEnabled())
            {
               mCameraOffset(3,2) -= 1.0f;
               mCameraOffset(3,1) -= 15.5f / 100.0f;
               mOverheadCamera->GetMatrixNode()->setMatrix(mCameraOffset);
            }
            return true;
         }

      case 'z':
         {
            if(mOverheadCamera->GetEnabled())
            {
               mCameraOffset(3,2) += 1.0f;
               mCameraOffset(3,1) += 15.5f / 100.0f;
               mOverheadCamera->GetMatrixNode()->setMatrix(mCameraOffset);
            }
            return true;
         }

      case osgGA::GUIEventAdapter::KEY_Escape:
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
   mCharacter->Update(float(deltaFrameTime));

   if(mCharacter->GetCurrentWaypoint() == mCurrentWaypoint)
   {
      //send the character to a random waypoint
      WaypointManager::WaypointMap::size_type pNumWaypoints = WaypointManager::GetInstance().GetWaypoints().size() - 1;
      unsigned pWaypointNum = dtUtil::RandRange(0U, unsigned(pNumWaypoints));      
      GoToWaypoint(pWaypointNum);           
   }
}


void TestAI::LoadDemoMap(const std::string& pStr)
{
   try
   {
      std::string contextName =  dtCore::GetDeltaRootPath()+"/examples/data/demoMap";
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
   const WaypointManager::WaypointMap& pWaypoints = WaypointManager::GetInstance().GetWaypoints(); 
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



