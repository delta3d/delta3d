// TestMultiWin.cpp : defines the implementation of the application

#include "testMultiWin.h"
#include <dtCore/object.h>
#include <dtCore/globals.h>
#include <dtCore/camera.h>
#include <dtCore/scene.h>
#include <dtCore/deltawin.h>

#include <osgViewer/CompositeViewer>

using namespace dtCore;
using namespace dtABC;

TestMultiWin::TestMultiWin( const std::string& configFilename )
: Application( configFilename )
{
}

TestMultiWin::~TestMultiWin()
{
}

void TestMultiWin::Config()
{
   //call the parent Config()
   dtABC::Application::Config();

   //change the title of the pre-built Window 
   //(this already has a Camera and Scene assignApped to it)
   GetWindow()->SetWindowTitle("testMultWin - Window 1");
   GetWindow()->SetPosition(10, 30, 640, 480);

   //create a new Window and Camera
   mWin2 = new DeltaWin("testMultWin - Window 2");
   mWin2->SetPosition(660, 30, 640, 960);

   //set the first Camera position
   Transform transform( 0.0f, -30.0f, 5.0f );
   GetCamera()->SetTransform( transform );
   GetCamera()->SetAspectRatio(1.0);

   //hook up a motion to the first camera, using the Application's Keyboard/Mouse
   mMotion1 = new OrbitMotionModel(GetKeyboard(), GetMouse());
   mMotion1->SetTarget(GetCamera());

   //Create second view, using the Application's Scene
   mView2 = new View("View 2");
   mView2->SetScene( GetScene() ); //use the default, pre-built Scene
   this->AddView( *mView2 );

   //create second Camera, added to second View, second Window
   mCam2 = new Camera("Camera 2");
   mCam2->SetWindow( mWin2.get() );
   mCam2->GetOSGCamera()->setViewport( new osg::Viewport(0.0, 0.0, 640.0, 480.0));
   mCam2->SetTransform( transform );
   mCam2->SetClearColor(1.f, 0.f, 0.f, 1.f);
   mCam2->SetAspectRatio(1.0);
   mView2->SetCamera( mCam2.get() );
   mMotion2 = new OrbitMotionModel(mView2->GetKeyboard(), mView2->GetMouse());
   mMotion2->SetTarget(mCam2.get());

   //Create a third View, using the Application's Scene
   mView3 = new View("View 3");
   mView3->SetScene( GetScene() ); //use the default, pre-built Scene
   this->AddView( *mView3 );

   //create a third Camera, added to third View, sharing the second Window
   mCam3 = new Camera("Camera 3");
   mView3->SetCamera( mCam3.get() );
   mCam3->SetWindow( mWin2.get() );
   mCam3->GetOSGCamera()->setViewport( new osg::Viewport(0.0, 480.0, 640.0, 480.0));
   mCam3->SetTransform( transform );
   mCam3->SetClearColor(0.f, 1.f, 0.f, 1.f);
   mCam3->SetAspectRatio(1.0);
   mMotion3 = new OrbitMotionModel(mView3->GetKeyboard(), mView3->GetMouse());
   mMotion3->SetTarget(mCam3.get());
   
   //setup scene here
   RefPtr<Object> terr = new Object();
   terr->LoadFile("models/terrain_simple.ive");
   GetScene()->AddDrawable( terr.get() );
}

int main()
{
   //set data search path to parent directory and delta3d/data
   dtCore::SetDataFilePathList( dtCore::GetDeltaRootPath() + "/examples/data" + ";");

   //Instantiate the application and look for the config file
   RefPtr<TestMultiWin> app = new TestMultiWin( "config.xml" );

   app->Config(); //configuring the application
   app->Run(); // running the simulation loop

   return 0;
}
