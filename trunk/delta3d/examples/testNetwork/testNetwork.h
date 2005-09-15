#ifndef DELTA_TestNetwork
#define DELTA_TestNetwork

// TestNetwork.cpp : declares the interface of the application

#include <dtCore/dt.h>
#include <dtABC/dtabc.h>
#include <dtNet/dtnet.h>

class TestNetwork : public dtABC::Application
{
   DECLARE_MANAGEMENT_LAYER( TestNetwork )

   public:
      TestNetwork( const std::string &hostName, const std::string& configFilename = "testnetworkconfig.xml" );
      ~TestNetwork();
   
      virtual void Config();

      virtual void KeyPressed(  dtCore::Keyboard*      keyboard, 
                                Producer::KeyboardKey  key,
                                Producer::KeyCharacter character );

      virtual void PreFrame( const double deltaFrameTime );
      virtual void Frame( const double deltaFrameTime );
      virtual void PostFrame( const double deltaFrameTime );
      virtual void Quit();

   private:
         //place member variables here
      dtCore::RefPtr<dtNet::NetMgr> mNet;
      std::string mHostName;
   
};

#endif // DELTA_TestNetwork
