#ifndef DELTA_TESTLIGHTS
#define DELTA_TESTLIGHTS

#include "dtCore/dt.h"
#include "dtABC/dtabc.h"

class TestLightsApp : public dtABC::Application
{
   DECLARE_MANAGEMENT_LAYER( TestLightsApp )

public:

   TestLightsApp( std::string configFilename = "config.xml" );
   ~TestLightsApp();

   void Config();

   void KeyPressed(  dtCore::Keyboard*      keyboard, 
                     Producer::KeyboardKey  key,
                     Producer::KeyCharacter character );

   void OnMessage( dtCore::Base::MessageData* data );

private:

   //static values between 0-360 used to calculate new colors + positions
   static float countOne;
   static float countTwo;
   static float countThree;

   osg::ref_ptr<dtCore::Object> mWarehouse;
   osg::ref_ptr<dtCore::Object> mSphere;

   osg::ref_ptr<dtCore::SpotLight> mGlobalSpot;
   osg::ref_ptr<dtCore::PositionalLight> mLocalPositional;
   osg::ref_ptr<dtCore::PositionalLight> mGlobalPositional;
   osg::ref_ptr<dtCore::InfiniteLight> mGlobalInfinite;

   osg::ref_ptr<dtCore::OrbitMotionModel> mOmm;

};

#endif // DELTA_TESTLIGHTS
