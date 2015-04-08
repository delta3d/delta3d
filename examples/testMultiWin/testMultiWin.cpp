// TestMultiWin.cpp : defines the implementation of the application

#include "testMultiWin.h"
#include <dtCore/object.h>
#include <dtCore/camera.h>
#include <dtCore/scene.h>
#include <dtCore/deltawin.h>
#include <dtCore/transform.h>
#include <dtUtil/datapathutils.h>

#include <osgViewer/CompositeViewer>

using namespace dtCore;
using namespace dtABC;

// Default window sizing properties
const int DEFAULT_FIRST_WIN_X    = 10;
const int DEFAULT_FIRST_WIN_Y    = 30;
const int DEFAULT_SECOND_WIN_X   = 660;
const int DEFAULT_SECOND_WIN_Y   = 30;
const int DEFAULT_WIN_WIDTH      = 640;
const int DEFAULT_WIN_HEIGHT     = 480;
const float DEFAULT_ASPECT_RATIO = DEFAULT_WIN_WIDTH / (float)DEFAULT_WIN_HEIGHT;

TestMultiWin::TestMultiWin(const std::string& configFilename)
   : Application(configFilename)
{
}

TestMultiWin::~TestMultiWin()
{
}

void TestMultiWin::Config()
{
   //call the parent Config()
   dtABC::Application::Config();

   //setup scene here
   RefPtr<Object> terr = new Object();
   terr->LoadFile("StaticMeshes/terrain_simple.ive");
   GetScene()->AddChild(terr.get());

   //change the title of the pre-built Window
   //(this already has a Camera and Scene assignApped to it)
   GetWindow()->SetWindowTitle("testMultWin - Window 1");
   GetWindow()->SetPosition(DEFAULT_FIRST_WIN_X, DEFAULT_FIRST_WIN_Y, DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT);

   //create a new Window and Camera
   dtCore::DeltaWin::DeltaWinTraits win2Traits;
   win2Traits.name = "testMultWin - Window 2";
   win2Traits.x = DEFAULT_SECOND_WIN_X;
   win2Traits.y = DEFAULT_SECOND_WIN_Y;
   win2Traits.width = DEFAULT_WIN_WIDTH;
   win2Traits.height = DEFAULT_WIN_HEIGHT * 2;

   //needed for some machines and not other (still investigating why):
   win2Traits.realizeUponCreate = false;

   mWin2 = new DeltaWin(win2Traits);

   //set the first Camera position
   Transform transform(0.0f, -30.0f, 5.0f);
   GetCamera()->SetTransform(transform);
   GetCamera()->SetAspectRatio(DEFAULT_ASPECT_RATIO);

   //hook up a motion to the first camera, using the Application's Keyboard/Mouse
   mMotion1 = new OrbitMotionModel(GetKeyboard(), GetMouse());
   mMotion1->SetTarget(GetCamera());

   //Create second view, using the Application's Scene
   mView2 = new View("View 2");

   //use the default, pre-built Scene
   mView2->SetScene(GetScene());
   //create second Camera, added to second View, second Window
   mCam2 = new Camera("Camera 2");
   mCam2->SetWindow(mWin2.get());
   mCam2->GetOSGCamera()->setViewport(new osg::Viewport(0.0, 0.0, (float)DEFAULT_WIN_WIDTH, (float)DEFAULT_WIN_HEIGHT));
   mCam2->SetTransform(transform);
   mCam2->SetClearColor(1.0f, 0.0f, 0.0f, 1.0f);
   mCam2->SetAspectRatio(DEFAULT_ASPECT_RATIO);
   mView2->SetCamera(mCam2.get());

   AddView(*mView2);

   mMotion2 = new OrbitMotionModel(mView2->GetKeyboard(), mView2->GetMouse());
   mMotion2->SetTarget(mCam2.get());

   //Create a third View, using the Application's Scene
   mView3 = new View("View 3");

   //use the default, pre-built Scene
   mView3->SetScene(GetScene());
   //create a third Camera, added to third View, sharing the second Window
   mCam3 = new Camera("Camera 3");
   mCam3->SetWindow(mWin2.get());
   mCam3->GetOSGCamera()->setViewport(new osg::Viewport(0.0, 480.0, (float)DEFAULT_WIN_WIDTH, (float)DEFAULT_WIN_HEIGHT));
   mCam3->SetTransform(transform);
   mCam3->SetClearColor(0.0f, 1.0f, 0.0f, 1.0f);
   mCam3->SetAspectRatio(DEFAULT_ASPECT_RATIO);

   mView3->SetCamera(mCam3.get());
   AddView(*mView3);

   mMotion3 = new OrbitMotionModel(mView3->GetKeyboard(), mView3->GetMouse());
   mMotion3->SetTarget(mCam3.get());

   GetCompositeViewer()->setUpThreading();
}

bool TestMultiWin::KeyPressed(const dtCore::Keyboard* keyboard, int kc)
{
   bool handled = false;

   switch (kc)
   {
   case '~':
   case '`':
      SetNextStatisticsType();
      handled = true;
      break;

   default:
      handled = dtABC::Application::KeyPressed(keyboard, kc);
   }

   return handled;
}

int main()
{
   //set data search path to parent directory and delta3d/data
   dtUtil::SetDataFilePathList(dtUtil::GetDeltaRootPath() + "/examples/data;" + 
                               dtUtil::GetDeltaRootPath() + "/examples/testMultiWin/;");

   //Instantiate the application and look for the config file
   RefPtr<TestMultiWin> app = new TestMultiWin("config.xml");

   //configuring the application
   app->Config();

   // running the simulation loop
   app->Run();

   return 0;
}
