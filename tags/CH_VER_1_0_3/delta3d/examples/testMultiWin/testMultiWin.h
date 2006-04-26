#ifndef DELTA_testMultiWin
#define DELTA_testMultiWin

// TestMultiWin.cpp : declares the interface of the application

#include <dtABC/application.h>

class TestMultiWin : public dtABC::Application
{
   public:
      TestMultiWin( const std::string& configFilename = "" );
   protected:
      virtual ~TestMultiWin() {}

   public:
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
