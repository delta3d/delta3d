#include <prefix/unittestprefix.h>
#include "initializepdu.h"            // for usage
#include "initializeupdatemessage.h"  // for usage
#include "samplegameactorproxy.h"     // for usage

#include <dtABC/application.h>

#include <dtActors/engineactorregistry.h>

#include <dtCore/scene.h>
#include <dtCore/system.h>

#include <dtDIS/plugins/default/espduprocessor.h>                    // for testing
#include <dtDIS/propertyname.h>
#include <dtDIS/sharedstate.h>

#include <dtGame/actorupdatemessage.h>         // for usage
#include <dtGame/defaultmessageprocessor.h>
#include <dtGame/gamemanager.h>
#include <dtGame/gmcomponent.h>
#include <dtGame/messagefactory.h>        // for usage

#include <dtUtil/mathdefines.h>

#include <cppunit/extensions/HelperMacros.h>

#include <DIS/EntityStatePdu.h>                                      // for usage

#include <cstddef>   // for NULL

extern dtABC::Application& GetGlobalApplication();

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
      CatcherComponent(const dtGame::MessageType* mt)
         : dtGame::GMComponent("catcher")
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
   dtCore::RefPtr<dtGame::GameManager> gm = new dtGame::GameManager( *GetGlobalApplication().GetScene() );

   dtCore::RefPtr<dtGame::DefaultMessageProcessor> mp = new dtGame::DefaultMessageProcessor();
   gm->AddComponent( *mp, dtGame::GameManager::ComponentPriority::NORMAL);
   dtCore::System::GetInstance().Start();
   dtCore::System::GetInstance().Step();
   dtCore::System::GetInstance().Step();

   //create a test pdu and initialize it
   DIS::EntityStatePdu pdu;
   InitializePdu initpdu;
   initpdu(pdu);

   //add a new mapping of DIS entity type and ActorType
   dtDIS::SharedState sharedState;
   sharedState.GetEntityMap().SetEntityActorType(pdu.getEntityType(), 
      dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE.get());
 
   //create our entity state pdu processor and feed it a PDU to work on.
   //In this case, it should create a new Actor and apply all the values
   //of the PDU to the Actor's Properties
   dtDIS::ESPduProcessor processor(gm.get(), &sharedState);
   processor.Process(pdu);

   dtCore::System::GetInstance().Step();

   //at this point, there should be a new Actor out there
   dtCore::ActorPtrVector proxies;
   gm->FindActorsByType(*dtActors::EngineActorRegistry::GAME_MESH_ACTOR_TYPE, proxies );
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Should only be one actor created", size_t(1), proxies.size());
}

void ESPduProcessorTests::TestNoActor()
{
}
