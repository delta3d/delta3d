#include <set>
#include <sstream>

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
CUI_TextBox*  TestWinApp::mCurrentResText = NULL;

template<class T>
std::string ToString(const T& val)
{
   std::ostringstream strm;
   strm << val;
   return strm.str();
} 

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
   //drawable->CreateShader( "cursor", "gui/cursor.rgba" );
   drawable->CreateShader( "edit_box", "gui/edit_box.rgb");
   drawable->CreateShader( "back_panel", "gui/back_panel.rgb");

   drawable->CreateShader( "drop_icon", "gui/drop_icon.tga" );
   drawable->CreateShader( "submenu_icon", "gui/submenu_icon.tga" );

   CUI_Frame* mainFrame = new CUI_Frame;
   mainFrame->SetShader(drawable->GetShader("back_panel"));
   drawable->AddRootFrame("testWin", mainFrame);

   CUI_TextBox* resTitle = new CUI_TextBox;
   resTitle->Move( 0.1, 0.8, 0.4, 0.885 );
   resTitle->SetSize(12, 1);
   resTitle->SetTextScale( 0.8f );
   resTitle->SetText( "Resolution" );
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

   mCurrentResText = new CUI_TextBox;
   mCurrentResText->Move( 0.41, 0.8, 0.79, 0.885 );
   mCurrentResText->SetSize(25, 1);
   mCurrentResText->SetTextScale( 0.8f );
   mCurrentResText->SetText( currentString.c_str() );
   mCurrentResText->SetFont(drawable->GetFont("raster8w") );
   mCurrentResText->SetShader(drawable->GetShader("edit_box"));
   mainFrame->AddChild( mCurrentResText );
   drawable->AddFrame( mCurrentResText );

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
   resolutionMenu->Move( 0.4, 0.715, 0.8, 0.8 );
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
   resButton->Move(0.8, 0.8, 0.9, 0.885 );
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
   fullScreenTitle->Move( 0.1, 0.565, 0.4, 0.65 );
   fullScreenTitle->SetSize(12, 1);
   fullScreenTitle->SetTextScale( 0.8f );
   fullScreenTitle->SetText( "FullScreen" );
   fullScreenTitle->SetFont(drawable->GetFont("raster8b") );
   fullScreenTitle->SetShader(drawable->GetShader("transparent"));
   mainFrame->AddChild( fullScreenTitle );
   drawable->AddFrame( fullScreenTitle );

   // combo Box for fullscreen
   CUI_ComboBox* fullScreenCombo =  new CUI_ComboBox;

   fullScreenCombo->SetFlag( UI_VERTICAL, true );  
   fullScreenCombo->SetFrameID( FULL_COMBO );
   fullScreenCombo->Move( 0.4, 0.565, 0.8, 0.65 );
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
   fullScreenButton->Move(0.8, 0.565, 0.9, 0.65 );
   fullScreenButton->SetShader(drawable->GetShader("button"));
   fullScreenButton->SetHoverShader(drawable->GetShader("button_hover"));
   fullScreenButton->SetClickShader(drawable->GetShader("button_click"));
   fullScreenButton->SetFont( drawable->GetFont("raster8w") );
   fullScreenButton->SetText("Set");
   fullScreenButton->SetTextScale(0.40f);
   fullScreenButton->SetFrameID( FULL_SET );
   mainFrame->AddChild( fullScreenButton );
   drawable->AddFrame( fullScreenButton );

   // window title manipulation
   CUI_TextBox* windowNameTitle = new CUI_TextBox;
   windowNameTitle->Move( 0.1, 0.415, 0.4, 0.5 );
   windowNameTitle->SetSize(12, 1);
   windowNameTitle->SetTextScale( 0.8f );
   windowNameTitle->SetText( "Window Title" );
   windowNameTitle->SetFont(drawable->GetFont("raster8b") );
   windowNameTitle->SetShader(drawable->GetShader("transparent"));
   mainFrame->AddChild( windowNameTitle );
   drawable->AddFrame( windowNameTitle );

   CUI_EditableTextBox* windowNameEdit = new CUI_EditableTextBox;
   windowNameEdit->Move( 0.41, 0.415, 0.79, 0.5 );
   windowNameEdit->SetSize(25, 1);
   windowNameEdit->SetTextScale( 0.8f );
   windowNameEdit->SetText( mWindow->GetWindowTitle().c_str() );
   windowNameEdit->SetFont(drawable->GetFont("raster8w") );
   windowNameEdit->SetShader(drawable->GetShader("edit_box"));
   windowNameEdit->SetFrameID( TITLE_EDIT );
   windowNameEdit->SetCursorType(UI_CURSOR_BAR);
   windowNameEdit->MoveCursor( mWindow->GetWindowTitle().length() );
   mainFrame->AddChild( windowNameEdit );
   drawable->AddFrame( windowNameEdit );

   // "set" button
   CUI_Button* windowNameButton = new CUI_Button;
   windowNameButton->Move(0.8, 0.415, 0.9, 0.5 );
   windowNameButton->SetShader(drawable->GetShader("button"));
   windowNameButton->SetHoverShader(drawable->GetShader("button_hover"));
   windowNameButton->SetClickShader(drawable->GetShader("button_click"));
   windowNameButton->SetFont( drawable->GetFont("raster8w") );
   windowNameButton->SetText("Set");
   windowNameButton->SetTextScale(0.40f);
   windowNameButton->SetFrameID( TITLE_SET );
   mainFrame->AddChild( windowNameButton );
   drawable->AddFrame( windowNameButton );

   // window title manipulation
   CUI_TextBox* positionTitle = new CUI_TextBox;
   positionTitle->Move( 0.1, 0.265, 0.4, 0.35 );
   positionTitle->SetSize(12, 1);
   positionTitle->SetTextScale( 0.8f );
   positionTitle->SetText( "Position" );
   positionTitle->SetFont(drawable->GetFont("raster8b") );
   positionTitle->SetShader(drawable->GetShader("transparent"));
   positionTitle->SetFrameID( POS_TITLE );
   mainFrame->AddChild( positionTitle );
   drawable->AddFrame( positionTitle );

   CUI_EditableTextBox* positionXEdit = new CUI_EditableTextBox;
   positionXEdit->Move( 0.41, 0.265, 0.5, 0.35 );
   positionXEdit->SetSize(4, 1);
   positionXEdit->SetTextScale( 0.8f );
   positionXEdit->SetText( ToString(x).c_str() );
   positionXEdit->SetFont(drawable->GetFont("raster8w") );
   positionXEdit->SetShader(drawable->GetShader("edit_box"));
   positionXEdit->SetFrameID( POS_X );
   positionXEdit->SetCursorType(UI_CURSOR_BAR);
   positionXEdit->MoveCursor( ToString(x).length()  );
   mainFrame->AddChild( positionXEdit );
   drawable->AddFrame( positionXEdit );

   CUI_EditableTextBox* positionYEdit = new CUI_EditableTextBox;
   positionYEdit->Move( 0.51, 0.265, 0.6, 0.35 );
   positionYEdit->SetSize(4, 1);
   positionYEdit->SetTextScale( 0.8f );
   positionYEdit->SetText( ToString(y).c_str() );
   positionYEdit->SetFont(drawable->GetFont("raster8w") );
   positionYEdit->SetShader(drawable->GetShader("edit_box"));
   positionYEdit->SetFrameID( POS_Y );
   positionYEdit->SetCursorType(UI_CURSOR_BAR);
   positionYEdit->MoveCursor( ToString(y).length() );
   mainFrame->AddChild( positionYEdit );
   drawable->AddFrame( positionYEdit );

   CUI_EditableTextBox* positionWEdit = new CUI_EditableTextBox;
   positionWEdit->Move( 0.61, 0.265, 0.7, 0.35 );
   positionWEdit->SetSize(4, 1);
   positionWEdit->SetTextScale( 0.8f );
   positionWEdit->SetText( ToString(w).c_str() );
   positionWEdit->SetFont(drawable->GetFont("raster8w") );
   positionWEdit->SetShader(drawable->GetShader("edit_box"));
   positionWEdit->SetFrameID( POS_W );
   positionWEdit->SetCursorType(UI_CURSOR_BAR);
   positionWEdit->MoveCursor( ToString(w).length() );
   mainFrame->AddChild( positionWEdit );
   drawable->AddFrame( positionWEdit );

   CUI_EditableTextBox* positionHEdit = new CUI_EditableTextBox;
   positionHEdit->Move( 0.71, 0.265, 0.8, 0.35 );
   positionHEdit->SetSize(4, 1);
   positionHEdit->SetTextScale( 0.8f );
   positionHEdit->SetText( ToString(h).c_str() );
   positionHEdit->SetFont(drawable->GetFont("raster8w") );
   positionHEdit->SetShader(drawable->GetShader("edit_box"));
   positionHEdit->SetFrameID( POS_H );
   positionHEdit->SetCursorType(UI_CURSOR_BAR);
   positionHEdit->MoveCursor( ToString(h).length() );
   mainFrame->AddChild( positionHEdit );
   drawable->AddFrame( positionHEdit );

   CUI_TextBox* positionXText = new CUI_TextBox;
   positionXText->Move( 0.41, 0.18, 0.5, 0.265 );
   positionXText->SetSize(1, 1);
   positionXText->SetTextScale( 0.8f );
   positionXText->SetText( "X" );
   positionXText->SetFont(drawable->GetFont("raster8b") );
   positionXText->SetShader(drawable->GetShader("transparent"));
   positionXText->SetFrameID( POS_X_TEXT );
   mainFrame->AddChild( positionXText );
   drawable->AddFrame( positionXText );

   CUI_TextBox* positionYText = new CUI_TextBox;
   positionYText->Move( 0.51, 0.18, 0.6, 0.265 );
   positionYText->SetSize(1, 1);
   positionYText->SetTextScale( 0.8f );
   positionYText->SetText( "Y" );
   positionYText->SetFont(drawable->GetFont("raster8b") );
   positionYText->SetShader(drawable->GetShader("transparent"));
   positionYText->SetFrameID( POS_Y_TEXT );
   mainFrame->AddChild( positionYText );
   drawable->AddFrame( positionYText );

   CUI_TextBox* positionWText = new CUI_TextBox;
   positionWText->Move( 0.61, 0.18, 0.7, 0.265 );
   positionWText->SetSize(1, 1);
   positionWText->SetTextScale( 0.8f );
   positionWText->SetText( "W" );
   positionWText->SetFont(drawable->GetFont("raster8b") );
   positionWText->SetShader(drawable->GetShader("transparent"));
   positionWText->SetFrameID( POS_W_TEXT );
   mainFrame->AddChild( positionWText );
   drawable->AddFrame( positionWText );

   CUI_TextBox* positionHText = new CUI_TextBox;
   positionHText->Move( 0.71, 0.18, 0.8, 0.265 );
   positionHText->SetSize(1, 1);
   positionHText->SetTextScale( 0.8f );
   positionHText->SetText( "H" );
   positionHText->SetFont(drawable->GetFont("raster8b") );
   positionHText->SetShader(drawable->GetShader("transparent"));
   positionHText->SetFrameID( POS_H_TEXT );
   mainFrame->AddChild( positionHText );
   drawable->AddFrame( positionHText );

   // "set" button
   CUI_Button* positionButton = new CUI_Button;
   positionButton->Move(0.8, 0.265, 0.9, 0.35 );
   positionButton->SetShader(drawable->GetShader("button"));
   positionButton->SetHoverShader(drawable->GetShader("button_hover"));
   positionButton->SetClickShader(drawable->GetShader("button_click"));
   positionButton->SetFont( drawable->GetFont("raster8w") );
   positionButton->SetText("Set");
   positionButton->SetTextScale(0.40f);
   positionButton->SetFrameID( POS_SET );
   mainFrame->AddChild( positionButton );
   drawable->AddFrame( positionButton );

   // "refresh" button
   CUI_Button* refreshButton = new CUI_Button;
   refreshButton->Move(0.8, 0.18, 0.9, 0.265 );
   refreshButton->SetShader(drawable->GetShader("button"));
   refreshButton->SetHoverShader(drawable->GetShader("button_hover"));
   refreshButton->SetClickShader(drawable->GetShader("button_click"));
   refreshButton->SetFont( drawable->GetFont("raster8w") );
   refreshButton->SetText("Refresh");
   refreshButton->SetTextScale(0.60f);
   refreshButton->SetFrameID( POS_REFRESH );
   mainFrame->AddChild( refreshButton );
   drawable->AddFrame( refreshButton );

   /// "exit" button
   CUI_Button* exit = new CUI_Button();
   exit->Move(0.1, 0.095, 0.4, 0.18 );
   exit->SetShader(drawable->GetShader("button"));
   exit->SetHoverShader(drawable->GetShader("button_hover"));
   exit->SetClickShader(drawable->GetShader("button_click"));
   exit->SetFont( drawable->GetFont("raster8w") );
   exit->SetText(" Exit ");
   exit->SetTextScale(0.40f);
   exit->SetFrameID( EXIT );
   mainFrame->AddChild(exit);
   drawable->AddFrame(exit);

   fullScreenCombo->BringToFront();
   resolutionMenu->BringToFront();

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
   case RES_SET: //Resolution Set
      {
         if( mSelectedRes != -1 )
         {
            DeltaWin::ChangeScreenResolution( mResVec[mSelectedRes] );
         }

         break;
      }
   case FULL_SET: //FullScreen set
      {
         CUI_ComboBox* fullScreenCombo = static_cast<CUI_ComboBox*>( mUI->GetFrame( FULL_COMBO ) );
         assert( fullScreenCombo );

         bool fullScreen = fullScreenCombo->GetSelectedID() - FULL_OFF;
         mWindow->SetFullScreenMode( fullScreen );

         CUI_TextBox* positionTitle = static_cast<CUI_TextBox*>( mUI->GetFrame( POS_TITLE ) );
         assert(positionTitle);
         CUI_Button* positionButton = static_cast<CUI_Button*>( mUI->GetFrame( POS_SET ) );
         assert(positionButton);
         
         CUI_EditableTextBox* xEdit = static_cast<CUI_EditableTextBox*>( mUI->GetFrame( POS_X ) );
         assert(xEdit);
         CUI_EditableTextBox* yEdit = static_cast<CUI_EditableTextBox*>( mUI->GetFrame( POS_Y ) );
         assert(yEdit);
         CUI_EditableTextBox* wEdit = static_cast<CUI_EditableTextBox*>( mUI->GetFrame( POS_W ) );
         assert(wEdit);
         CUI_EditableTextBox* hEdit = static_cast<CUI_EditableTextBox*>( mUI->GetFrame( POS_H ) );
         assert(hEdit);

         CUI_TextBox* xText = static_cast<CUI_TextBox*>( mUI->GetFrame( POS_X_TEXT ) );
         assert(xText);
         CUI_TextBox* yText = static_cast<CUI_TextBox*>( mUI->GetFrame( POS_Y_TEXT ) );
         assert(yText);
         CUI_TextBox* wText = static_cast<CUI_TextBox*>( mUI->GetFrame( POS_W_TEXT ) );
         assert(wText);
         CUI_TextBox* hText = static_cast<CUI_TextBox*>( mUI->GetFrame( POS_H_TEXT ) );
         assert(hText);

         positionTitle->Activate( !fullScreen );
         positionButton->Activate( !fullScreen );
         
         xEdit->Activate( !fullScreen );
         yEdit->Activate( !fullScreen );
         wEdit->Activate( !fullScreen );
         hEdit->Activate( !fullScreen );
         
         xText->Activate( !fullScreen );
         yText->Activate( !fullScreen );
         wText->Activate( !fullScreen );
         hText->Activate( !fullScreen );
         
         if( !fullScreen )
         {
            UpdatePosition();
         }
         
         break;
      }
   case TITLE_SET: //Window Title Set
      {
         CUI_EditableTextBox* titleEdit = static_cast<CUI_EditableTextBox*>( mUI->GetFrame( TITLE_EDIT ) );
         assert(titleEdit);
         
         mWindow->SetWindowTitle( titleEdit->GetText() );
         break;
      }
   case POS_SET: //Position Set
      {
         CUI_EditableTextBox* xEdit = static_cast<CUI_EditableTextBox*>( mUI->GetFrame( POS_X ) );
         assert(xEdit);
         CUI_EditableTextBox* yEdit = static_cast<CUI_EditableTextBox*>( mUI->GetFrame( POS_Y ) );
         assert(yEdit);
         CUI_EditableTextBox* wEdit = static_cast<CUI_EditableTextBox*>( mUI->GetFrame( POS_W ) );
         assert(wEdit);
         CUI_EditableTextBox* hEdit = static_cast<CUI_EditableTextBox*>( mUI->GetFrame( POS_H ) );
         assert(hEdit);

         int x = atoi( xEdit->GetText() );
         int y = atoi( yEdit->GetText() );
         int w = atoi( wEdit->GetText() );
         int h = atoi( hEdit->GetText() );

         mWindow->SetPosition( x, y, w, h );
         UpdatePosition();
         break;
      }
   case POS_REFRESH:
      {
         UpdatePosition();
         break;
      }
   default: //menu selection
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

            mCurrentResText->SetText( selectedString.c_str() );
         }

         break;
      }
   }
   return false;
}

void
TestWinApp::UpdatePosition()
{
   CUI_EditableTextBox* xEdit = static_cast<CUI_EditableTextBox*>( mUI->GetFrame( POS_X ) );
   assert(xEdit);
   CUI_EditableTextBox* yEdit = static_cast<CUI_EditableTextBox*>( mUI->GetFrame( POS_Y ) );
   assert(yEdit);
   CUI_EditableTextBox* wEdit = static_cast<CUI_EditableTextBox*>( mUI->GetFrame( POS_W ) );
   assert(wEdit);
   CUI_EditableTextBox* hEdit = static_cast<CUI_EditableTextBox*>( mUI->GetFrame( POS_H ) );
   assert(hEdit);

   //query values
   int x,y,w,h;
   mWindow->GetPosition( &x, &y, &w, &h );
   xEdit->SetText( ToString(x).c_str() );
   yEdit->SetText( ToString(y).c_str() );
   wEdit->SetText( ToString(w).c_str() );
   hEdit->SetText( ToString(h).c_str() );

   xEdit->MoveCursor( ToString(x).length() );
   yEdit->MoveCursor( ToString(y).length() );
   wEdit->MoveCursor( ToString(w).length() );
   hEdit->MoveCursor( ToString(h).length() );
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
