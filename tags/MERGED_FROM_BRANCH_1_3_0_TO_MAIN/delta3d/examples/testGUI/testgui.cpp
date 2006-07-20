#include <CEGUI/CEGUI.h>

#include <dtGUI/dtgui.h>
#include <dtCore/dt.h>
#include <dtABC/dtabc.h>

using namespace dtCore;
using namespace dtABC;
using namespace dtUtil;
using namespace dtGUI;

class TestGUIApp : public dtABC::Application
{
public:
   TestGUIApp(const std::string& configFilename = "" ): Application( configFilename ),
      mScriptModule(new ScriptModule())
   {
   }

   virtual ~TestGUIApp()
   {
      mGUI->ShutdownGUI();
      delete mScriptModule;
   }

   virtual void Config()
   {
      dtABC::Application::Config();

      ///put something in the background to look at
      RefPtr<Object> helo = new Object( "Helo" );
      helo->LoadFile( "models/uh-1n.ive" );
      AddDrawable( helo.get() );

      ///move the camera around to see the side of the 3D Object
      Transform xform(-30.0f, 0.0f, 0.0f, -90.0f, 0.0f, 0.0f );
      GetCamera()->SetTransform( &xform );

      ///lets hide the stock cursor and just use CEGUI's rendered cursor
      GetWindow()->ShowCursor( false );

      ///We'll make a new ScriptModule which will handle subscribing callbacks
      ///to widgets when it loads the Layout file.
      mScriptModule->AddCallback("quitHandler", &quitHandler );
      mScriptModule->AddCallback("sliderHandler", &sliderHandler );

      ///make a new drawable, supplying the DeltaWin and the ScriptModule
      mGUI = new dtGUI::CEUIDrawable(GetWindow(), mScriptModule);

      ///make some cool UI
      BuildGUI();

      ///and finally add the CEUIDrawable to the Scene for rendering
      GetScene()->AddDrawable(mGUI.get());
   }


   void SetLayoutFilename(const std::string& filename)
   {
      mLayoutFilename = filename;
   }

private:
   RefPtr<dtGUI::CEUIDrawable> mGUI;
   std::string mLayoutFilename;
   dtGUI::ScriptModule* mScriptModule;

   void BuildGUI(void)
   {
      try
      {
         std::string schemeFileName = osgDB::findDataFile("schemes/WindowsLookSkin.scheme");

         CEGUI::SchemeManager::getSingleton().loadScheme(schemeFileName);
         CEGUI::System::getSingleton().setDefaultMouseCursor("WindowsLook", "MouseArrow");
         CEGUI::System::getSingleton().setDefaultFont("Tahoma-12");

         CEGUI::WindowManager *wm = CEGUI::WindowManager::getSingletonPtr();

         CEGUI::Window* sheet = wm->createWindow("DefaultGUISheet", "root_wnd");
         CEGUI::System::getSingleton().setGUISheet(sheet);

         if (!mLayoutFilename.empty())
         {
            //load GUI layout from file
            CEGUI::Window *w = wm->loadWindowLayout( mLayoutFilename.c_str() );                 
            sheet->addChildWindow(w);
         }
         else
         {
            // background panel
            CEGUI::StaticImage* panel = (CEGUI::StaticImage*)wm->createWindow("WindowsLook/StaticImage", "Panel 1");
            sheet->addChildWindow(panel);
            panel->setPosition(CEGUI::Point(0.0f, 0.0f));
            panel->setSize(CEGUI::Size(1.f, 1.f));

            //Delta3D text
            CEGUI::StaticText* st = (CEGUI::StaticText*)wm->createWindow("WindowsLook/StaticText","Delta_3D");
            panel->addChildWindow(st);
            st->setPosition(CEGUI::Point(0.2f, 0.3f));
            st->setSize(CEGUI::Size(0.6f, 0.2f));
            st->setText("Delta 3D");
            st->setFrameEnabled(false);
            st->setBackgroundEnabled(false);
            st->setFont("Digi-48");
            st->setHorizontalFormatting(CEGUI::StaticText::HorzCentred);

            // Edit box for text entry
            CEGUI::Editbox* eb = (CEGUI::Editbox*)wm->createWindow("WindowsLook/Editbox", "EditBox");
            panel->addChildWindow(eb);
            eb->setPosition(CEGUI::Point(0.3f, 0.55f));
            eb->setSize(CEGUI::Size(0.4f, 0.1f));
            eb->setText("Editable text box");

            //slider
            CEGUI::Scrollbar* slider = (CEGUI::Scrollbar*)wm->createWindow("WindowsLook/HorizontalScrollbar", "slider1");
            panel->addChildWindow(slider);
            slider->setPosition(CEGUI::Point(0.12f, 0.1f));
            slider->setSize(CEGUI::Size(0.76f, 0.05f));
            slider->setDocumentSize(100.f);
            slider->setPageSize(16.f);
            slider->setStepSize(1.f);
            slider->setOverlapSize(1.f);
            slider->setScrollPosition(100.f);
            slider->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, &sliderHandler);

            // quit button
            CEGUI::PushButton* btn = (CEGUI::PushButton*)wm->createWindow("WindowsLook/Button", "QuitButton");
            panel->addChildWindow(btn);
            btn->setText("Exit");
            btn->setPosition( CEGUI::Point(0.4f, 0.7f) );
            btn->setSize( CEGUI::Size(0.2f, 0.1f) );
            btn->subscribeEvent(CEGUI::PushButton::EventClicked, &quitHandler);
         }
      }

