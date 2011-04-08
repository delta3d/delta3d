// TestMultiWin.cpp : defines the implementation of the application

#include "testMultiWin.h"
#include <dtCore/object.h>
#include <dtCore/globals.h>
#include <dtCore/camera.h>
#include <dtCore/scene.h>

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
   //(this already has a Camera and Scene assigned to it)
   GetWindow()->SetWindowTitle("testMultWin - Window 1");
   GetWindow()->SetPosition(0, 0, 640, 480);

   //create a new Window and Camera
   mWin2 = new DeltaWin("testMultWin - Window 2");
   mWin2->SetPosition(640, 0, 640, 480);

   Transform transform( 0.0f, 0.0f, 5.0f );

   GetCamera()->SetTransform( &transform );

   mCam2 = new Camera("Camera 2");
   mCam2->SetWindow( mWin2.get() );
   mCam2->SetScene( GetScene() ); //use the default, pre-built Scene
   mCam2->GetCamera()->setProjectionRectangle (0.0f, 1.0f, 0.5f, 1.0f);
   mCam2->SetTransform( &transform );

   mCam3 = new Camera("Camera 3");
   mCam3->SetWindow( mWin2.get() );
   mCam3->SetScene( GetScene() );
   mCam3->GetCamera()->setProjectionRectangle (0.0f, 1.0f, 0.0f, 0.5f);
   mCam3->SetTransform( &transform );

   //setup scene here
   RefPtr<Object> terr = new Object();
   terr->LoadFile("models/terrain_simple.ive");
   GetScene()->AddDrawable( terr.get() );

   // make sure any camera's which share the same render surface also share the same osg::State.
   // use a std::map to keep track of what render surfaces are associated with what state.
   typedef std::map<Producer::RenderSurface*,osg::State*> RenderSurfaceStateMap;
   RenderSurfaceStateMap _renderSurfaceStateMap;
   unsigned int contextID = 0;

   for( int i = 0; i < Camera::GetInstanceCount(); i++ )
   {
      Producer::Camera *cam = Camera::GetInstance(i)->GetCamera();
      Producer::RenderSurface* rs = cam->getRenderSurface();

      // get or create the scene handler.
      Camera::_SceneHandler *sh;
      sh = dynamic_cast<Camera::_SceneHandler*>(cam->getSceneHandler());
      if(sh == NULL)
         sh = new Camera::_SceneHandler();

      osgUtil::SceneView* sv = sh->GetSceneView();

      if (_renderSurfaceStateMap.count(rs)==0)
      {
         _renderSurfaceStateMap[rs] = sv->getState();
         sv->getState()->setContextID(contextID++);
      }
      else
      {
         sv->setState(_renderSurfaceStateMap[rs]);
      }
   }
}

int main()
{
   //set data search path to parent directory and delta3d/data
   SetDataFilePathList( GetDeltaRootPath() + "/examples/testMultiWin/;" +
                        GetDeltaDataPathList()  );

   //Instantiate the application and look for the config file
   RefPtr<TestMultiWin> app = new TestMultiWin( "config.xml" );

   app->Config(); //configuring the application
   app->Run(); // running the simulation loop

   return 0;
}