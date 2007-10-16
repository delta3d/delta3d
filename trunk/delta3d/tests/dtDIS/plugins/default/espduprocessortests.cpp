#include <cppunit/extensions/HelperMacros.h>

#include <dtDIS/plugins/default/espduprocessor.h>                    // for testing

#include <DIS/EntityStatePdu.h>                                      // for usage

#include <dtDIS/plugins/default/initializepdu.h>            // for usage
#include <dtDIS/plugins/default/initializeupdatemessage.h>  // for usage
#include <dtDIS/plugins/default/samplegameactorproxy.h>     // for usage
#include <dtDIS/sharedstate.h>

#include <dtGame/gamemanager.h>
#include <dtCore/scene.h>

#include <dtActors/engineactorregistry.h>

#include <cstddef>   // for NULL
#include <dtGame/gmcomponent.h>
#include <dtCore/system.h>
#include <dtGame/messagefactory.h>        // for usage
#include <dtGame/actorupdatemessage.h>         // for usage
#include <dtGame/defaultmessageprocessor.h>


namespace dtTest
{
   /// tests how the processor performs
   class ESPduProcessorTests : public CPPUNIT_NS::TestFixture
   {
   public:
      void setup();
      void teardown(); 

      /// see if the pdu values show up on the actor
      void CreateRemoteActorFromEntityStatePDU();

      /// see if the actor values show up on the pdu
      void TestNoActor();

      CPPUNIT_TEST_SUITE( ESPduProcessorTests );
         CPPUNIT_TEST( CreateRemoteActorFromEntityStatePDU );
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

void ESPduProcessorTests::CreateRemoteActorFromEntityStatePDU()
{
   dtCore::RefPtr<dtCore::Scene> scene = new dtCore::Scene();
   dtCore::RefPtr<dtGame::GameManager> gm = new dtGame::GameManager( *scene );

   dtCore::RefPtr<dtGame::DefaultMessageProcessor> mp = new dtGame::DefaultMessageProcessor();
   gm->AddComponent( *mp, dtGame::GameManager::ComponentPriority::NORMAL);
   dtCore::System::GetInstance().Start();
   dtCore::System::GetInstance().Step();
   dtCore::System::GetInstance().Step();

   //create a test pdu and initialize it
   DIS::EntityStatePdu pdu;
   InitializePdu initpdu;
   initpdu( pdu );

   //add a new mapping of DIS entity type and ActorType
   dtDIS::SharedState sharedState;
   sharedState.GetActorMap().AddActorMapping(pdu.getEntityType(), 
      dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE.get() );
 
   //create our entity state pdu processor and feed it a PDU to work on.
   //In this case, it should create a new Actor and apply all the values
   //of the PDU to the Actor's Properties
   dtDIS::ESPduProcessor processor( gm.get(), &sharedState );
   processor.Process( pdu );

   ///\todo need to tick the system so that message is really applied?
   dtCore::System::GetInstance().Step();


   //CPPUNIT_ASSERT_EQUAL( pdu.getEntityAppearance() , sap->mAppearance );
   //CPPUNIT_ASSERT_EQUAL( pdu.getEntityLocation() , sap->mLocation );
   //CPPUNIT_ASSERT_EQUAL( pdu.getEntityOrientation() , sap->mOrientation );
   //CPPUNIT_ASSERT_EQUAL( pdu.getEntityID().getEntity() , sap->mEntityIDEntity );
   //CPPUNIT_ASSERT_EQUAL( pdu.getEntityID().getApplication() , sap->mEntityIDApplication );
   //CPPUNIT_ASSERT_EQUAL( pdu.getEntityID().getSite() , sap->mEntityIDSite );

   //at this point, there should be a new Actor out there
   std::vector<dtDAL::ActorProxy*> proxies;
   gm->FindActorsByType(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE, proxies );
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Should only be one actor created", size_t(1), proxies.size());
}

void ESPduProcessorTests::TestNoActor()
{
}