      // catch to prevent exit (errors will be logged).
      catch(CEGUI::Exception &e)
      {
         Log::GetInstance().LogMessage(Log::LOG_WARNING, __FUNCTION__, 
            "CEGUI::%s", e.getMessage().c_str() );
      }
   }

   //quit!
   static bool quitHandler(const CEGUI::EventArgs& e)
   {
      dtABC::Application::GetInstance(0)->Quit();
      return true;
   }

   //the scrollbar has changed positions, do something cool like change the alpha value.
   static bool sliderHandler(const CEGUI::EventArgs& e)
   {
      CEGUI::Scrollbar* slider = (CEGUI::Scrollbar*)((const CEGUI::WindowEventArgs&)e).window;

      //get the scrollbar position (0..1)
      float alphaPercent = slider->getScrollPosition()/slider->getDocumentSize();

      //we don't want alpha of 0.0, so map it to something greater.
      float alphaVal = dtUtil::MapRangeValue(alphaPercent, 0.f, 1.f, 0.2f, 1.f);

      CEGUI::Window* sheet = CEGUI::System::getSingleton().getGUISheet();

      if(sheet) sheet->setAlpha(alphaVal);

      return true;
   }
};



int main( int argc, const char* argv[] )
{
   //set data search path to parent directory and delta3d/data

   std::string guifilespath(  dtCore::GetDeltaDataPathList() + ";" +
                              dtCore::GetDeltaDataPathList()+"/gui/;" + 
                              dtCore::GetDeltaRootPath() + "/examples/testGUI" );

   dtCore::SetDataFilePathList( guifilespath );


   std::string filename = "";
   if (argc > 1)
   {
      Log::GetInstance().LogMessage(Log::LOG_ALWAYS, __FUNCTION__,
         "Using GUI file %s...",argv[1]);
      filename = argv[1];
   }

   //Instantiate the application and look for the config file
   RefPtr<TestGUIApp> app = new TestGUIApp("config.xml");
   app->SetLayoutFilename(filename);

   app->Config(); //configuring the application
   app->Run(); // running the simulation loop

   return 0;
}

