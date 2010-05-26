#ifndef DELTA_testMultiWin
#define DELTA_testMultiWin

// TestNestedView.cpp : declares the interface of the application

#include <dtABC/application.h>

class TestNestedView : public dtABC::Application
{
   public:
      TestNestedView(const std::string& configFilename = "");
   protected:
      virtual ~TestNestedView();

   public:
      virtual void Config();

   /**
    * KeyboardListener override
    * Called when a key is pressed.
    *
    * @param keyboard the source of the event
    * @param key the key pressed
    * @param character the corresponding character
    */
   bool KeyPressed(const dtCore::Keyboard* keyboard, int key);

   private:      
      dtCore::RefPtr<dtCore::Camera>   mCam2;
      dtCore::RefPtr<dtCore::Camera>   mCam3;
      
      dtCore::RefPtr<dtCore::View> mRedView;
      dtCore::RefPtr<dtCore::View> mGreenView;
      dtCore::RefPtr<dtCore::View> mBlueView;
};

#endif // DELTA_testMultiWin
