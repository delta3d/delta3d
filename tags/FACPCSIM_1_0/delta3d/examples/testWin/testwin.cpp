#include "testwin.h"

#include <dtGUI/scriptmodule.h>
#include <dtCore/globals.h>
#include <dtUtil/stringutils.h>

#include <sstream>

#include <osgDB/FileUtils>

using namespace dtCore;
using namespace dtABC;
using namespace dtGUI;
using namespace dtUtil;

TestWinApp::TestWinApp( const std::string& configFilename )
: Application( configFilename )
{
}

void TestWinApp::Config()
{
   dtABC::Application::Config();

   int x,y,w,h;
   GetWindow()->GetPosition(&x, &y, &w, &h);
   GetWindow()->ShowCursor(false);
   ScriptModule *sm = new ScriptModule();

   mGUI = new dtGUI::CEUIDrawable( w, h, sm);

   dtGUI::ScriptModule::HandlerFunctor handler0( dtUtil::MakeFunctor( &TestWinApp::FullScreenToggleCB, this ) );
   sm->AddCallback("FullScreenToggleCB", handler0 );

   dtGUI::ScriptModule::HandlerFunctor handler1( dtUtil::MakeFunctor( &TestWinApp::WindowPositionCB, this ) );
   sm->AddCallback("WindowPositionCB", handler1 );

   dtGUI::ScriptModule::HandlerFunctor handler2( dtUtil::MakeFunctor( &TestWinApp::WindowTitleCB, this ) );
   sm->AddCallback("WindowTitleCB", handler2 );

   dtGUI::ScriptModule::HandlerFunctor handler3( dtUtil::MakeFunctor( &TestWinApp::ChangeResolutionCB, this ) );
   sm->AddCallback("ChangeResolutionCB", handler3 );

   // dump all valid resolutions into a vector
   mResolutionVec = DeltaWin::GetResolutions();
   DeltaWin::Resolution currRes = DeltaWin::GetCurrentResolution();

   //create the GUI and fill it out
   BuildGUI(mResolutionVec, currRes);

   GetScene()->AddDrawable(mGUI.get());
}

void TestWinApp::PostFrame( const double deltaFrameTime )
{
   UpdateWidgets();
}

/** Setup the GUI with starting values.  
*/
void TestWinApp::BuildGUI( DeltaWin::ResolutionVec &resolutions,
                           DeltaWin::Resolution &currentRes)
{
   try
   {
      std::string schemeFileName = osgDB::findDataFile("schemes/WindowsLookSkin.scheme");

      CEGUI::SchemeManager::getSingleton().loadScheme(schemeFileName);
      CEGUI::System::getSingleton().setDefaultMouseCursor("WindowsLook", "MouseArrow");
      CEGUI::System::getSingleton().setDefaultFont("Tahoma-12");

      CEGUI::WindowManager *wm = CEGUI::WindowManager::getSingletonPtr();

      CEGUI::Window *sheet = wm->loadWindowLayout("testWinLayout.xml");
      CEGUI::System::getSingleton().setGUISheet(sheet);
      sheet->setUserData( static_cast<TestWinApp*>(this) );
      CEGUI::MultiColumnList *list = static_cast<CEGUI::MultiColumnList*>(wm->getWindow("List"));

      int rowNum = 0;
      std::string str;

      for( DeltaWin::ResolutionVec::iterator itr = resolutions.begin();
           itr != resolutions.end();
           itr++ )
      {         
         list->addRow();

         str = ToString<int>( (*itr).width );
         CEGUI::ListboxTextItem *item = new CEGUI::ListboxTextItem( str );
         item->setTextColours( CEGUI::colour(0.f, 0.f, 0.f) );
         item->setSelectionColours( CEGUI::colour(1.f, 0.f, 0.f) );
         item->setSelectionBrushImage("WindowsLook", "Background");
         item->setUserData( static_cast<void*>(&(*itr)) ); 
         list->setItem(item, 0, rowNum);

         str = ToString<int>((*itr).height);
         item = new CEGUI::ListboxTextItem( str );
         item->setTextColours( CEGUI::colour(0.f, 0.f, 0.f) );
         item->setSelectionColours( CEGUI::colour(1.f, 0.f, 0.f) );
         item->setSelectionBrushImage("WindowsLook", "Background");
         item->setUserData( static_cast<void*>(&(*itr)) );
         list->setItem(item, 1, rowNum);

         str = ToString<int>((*itr).bitDepth);
         item = new CEGUI::ListboxTextItem( str );
         item->setTextColours( CEGUI::colour(0.f, 0.f, 0.f) );
         item->setSelectionColours( CEGUI::colour(1.f, 0.f, 0.f) );
         item->setSelectionBrushImage("WindowsLook", "Background");
         item->setUserData( static_cast<void*>(&(*itr)) );
         list->setItem(item, 2, rowNum);

         str = ToString<int>((*itr).refresh);
         item = new CEGUI::ListboxTextItem( str );
         item->setTextColours( CEGUI::colour(0.f, 0.f, 0.f) );
         item->setSelectionColours( CEGUI::colour(1.f, 0.f, 0.f) );
         item->setSelectionBrushImage("WindowsLook", "Background");
         item->setUserData( static_cast<void*>(&(*itr)) );
         list->setItem(item, 3, rowNum);

         rowNum++;
      }

      //use our current resolution to look up it's position in the available
      //resolution list.  
      //Also select the GUI row which so happens to 
      //correspond to the same index number
      int idx = DeltaWin::IsValidResolution( resolutions, 
                                             currentRes.width, currentRes.height,
                                             currentRes.refresh, currentRes.bitDepth );
      if (idx>-1)
      {
         //todo also need to scroll the window to the correct row.
         //list->ensureItemIsVisible();
         list->setItemSelectState(CEGUI::MCLGridRef(idx, 0), true );
      }

      CEGUI::Checkbox *check = static_cast<CEGUI::Checkbox*>(wm->getWindow("FullscreenToggle"));
      check->setSelected( GetWindow()->GetFullScreenMode() );

      CEGUI::Editbox *title = static_cast<CEGUI::Editbox*>(wm->getWindow("WindowTitle"));
      title->setText( GetWindow()->GetWindowTitle() );

   }
   // catch to prevent exit (errors will be logged).
   catch(CEGUI::Exception &e)
   {
      Log::GetInstance().LogMessage(Log::LOG_WARNING, __FILE__,  "CEGUI::%s", e.getMessage().c_str() );
   }
}

