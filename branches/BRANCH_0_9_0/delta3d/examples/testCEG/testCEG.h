#ifndef DELTA_testCEG
#define DELTA_testCEG

// testCEG.cpp : declares the interface of the application

#ifdef _MSC_VER
#pragma warning(disable : 4251)
#endif // _MSC_VER

#include "dtGUI/ceuidrawable.h"
#include "dtABC/application.h"

class TestCEGApp : public dtABC::Application
{
   DECLARE_MANAGEMENT_LAYER( TestCEGApp )

   public:
      TestCEGApp( std::string configFilename = "" );
      ~TestCEGApp();
   
      virtual void Config();

      virtual void KeyPressed(  dtCore::Keyboard*      keyboard, 
                                Producer::KeyboardKey  key,
                                Producer::KeyCharacter character );

      void SetLayoutFilename(std::string filename);

      static void OutputProperties(CEGUI::Window *window);

   private:
   
      //place member variables here
      dtCore::RefPtr<dtGUI::CEUIDrawable> mGUI;

      void BuildGUI(void);
      std::string mLayoutFilename;
   
};

#endif // DELTA_testCEG