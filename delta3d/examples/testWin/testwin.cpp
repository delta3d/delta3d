#include <set>
#include <sstream>

#include "testwin.h"
#include "UI/CUI_ComboBox.h"
#include "UI/CUI_Menu.h"

using namespace std;
using namespace dtCore;
using namespace dtABC;

CUI_UI*              TestWinApp::mUI = NULL;
RefPtr<DeltaWin>     TestWinApp::mWindow = NULL;
ResolutionVec        TestWinApp::mResVec;
int                  TestWinApp::mSelectedRes = -1;
CUI_TextBox*         TestWinApp::mCurrentResText = NULL;

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

void 
TestWinApp::Config()
{
   Application::Config();

   int w,h,x,y;
   mWindow = GetWindow();
   mWindow->GetPosition(&x, &y, &w, &h);

   mDrawable = new UIDrawable(w,h);
   mDrawable->SetWindowResolution(w,h);

   mUI = mDrawable->GetUI();

   osg::Vec4 black_col ( 0.0, 0.0, 0.0, 1.0 );
   osg::Vec4 white_col ( 1.0, 1.0, 1.0, 1.0 );
   osg::Vec4 trans_col ( 0.0, 0.0, 0.0, 0.0 );
   //sgVec4 grey_col = { 0.3, 0.3, 0.3, 1.0 };
   mDrawable->CreateShader( "black", black_col );
   mDrawable->CreateShader( "transparent", trans_col );

   // fonts
   mDrawable->CreateShader( "raster8b", black_col, "gui/raster8.rgba" );
   mDrawable->CreateShader( "raster8w", white_col, "gui/raster8.rgba" );

   mDrawable->CreateFixedFont("raster8b", "raster8b");
   mDrawable->CreateFixedFont("raster8w", "raster8w");
   mDrawable->CreateFixedFont("default", "raster8w");

   // textures
   mDrawable->CreateShader( "button", "gui/button.tga" );
   mDrawable->CreateShader( "button_hover", "gui/button_hover.tga" );
   mDrawable->CreateShader( "button_click", "gui/button_click.tga" );
   mDrawable->CreateShader( "edit_box", "gui/edit_box.rgb");
   mDrawable->CreateShader( "back_panel", "gui/back_panel.rgb");

   mDrawable->CreateShader( "drop_icon", "gui/drop_icon.tga" );
   mDrawable->CreateShader( "submenu_icon", "gui/submenu_icon.tga" );

   CUI_Frame* mainFrame = new CUI_Frame;
   mainFrame->SetShader(mDrawable->GetShader("back_panel"));
   mDrawable->AddRootFrame("testWin", mainFrame);

   CUI_TextBox* resTitle = new CUI_TextBox;
   resTitle->Move( 0.1, 0.8, 0.4, 0.885 );
   resTitle->SetSize(12, 1);
   resTitle->SetTextScale( 0.8f );
   resTitle->SetText( "Resolution" );
   resTitle->SetFont(mDrawable->GetFont("raster8b") );
   resTitle->SetShader(mDrawable->GetShader("transparent"));
   mainFrame->AddChild( resTitle );
   mDrawable->AddFrame( resTitle );

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
   mCurrentResText->SetFont(mDrawable->GetFont("raster8w") );
   mCurrentResText->SetShader(mDrawable->GetShader("edit_box"));
   mainFrame->AddChild( mCurrentResText );
   mDrawable->AddFrame( mCurrentResText );

   // dump all valid resolutions into a vector
   mResVec = DeltaWin::GetResolutions();

   set< pair<int, int> > widthHeightSet;
   set< int > refreshSet;
   set< int > depthSet;

   for(  ResolutionVec::iterator resIter = mResVec.begin(); 
      resIter != mResVec.end(); 
      resIter++ )
   {
      widthHeightSet.insert( make_pair( (*resIter).width, (*resIter).height) );
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
   resolutionMenu->SetShader( mDrawable->GetShader( "button" ) );
   resolutionMenu->SetItemShader( mDrawable->GetShader("button") );
   resolutionMenu->SetSelectedShader( mDrawable->GetShader("button") );
   resolutionMenu->SetHoverShader( mDrawable->GetShader("button") );
   resolutionMenu->SetFlag( UI_SQUEEZE, true );
   resolutionMenu->SetItemSize( 0.04 );
   resolutionMenu->Move( 0.4, 0.715, 0.8, 0.8 );
   resolutionMenu->SetFrameID( RES_MENU );

   mDrawable->AddFrame( resolutionMenu );
   mainFrame->AddChild( resolutionMenu );

   resolutionMenu->AddItem( "Select resolution...", 0, NULL, UI_ITEM_SUBMENU, widthHeightMenu );

   widthHeightMenu->SetShader( mDrawable->GetShader( "button" ) );
   widthHeightMenu->SetItemShader( mDrawable->GetShader("button") );
   widthHeightMenu->SetHoverShader( mDrawable->GetShader("button") );
   widthHeightMenu->SetSelectedShader( mDrawable->GetShader("button") );
   widthHeightMenu->SetSubMenuIcon( mDrawable->GetShader("submenu_icon") );
   widthHeightMenu->SetFlag( UI_INACTIVE, true );
   widthHeightMenu->SetFlag( UI_VERTICAL, true );
   widthHeightMenu->Move( 0.0, 0.96, 0.15, 1.0 );
   widthHeightMenu->SetItemSize( 0.04 );
   widthHeightMenu->SetFrameID( WH_MENU );

   mDrawable->AddFrame( widthHeightMenu );
   mainFrame->AddChild( widthHeightMenu );  

   // populate menus based on resolution data
   for( set< pair<int, int> >::iterator whIter = widthHeightSet.begin(); whIter != widthHeightSet.end(); whIter++ )
   {
      int w = (*whIter).first;
      int h = (*whIter).second;

      if( DeltaWin::IsValidResolution( mResVec, w, h ) != -1 )
      {
         CUI_Menu* refreshMenu = new CUI_Menu;

         refreshMenu->SetShader( mDrawable->GetShader( "button" ) );
         refreshMenu->SetItemShader( mDrawable->GetShader("button") );
         refreshMenu->SetHoverShader( mDrawable->GetShader("button") );
         refreshMenu->SetSelectedShader( mDrawable->GetShader("button") );
         refreshMenu->SetSubMenuIcon( mDrawable->GetShader("submenu_icon") );
         refreshMenu->SetFlag( UI_INACTIVE, true );
         refreshMenu->SetFlag( UI_VERTICAL, true );
         refreshMenu->Move( 0.0, 0.96, 0.15, 1.0 );
         refreshMenu->SetItemSize( 0.04 );
         refreshMenu->SetFrameID( refreshMenuIndex++ );

         mainFrame->AddChild( refreshMenu );
         mDrawable->AddFrame( refreshMenu );

         for( set<int>::iterator refreshIter = refreshSet.begin(); refreshIter != refreshSet.end(); ++refreshIter )
         {
            int r = *refreshIter;

            if( DeltaWin::IsValidResolution( mResVec, w, h, r) != -1 )
            {              
               CUI_Menu* depthMenu = new CUI_Menu;

               depthMenu->SetShader( mDrawable->GetShader( "button" ) );
               depthMenu->SetItemShader( mDrawable->GetShader("button") );
               depthMenu->SetHoverShader( mDrawable->GetShader("button") );
               depthMenu->SetSelectedShader( mDrawable->GetShader("button") );
               depthMenu->SetSubMenuIcon( mDrawable->GetShader("submenu_icon") );
               depthMenu->SetFlag( UI_INACTIVE, true );
               depthMenu->SetFlag( UI_VERTICAL, true );
               depthMenu->Move( 0.0, 0.96, 0.15, 1.0 );
               depthMenu->SetItemSize(0.04 );

               depthMenu->SetFrameID( depthMenuIndex++ );

               mDrawable->AddFrame( depthMenu );
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
                     selectedMenu->SetShader( mDrawable->GetShader( "button" ) );
                     selectedMenu->SetItemShader( mDrawable->GetShader("button") );
                     selectedMenu->SetHoverShader( mDrawable->GetShader("button") );
                     selectedMenu->SetSelectedShader( mDrawable->GetShader("button") );
                     selectedMenu->SetFlag( UI_INACTIVE, true );
                     selectedMenu->SetFlag( UI_VERTICAL, true );
                     selectedMenu->Move( 0.0, 0.96, 0.15, 1.0 );
                     selectedMenu->SetItemSize(0.05 );
                     selectedMenu->SetFrameID( depthIndex );
                     mDrawable->AddFrame( selectedMenu );
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
   resButton->SetShader(mDrawable->GetShader("button"));
   resButton->SetHoverShader(mDrawable->GetShader("button_hover"));
   resButton->SetClickShader(mDrawable->GetShader("button_click"));
   resButton->SetFont( mDrawable->GetFont("raster8w") );
   resButton->SetText("Set");
   resButton->SetTextScale(0.40f);
   resButton->SetFrameID( RES_SET );

   mainFrame->AddChild( resButton );
   mDrawable->AddFrame( resButton );  

   CUI_TextBox* fullScreenTitle = new CUI_TextBox;
   fullScreenTitle->Move( 0.1, 0.565, 0.4, 0.65 );
   fullScreenTitle->SetSize(12, 1);
   fullScreenTitle->SetTextScale( 0.8f );
   fullScreenTitle->SetText( "FullScreen" );
   fullScreenTitle->SetFont(mDrawable->GetFont("raster8b") );
   fullScreenTitle->SetShader(mDrawable->GetShader("transparent"));
   mainFrame->AddChild( fullScreenTitle );
   mDrawable->AddFrame( fullScreenTitle );

   // combo Box for fullscreen
   CUI_ComboBox* fullScreenCombo =  new CUI_ComboBox;

   fullScreenCombo->SetFlag( UI_VERTICAL, true );  
   fullScreenCombo->SetFrameID( FULL_COMBO );
   fullScreenCombo->Move( 0.4, 0.565, 0.8, 0.65 );
   fullScreenCombo->SetShader( mDrawable->GetShader( "transparent" ) );
   fullScreenCombo->SetItemShader( mDrawable->GetShader("button") );
   fullScreenCombo->SetSelectedShader( mDrawable->GetShader("button") );
   fullScreenCombo->SetHoverShader( mDrawable->GetShader("button") );
   fullScreenCombo->SetDropIconShader( mDrawable->GetShader("drop_icon") );

   mDrawable->AddFrame( fullScreenCombo );
   mainFrame->AddChild( fullScreenCombo );

   fullScreenCombo->AddItem( "Off       ", FULL_OFF );
   fullScreenCombo->AddItem( "On        ", FULL_ON );

   // "set" button
   CUI_Button* fullScreenButton = new CUI_Button;
   fullScreenButton->Move(0.8, 0.565, 0.9, 0.65 );
   fullScreenButton->SetShader(mDrawable->GetShader("button"));
   fullScreenButton->SetHoverShader(mDrawable->GetShader("button_hover"));
   fullScreenButton->SetClickShader(mDrawable->GetShader("button_click"));
   fullScreenButton->SetFont( mDrawable->GetFont("raster8w") );
   fullScreenButton->SetText("Set");
   fullScreenButton->SetTextScale(0.40f);
   fullScreenButton->SetFrameID( FULL_SET );
   mainFrame->AddChild( fullScreenButton );
   mDrawable->AddFrame( fullScreenButton );

   // window title manipulation
   CUI_TextBox* windowNameTitle = new CUI_TextBox;
   windowNameTitle->Move( 0.1, 0.415, 0.4, 0.5 );
   windowNameTitle->SetSize(12, 1);
   windowNameTitle->SetTextScale( 0.8f );
   windowNameTitle->SetText( "Window Title" );
   windowNameTitle->SetFont(mDrawable->GetFont("raster8b") );
   windowNameTitle->SetShader(mDrawable->GetShader("transparent"));
   mainFrame->AddChild( windowNameTitle );
   mDrawable->AddFrame( windowNameTitle );

   CUI_EditableTextBox* windowNameEdit = new CUI_EditableTextBox;
   windowNameEdit->Move( 0.41, 0.415, 0.79, 0.5 );
   windowNameEdit->SetSize(25, 1);
   windowNameEdit->SetTextScale( 0.8f );
   windowNameEdit->SetText( mWindow->GetWindowTitle().c_str() );
   windowNameEdit->SetFont(mDrawable->GetFont("raster8w") );
   windowNameEdit->SetShader(mDrawable->GetShader("edit_box"));
   windowNameEdit->SetFrameID( TITLE_EDIT );
   windowNameEdit->SetCursorType(UI_CURSOR_BAR);
   windowNameEdit->MoveCursor( mWindow->GetWindowTitle().length() );
   mainFrame->AddChild( windowNameEdit );
   mDrawable->AddFrame( windowNameEdit );

   // "set" button
   CUI_Button* windowNameButton = new CUI_Button;
   windowNameButton->Move(0.8, 0.415, 0.9, 0.5 );
   windowNameButton->SetShader(mDrawable->GetShader("button"));
   windowNameButton->SetHoverShader(mDrawable->GetShader("button_hover"));
   windowNameButton->SetClickShader(mDrawable->GetShader("button_click"));
   windowNameButton->SetFont( mDrawable->GetFont("raster8w") );
   windowNameButton->SetText("Set");
   windowNameButton->SetTextScale(0.40f);
   windowNameButton->SetFrameID( TITLE_SET );
   mainFrame->AddChild( windowNameButton );
   mDrawable->AddFrame( windowNameButton );

   // window title manipulation
   CUI_TextBox* positionTitle = new CUI_TextBox;
   positionTitle->Move( 0.1, 0.265, 0.4, 0.35 );
   positionTitle->SetSize(12, 1);
   positionTitle->SetTextScale( 0.8f );
   positionTitle->SetText( "Position" );
   positionTitle->SetFont(mDrawable->GetFont("raster8b") );
   positionTitle->SetShader(mDrawable->GetShader("transparent"));
   positionTitle->SetFrameID( POS_TITLE );
   mainFrame->AddChild( positionTitle );
   mDrawable->AddFrame( positionTitle );

   CUI_EditableTextBox* positionXEdit = new CUI_EditableTextBox;
   positionXEdit->Move( 0.41, 0.265, 0.5, 0.35 );
   positionXEdit->SetSize(4, 1);
   positionXEdit->SetTextScale( 0.8f );
   positionXEdit->SetText( ToString(x).c_str() );
   positionXEdit->SetFont(mDrawable->GetFont("raster8w") );
   positionXEdit->SetShader(mDrawable->GetShader("edit_box"));
   positionXEdit->SetFrameID( POS_X );
   positionXEdit->SetCursorType(UI_CURSOR_BAR);
   positionXEdit->MoveCursor( ToString(x).length()  );
   mainFrame->AddChild( positionXEdit );
   mDrawable->AddFrame( positionXEdit );

   CUI_EditableTextBox* positionYEdit = new CUI_EditableTextBox;
   positionYEdit->Move( 0.51, 0.265, 0.6, 0.35 );
   positionYEdit->SetSize(4, 1);
   positionYEdit->SetTextScale( 0.8f );
   positionYEdit->SetText( ToString(y).c_str() );
   positionYEdit->SetFont(mDrawable->GetFont("raster8w") );
   positionYEdit->SetShader(mDrawable->GetShader("edit_box"));
   positionYEdit->SetFrameID( POS_Y );
   positionYEdit->SetCursorType(UI_CURSOR_BAR);
   positionYEdit->MoveCursor( ToString(y).length() );
   mainFrame->AddChild( positionYEdit );
   mDrawable->AddFrame( positionYEdit );

   CUI_EditableTextBox* positionWEdit = new CUI_EditableTextBox;
   positionWEdit->Move( 0.61, 0.265, 0.7, 0.35 );
   positionWEdit->SetSize(4, 1);
   positionWEdit->SetTextScale( 0.8f );
   positionWEdit->SetText( ToString(w).c_str() );
   positionWEdit->SetFont(mDrawable->GetFont("raster8w") );
   positionWEdit->SetShader(mDrawable->GetShader("edit_box"));
   positionWEdit->SetFrameID( POS_W );
   positionWEdit->SetCursorType(UI_CURSOR_BAR);
   positionWEdit->MoveCursor( ToString(w).length() );
   mainFrame->AddChild( positionWEdit );
   mDrawable->AddFrame( positionWEdit );

   CUI_EditableTextBox* positionHEdit = new CUI_EditableTextBox;
   positionHEdit->Move( 0.71, 0.265, 0.8, 0.35 );
   positionHEdit->SetSize(4, 1);
   positionHEdit->SetTextScale( 0.8f );
   positionHEdit->SetText( ToString(h).c_str() );
   positionHEdit->SetFont(mDrawable->GetFont("raster8w") );
   positionHEdit->SetShader(mDrawable->GetShader("edit_box"));
   positionHEdit->SetFrameID( POS_H );
   positionHEdit->SetCursorType(UI_CURSOR_BAR);
   positionHEdit->MoveCursor( ToString(h).length() );
   mainFrame->AddChild( positionHEdit );
   mDrawable->AddFrame( positionHEdit );

   CUI_TextBox* positionXText = new CUI_TextBox;
   positionXText->Move( 0.41, 0.18, 0.5, 0.265 );
   positionXText->SetSize(1, 1);
   positionXText->SetTextScale( 0.8f );
   positionXText->SetText( "X" );
   positionXText->SetFont(mDrawable->GetFont("raster8b") );
   positionXText->SetShader(mDrawable->GetShader("transparent"));
   positionXText->SetFrameID( POS_X_TEXT );
   mainFrame->AddChild( positionXText );
   mDrawable->AddFrame( positionXText );

   CUI_TextBox* positionYText = new CUI_TextBox;
   positionYText->Move( 0.51, 0.18, 0.6, 0.265 );
   positionYText->SetSize(1, 1);
   positionYText->SetTextScale( 0.8f );
   positionYText->SetText( "Y" );
   positionYText->SetFont(mDrawable->GetFont("raster8b") );
   positionYText->SetShader(mDrawable->GetShader("transparent"));
   positionYText->SetFrameID( POS_Y_TEXT );
   mainFrame->AddChild( positionYText );
   mDrawable->AddFrame( positionYText );

   CUI_TextBox* positionWText = new CUI_TextBox;
   positionWText->Move( 0.61, 0.18, 0.7, 0.265 );
   positionWText->SetSize(1, 1);
   positionWText->SetTextScale( 0.8f );
   positionWText->SetText( "W" );
   positionWText->SetFont(mDrawable->GetFont("raster8b") );
   positionWText->SetShader(mDrawable->GetShader("transparent"));
   positionWText->SetFrameID( POS_W_TEXT );
   mainFrame->AddChild( positionWText );
   mDrawable->AddFrame( positionWText );

   CUI_TextBox* positionHText = new CUI_TextBox;
   positionHText->Move( 0.71, 0.18, 0.8, 0.265 );
   positionHText->SetSize(1, 1);
   positionHText->SetTextScale( 0.8f );
   positionHText->SetText( "H" );
   positionHText->SetFont(mDrawable->GetFont("raster8b") );
   positionHText->SetShader(mDrawable->GetShader("transparent"));
   positionHText->SetFrameID( POS_H_TEXT );
   mainFrame->AddChild( positionHText );
   mDrawable->AddFrame( positionHText );

   // "set" button
   CUI_Button* positionButton = new CUI_Button;
   positionButton->Move(0.8, 0.265, 0.9, 0.35 );
   positionButton->SetShader(mDrawable->GetShader("button"));
   positionButton->SetHoverShader(mDrawable->GetShader("button_hover"));
   positionButton->SetClickShader(mDrawable->GetShader("button_click"));
   positionButton->SetFont( mDrawable->GetFont("raster8w") );
   positionButton->SetText("Set");
   positionButton->SetTextScale(0.40f);
   positionButton->SetFrameID( POS_SET );
   mainFrame->AddChild( positionButton );
   mDrawable->AddFrame( positionButton );

   // "refresh" button
   CUI_Button* refreshButton = new CUI_Button;
   refreshButton->Move(0.8, 0.18, 0.9, 0.265 );
   refreshButton->SetShader(mDrawable->GetShader("button"));
   refreshButton->SetHoverShader(mDrawable->GetShader("button_hover"));
   refreshButton->SetClickShader(mDrawable->GetShader("button_click"));
   refreshButton->SetFont( mDrawable->GetFont("raster8w") );
   refreshButton->SetText("Refresh");
   refreshButton->SetTextScale(0.60f);
   refreshButton->SetFrameID( POS_REFRESH );
   mainFrame->AddChild( refreshButton );
   mDrawable->AddFrame( refreshButton );

   // "exit" button
   CUI_Button* exit = new CUI_Button();
   exit->Move(0.1, 0.095, 0.4, 0.18 );
   exit->SetShader(mDrawable->GetShader("button"));
   exit->SetHoverShader(mDrawable->GetShader("button_hover"));
   exit->SetClickShader(mDrawable->GetShader("button_click"));
   exit->SetFont( mDrawable->GetFont("raster8w") );
   exit->SetText(" Exit ");
   exit->SetTextScale(0.40f);
   exit->SetFrameID( EXIT );
   mainFrame->AddChild(exit);
   mDrawable->AddFrame(exit);

   fullScreenCombo->BringToFront();
   resolutionMenu->BringToFront();
   
   // hook up the main callback
   mDrawable->SetCallbackFunc("testWin", (CUI_UI::callbackfunc)GuiHandler );
   mDrawable->SetActiveRootFrame("testWin");

   /// add the mDrawable to the scene
   AddDrawable( mDrawable.get() );
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
      Quit();
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

         //toggle position button/textboxes
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
   SetDataFilePathList( GetDeltaRootPath() + "/examples/testWin/;" +
                        GetDeltaDataPathList()  );


   RefPtr<TestWinApp> app = new TestWinApp( "config.xml" );

   app->Config();
   app->Run();

   return 0;
}
