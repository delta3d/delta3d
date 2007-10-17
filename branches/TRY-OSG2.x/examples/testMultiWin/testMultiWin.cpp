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
   GetWindow()->SetPosition(0, 0, 640, 480);

   //create a new Window and Camera
   mWin2 = new DeltaWin("testMultWin - Window 2");
   mWin2->SetPosition(640, 0, 640, 480);

   Transform transform( 0.0f, 0.0f, 5.0f );
   GetCamera()->SetTransform( transform );


   mView2 = new View("View 2");
   mView2->SetScene( GetScene() ); //use the default, pre-built Scene
   this->AddView( *mView2 );
   
   mView3 = new View("View 3");
   mView3->SetScene( GetScene() ); //use the default, pre-built Scene
   this->AddView( *mView3 );
   
   
   //setup scene here
   RefPtr<Object> terr = new Object();
   terr->LoadFile("models/terrain_simple.ive");
   GetScene()->AddDrawable( terr.get() );
   
//   mCam2 = new Camera("Camera 2");
   mCam2 = mView2->GetOrCreateCamera();
   mCam2->SetWindow( mWin2.get() );
   mCam2->GetOsgCamera()->setViewport( new osg::Viewport(0.0, 0.0, 640.0, 240.0));
   mCam2->SetTransform( transform );
   mView2->SetCamera( mCam2.get() );
   
   mCam3 = mView3->GetOrCreateCamera();
   mView3->SetCamera( mCam3.get() );
   mCam3->SetWindow( mWin2.get() );
   mCam3->GetOsgCamera()->setViewport( new osg::Viewport(0.0, 240.0, 640.0, 480.0));
   mCam3->SetTransform( transform );
   
   
   
//   //setup scene here
//   RefPtr<Object> terr = new Object();
//   terr->LoadFile("models/terrain_simple.ive");
//   GetScene()->AddDrawable( terr.get() );

   // make sure any camera's which share the same render surface also share the same osg::State.
   // use a std::map to keep track of what render surfaces are associated with what state.
//   typedef std::map<Producer::RenderSurface*,osg::State*> RenderSurfaceStateMap;
//   RenderSurfaceStateMap _renderSurfaceStateMap;
//   unsigned int contextID = 0;

//   for( int i = 0; i < Camera::GetInstanceCount(); i++ )
//   {
//      Producer::Camera *cam = Camera::GetInstance(i)->GetCamera();
//      Producer::RenderSurface* rs = cam->getRenderSurface();
//
//      // get or create the scene handler.
//      Camera::_SceneHandler *sh;
//      sh = dynamic_cast<Camera::_SceneHandler*>(cam->getSceneHandler());
//      if(sh == NULL)
//         sh = new Camera::_SceneHandler();
//
//      osgUtil::SceneView* sv = sh->GetSceneView();
//
//      if (_renderSurfaceStateMap.count(rs)==0)
//      {
//         _renderSurfaceStateMap[rs] = sv->getState();
//         sv->getState()->setContextID(contextID++);
//      }
//      else
//      {
//         sv->setState(_renderSurfaceStateMap[rs]);
//      }
//   }
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
