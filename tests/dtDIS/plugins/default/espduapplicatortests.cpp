#include <cppunit/extensions/HelperMacros.h>

#include <dtDIS/plugins/default/espduapplicator.h>   // for testing

#include <DIS/EntityStatePdu.h> 
#include <vector>
#include <dtGame/actorupdatemessage.h>
#include <dtCore/refptr.h>

#include <dtGame/messageparameter.h>
#include <dtDIS/propertyname.h>

#include <dtDIS/plugins/default/initializepdu.h>
#include <dtDIS/plugins/default/initializeupdatemessage.h>
#include <dtDIS/plugins/default/comparehelpers.h>

namespace dtTest
{
   /// how the applicator utilily performs
   class ESPduApplicatorTests : public CPPUNIT_NS::TestFixture
   {
   public:
      void setup();
      void teardown(); 

      void TestApplyToMessage();
      void TestApplyToPdu();

      CPPUNIT_TEST_SUITE( ESPduApplicatorTests );
         CPPUNIT_TEST( TestApplyToMessage );
         CPPUNIT_TEST( TestApplyToPdu );
      CPPUNIT_TEST_SUITE_END();
   };
}

using namespace dtTest;
CPPUNIT_TEST_SUITE_REGISTRATION( ESPduApplicatorTests );

void ESPduApplicatorTests::TestApplyToMessage()
{
   DIS::EntityStatePdu pdu;
   InitializePdu initpdu;
   initpdu( pdu );

   dtCore::RefPtr<dtGame::ActorUpdateMessage> msg = new dtGame::ActorUpdateMessage();
   dtDIS::details::FullApplicator apply;
   apply( pdu , *msg, NULL );

   // test to see if the update message is the same as what it should be
   dtCore::RefPtr<dtGame::ActorUpdateMessage> should_be = new dtGame::ActorUpdateMessage();
   InitializeUpdateMessage initmsg;
   initmsg( *should_be );

   CompareParams( dtDIS::EnginePropertyName::ROTATION    , *should_be, *msg ) ;
   CompareParams( dtDIS::EnginePropertyName::TRANSLATION , *should_be, *msg ) ;
   CompareParams( dtDIS::EnginePropertyName::VELOCITY    , *should_be, *msg ) ;

   CompareParams( dtDIS::HLABaseEntityPropertyName::PROPERTY_LAST_KNOWN_ROTATION    , *should_be, *msg ) ;
   CompareParams( dtDIS::HLABaseEntityPropertyName::PROPERTY_LAST_KNOWN_TRANSLATION , *should_be, *msg ) ;
   CompareParams( dtDIS::HLABaseEntityPropertyName::PROPERTY_VELOCITY_VECTOR        , *should_be, *msg ) ;

   //CompareParams( dtDIS::EnginePropertyName::ENTITY_APPEARANCE, *should_be, *msg );
   //CompareParams( dtDIS::EnginePropertyName::ENTITYID_ENTITY, *should_be, *msg );
   //CompareParams( dtDIS::EnginePropertyName::ENTITYID_SITE, *should_be, *msg );
   //CompareParams( dtDIS::EnginePropertyName::ENTITYID_APPLICATION, *should_be, *msg );
}

void ESPduApplicatorTests::TestApplyToPdu()
{
   dtCore::RefPtr<dtGame::ActorUpdateMessage> msg = new dtGame::ActorUpdateMessage();
   InitializeUpdateMessage initmsg;
   initmsg( *msg );

   DIS::EntityStatePdu pdu;
   dtDIS::details::FullApplicator apply;
   DIS::EntityID eid;
   eid.setSite(23);
   eid.setApplication(24);
   eid.setEntity(25);

   apply( *msg , eid ,  pdu, NULL);

   DIS::EntityStatePdu should_be;
   InitializePdu initpdu;
   initpdu( should_be );

   CPPUNIT_ASSERT( CompareVec3(should_be.getEntityLocation(), pdu.getEntityLocation()) );

   CPPUNIT_ASSERT_MESSAGE( "Orientation doesn't match.",
      CompareOrientation(should_be.getEntityOrientation(), pdu.getEntityOrientation()) );

   CPPUNIT_ASSERT_MESSAGE( "Linear velocity doesn't match.",
      CompareVec3(should_be.getEntityLinearVelocity(), pdu.getEntityLinearVelocity()) );

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Appearance doesn't match", 
      should_be.getEntityAppearance(), pdu.getEntityAppearance() );

   CPPUNIT_ASSERT_EQUAL_MESSAGE( "Entity doesn't match",
      should_be.getEntityID().getEntity(), pdu.getEntityID().getEntity() );

   CPPUNIT_ASSERT_EQUAL_MESSAGE( "Site doesn't match",
      should_be.getEntityID().getSite(), pdu.getEntityID().getSite() );

   CPPUNIT_ASSERT_EQUAL_MESSAGE( "Application doesn't match",
      should_be.getEntityID().getApplication(), pdu.getEntityID().getApplication() );
}
