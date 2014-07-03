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
#include <osgViewer/View>
#include <dtCore/project.h>
#include <dtCore/map.h>

using namespace dtCore;
using namespace dtABC;
using namespace dtUtil;
using namespace dtGUI;


class TestGUIApp : public dtABC::Application
{
public:

   /////////////////////////////////////////////////////////////////////////////
   TestGUIApp(const std::string& configFilename = "")
      : Application(configFilename)
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   virtual ~TestGUIApp()
   {
      CEGUI::ScriptModule* sm = CEGUI::System::getSingleton().getScriptingModule();
      if (sm)
      {
         CEGUI::System::getSingleton().setScriptingModule(NULL);
         delete sm;
      }

      dtCore::Project::GetInstance().CloseAllMaps(true);
   }

   /////////////////////////////////////////////////////////////////////////////
   virtual void Config()
   {
      dtABC::Application::Config();

      std::string contextName = dtUtil::GetDeltaRootPath() + "/examples/data";
      dtCore::Project::GetInstance().SetContext(contextName, true);
      dtCore::Map& myMap = dtCore::Project::GetInstance().GetMap("MyCoolMap");

      //Since we are in an Application we can simply call...
      LoadMap(myMap);

      // translate the camera to the predefined start position
      {
         std::vector< dtCore::RefPtr<dtCore::BaseActorObject> > proxies;
         myMap.FindProxies(proxies, "startPosition");
         if (!proxies.empty())
         {
            dtCore::Transformable *startPoint;
            proxies[0]->GetDrawable(startPoint);

            if (startPoint != NULL)
            {
               dtCore::Transform xform;
               startPoint->GetTransform(xform);
               GetCamera()->SetTransform(xform);
            }
         }
      }

      ///lets hide the stock cursor and just use CEGUI's rendered cursor
      GetWindow()->SetShowCursor(false);

      mGUI = new dtGUI::GUI(GetCamera(), GetKeyboard(), GetMouse());

      ///make some cool UI
      BuildGUI();
   }

private:
   RefPtr<dtGUI::GUI> mGUI;
   RefPtr<dtCore::Camera> mRTTCamera;

   /////////////////////////////////////////////////////////////////////////////
   void BuildGUI(void)
   {
      try
      {
         dtGUI::GUI::SetDefaultParser("XercesParser");

         mGUI->LoadScheme("WindowsLook.scheme");
         mGUI->SetMouseCursor("WindowsLook", "MouseArrow");

         ///We'll make a new ScriptModule which will handle subscribing callbacks
         ///to widgets when it loads the Layout file.
         dtGUI::ScriptModule* sm = new dtGUI::ScriptModule();
         sm->AddCallback("quitHandler", CEGUI::SubscriberSlot(&TestGUIApp::quitHandler, this));

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

      CEGUI::Window* w = mGUI->GetWidget("camera1target");
      dtCore::RefPtr<osg::Texture2D> rttTex = mGUI->CreateRenderTargetTexture(*w, NULL, "RTT", "Image", "RTTImage");
      osg::Vec2 viewDims(w->getPixelSize().d_width, w->getPixelSize().d_height);
      mRTTCamera = mGUI->CreateCameraForRenderTargetTexture(*rttTex, viewDims);

      dtCore::View *rttView = new dtCore::View("rttView");
      rttView->SetCamera(mRTTCamera.get());
      rttView->SetScene(GetScene());
      AddView(*rttView);

      dtCore::Transform xform;
      GetCamera()->GetTransform(xform);
      mRTTCamera->SetTransform(xform);
   }

   /////////////////////////////////////////////////////////////////////////////
   bool quitHandler(const CEGUI::EventArgs& e)
   {
      dtABC::Application::GetInstance(0)->Quit();
      return true;
   }

protected:
   /////////////////////////////////////////////////////////////////////////////
   virtual void PreFrame(const double deltaSimTime)
   { 
      // staticImage is unfortenately static and must be invalidated every frame to show animated models
      //TODO: if RTT-Scenes are added the imageWindow(s) should updated by/inside CEGUIDrawable
      CEGUI::Window* imageWindow = mGUI->GetWidget("camera1target");
      if (imageWindow) {imageWindow->invalidate();}
   }
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main(int argc, const char* argv[])
{
   //set data search path to find the required Delta3D files and the example data files
   std::string dataPath = dtUtil::GetDeltaDataPathList();
   dtUtil::SetDataFilePathList(
                               dtUtil::GetDeltaRootPath() + "/examples/data;" +
                               dtUtil::GetDeltaRootPath() + "/examples/testGUI;" +
                               dataPath + ";");

   //Instantiate the application and look for the config file
   RefPtr<TestGUIApp> app = new TestGUIApp("config.xml");

   app->Config(); //configuring the application
   app->Run(); // running the simulation loop

   return 0;
}
