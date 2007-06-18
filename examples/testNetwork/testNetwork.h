#ifndef DELTA_TestNetwork
#define DELTA_TestNetwork

#include <dtCore/dt.h>
#include <dtABC/dtabc.h>
#include <dtNet/dtnet.h>

#include "mynetwork.h"

class TestNetwork : public dtABC::Application
{
   public:
      TestNetwork( const std::string &hostName, const std::string& configFilename = "testnetworkconfig.xml" );
   protected:
      virtual ~TestNetwork() {}
   public:
      virtual void Config();

      bool KeyPressed(const dtCore::Keyboard* keyboard, Producer::KeyboardKey key, Producer::KeyCharacter character);

      virtual void PreFrame( const double deltaFrameTime );
      virtual void Frame( const double deltaFrameTime );
      virtual void Quit();

   private:
         
      dtCore::RefPtr<MyNetwork> mNet; ///<Reference the NetMgr derived class
      std::string mHostName; ///<The hostname to connect to (if we're a client)
      dtCore::RefPtr<dtCore::Object> mTerrain; ///<Ground
      dtCore::RefPtr<dtCore::FlyMotionModel> mMotion; ///<Motion model

      void SendPosition();
   
};

#endif // DELTA_TestNetwork