/** \page testGUI GUI Tutorial
* \dontinclude testgui.cpp
* This tutorial will break down the testgui example and explain some of the more
* interesting features of the dtGUI namespace.  You can find the whole example code
* in the delta3d/examples/testgui folder.
* 
* Below is a screen of the application.  You should see the same if you run the
* application yourself.
* \image html testGui.jpg
*
* We need to include some header files to define the classes we'll be using.
*
* This is the main header definition for the CEGUI system.
* \skipline CEGUI.h
* 
* Here are our Delta3D header files.
* \note Keep in mind, the namespace header files will automatically link in the appropriate
* library files on Windows platforms.
*
* \skip dtgui.h
* \until dtabc.h
* 
* Lets also tell the compiler about the namespaces we're using so we don't have to
* type as much.
* \skip using namespace dtCore
* \until using namespace dtGUI
*
* Now we can create the standard dtABC::Application derivative:
* \skip class 
* \until Application::Config()
* 
* Lets add something to the dtCore::Scene to make things interesting.  In this case,
* we'll add a helicopter by creating a new dtCore::Object, loading a 3D file, and
* adding it to the dtCore::Scene.
* \skip new Object
* \until AddDrawable
* 
* We better move our dtCore::Camera around a little to see the Object we just loaded.
* Here we move it down the negative X axis and rotate the heading \e clockwise.
* \skip Transform
* \until SetTransform
* 
* Since CEGUI will render a cursor, we should probably turn off the rendering of the
* Operating System cursor.  Otherwise we'll see two cursors at once.  
* \skipline ShowCursor
*
* Now we can start registering some functions for CEGUI to call.  Here, we add two static 
* functions and give them a string "name".  We can also use non-static member functions if
* we want.  See the docs for dtGUI::ScriptModule::AddCallback for more information.  These
* functions will now be available for use when we load a CEGUI layout file. 
*
* For example, we can
* have a CEGUI::PushButton call our quitHandler when the button is pressed by adding in the 
* the following line in our .xml Layout file:
* <tt> Event Name="Clicked" Function="quitHandler" </tt>
* \skipline AddCallback
* \skipline AddCallback
* 
* Finally, can create an instance of the drawable which will actually render the GUI.  To do this,
* we create a new CEGUIDrawable, call a member function to create some GUI, then 
* add it to the dtCore::Scene for rendering.  Notice we're passing the dtCore::DeltaWin and the
* dtGUI::ScriptModule to the CEGUIDrawable's constructor.  By supplying the Window,
* CEGUI will automatically resize its rendering area to match the pixel size of the
* Window.
* \skip mGUI
* \until }
*
* This is a little method used to store the filename of a CEGUI .xml layout file.
* The application will load this file if a valid name is supplied.  
* \skip SetLayoutFilename
* \until }
*
* Here we dive into the world of CEGUI.  The first thing we need to do is load the 
* CEGUI scheme, default cursor, and font.  We absolutely have to have a Scheme file
* loaded before we can continue.
* \skip BuildGUI
* \until setDefaultFont
* \note Notice we use the osgDB::findDataFile method to find the .scheme file. This will
* use the search paths we supplied to find the file.
*
* Now we can create a default, root widget to hold our UI.  We'll use the CEGUI::WindowManager
* to create the wigets for us.
* \skip WindowManager
* \until createWindow
* 
* ..and once thats been created, lets tell CEGUI to use it.
* \skipline setGUISheet(
*
* Since this application can optionally take in an xml Layout file on the command line,
* lets see if it has been supplied and try to load it.  Once its loaded, we can add the 
* returned Window to the default root window we created before.
* \skip mLayoutFilename.empty
* \until }
*
* Otherwise, we'll have to create all the widgets by hand.  At this point, you may wish to
* see the API doc for CEGUI at http://www.cegui.org.uk/api_reference/.  Here, we'll create
* a handfull of widgets, including a Scrollbar and a PushButton which we subscribe to.
* \skip else
* \until }
* 
* CEGUI like to throw exceptions when bad things happen.  Make sure to catch them!
* \skip catch
* \until }
*
* Here's our quit function.  We'll just get the first instance of a dtABC::Application
* and tell it to quit.  Returning true tells CEGUI that we have handled this Event.
* \skip quitHandler
* \until }
*
* This is a little example of how to get the actual widget that triggered the event.
* \skip sliderHandler
* \until CEGUI::Scrollbar
* 
* Once we have the widget, we can do a little math and convert the value of the Scrollbar
* into a value we can use for alpha coloring.
* \skip alphaPercent
* \until alphaVal
*
* Lets use this value to set the overall alpha transparency of the GUI.  To do that, we'll
* grab a handle to the highest widget in the hierarchy, and simply set it's alpha.
* \skip sheet
* \until };
* 
* Here is the typical Delta3D main:
* \skip main
* \until SetDataFilePathList
* \note Notice we added a search path to the Delta3D/data/gui folder.  We have to do this
* to pickup some schema and GUI files.  If CEGUI can't find the schema files, exceptions
* will be thrown all over the place.
*
* Now we simply see if a GUI layout filename was supplied on the command line and save
* it for later use:
* \skip filename
* \until }
* 
* Lets create our cool little application, making sure to store it in a dtCore::RefPtr.  We
* also supply the filename of any command line parameters that might have been set.
* \skip TestGUIApp
* \until Run(
*
* \skip return
* \until }
*/

