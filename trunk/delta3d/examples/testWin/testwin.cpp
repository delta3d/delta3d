#include <set>

#include "testwin.h"
#include "UI/CUI_ComboBox.h"
#include "UI/CUI_Menu.h"

using namespace std;
using namespace dtCore;
using namespace dtABC;

IMPLEMENT_MANAGEMENT_LAYER( TestWinApp )

CUI_UI*       TestWinApp::mUI = NULL;
DeltaWin*     TestWinApp::mWindow = NULL;
ResolutionVec TestWinApp::mResVec;
int           TestWinApp::mSelectedRes = -1;
CUI_TextBox*  TestWinApp::mCurrentRexText = NULL;

TestWinApp::TestWinApp( std::string configFilename /*= "config.xml"*/ )
:  Application(configFilename)
{
}

TestWinApp::~TestWinApp()
{
}

void 
TestWinApp::Config()
{
   Application::Config();

   int w,h,x,y;
   mWindow = GetWindow();
   mWindow->GetPosition(&x, &y, &w, &h);

   UIDrawable *drawable = new UIDrawable(w,h);
   drawable->SetWindowResolution(w,h);

   mUI = drawable->GetUI();

   sgVec4 black_col = { 0.0, 0.0, 0.0, 1.0 };
   sgVec4 white_col = { 1.0, 1.0, 1.0, 1.0 };
   sgVec4 trans_col = { 0.0, 0.0, 0.0, 0.0 };
   sgVec4 grey_col = { 0.3, 0.3, 0.3, 1.0 };
   drawable->CreateShader( "black", black_col );
   drawable->CreateShader( "transparent", trans_col );

   // fonts
   drawable->CreateShader( "raster8b", black_col, "gui/raster8.rgba" );
   drawable->CreateShader( "raster8w", white_col, "gui/raster8.rgba" );

   drawable->CreateFixedFont("raster8b", "raster8b");
   drawable->CreateFixedFont("raster8w", "raster8w");
   drawable->CreateFixedFont("default", "raster8w");

   // textures
   drawable->CreateShader( "button", "gui/button.tga" );
   drawable->CreateShader( "button_hover", "gui/button_hover.tga" );
   drawable->CreateShader( "button_click", "gui/button_click.tga" );
   drawable->CreateShader( "cursor", "gui/cursor.rgba" );
   drawable->CreateShader( "edit_box", "gui/edit_box.rgb");
   drawable->CreateShader( "back_panel", "gui/back_panel.rgb");

   drawable->CreateShader( "drop_icon", "gui/drop_icon.tga" );
   drawable->CreateShader( "submenu_icon", "gui/submenu_icon.tga" );

   CUI_Frame* mainFrame = new CUI_Frame;
   mainFrame->SetShader(drawable->GetShader("back_panel"));
   drawable->AddRootFrame("testWin", mainFrame);

   CUI_TextBox* resTitle = new CUI_TextBox;
   resTitle->Move( 0.1, 0.875, 0.4, 0.925 );
   resTitle->SetSize(15, 1);
   resTitle->SetTextScale( 1.0f );
   resTitle->SetText( "Resolution:" );
   resTitle->SetFont(drawable->GetFont("raster8b") );
   resTitle->SetShader(drawable->GetShader("transparent"));
   mainFrame->AddChild( resTitle );
   drawable->AddFrame( resTitle );

   Resolution currentRes = DeltaWin::GetCurrentResolution();

   char wh[10];
   char r[10];
   char d[10];

   sprintf( wh, "%dx%d", currentRes.width, currentRes.height );
   sprintf( r, "%d Hz", currentRes.refresh );
   sprintf( d, "%d-bit", currentRes.bitDepth );

   string currentString = string(wh) + " " + string(r) + " " + string(d);

   mCurrentRexText = new CUI_TextBox;
   mCurrentRexText->Move( 0.41, 0.835, 0.79, 0.915 );
   mCurrentRexText->SetSize(25, 1);
   mCurrentRexText->SetTextScale( 1.0f );
   mCurrentRexText->SetText( currentString.c_str() );
   mCurrentRexText->SetFont(drawable->GetFont("raster8w") );
   mCurrentRexText->SetShader(drawable->GetShader("edit_box"));
   mainFrame->AddChild( mCurrentRexText );
   drawable->AddFrame( mCurrentRexText );

   // dump all valid resolutions into a vector
   mResVec = DeltaWin::GetResolutions();

   map< int, int > widthHeightMap;
   set< int > refreshSet;
   set< int > depthSet;

   for(  ResolutionVec::iterator resIter = mResVec.begin(); 
      resIter != mResVec.end(); 
      resIter++ )
   {
      widthHeightMap.insert( map< int, int >::value_type((*resIter).width, (*resIter).height ));
      refreshSet.insert( (*resIter).refresh );
      depthSet.insert( (*resIter).bitDepth );
   }

   CUI_Menu* widthHeightMenu = new CUI_Menu;
   CUI_Menu* resolutionMenu  = new CUI_Menu;

   // unique indices for menus
   unsigned int refreshMenuIndex = REFRESH_MENU_1;
   unsigned int depthMenuIndex = DEPTH_MENU_1;

   // unique indices for menu items
   unsigned int whItemIndex = WH_ITEMS;
   unsigned int refreshItemIndex = REFRESH_ITEMS;

   // create menus
   resolutionMenu->SetShader( drawable->GetShader( "button" ) );
   resolutionMenu->SetItemShader( drawable->GetShader("button") );
   resolutionMenu->SetSelectedShader( drawable->GetShader("button") );
   resolutionMenu->SetHoverShader( drawable->GetShader("button") );
   resolutionMenu->SetFlag( UI_SQUEEZE, true );
   resolutionMenu->SetItemSize( 0.04 );
   resolutionMenu->Move( 0.4, 0.725, 0.8, 0.825 );
   resolutionMenu->SetFrameID( RES_MENU );

   drawable->AddFrame( resolutionMenu );
   mainFrame->AddChild( resolutionMenu );

   

   resolutionMenu->AddItem( "Select resolution...", 0, NULL, UI_ITEM_SUBMENU, widthHeightMenu );

   widthHeightMenu->SetShader( drawable->GetShader( "button" ) );
   widthHeightMenu->SetItemShader( drawable->GetShader("button") );
   widthHeightMenu->SetHoverShader( drawable->GetShader("button") );
   widthHeightMenu->SetSelectedShader( drawable->GetShader("button") );
   widthHeightMenu->SetSubMenuIcon( drawable->GetShader("submenu_icon") );
   widthHeightMenu->SetFlag( UI_INACTIVE, true );
   widthHeightMenu->SetFlag( UI_VERTICAL, true );
   widthHeightMenu->Move( 0.0, 0.96, 0.15, 1.0 );
   widthHeightMenu->SetItemSize( 0.04 );
   widthHeightMenu->SetFrameID( WH_MENU );

   drawable->AddFrame( widthHeightMenu );
   mainFrame->AddChild( widthHeightMenu );  

   // populate menus based on resolution data
   for( map< int, int >::iterator whIter = widthHeightMap.begin(); whIter != widthHeightMap.end(); whIter++ )
   {
      int w = (*whIter).first;
      int h = (*whIter).second;

      if( DeltaWin::IsValidResolution( mResVec, w, h ) != -1 )
      {
         CUI_Menu* refreshMenu = new CUI_Menu;

         refreshMenu->SetShader( drawable->GetShader( "button" ) );
         refreshMenu->SetItemShader( drawable->GetShader("button") );
         refreshMenu->SetHoverShader( drawable->GetShader("button") );
         refreshMenu->SetSelectedShader( drawable->GetShader("button") );
         refreshMenu->SetSubMenuIcon( drawable->GetShader("submenu_icon") );
         refreshMenu->SetFlag( UI_INACTIVE, true );
         refreshMenu->SetFlag( UI_VERTICAL, true );
         refreshMenu->Move( 0.0, 0.96, 0.15, 1.0 );
         refreshMenu->SetItemSize( 0.04 );
         refreshMenu->SetFrameID( refreshMenuIndex++ );

         mainFrame->AddChild( refreshMenu );
         drawable->AddFrame( refreshMenu );

         for( set<int>::iterator refreshIter = refreshSet.begin(); refreshIter != refreshSet.end(); ++refreshIter )
         {
            int r = *refreshIter;

            if( DeltaWin::IsValidResolution( mResVec, w, h, r) != -1 )
            {              
               CUI_Menu* depthMenu = new CUI_Menu;

               depthMenu->SetShader( drawable->GetShader( "button" ) );
               depthMenu->SetItemShader( drawable->GetShader("button") );
               depthMenu->SetHoverShader( drawable->GetShader("button") );
               depthMenu->SetSelectedShader( drawable->GetShader("button") );
               depthMenu->SetSubMenuIcon( drawable->GetShader("submenu_icon") );
               depthMenu->SetFlag( UI_INACTIVE, true );
               depthMenu->SetFlag( UI_VERTICAL, true );
               depthMenu->Move( 0.0, 0.96, 0.15, 1.0 );
               depthMenu->SetItemSize(0.04 );

               depthMenu->SetFrameID( depthMenuIndex++ );

               drawable->AddFrame( depthMenu );
               mainFrame->AddChild( depthMenu );

               for( set<int>::iterator depthIter = depthSet.begin(); depthIter != depthSet.end(); depthIter++ )
               {
                  int d = *(depthIter);

                  int iii = DeltaWin::IsValidResolution( mResVec, w, h, r, d );

                  if( iii != -1 )
                  {           
                     char depth[10];
                     sprintf( depth, "%d-bit", *(depthIter) );

                     int depthIndex = iii + DEPTH_ITEMS;

                     CUI_Menu* selectedMenu = new CUI_Menu;
                     selectedMenu->SetShader( drawable->GetShader( "button" ) );
                     selectedMenu->SetItemShader( drawable->GetShader("button") );
                     selectedMenu->SetHoverShader( drawable->GetShader("button") );
                     selectedMenu->SetSelectedShader( drawable->GetShader("button") );
                     selectedMenu->SetFlag( UI_INACTIVE, true );
                     selectedMenu->SetFlag( UI_VERTICAL, true );
                     selectedMenu->Move( 0.0, 0.96, 0.15, 1.0 );
                     selectedMenu->SetItemSize(0.05 );
                     selectedMenu->SetFrameID( depthIndex );
                     drawable->AddFrame( selectedMenu );
                     mainFrame->AddChild( selectedMenu );
                     selectedMenu->AddItem( "Select?", 0, NULL, UI_ITEM_BUTTON );

                     depthMenu->AddItem( depth, depthIndex, NULL, UI_ITEM_SUBMENU, selectedMenu);
                  }
               }

               char refresh[10];
               sprintf( refresh, "%d Hz", *(refreshIter) );
               refreshMenu->AddItem( refresh, refreshItemIndex++, NULL, UI_ITEM_SUBMENU, depthMenu);
            }
         }

         char widthHeightChar[10];
         sprintf( widthHeightChar, "%dx%d", w, h );
         widthHeightMenu->AddItem( widthHeightChar, whItemIndex++, NULL, UI_ITEM_SUBMENU, refreshMenu);
      }
   }

   // "set" button
   CUI_Button* resButton = new CUI_Button;
   resButton->Move(0.8, 0.825, 0.9, 0.925 );
   resButton->SetShader(drawable->GetShader("button"));
   resButton->SetHoverShader(drawable->GetShader("button_hover"));
   resButton->SetClickShader(drawable->GetShader("button_click"));
   resButton->SetFont( drawable->GetFont("raster8w") );
   resButton->SetText("Set");
   resButton->SetTextScale(0.40f);
   resButton->SetFrameID( RES_SET );

   mainFrame->AddChild( resButton );
   drawable->AddFrame( resButton );  

   CUI_TextBox* fullScreenTitle = new CUI_TextBox;
   fullScreenTitle->Move( 0.1, 0.6, 0.4, 0.65 );
   fullScreenTitle->SetSize(15, 1);
   fullScreenTitle->SetTextScale( 1.0f );
   fullScreenTitle->SetText( "FullScreen" );
   fullScreenTitle->SetFont(drawable->GetFont("raster8b") );
   fullScreenTitle->SetShader(drawable->GetShader("transparent"));
   mainFrame->AddChild( fullScreenTitle );
   drawable->AddFrame( fullScreenTitle );

   // combo Box for fullscreen
   CUI_ComboBox* fullScreenCombo =  new CUI_ComboBox;

   fullScreenCombo->SetFlag( UI_VERTICAL, true );  
   fullScreenCombo->SetFrameID( FULL_COMBO );
   fullScreenCombo->Move( 0.4, 0.55, 0.8, 0.65 );
   fullScreenCombo->SetShader( drawable->GetShader( "transparent" ) );
   fullScreenCombo->SetItemShader( drawable->GetShader("button") );
   fullScreenCombo->SetSelectedShader( drawable->GetShader("button") );
   fullScreenCombo->SetHoverShader( drawable->GetShader("button") );
   fullScreenCombo->SetDropIconShader( drawable->GetShader("drop_icon") );

   drawable->AddFrame( fullScreenCombo );
   mainFrame->AddChild( fullScreenCombo );

   fullScreenCombo->AddItem( "Off       ", FULL_OFF );
   fullScreenCombo->AddItem( "On        ", FULL_ON );

   // "set" button
   CUI_Button* fullScreenButton = new CUI_Button;
   fullScreenButton->Move(0.8, 0.55, 0.9, 0.65 );
   fullScreenButton->SetShader(drawable->GetShader("button"));
   fullScreenButton->SetHoverShader(drawable->GetShader("button_hover"));
   fullScreenButton->SetClickShader(drawable->GetShader("button_click"));
   fullScreenButton->SetFont( drawable->GetFont("raster8w") );
   fullScreenButton->SetText("Set");
   fullScreenButton->SetTextScale(0.40f);
   fullScreenButton->SetFrameID( FULL_SET );
   mainFrame->AddChild( fullScreenButton );
   drawable->AddFrame( fullScreenButton );    

   /// "exit" button
   CUI_Button* exit = new CUI_Button();
   exit->Move(0.1, 0.2, 0.3, 0.3 );
   exit->SetShader(drawable->GetShader("button"));
   exit->SetHoverShader(drawable->GetShader("button_hover"));
   exit->SetClickShader(drawable->GetShader("button_click"));
   exit->SetFont( drawable->GetFont("raster8w") );
   exit->SetText("Exit");
   exit->SetTextScale(0.40f);
   exit->SetFrameID( EXIT );
   mainFrame->AddChild(exit);
   drawable->AddFrame(exit);    

   // hook up the main callback
   drawable->SetCallbackFunc("testWin", (CUI_UI::callbackfunc)GuiHandler );
   drawable->SetActiveRootFrame("testWin");

   /// add the drawable to the scene
   GetScene()->AddDrawable( drawable );

}

