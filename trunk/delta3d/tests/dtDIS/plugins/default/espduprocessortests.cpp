#include <cppunit/extensions/HelperMacros.h>

#include <dtDIS/plugins/default/espduprocessor.h>                    // for testing

#include <DIS/EntityStatePdu.h>                                      // for usage

#include <UnitTest_Plugin_EntityStatePdu/InitializePdu.h>            // for usage
#include <UnitTest_Plugin_EntityStatePdu/InitializeUpdateMessage.h>  // for usage
#include <UnitTest_Plugin_EntityStatePdu/SampleGameActorProxy.h>     // for usage

#include <dtGame/gamemanager.h>
#include <dtCore/scene.h>

#include <cstddef>   // for NULL
#include <dtGame/gmcomponent.h>
#include <dtCore/system.h>
#include <dtGame/messagefactory.h>        // for usage
#include <dtGame/actorupdatemessage.h>         // for usage


namespace dtTest
{
   /// tests how the processor performs
   class ESPduProcessorTests : public CPPUNIT_NS::TestFixture
   {
   public:
      void setup();
      void teardown(); 

      /// see if the pdu values show up on the actor
      void TestApplyToLocalActor();

      /// see if the actor values show up on the pdu
      void TestNoActor();

      CPPUNIT_TEST_SUITE( ESPduProcessorTests );
         CPPUNIT_TEST( TestApplyToLocalActor );
         CPPUNIT_TEST( TestNoActor );
      CPPUNIT_TEST_SUITE_END();

   protected:
   };


   class CatcherComponent : public dtGame::GMComponent
   {
   public:
      CatcherComponent(const dtGame::MessageType* mt, const std::string& name="catcher")
         : dtGame::GMComponent(name)
         , mCaught( false )
         , mMT( NULL )
      {
      }

   public:
      bool mCaught;
      const dtGame::MessageType* mMT;

      void ProcessMessage(const dtGame::Message& msg)
      {
         if( mMT == &(msg.GetMessageType()) )
         {
            mCaught = true;
         }
      }

   protected:
      ~CatcherComponent()
      {
      }
   };
}

using namespace dtTest;
CPPUNIT_TEST_SUITE_REGISTRATION( ESPduProcessorTests );

void ESPduProcessorTests::TestApplyToLocalActor()
{
   dtCore::RefPtr<dtCore::Scene> scene = new dtCore::Scene();
   dtCore::RefPtr<dtGame::GameManager> gm = new dtGame::GameManager( *scene );
   dtCore::System::Instance()->Start();
   dtCore::System::Instance()->Step();
   dtCore::System::Instance()->Step();

   // locally, add a GameActorProxy
   dtCore::RefPtr<dtTest::SampleGameActorProxy> sap = new dtTest::SampleGameActorProxy();
   sap->CreateActor();
   sap->BuildPropertyMap();
   dtCore::RefPtr<dtGame::ActorUpdateMessage> created;
   dtCore::RefPtr<dtGame::MachineInfo> blah = new dtGame::MachineInfo("blah");
   dtGame::MessageFactory mfact("blah", *blah, "blah");
   mfact.CreateMessage(dtGame::MessageType::INFO_ACTOR_CREATED,created);
   gm->SendMessage( *created );
   gm->AddActor( *sap , true , false );

   ///\todo need to tick the system so that proxy is really added?
   dtCore::System::Instance()->Step();
   dtCore::System::Instance()->Step();

   DIS::EntityStatePdu pdu;
   InitializePdu initpdu;
   initpdu( pdu );

   dtDIS::ESPduProcessor processor( gm.get(), NULL );

   dtCore::RefPtr<dtTest::CatcherComponent> cc = new dtTest::CatcherComponent(&dtGame::MessageType::INFO_ACTOR_UPDATED);
   gm->AddComponent( *cc , dtGame::GameManager::ComponentPriority::NORMAL );

   processor.Process( pdu );

   ///\todo need to tick the system so that message is really applied?
   dtCore::System::Instance()->Step();

   CPPUNIT_ASSERT( cc->mCaught );

   CPPUNIT_ASSERT_EQUAL( pdu.getEntityAppearance() , sap->mAppearance );
   //CPPUNIT_ASSERT_EQUAL( pdu.getEntityLocation() , sap->mLocation );
   //CPPUNIT_ASSERT_EQUAL( pdu.getEntityOrientation() , sap->mOrientation );
   CPPUNIT_ASSERT_EQUAL( pdu.getEntityID().getEntity() , sap->mEntityIDEntity );
   CPPUNIT_ASSERT_EQUAL( pdu.getEntityID().getApplication() , sap->mEntityIDApplication );
   CPPUNIT_ASSERT_EQUAL( pdu.getEntityID().getSite() , sap->mEntityIDSite );
}

void ESPduProcessorTests::TestNoActor()
{
}
