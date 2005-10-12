#ifndef DELTA_TestNetwork
#define DELTA_TestNetwork

// TestNetwork.cpp : declares the interface of the application

#include <dtCore/dt.h>
#include <dtABC/dtabc.h>
#include <dtNet/dtnet.h>

#include "mynetwork.h"


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

      virtual void Frame( const double deltaFrameTime );
      virtual void Quit();

   private:
         
      dtCore::RefPtr<MyNetwork> mNet; ///<Reference the NetMgr derived class
      std::string mHostName; ///<The hostname to connect to (if we're a client)
      dtCore::RefPtr<dtCore::Object> mTerrain; ///<ground
      dtCore::RefPtr<dtCore::FlyMotionModel> mMotion; ///<motion model

      void SendPosition();
   
};

#endif // DELTA_TestNetwork
