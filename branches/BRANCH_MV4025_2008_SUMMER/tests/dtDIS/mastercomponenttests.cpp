#include <cppunit/extensions/HelperMacros.h>

#include <dtDIS/mastercomponent.h>       // for testing
#include <dtDIS/sharedstate.h>

#include <dtCore/scene.h>                // for using the GM
#include <dtGame/gamemanager.h>          // for using the GM

#include <cstddef>                       // for size_t definition

#include <dtABC/application.h>
extern dtABC::Application& GetGlobalApplication();

namespace dtDIS
{
   /// a way to spin some cycles while I am waiting for an actual design.
   class MasterComponentTests : public CPPUNIT_NS::TestFixture
   {
   public:
      void setup();
      void teardown(); 

      void Test();

      CPPUNIT_TEST_SUITE( MasterComponentTests );
         CPPUNIT_TEST( Test );
      CPPUNIT_TEST_SUITE_END();
   };
}

using namespace dtDIS;
CPPUNIT_TEST_SUITE_REGISTRATION( MasterComponentTests );

void MasterComponentTests::Test()
{
   //dtUtil::Log::GetInstance().SetLogLevel( dtUtil::Log::LOG_DEBUG );

   // get a GM going
   dtCore::RefPtr<dtCore::Scene> tscene = new dtCore::Scene();
   dtCore::RefPtr<dtGame::GameManager> gm = new dtGame::GameManager( *GetGlobalApplication().GetScene() );

   // add the component for testing
   ///\todo supply real file handles
   dtDIS::SharedState config;
   dtDIS::ConnectionData cd = { 62040,
			 "239.1.2.3",
			 "../../dtDIS_trunk/bin/plugins",
			 1,
			 1500 };
   
   config.SetConnectionData(cd);

   dtCore::RefPtr<dtDIS::MasterComponent> mc = new dtDIS::MasterComponent( &config );
   // needs to load at least one plugin for the following unit test to pass
   gm->AddComponent( *mc , dtGame::GameManager::ComponentPriority::NORMAL );
   //////CPPUNIT_ASSERT( mc->GetPluginManager().GetRegistry().size() > 0 );

   // remove the component for testing
   gm->RemoveComponent( *mc );
   //////CPPUNIT_ASSERT_EQUAL( mc->GetPluginManager().GetRegistry().size() , size_t(0) );
}