#ifndef DELTA_TESTLIGHTS
#define DELTA_TESTLIGHTS

#include "dtCore/dt.h"
#include "dtABC/dtabc.h"

class TestLightsApp : public dtABC::Application
{
   DECLARE_MANAGEMENT_LAYER( TestLightsApp )

public:

   TestLightsApp( std::string configFilename = "config.xml" );

   void Config();

   void KeyPressed(  dtCore::Keyboard*      keyboard, 
                     Producer::KeyboardKey  key,
                     Producer::KeyCharacter character );

   void PreFrame( const double deltaFrameTime );

private:

   //static values between 0-360 used to calculate new colors + positions
   static float countOne;
   static float countTwo;
   static float countThree;

   dtCore::RefPtr<dtCore::Object> mWarehouse;
   dtCore::RefPtr<dtCore::Object> mSphere;

   dtCore::RefPtr<dtCore::SpotLight> mGlobalSpot;
   dtCore::RefPtr<dtCore::PositionalLight> mPositional;
   dtCore::RefPtr<dtCore::InfiniteLight> mGlobalInfinite;

   dtCore::RefPtr<dtCore::OrbitMotionModel> mOmm;

};

#endif // DELTA_TESTLIGHTS
