#include "dtGUI/dtgui.h"
#include "dtCore/dt.h"
#include "dtABC/dtabc.h"

#include <CEGUI/CEGUI.h>
#include <CEGUI/CEGUIPropertySet.h>


using namespace dtCore;
using namespace dtABC;
using namespace std;



class TestGUIApp : public dtABC::Application
{
public:
   TestGUIApp( std::string configFilename = "" ):
      Application( configFilename )
      {
      }

   ~TestGUIApp()
   {
   }

   virtual void Config()
   {
      SetDataFilePathList( "..;" + GetDeltaDataPathList() + ";" +
                           (GetDeltaDataPathList()+"/gui/;") );

      dtABC::Application::Config();

      GetWindow()->SetWindowTitle("TestCEGApp");

      //setup scene here
      ///put something in the background to look at
      RefPtr<Object> helo = new Object( "Helo" );
      helo->LoadFile( "models/uh-1n.ive" );
      AddDrawable( helo.get() );

      ///move the camera up
      Transform xform(-30.0f, 0.0f, 0.0f, -90.0f, 0.0f, 0.0f );
      GetCamera()->SetTransform( &xform );
      GetCamera()->SetClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

      int x,y,w,h;
      GetWindow()->GetPosition(&x, &y, &w, &h);
      GetWindow()->ShowCursor( false );
      mGUI = new dtGUI::CEUIDrawable(w, h);

      BuildGUI();

      GetScene()->AddDrawable(mGUI.get());
   }

   virtual void KeyPressed( dtCore::Keyboard *keyboard, 
                            Producer::KeyboardKey key,
                            Producer::KeyCharacter character )
   {
      switch( key )
      {
      case Producer::Key_Escape: Quit(); break;
        
      default:  break;       
      }
   }


   void SetLayoutFilename(std::string filename)
   {
      mLayoutFilename = filename;
   }


   static void OutputProperties(CEGUI::Window *window)
   {
      // Log all its properties + values
      CEGUI::PropertySet::PropertyIterator itr = ((CEGUI::PropertySet*)window)->getIterator();
      while (!itr.isAtEnd()) 
      {
         try 
         {
            if (!window->isPropertyDefault(itr.getCurrentKey()))
            {
               Notify (ALWAYS, "%s, Prop: %s, %s", window->getName().c_str(),
                  itr.getCurrentKey().c_str(),
                  window->getProperty(itr.getCurrentKey()).c_str());
            }
         }
         catch (CEGUI::InvalidRequestException& exception) 
         {
            // If something goes wrong, show user
            Notify(WARN, "InvalidRequestException for %s: %s", itr.getCurrentKey().c_str(), exception.getMessage().c_str());
         }
         itr++;
      }
   }

private:
   //place member variables here
   RefPtr<dtGUI::CEUIDrawable> mGUI;
   std::string mLayoutFilename;

