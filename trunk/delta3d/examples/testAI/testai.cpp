// TestAI.cpp : defines the implementation of the application
#include <dtUtil/mswinmacros.h>
#ifdef DELTA_WIN32
   #pragma warning(push)
   #pragma warning(disable : 4005)
#endif

#include "testai.h"
#include <dtCore/camera.h>
#include <dtCore/transform.h>
#include <dtCore/map.h>
#include <dtCore/project.h>
#include <dtCore/exceptionenum.h>
#include <dtUtil/datapathutils.h>

#include <dtAI/waypoint.h>
#include <dtAI/waypointmanager.h>

#include <dtUtil/mathdefines.h>
#include <dtUtil/log.h>
#include <dtUtil/fileutils.h>
#include <osg/MatrixTransform>

#ifdef DELTA_WIN32
   #pragma warning(pop)
#endif

using namespace dtCore;
using namespace dtABC;
using namespace dtCore;
using namespace dtAI;

////////////////////////////////////////////////////////////////////////////////
TestAI::TestAI(const std::string& pMapFilename, const std::string& configFilename)
   : Application(configFilename)
   , mDrawNavMesh()
   , mMapFilename(pMapFilename)
{
   // Generating a default config file if there isn't one already
   if (!dtUtil::FileUtils::GetInstance().FileExists(configFilename))
   {
      GenerateDefaultConfigFile();
   }
}

////////////////////////////////////////////////////////////////////////////////
TestAI::~TestAI()
{
   // Clean up so that so that dead references won't get called "atexit"
   dtCore::Project::GetInstance().CloseAllMaps(true);
}

////////////////////////////////////////////////////////////////////////////////
void TestAI::Config()
{
   Application::Config();

   // initialize the Waypoint Helper
   WaypointManager::GetInstance();
   GetScene()->AddChild(&WaypointManager::GetInstance());

   // set the waypoint manager to not (redundantly) delete waypoints
   dtAI::WaypointManager::GetInstance().SetDeleteOnClear(false);

   try
   {
      std::string contextName =  dtUtil::GetDeltaRootPath()+"/examples/data";
      Project::GetInstance().SetContext(contextName);
   }
   catch (const dtUtil::Exception& e)
   {
      e.LogException();
   }

   LoadDemoMap(mMapFilename);

   // turn on viewing of waypoints
   WaypointManager::GetInstance().SetDrawWaypoints(true);

   // by default we wont draw the navmesh
   mDrawNavMesh = false;
   WaypointManager::GetInstance().SetDrawNavMesh(mDrawNavMesh, true);

   // set camera offset
   dtCore::Transform trans;
   trans.SetTranslation(-1.0f, 5.5f, 1.5f);
   trans.SetRotation(180.0f, -2.0f, 0.0f);
   GetCamera()->SetTransform(trans);

   // this is needed so OSG knows which camera can generate events
   GetCamera()->GetOSGCamera()->setAllowEventFocus(true);

   // create overhead camera and disable it by default
   dtCore::View *overheadView = new dtCore::View("overhead view");
   overheadView->SetScene( GetScene() );
   AddView( *overheadView );

   mOverheadCamera = new dtCore::Camera();
   mOverheadCamera->SetWindow(GetWindow());
   mOverheadCamera->SetEnabled(false);
   overheadView->SetCamera( mOverheadCamera.get() );

   // this is needed so OSG knows which camera can generate events
   mOverheadCamera->GetOSGCamera()->setAllowEventFocus(false);

   // set overhead camera offset
   trans.SetTranslation(0.0f, -5.0f, 70.0f);
   trans.SetRotation(0.0f, -90.0f, 0.0f);
   trans.Get(mCameraOffset);
   mOverheadCamera->GetMatrixNode()->setMatrix(mCameraOffset);

   // get the first waypoint to spawn the character at
   const WaypointManager::WaypointMap& pContainer = WaypointManager::GetInstance().GetWaypoints();
   if (pContainer.empty())
   {
      LOG_ERROR("Map '" + mMapFilename + "' does not have any valid waypoints");
      exit(1);
   }
   WaypointManager::WaypointMap::const_iterator iter = pContainer.begin();
   const Waypoint* pWaypoint = (*iter).second;

   dtCore::ResourceDescriptor characterFile("SkeletalMeshes:Marine:marine.xml");

//   // spawn our character
   mCharacter = new dtAI::AICharacter(GetScene(), pWaypoint, characterFile, 3);
//
//   // add the two Cameras as children so they get moved along with the character
//   mCharacter->GetCharacter()->AddChild(GetCamera());
//   mCharacter->GetCharacter()->AddChild(mOverheadCamera.get());

   GoToWaypoint(1);

   // seed the random generator
   srand(4);

   CreateHelpLabel();
}

