#ifndef DELTA_TestAI
#define DELTA_TestAI

#include <string>

#include <dtCore/refptr.h>
#include <dtABC/application.h>
#include <dtCore/camera.h>
#include "aicharacter.h"

#include <osg/Matrix>

class TestAI : public dtABC::Application
{

   public:
      TestAI(const std::string& mapName = "TesttownLt", const std::string& configFilename = "config.xml");
   protected:
      /*virtual*/~TestAI();
   public:

      virtual void Config();

      virtual bool KeyPressed(const dtCore::Keyboard* keyboard, int key);

      virtual void PreFrame( const double deltaFrameTime );
     
   private:

      void LoadDemoMap(const std::string& pStr);
      bool GoToWaypoint(int pNum);
 
      float mSpeed;
      bool mDrawNavMesh;
      std::string mMapFilename;
      dtCore::RefPtr<dtCore::Camera> mOverheadCamera;
      dtCore::RefPtr<dtAI::AICharacter> mCharacter;
      dtAI::Waypoint* mCurrentWaypoint;
      osg::Matrix mCameraOffset; 
   
};

#endif // DELTA_TestAI
