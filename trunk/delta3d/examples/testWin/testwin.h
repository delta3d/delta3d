#ifndef DELTA_TESTWINAPP
#define DELTA_TESTWINAPP

#include "dt.h"
#include "dtabc.h"

class TestWinApp   :  public   dtABC::Application
{
   DECLARE_MANAGEMENT_LAYER( TestWinApp )

public:

   TestWinApp( std::string configFilename = "config.xml" );
   virtual  ~TestWinApp();

   virtual void Config();

   virtual void              KeyPressed( 
      dtCore::Keyboard*       keyboard, 
      Producer::KeyboardKey   key,
      Producer::KeyCharacter  character );

   static bool GuiHandler( int id, int numparam, void *value );

private:

   enum FrameIDs
   {
      FULL_COMBO     = 100,
      FULL_SET       = 101,
      FULL_OFF       = 102,
      FULL_ON        = 103,
      EXIT           = 200,
      RES_MENU       = 300,
      RES_SET        = 301,
      WH_MENU        = 302,
      REFRESH_MENU_1 = 1000,
      DEPTH_MENU_1   = 2000
   };

   enum ItemIDs
   {
      WH_ITEMS      = 3000,
      REFRESH_ITEMS = 4000,
      DEPTH_ITEMS   = 5000,
   };

   static CUI_UI* mUI;
   static dtCore::DeltaWin* mWindow;
   static dtCore::ResolutionVec mResVec;
   static int mSelectedRes;
   static CUI_TextBox* mCurrentRexText;
};

#endif // DELTA_TESTWINAPP