///called every frame
void TestWinApp::UpdateWidgets()
{
   CEGUI::WindowManager *wm = CEGUI::WindowManager::getSingletonPtr();

   static int lastX, lastY, lastW, lastH;

   int x,y,w,h;
   GetWindow()->GetPosition(&x, &y, &w, &h);

   if (lastX != x || lastY != y || lastW != w || lastH != h)
   {
      // if something is different, update the widgets
      lastX = x; lastY =y; lastW = w; lastH = h;

      CEGUI::Editbox *wBox = static_cast<CEGUI::Editbox*>(wm->getWindow("WindowWidth"));
      wBox->setText( ToString(w).c_str() );
      CEGUI::Editbox *hBox = static_cast<CEGUI::Editbox*>(wm->getWindow("WindowHeight"));
      hBox->setText( ToString(h).c_str() );
      CEGUI::Editbox *xBox = static_cast<CEGUI::Editbox*>(wm->getWindow("WindowPosX"));
      xBox->setText( ToString(x).c_str() );
      CEGUI::Editbox *yBox = static_cast<CEGUI::Editbox*>(wm->getWindow("WindowPosY"));
      yBox->setText( ToString(y).c_str() );
   }
}

bool TestWinApp::FullScreenToggleCB(const CEGUI::EventArgs &e)
{
   TestWinApp *app = static_cast<TestWinApp*>(CEGUI::System::getSingleton().getGUISheet()->getUserData());
   CEGUI::Checkbox *check = static_cast<CEGUI::Checkbox*>(CEGUI::WindowManager::getSingleton().getWindow("FullscreenToggle"));
   app->GetWindow()->SetFullScreenMode( check->isSelected() );
   return true;
}

bool TestWinApp::WindowPositionCB( const CEGUI::EventArgs &e)
{
   CEGUI::WindowManager *wm = CEGUI::WindowManager::getSingletonPtr();

   TestWinApp *app = static_cast<TestWinApp*>(CEGUI::System::getSingleton().getGUISheet()->getUserData());

   int x,y,w,h;

   CEGUI::Editbox *wBox = static_cast<CEGUI::Editbox*>(wm->getWindow("WindowWidth"));
   w = atoi( wBox->getText().c_str() );

   CEGUI::Editbox *hBox = static_cast<CEGUI::Editbox*>(wm->getWindow("WindowHeight"));
   h = atoi( hBox->getText().c_str() );

   CEGUI::Editbox *xBox = static_cast<CEGUI::Editbox*>(wm->getWindow("WindowPosX"));
   x = atoi( xBox->getText().c_str() );

   CEGUI::Editbox *yBox = static_cast<CEGUI::Editbox*>(wm->getWindow("WindowPosY"));
   y = atoi( yBox->getText().c_str() );

   app->GetWindow()->SetPosition(x, y, w, h );
   return true;
}

bool TestWinApp::WindowTitleCB( const CEGUI::EventArgs &e)
{
   TestWinApp *app = static_cast<TestWinApp*>(CEGUI::System::getSingleton().getGUISheet()->getUserData());

   CEGUI::Editbox *box = static_cast<CEGUI::Editbox*>(CEGUI::WindowManager::getSingleton().getWindow("WindowTitle"));
   app->GetWindow()->SetWindowTitle( box->getText().c_str() );
   return true;
}

bool TestWinApp::ChangeResolutionCB( const CEGUI::EventArgs &e)
{
   CEGUI::MultiColumnList *list = static_cast<CEGUI::MultiColumnList*>(CEGUI::WindowManager::getSingleton().getWindow("List"));

   CEGUI::ListboxItem *item = list->getFirstSelectedItem();

   DeltaWin::Resolution *res = static_cast<DeltaWin::Resolution*>( item->getUserData() );
   DeltaWin::ChangeScreenResolution(*res);
   return true;
}


int main( int argc, char **argv )
{
   SetDataFilePathList( GetDeltaRootPath() + "/examples/testWin/;" +
                        GetDeltaDataPathList() + ";" +
                        GetDeltaDataPathList()+"/gui/;" );

   RefPtr<TestWinApp> app = new TestWinApp( "config.xml" );

   app->Config();
   app->Run();

   return 0;
}
