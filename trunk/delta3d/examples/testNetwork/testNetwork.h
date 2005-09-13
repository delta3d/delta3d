#ifndef DELTA_testNetwork
#define DELTA_testNetwork

// testNetwork.cpp : declares the interface of the application

#include <dtCore/dt.h>
#include <dtABC/dtabc.h>

class testNetwork : public dtABC::Application
{
   DECLARE_MANAGEMENT_LAYER( testNetwork )

   public:
      testNetwork( const std::string& configFilename = "testnetworkconfig.xml" );
      ~testNetwork();
   
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

#endif // DELTA_testNetwork