   void BuildGUI(void)
   {
      try
      {
         std::string schemeFileName = osgDB::findDataFile("gui/schemes/WindowsLook.scheme");

         CEGUI::SchemeManager::getSingleton().loadScheme(schemeFileName);
         CEGUI::System::getSingleton().setDefaultMouseCursor("WindowsLook", "MouseArrow");
         CEGUI::System::getSingleton().setDefaultFont("Tahoma-12");

         CEGUI::Window* sheet = CEGUI::WindowManager::getSingleton().createWindow("DefaultGUISheet", "root_wnd");
         CEGUI::System::getSingleton().setGUISheet(sheet);

         if (!mLayoutFilename.empty())
         {
            //load GUI layout from file
            CEGUI::Window *w = CEGUI::WindowManager::getSingleton().loadWindowLayout(
                  mLayoutFilename.c_str() ); 

            sheet->addChildWindow(w);
         }
         else
         {
            // background panel
            CEGUI::StaticImage* panel = (CEGUI::StaticImage*)CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage", "Panel 1");
            sheet->addChildWindow(panel);
            panel->setPosition(CEGUI::Point(0.0f, 0.0f));
            panel->setSize(CEGUI::Size(1.f, 1.f));

            //Delta3D text
            CEGUI::StaticText* st = (CEGUI::StaticText*)CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticText","Delta_3D");
            panel->addChildWindow(st);
            st->setPosition(CEGUI::Point(0.2f, 0.3f));
            st->setSize(CEGUI::Size(0.6f, 0.2f));
            st->setText("Delta 3D");
            st->setFrameEnabled(false);
            st->setBackgroundEnabled(false);
            st->setFont("Digi-48");
            st->setHorizontalFormatting(CEGUI::StaticText::HorzCentred);

            // Edit box for text entry
            CEGUI::Editbox* eb = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/Editbox", "EditBox");
            panel->addChildWindow(eb);
            eb->setPosition(CEGUI::Point(0.3f, 0.55f));
            eb->setSize(CEGUI::Size(0.4f, 0.1f));
            eb->setText("Editable text box");

            //slider
            CEGUI::Scrollbar* slider = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/HorizontalScrollbar", "slider1");
            panel->addChildWindow(slider);
            slider->setPosition(CEGUI::Point(0.12f, 0.1f));
            slider->setSize(CEGUI::Size(0.76f, 0.05f));
            slider->setDocumentSize(256.f);
            slider->setPageSize(16.f);
            slider->setStepSize(1.f);
            slider->setOverlapSize(1.f);
            slider->setScrollPosition(255.f);

            // quit button
            CEGUI::PushButton* btn = (CEGUI::PushButton*)CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/Button", "QuitButton");
            panel->addChildWindow(btn);
            btn->setText("Exit");
            btn->setPosition( CEGUI::Point(0.4f, 0.7f) );
            btn->setSize( CEGUI::Size(0.2f, 0.1f) );
         }

         //hook up subscribers
         CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();

         wmgr.getWindow("Panel 1")->subscribeEvent(CEGUI::Window::EventMouseDoubleClick, &OutputHandler);      
         wmgr.getWindow("Delta_3D")->subscribeEvent(CEGUI::Window::EventMouseDoubleClick, &OutputHandler);
         wmgr.getWindow("EditBox")->subscribeEvent(CEGUI::Window::EventMouseDoubleClick, &OutputHandler);
         wmgr.getWindow("slider1")->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, &sliderHandler);
         wmgr.getWindow("slider1")->subscribeEvent(CEGUI::Window::EventMouseDoubleClick, &OutputHandler);
         wmgr.getWindow("QuitButton")->subscribeEvent(CEGUI::PushButton::EventClicked, &quitHandler);
         wmgr.getWindow("QuitButton")->subscribeEvent(CEGUI::Window::EventMouseDoubleClick, &OutputHandler);
      }

      // catch to prevent exit (errors will be logged).
      catch(CEGUI::Exception &e)
      {
         Notify(WARN, "CEGUI::%s", e.getMessage().c_str() );
      }
   }

   static bool OutputHandler(const CEGUI::EventArgs &e)
   {
      CEGUI::Window *w = (CEGUI::Window*)((const CEGUI::WindowEventArgs&)e).window;
      TestGUIApp::OutputProperties(w);
      return true;
   }

   static bool quitHandler(const CEGUI::EventArgs& e)
   {
      dtABC::Application::GetInstance(0)->Quit();
      return true;
   }

   static bool sliderHandler(const CEGUI::EventArgs& e)
   {
      CEGUI::Scrollbar* slider = (CEGUI::Scrollbar*)((const CEGUI::WindowEventArgs&)e).window;

      float alpha = slider->getScrollPosition()/255.f;
      CEGUI::Window* sheet=CEGUI::System::getSingleton().getGUISheet();

      if(sheet) sheet->setAlpha(alpha);

      return true;
   }

};



int main( int argc, const char* argv[] )
{
   //set data search path to parent directory and delta3d/data
   SetDataFilePathList( "..;" + GetDeltaDataPathList() ); 

   std::string filename = "";
   if (argc > 1)
   {
      Notify(ALWAYS,"Using GUI file %s...",argv[1]);
      filename = argv[1];
   }

   //Instantiate the application and look for the config file
   RefPtr<TestGUIApp> app = new TestGUIApp();
   app->SetLayoutFilename(filename);

   app->Config(); //configuring the application
   app->Run(); // running the simulation loop

   return 0;
}

