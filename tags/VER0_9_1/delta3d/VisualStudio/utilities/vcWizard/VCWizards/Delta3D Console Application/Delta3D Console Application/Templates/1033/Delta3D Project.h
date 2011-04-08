#ifndef DELTA_[!output PROJECT_NAME]
#define DELTA_[!output PROJECT_NAME]

// [!output PROJECT_NAME].cpp : declares the interface of the application

#include "dtCore/dt.h"
#include "dtABC/dtabc.h"

class [!output PROJECT_NAME] : public dtABC::Application
{
   DECLARE_MANAGEMENT_LAYER( [!output PROJECT_NAME] )

   public:
      [!output PROJECT_NAME]( std::string configFilename = "" );
      ~[!output PROJECT_NAME]();
   
      virtual void Config();

      virtual void KeyPressed(  dtCore::Keyboard*      keyboard, 
                                Producer::KeyboardKey  key,
                                Producer::KeyCharacter character );

      virtual void PreFrame( const double deltaFrameTime );
      virtual void Frame( const double deltaFrameTime );
      virtual void PostFrame( const double deltaFrameTime );

   private:
   
      //place member variables here
   
};

#endif // DELTA_[!output PROJECT_NAME]