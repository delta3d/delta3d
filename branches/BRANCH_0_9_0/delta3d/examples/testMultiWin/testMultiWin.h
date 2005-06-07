#ifndef DELTA_testMultiWin
#define DELTA_testMultiWin

// testMultiWin.cpp : declares the interface of the application

#include "dtCore/dt.h"
#include "dtABC/dtabc.h"

class testMultiWin : public dtABC::Application
{
   DECLARE_MANAGEMENT_LAYER( testMultiWin )

   public:
      testMultiWin( std::string configFilename = "" );
      ~testMultiWin();
   
      virtual void Config();

      virtual void KeyPressed(  dtCore::Keyboard*      keyboard, 
                                Producer::KeyboardKey  key,
                                Producer::KeyCharacter character );

   private:
      dtCore::RefPtr<dtCore::DeltaWin> mWin2;
      dtCore::RefPtr<dtCore::Camera> mCam2;
      dtCore::RefPtr<dtCore::Camera> mCam3;
};

#endif // DELTA_testMultiWin
