/* -*-c++-*-
 * testGUI - testgui (.h & .cpp) - Using 'The MIT License'
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

//remove useless macros in X11 headers for PCH
#ifdef None
#undef None
#endif

#include <dtABC/application.h>
#include <dtCore/camera.h>
#include <dtCore/deltawin.h>
#include <dtCore/scene.h>
#include <dtCore/transform.h>
#include <dtGUI/scriptmodule.h>
#include <dtGUI/gui.h>
#include <dtUtil/log.h>
#include <dtUtil/datapathutils.h>

#include <CEGUI/CEGUIWindow.h>
#include <CEGUI/CEGUIExceptions.h>
#include <CEGUI/CEGUIDefaultResourceProvider.h>

#include <CEGUI/CEGUIImagesetManager.h>
#include <CEGUI/CEGUITexture.h>
#include <CEGUI/CEGUIPropertyHelper.h>
#include <CEGUI/RendererModules/OpenGL/CEGUIOpenGLTexture.h>
#include <osg/Texture2D>
#include <osgViewer/GraphicsWindow>
#include <dtDAL/project.h>
#include <dtDAL/map.h>

using namespace dtCore;
using namespace dtABC;
using namespace dtUtil;
using namespace dtGUI;


class TestGUIApp : public dtABC::Application
{
public:
   TestGUIApp(const std::string& configFilename = "")
      : Application(configFilename)
   {
   }

   virtual ~TestGUIApp()
   {
      CEGUI::ScriptModule* sm = CEGUI::System::getSingleton().getScriptingModule();
      if (sm)
      {
         CEGUI::System::getSingleton().setScriptingModule(NULL);
         delete sm;
      }
   }

   virtual void Config()
   {
      dtABC::Application::Config();

      std::string contextName = dtUtil::GetDeltaRootPath() + "/examples/data/demoMap";
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

      ///lets hide the stock cursor and just use CEGUI's rendered cursor
      GetWindow()->ShowCursor(false);

      mGUI = new dtGUI::GUI(GetCamera(), GetKeyboard(), GetMouse());

      ///make some cool UI
      BuildGUI();
   }

private:
   RefPtr<dtGUI::GUI> mGUI;
   RefPtr<dtCore::Camera> mRTTCamera;

   void BuildGUI(void)
   {
      try
      {
         mGUI->LoadScheme("WindowsLook.scheme");
         mGUI->SetMouseCursor("WindowsLook", "MouseArrow");

         CEGUI::Window* sheet = mGUI->GetRootSheet();

         //overwrite the default "layouts" directory search path to find the supplied layout filename
         CEGUI::DefaultResourceProvider* rp = dynamic_cast<CEGUI::DefaultResourceProvider*>
            (CEGUI::System::getSingleton().getResourceProvider());
         rp->setResourceGroupDirectory("layouts",  dtUtil::GetDeltaRootPath() + "/examples/testGUI");

         ///We'll make a new ScriptModule which will handle subscribing callbacks
         ///to widgets when it loads the Layout file.
         dtGUI::ScriptModule* sm = new dtGUI::ScriptModule();
         sm->AddCallback("quitHandler", &quitHandler);

         CEGUI::System::getSingleton().setScriptingModule(sm);

         //load GUI layout from file
         mGUI->LoadLayout("gui.layout");
      }

      // catch to prevent exit (errors will be logged).
      catch(CEGUI::Exception& e)
      {
         Log::GetInstance().LogMessage(Log::LOG_WARNING, __FUNCTION__,
            "CEGUI::%s", e.getMessage().c_str() );
      }

      //create a new CEGUI Texture and apply it to a loaded widget
      CEGUI::Window* w = mGUI->GetWidget("camera1target");
      CEGUI::Texture& texture = CEGUI::System::getSingleton().getRenderer()->createTexture(w->getPixelSize());
      CEGUI::Imageset& imageset = CEGUI::ImagesetManager::getSingleton().create("RTT", texture);
      imageset.defineImage("RTTImage", CEGUI::Point(0,0), texture.getSize(), CEGUI::Point(0,0) );
      w->setProperty("Image",  CEGUI::PropertyHelper::imageToString(&imageset.getImage("RTTImage")));

      // create/allocate/setup osg-texture-rendertarget
      osg::Texture2D* rttTexture = new osg::Texture2D();
      osgViewer::GraphicsWindow* gc = GetWindow()->GetOsgViewerGraphicsWindow();
      rttTexture->setTextureSize(w->getPixelSize().d_width, w->getPixelSize().d_height);
      rttTexture->setInternalFormat(GL_RGBA);
      rttTexture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
      rttTexture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
      rttTexture->apply(*gc->getState());

      //tell the CEGUI texture to use our OSG Texture
      GLuint textureID = rttTexture->getTextureObject(gc->getState()->getContextID())->_id;
      static_cast<CEGUI::OpenGLTexture&>(texture).setOpenGLTexture(textureID, w->getPixelSize());

      //make a new Camera to render the RTT
      mRTTCamera = new dtCore::Camera("RttCamera");
      dtCore::Transform xform;
      GetCamera()->GetTransform(xform);
      mRTTCamera->SetTransform(xform);

      osg::Camera* osgCam = mRTTCamera->GetOSGCamera();
      osgCam->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
      osgCam->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      osgCam->setViewport(0, 0, w->getPixelSize().d_width, w->getPixelSize().d_height);
      osgCam->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
      osgCam->setRenderOrder(osg::Camera::PRE_RENDER);
      osgCam->attach(osg::Camera::COLOR_BUFFER, rttTexture);

      osgCam->addChild(GetScene()->GetSceneNode());
      GetCamera()->GetOSGCamera()->addChild(osgCam);   
   }

   //quit!
   static bool quitHandler(const CEGUI::EventArgs& e)
   {
      dtABC::Application::GetInstance(0)->Quit();
      return true;
   }

};


int main(int argc, const char* argv[])
{
   //set data search path to find the required Delta3D files and the example data files
   std::string dataPath = dtUtil::GetDeltaDataPathList();
   dtUtil::SetDataFilePathList(
                               dtUtil::GetDeltaRootPath() + "/examples/data;" +
                               dtUtil::GetDeltaRootPath() + "/examples/data/gui/;" +
                               dtUtil::GetDeltaRootPath() + "/examples/testGUI;" +
                               dataPath + ";");

   //Instantiate the application and look for the config file
   RefPtr<TestGUIApp> app = new TestGUIApp("config.xml");

   app->Config(); //configuring the application
   app->Run(); // running the simulation loop

   return 0;
}
