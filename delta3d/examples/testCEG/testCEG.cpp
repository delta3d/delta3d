// testCEG.cpp : defines the implementation of the application

#include <CEGUI/CEGUI.h>
#include <CEGUI/CEGUIPropertySet.h>

#include "dtGUI/dtgui.h"
#include "testCEG.h"
#include "dtCore/dt.h"

using namespace dtCore;
using namespace dtABC;
using namespace std;

IMPLEMENT_MANAGEMENT_LAYER( testCEG )

testCEG::testCEG( string configFilename )
: Application( configFilename )
{
   RegisterInstance( this );
}


testCEG::~testCEG()
{
   DeregisterInstance( this );
}

void testCEG::SetLayoutFilename(std::string filename)
{
   mLayoutFilename = filename;
}

void testCEG::Config()
{
   SetDataFilePathList( "..;" + GetDeltaDataPathList() + ";" +
                        (GetDeltaDataPathList()+"/gui/;") );

   dtABC::Application::Config();

   GetWindow()->SetWindowTitle("testCEG");
   
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

void testCEG::KeyPressed(   Keyboard*      keyboard, 
                                    Producer::KeyboardKey  key,
                                    Producer::KeyCharacter character )
{
   switch( key )
   {
      case Producer::Key_Escape:
         Quit();
         break;
      //make cases for other keys
      default:
         break;
   }
}

bool OutputHandler(const CEGUI::EventArgs &e)
{
   CEGUI::Window *w = (CEGUI::Window*)((const CEGUI::WindowEventArgs&)e).window;
   testCEG::OutputProperties(w);
   return true;
}

bool quitHandler(const CEGUI::EventArgs& e)
{
   dtABC::Application::GetInstance(0)->Quit();
   return true;
}

bool sliderHandler(const CEGUI::EventArgs& e)
{
   using namespace CEGUI;

   Scrollbar* slider = (Scrollbar*)((const WindowEventArgs&)e).window;

   float alpha = slider->getScrollPosition()/255.f;
   CEGUI::Window* sheet=CEGUI::System::getSingleton().getGUISheet();

   if(sheet) sheet->setAlpha(alpha);
      
   return true;
}


void testCEG::BuildGUI(void)
{
   using namespace CEGUI;

   try
   {
      std::string schemeFileName = osgDB::findDataFile("gui/schemes/TaharezLook.scheme");

      SchemeManager::getSingleton().loadScheme(schemeFileName);
      Imageset *imageset = ImagesetManager::getSingleton().getImageset("TaharezLook");
      CEGUI::System::getSingleton().setDefaultMouseCursor((utf8*)"TaharezLook", (utf8*)"MouseArrow");
      CEGUI::System::getSingleton().setDefaultFont((utf8*)"Tahoma-12");

      CEGUI::Window* sheet = WindowManager::getSingleton().createWindow((utf8*)"DefaultGUISheet", (utf8*)"root_wnd");
      CEGUI::System::getSingleton().setGUISheet(sheet);
     
      if (!mLayoutFilename.empty())
      {
         //load GUI layout from file
         CEGUI::Window *w = CEGUI::WindowManager::getSingleton().loadWindowLayout(
            (CEGUI::utf8*)mLayoutFilename.c_str() ); 

         sheet->addChildWindow(w);
      }
      else
      {
         // background panel
         StaticImage* panel = (StaticImage*)WindowManager::getSingleton().createWindow((utf8*)"TaharezLook/StaticImage", (utf8*)"Panel 1");
         sheet->addChildWindow(panel);
         panel->setPosition(Point(0.0f, 0.0f));
         panel->setSize(Size(1.f, 1.f));

         //Delta3D text
         StaticText* st = (StaticText*)WindowManager::getSingleton().createWindow("TaharezLook/StaticText","Delta_3D");
         panel->addChildWindow(st);
         st->setPosition(Point(0.2f, 0.3f));
         st->setSize(Size(0.6f, 0.2f));
         st->setText("Delta 3D");
         st->setFrameEnabled(false);
         st->setBackgroundEnabled(false);
         st->setFont("Digi-48");
         st->setHorizontalFormatting(StaticText::HorzCentred);

         // Edit box for text entry
         Editbox* eb = (Editbox*)WindowManager::getSingleton().createWindow((utf8*)"TaharezLook/Editbox", (utf8*)"EditBox");
         panel->addChildWindow(eb);
         eb->setPosition(Point(0.3f, 0.55f));
         eb->setSize(Size(0.4f, 0.1f));
         eb->setText("Editable text box");

         //slider
         Scrollbar* slider = (Scrollbar*)WindowManager::getSingleton().createWindow((utf8*)"TaharezLook/HorizontalScrollbar", (utf8*)"slider1");
         panel->addChildWindow(slider);
         slider->setPosition(Point(0.12f, 0.1f));
         slider->setSize(Size(0.76f, 0.05f));
         slider->setDocumentSize(256.f);
         slider->setPageSize(16.f);
         slider->setStepSize(1.f);
         slider->setOverlapSize(1.f);
         slider->setScrollPosition(255.f);

         // quit button
         PushButton* btn = (PushButton*)WindowManager::getSingleton().createWindow((utf8*)"TaharezLook/Button", (utf8*)"QuitButton");
         panel->addChildWindow(btn);
         btn->setText((utf8*)"Exit");
         btn->setPosition( Point(0.4f, 0.7f) );
         btn->setSize( Size(0.2f, 0.1f) );
      }

      //hook up subscribers
      CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();

      wmgr.getWindow("Panel 1")->subscribeEvent(CEGUI::Window::EventMouseDoubleClick, &OutputHandler);      
      wmgr.getWindow("Delta_3D")->subscribeEvent(CEGUI::Window::EventMouseDoubleClick, &OutputHandler);
      wmgr.getWindow("EditBox")->subscribeEvent(CEGUI::Window::EventMouseDoubleClick, &OutputHandler);
      wmgr.getWindow("slider1")->subscribeEvent(Scrollbar::EventScrollPositionChanged, &sliderHandler);
      wmgr.getWindow("slider1")->subscribeEvent(CEGUI::Window::EventMouseDoubleClick, &OutputHandler);
      wmgr.getWindow("QuitButton")->subscribeEvent( PushButton::EventClicked, &quitHandler);
      wmgr.getWindow("QuitButton")->subscribeEvent(CEGUI::Window::EventMouseDoubleClick, &OutputHandler);
   }

   // catch to prevent exit (errors will be logged).
   catch(CEGUI::Exception &e)
   {
      Notify(WARN, "CEGUI::%s", e.getMessage().c_str() );
   }
}

//output all the non-default properties of this window
void testCEG::OutputProperties(CEGUI::Window *window)
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