void
TestWinApp::KeyPressed(  dtCore::Keyboard*       keyboard,
                       Producer::KeyboardKey   key,
                       Producer::KeyCharacter  character )
{
   Application::KeyPressed( keyboard, key, character );

   switch( key )
   {
   case Producer::Key_Escape:
      System::GetSystem()->Stop();
      break;
   default:
      break;
   }
}


bool 
TestWinApp::GuiHandler( int id, int numparam, void *value )
{
   switch(id) 
   {
   case EXIT:
      {
         System::GetSystem()->Stop();
         break;
      }
   case FULL_SET:
      {
         CUI_ComboBox* fullScreenCombo = static_cast<CUI_ComboBox*>( mUI->GetFrame( FULL_COMBO ) );
         assert( fullScreenCombo );

         mWindow->SetFullScreenMode( fullScreenCombo->GetSelectedID() - FULL_OFF );
         break;
      }
   case RES_SET:
      {
         if( mSelectedRes != -1 )
         {
            DeltaWin::ChangeScreenResolution( mResVec[mSelectedRes] );
         }

         break;
      }
   default:
      {
         if( id >= DEPTH_ITEMS )
         {
            mSelectedRes = id - DEPTH_ITEMS;

            char wh[10];
            char r[10];
            char d[10];

            sprintf( wh, "%dx%d", mResVec[mSelectedRes].width, mResVec[mSelectedRes].height );
            sprintf( r, "%d Hz", mResVec[mSelectedRes].refresh );
            sprintf( d, "%d-bit", mResVec[mSelectedRes].bitDepth );

            string selectedString = string(wh) + " " + string(r) + " " + string(d);

            mCurrentRexText->SetText( selectedString.c_str() );
         }

         break;
      }
   }
   return false;
}


int 
main( int argc, char **argv )
{
   SetDataFilePathList( "..;" + GetDeltaDataPathList() );

   TestWinApp* app = new TestWinApp( "config.xml" );

   app->Config();
   app->Run();

   delete app;

   return 0;
}
