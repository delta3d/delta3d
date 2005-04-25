#ifndef DELTA_testCEG
#define DELTA_testCEG

// testCEG.cpp : declares the interface of the application


#include "dtGUI/ceuidrawable.h"
#include "dtABC/application.h"

class testCEG : public dtABC::Application
{
   DECLARE_MANAGEMENT_LAYER( testCEG )

   public:
      testCEG( std::string configFilename = "" );
      ~testCEG();
   
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