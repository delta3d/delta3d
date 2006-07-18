#ifndef DELTA_TestAI
#define DELTA_TestAI

#include <string>

#include <dtCore/refptr.h>
#include <dtABC/application.h>
#include <dtCore/camera.h>
#include "aicharacter.h"

class TestAI : public dtABC::Application
{

   public:
      TestAI( const std::string& configFilename = "config.xml" );
   protected:
      /*virtual*/~TestAI();
   public:

      virtual void Config();

      virtual bool KeyPressed(const dtCore::Keyboard* keyboard, Producer::KeyboardKey key, Producer::KeyCharacter character);

      virtual void PreFrame( const double deltaFrameTime );
     
   private:

      void LoadDemoMap(const std::string& pStr);
      bool GoToWaypoint(int pNum);
 
      float mSpeed;
      bool mDrawNavMesh;
      dtCore::RefPtr<dtCore::Camera> mOverheadCamera;
      dtCore::RefPtr<dtAI::AICharacter> mCharacter;
      dtAI::Waypoint* mCurrentWaypoint;
   
};

#endif // DELTA_TestAI
