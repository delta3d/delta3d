#include "TestHUD.h"

#include <osg/Texture2D>

#include <dtCore/deltawin.h>
#include <dtCore/scene.h>
#include <dtCore/camera.h>
#include <dtCore/globals.h>
#include <dtCore/transform.h>
#include <dtDAL/project.h>
#include <dtDAL/actorproxy.h>
#include <dtDAL/map.h>

#include <CEGUI/CEGUIWindowManager.h>

TestHUD::TestHUD( const std::string& configFilename ) : Application( configFilename )
{
}

TestHUD::~TestHUD()
{
}

void TestHUD::Config()
{
   //configure defaults:
   dtABC::Application::Config();
   _ConfigScene();

   //configure window
   GetWindow()->ShowCursor(false);
   GetWindow()->SetWindowTitle("delta-window");

   //setup a hud:
   m_pHUD = new dtGUI::HUD(GetCamera(), GetKeyboard(), GetMouse());
   m_pHUD->LoadScheme("WindowsLook.scheme");
   m_pHUD->SetMouseCursor("WindowsLook", "MouseArrow");
   m_pHUD->LoadLayout("testHUD.layout");

   //connect hud-button to local method:
   m_pHUD->Connect("MyButton", "Clicked", &TestHUD::OnButtonClicked, this);

   //create a hud-camera which is rendering to a hud's (see above) widget:
   m_pCamera1 = new dtCore::Camera();

   //set the two cameras to the same position
   dtCore::Transform camPos;
   GetCamera()->GetTransform(camPos);
   m_pCamera1->SetTransform(camPos);

   osg::Camera *pOSGCamera = m_pCamera1->GetOSGCamera();

   //get osg-texture for the  hud-widget "camera1target":
   osg::Texture2D *pOSGTexture = m_pHUD->GetOrCreateOSGTexture("camera1target");

   //setup the hud-cam
   pOSGCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
   pOSGCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   pOSGCamera->setViewport(0,0,pOSGTexture->getTextureWidth(),pOSGTexture->getTextureHeight());
   pOSGCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
   pOSGCamera->setRenderOrder(osg::Camera::PRE_RENDER);
   pOSGCamera->attach(osg::Camera::COLOR_BUFFER, pOSGTexture);

   //hud-cam watches the default scene:
   m_pCamera1->GetOSGCamera()->addChild(GetScene()->GetSceneNode()); //watch default scene
   m_pCamera1->SetClearColor(1,0,0,1.0);
   GetCamera()->GetOSGCamera()->addChild(m_pCamera1->GetOSGCamera()); //run as hud-cam*/
}

bool TestHUD::OnButtonClicked(const CEGUI::EventArgs&) //non-static
{
   this->Quit();
   return true;
}

void TestHUD::_ConfigScene()
{
   std::string contextName = dtCore::GetDeltaRootPath() + "/examples/data/demoMap";
   dtDAL::Project::GetInstance().SetContext(contextName, true);
   dtDAL::Map &myMap = dtDAL::Project::GetInstance().GetMap("MyCoolMap");

   //Since we are in an Application we can simply call...
   LoadMap(myMap);

   // translate the camera to the predefined start position
   {
      std::vector< dtCore::RefPtr<dtDAL::ActorProxy> > proxies;
      myMap.FindProxies(proxies, "startPosition");
      if (!proxies.empty())
      {
         dtCore::Transformable *startPoint;
         proxies[0]->GetActor(startPoint);

         if (startPoint != NULL)
         {
            dtCore::Transform xform;
            startPoint->GetTransform(xform);
            GetCamera()->SetTransform(xform);
         }
      }
   }
}

int main()
{
   std::string dataPath = dtCore::GetDeltaDataPathList();
   dtCore::SetDataFilePathList(dataPath + ";" +
      dtCore::GetDeltaRootPath() + "/examples/data" + ";");

   //dtGUI::HUD::SetFilePath( dtCore::GetDeltaRootPath() + "/examples/data/cegui" );
   dtGUI::HUD::SetFilePath( dtCore::GetDeltaRootPath() + "/examples/data/gui" );

   dtCore::RefPtr<TestHUD> app = new TestHUD( "test.xml" );

   app->Config();
   app->Run();

   return 0;
}