////////////////////////////////////////////////////////////////////////////////
bool TestAI::KeyPressed(const dtCore::Keyboard* keyboard, int key)
{
   switch (key)
   {
   case ' ':
      if (GetCamera()->GetEnabled())
      {
         GetCamera()->SetEnabled(false);
         mOverheadCamera->SetEnabled(true);
         if (mLabel->GetActive())
         {
            GetCamera()->RemoveChild(mLabel.get());
            mOverheadCamera->AddChild(mLabel.get());
         }
      }
      else
      {
         mOverheadCamera->SetEnabled(false);
         GetCamera()->SetEnabled(true);
         if (mLabel->GetActive())
         {
            mOverheadCamera->RemoveChild(mLabel.get());
            GetCamera()->AddChild(mLabel.get());
         }
      }
      return true;

   case 'n':
      mDrawNavMesh = !mDrawNavMesh;
      WaypointManager::GetInstance().SetDrawNavMesh(mDrawNavMesh, true);
      return true;

   case 'a':
      if (mOverheadCamera->GetEnabled())
      {
         mCameraOffset(3,2) -= 1.0f;
         mCameraOffset(3,1) -= 15.5f / 100.0f;
         mOverheadCamera->GetMatrixNode()->setMatrix(mCameraOffset);
      }
      return true;

   case 'z':
      if (mOverheadCamera->GetEnabled())
      {
         mCameraOffset(3,2) += 1.0f;
         mCameraOffset(3,1) += 15.5f / 100.0f;
         mOverheadCamera->GetMatrixNode()->setMatrix(mCameraOffset);
      }
      return true;

   case osgGA::GUIEventAdapter::KEY_Escape:
      Quit();
      return true;

   case osgGA::GUIEventAdapter::KEY_F1:
      mLabel->SetActive(!mLabel->GetActive());
      if (mLabel->GetActive())
      {
         if (GetCamera()->GetEnabled() &&
            GetCamera()->GetChildIndex(mLabel.get()) == GetCamera()->GetNumChildren())
         {
            mOverheadCamera->RemoveChild(mLabel.get());
            GetCamera()->AddChild(mLabel.get());
         }
         else if (mOverheadCamera->GetEnabled() &&
            mOverheadCamera->GetChildIndex(mLabel.get()) == mOverheadCamera->GetNumChildren())
         {
            GetCamera()->RemoveChild(mLabel.get());
            mOverheadCamera->AddChild(mLabel.get());
         }
      }
      return true;

   default:
      break;
   }
   return false;
}

////////////////////////////////////////////////////////////////////////////////
void TestAI::PreFrame(const double deltaFrameTime)
{
   mCharacter->Update(float(deltaFrameTime));

   if (mCharacter->GetCurrentWaypoint() == mCurrentWaypoint)
   {
      // send the character to a random waypoint
      WaypointManager::WaypointMap::size_type pNumWaypoints = WaypointManager::GetInstance().GetWaypoints().size() - 1;
      unsigned pWaypointNum = dtUtil::RandRange(0U, unsigned(pNumWaypoints));
      GoToWaypoint(pWaypointNum);
   }
}

////////////////////////////////////////////////////////////////////////////////
void TestAI::LoadDemoMap(const std::string& pStr)
{
   try
   {
      std::string pContext = Project::GetInstance().GetContext();
      Map& myMap = Project::GetInstance().GetMap(pStr);

      // Since we are in an Application we can simply call...
      LoadMap(myMap);
   }
   catch (const dtUtil::Exception&)
   {
      LOG_ERROR("ERROR: Map Not Found");
   }
}

////////////////////////////////////////////////////////////////////////////////
bool TestAI::GoToWaypoint(int pWaypointNum)
{
   // loop through the waypoints and send our character to the one
   // whose index is pWaypointNum in the WaypointMap contained within WaypointManager
   const WaypointManager::WaypointMap& pWaypoints = WaypointManager::GetInstance().GetWaypoints();
   WaypointManager::WaypointMap::const_iterator iter = pWaypoints.begin();
   WaypointManager::WaypointMap::const_iterator endOfMap = pWaypoints.end();

   bool pHasPath = false;

   for (int i = 0; iter != endOfMap; ++i, ++iter)
   {
      if (i == pWaypointNum)
      {
         pHasPath = mCharacter->FindPathAndGoToWaypoint((*iter).second);
         if (pHasPath)
         {
            mCurrentWaypoint = (*iter).second;
            return true;
         }
         break;
      }
   }
   return false;
}

////////////////////////////////////////////////////////////////////////////////
void TestAI::CreateHelpLabel()
{
   mLabel = new dtABC::LabelActor();
   osg::Vec2 testSize(32.0f, 5.5f);
   mLabel->SetBackSize(testSize);
   mLabel->SetFontSize(0.8f);
   mLabel->SetTextAlignment(dtABC::LabelActor::AlignmentEnum::LEFT_CENTER);
   mLabel->SetText(CreateHelpLabelText());
   mLabel->SetEnableDepthTesting(false);
   mLabel->SetEnableLighting(false);

   GetCamera()->AddChild(mLabel.get());
   dtCore::Transform labelOffset(-17.0f, 50.0f, 10.5f, 0.0f, 90.0f, 0.0f);
   mLabel->SetTransform(labelOffset, dtCore::Transformable::REL_CS);
   AddDrawable(GetCamera());
}

////////////////////////////////////////////////////////////////////////////////
std::string TestAI::CreateHelpLabelText()
{
   std::string testString("");
   testString += "F1: Toggle Help Screen\n";
   testString += "\n";
   testString += "Space: Toggle overhead/first person camera\n";
   testString += "n: Toggle navmesh\n";
   testString += "a: Zoom in overhead camera\n";
   testString += "z: Zoom out overhead camera\n";

   return testString;
}
