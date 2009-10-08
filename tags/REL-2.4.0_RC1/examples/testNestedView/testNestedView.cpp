// TestNestedView.cpp : defines the implementation of the application

#include "testNestedView.h"
#include <dtCore/object.h>
#include <dtCore/globals.h>
#include <dtCore/camera.h>
#include <dtCore/scene.h>
#include <dtCore/deltawin.h>

#include <osgViewer/CompositeViewer>

#include <iostream>

using namespace dtCore;
using namespace dtABC;

// Default window sizing properties
const int DEFAULT_WIN_X   = 60;
const int DEFAULT_WIN_Y   = 30;
const int DEFAULT_VIEW_WIDTH      = 640;
const int DEFAULT_VIEW_HEIGHT     = 480;
const float DEFAULT_ASPECT_RATIO = DEFAULT_VIEW_WIDTH / (float)DEFAULT_VIEW_HEIGHT;

TestNestedView::TestNestedView(const std::string& configFilename)
: Application(configFilename)
{
}

TestNestedView::~TestNestedView()
{
}

void TestNestedView::Config()
{
   //call the parent Config()
   dtABC::Application::Config();

   //change the title of the pre-built Window 
   //(this already has a Camera and Scene assignApped to it)
   GetWindow()->SetWindowTitle("testNestedView");
   GetWindow()->SetPosition(DEFAULT_WIN_X, DEFAULT_WIN_Y, 
                            DEFAULT_VIEW_WIDTH + DEFAULT_VIEW_WIDTH / 2,
                            DEFAULT_VIEW_HEIGHT + DEFAULT_VIEW_HEIGHT / 2);

   //set the first Camera position
   GetCamera()->SetAspectRatio(DEFAULT_ASPECT_RATIO);
   GetCamera()->GetOSGCamera()->setViewport(new osg::Viewport((float) DEFAULT_VIEW_WIDTH / 4.0f, 0.0f,   
                                                              (float)DEFAULT_VIEW_WIDTH, (float)DEFAULT_VIEW_HEIGHT));
   GetCamera()->SetClearColor(1.0f, 0.0f, 0.0f, 1.0f);
   
   //Default view frame bin
   mRedView = GetView();
   mRedView->SetRenderOrder(0);

   //Create second view, using the Application's Scene
   mGreenView = new View("View 2");

   //use the default, pre-built Scene
   mGreenView->SetScene(GetScene());
   this->AddView(*mGreenView);

   //create second Camera, added to second View
   mCam2 = new Camera("Camera 2");
   mCam2->SetWindow(GetWindow());   
   mCam2->SetClearColor(0.f, 1.0f, 0.f, 1.f);
   mCam2->SetAspectRatio(DEFAULT_ASPECT_RATIO);
   mCam2->GetOSGCamera()->setViewport(new osg::Viewport((float) DEFAULT_VIEW_WIDTH / 2.0f, (float) DEFAULT_VIEW_HEIGHT / 4.0f,
                                     (float)DEFAULT_VIEW_WIDTH, (float)DEFAULT_VIEW_HEIGHT));
   mGreenView->SetCamera(mCam2.get());
   
   //2nd view frame bin
   mGreenView->SetRenderOrder(1);

   //Create a third View, using the Application's Scene
   mBlueView = new View("View 3");

   //use the default, pre-built Scene
   mBlueView->SetScene(GetScene());
   this->AddView(*mBlueView);

   //create a third Camera, added to third View, sharing the second Window
   mCam3 = new Camera("Camera 3");
   mBlueView->SetCamera(mCam3.get());
   mCam3->SetWindow(GetWindow());
   mCam3->GetOSGCamera()->setViewport(new osg::Viewport(0.0, (float) DEFAULT_VIEW_HEIGHT / 2.0f,
                                     (float) DEFAULT_VIEW_WIDTH, (float) DEFAULT_VIEW_HEIGHT));
   mCam3->SetClearColor(0.f, 0.f, 1.0f, 1.f);
   mCam3->SetAspectRatio(DEFAULT_ASPECT_RATIO);

   //3rd view frame bin
   mBlueView->SetRenderOrder(2);   
}

bool TestNestedView::KeyPressed(const dtCore::Keyboard* keyboard, int key)
{
   // pass the keyboard event to the overlords
   bool handled = Application::KeyPressed(keyboard, key);
   if (handled)  /// == true
   {
      return handled;
   }

   if (mRedView->GetRenderOrder() == 0)
   {
      mRedView->SetRenderOrder(1);
      mGreenView->SetRenderOrder(2);
      mBlueView->SetRenderOrder(0);
      std::cout << "\nBlue on bottom, Red next, Green on top.\n";
   }
   else if (mRedView->GetRenderOrder() == 1)
   {
      mRedView->SetRenderOrder(2);
      mGreenView->SetRenderOrder(0);
      mBlueView->SetRenderOrder(1);

      std::cout << "\nGreen on bottom, Blue next, Red on top.\n";
   }
   else
   {
      mRedView->SetRenderOrder(0);
      mGreenView->SetRenderOrder(1);
      mBlueView->SetRenderOrder(2);

      std::cout << "\nRed on bottom, Green next, Blue on top.\n";
   }

   return true;
}

int main(int argc, char **argv)
{
   //set data search path to parent directory and delta3d/data
   dtCore::SetDataFilePathList(dtCore::GetDeltaRootPath() + "/examples/data" + ";");

   //Instantiate the application and look for the config file
   RefPtr<TestNestedView> app = new TestNestedView("config.xml");

   //configuring the application
   app->Config();   
   
   std::cout << "\nPush any key to cycle which view is 'on top.'\n\n";
   std::cout << "Push Escape to exit app.\n";

   // running the simulation loop
   app->Run();

   return 0;
}
