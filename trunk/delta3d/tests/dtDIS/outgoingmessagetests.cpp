#include <cppunit/extensions/HelperMacros.h>
#include <dtDIS/OutgoingMessage.h>        // for testing

#include <dtDIS/IMessageToPacketAdapter.h>    // for base class
#include <DIS/Pdu.h>                      // for usage
#include <dtDIS/Connection.h>             // for usage

#include <dtGame/actorupdatemessage.h>         // for usage
#include <dtGame/basemessages.h>          // for usage
#include <dtGame/messagefactory.h>        // for usage
#include <dtGame/message.h>

namespace dtTest
{
   /// tests the multicast socket code needed for DIS.
   class OutgoingMessageTests : public CPPUNIT_NS::TestFixture
   {
   public:
      void setup();
      void teardown(); 

      void TestAddRemove();
      void TestHandle();
      void TestMultimap();

      CPPUNIT_TEST_SUITE( OutgoingMessageTests );
         CPPUNIT_TEST( TestAddRemove );
         CPPUNIT_TEST( TestHandle );
         CPPUNIT_TEST( TestMultimap );
      CPPUNIT_TEST_SUITE_END();
   };

   class SampleAdapter : public dtDIS::IMessageToPacketAdapter
   {
   public:
      bool _hit;

      DIS::Pdu* _pdu;

      SampleAdapter()
      {
         Reset();
         _pdu = new DIS::Pdu();
      }

      ~SampleAdapter()
      {
         delete _pdu;
         _pdu = NULL;
      }

      DIS::Pdu* Convert(const dtGame::Message& source)
      {
         _hit = true;
         return _pdu;
      }

      void Reset()
      {
         _hit = false;
      }
   };

}

using namespace dtTest;
using namespace dtDIS;
CPPUNIT_TEST_SUITE_REGISTRATION( OutgoingMessageTests );

void OutgoingMessageTests::TestAddRemove()
{
   OutgoingMessage outgoing( DIS::BIG, 3 );
   //CPPUNIT_ASSERT( outgoing.GetAdapters().empty() );

   SampleAdapter adapter;

   outgoing.AddAdaptor( &dtGame::MessageType::INFO_ACTOR_CREATED, &adapter );
   //CPPUNIT_ASSERT_EQUAL( outgoing.GetAdapters().size() , size_t(1) );

   // add another one to the same key
   {
      SampleAdapter adapter2;
      outgoing.AddAdaptor( &dtGame::MessageType::INFO_ACTOR_CREATED, &adapter2 );
      //CPPUNIT_ASSERT_EQUAL( outgoing.GetAdapters().size() , size_t(2) );

      outgoing.RemoveAdaptor( &dtGame::MessageType::INFO_ACTOR_CREATED, &adapter2 );
      //CPPUNIT_ASSERT_EQUAL( outgoing.GetAdapters().size() , size_t(1) );    // should have removed 1 element
   }

   // add another one to a different key
   {
      SampleAdapter adapter2;
      outgoing.AddAdaptor( &dtGame::MessageType::INFO_ACTOR_DELETED, &adapter2 );
      //CPPUNIT_ASSERT_EQUAL( outgoing.GetAdapters().size() , size_t(2) );

      outgoing.RemoveAdaptor( &dtGame::MessageType::INFO_ACTOR_CREATED, &adapter2 );
      //CPPUNIT_ASSERT_EQUAL( outgoing.GetAdapters().size() , size_t(2) );  // should not remove any elements

      // go ahead and clean up
      outgoing.RemoveAdaptor( &dtGame::MessageType::INFO_ACTOR_DELETED, &adapter2 );
      //CPPUNIT_ASSERT_EQUAL( outgoing.GetAdapters().size() , size_t(1) );  // should not remove any elements
   }

   outgoing.RemoveAdaptor( &dtGame::MessageType::INFO_ACTOR_CREATED, &adapter );
   //CPPUNIT_ASSERT( outgoing.GetAdapters().empty() );
}

void OutgoingMessageTests::TestHandle()
{
   OutgoingMessage outgoing( DIS::BIG, 3 );

   // hook up an adapter
   SampleAdapter adapter;
   // be sure of initial state for testing
   CPPUNIT_ASSERT_EQUAL( adapter._hit , false );

   const dtGame::MessageType* subscribed_type = &dtGame::MessageType::INFO_ACTOR_UPDATED;
   outgoing.AddAdaptor( subscribed_type, &adapter );

   // for usage
   dtCore::RefPtr<dtGame::MachineInfo> blah = new dtGame::MachineInfo("blah");
   dtGame::MessageFactory mfact("blah", *blah, "blah");

   // execute the handle
   dtCore::RefPtr<dtGame::ActorDeletedMessage> dmsg;
   mfact.CreateMessage(dtGame::MessageType::INFO_ACTOR_DELETED,dmsg);
   outgoing.Handle( *dmsg );

   // see if the DeleteMessage was received
   CPPUNIT_ASSERT_EQUAL( adapter._hit , false );
   adapter.Reset(); // just in case we needed a reset, but we shouldn't.

   // execute the handle
   dtCore::RefPtr<dtGame::ActorUpdateMessage> cmsg;
   mfact.CreateMessage(*subscribed_type,cmsg);
   outgoing.Handle( *cmsg );

   // see if the CreateMessage was received
   CPPUNIT_ASSERT_EQUAL( adapter._hit , true );

   // clean up
   outgoing.RemoveAdaptor( subscribed_type, &adapter );
   CPPUNIT_ASSERT( outgoing.GetAdapters().empty() );
}

void OutgoingMessageTests::TestMultimap()
{
   OutgoingMessage outgoing( DIS::BIG, 3 );  
   SampleAdapter firstAdapter;
   SampleAdapter secondAdapter;

   const dtGame::MessageType* subscribed_type = &dtGame::MessageType::INFO_ACTOR_UPDATED;  

   // test a single mapping
   outgoing.AddAdaptor( subscribed_type, &firstAdapter );   
   CPPUNIT_ASSERT_EQUAL( outgoing.GetAdapters().size(), size_t(1) );

   outgoing.RemoveAdaptor( subscribed_type, &firstAdapter );   
   CPPUNIT_ASSERT_EQUAL( outgoing.GetAdapters().size(), size_t(0) );

   // test the multimap
   outgoing.AddAdaptor( subscribed_type, &firstAdapter );   
   CPPUNIT_ASSERT_EQUAL(  outgoing.GetAdapters().size(), size_t(1) );

   outgoing.AddAdaptor( subscribed_type, &secondAdapter );   
   CPPUNIT_ASSERT_EQUAL( outgoing.GetAdapters().size(), size_t(2) );

   // This should not allow multiples of the exact same pair
   outgoing.AddAdaptor( subscribed_type, &firstAdapter );   
   CPPUNIT_ASSERT_EQUAL( outgoing.GetAdapters().size(), size_t(2) );

   outgoing.RemoveAdaptor( subscribed_type, &firstAdapter );   
   CPPUNIT_ASSERT_EQUAL( outgoing.GetAdapters().size(), size_t(1) );

   outgoing.RemoveAdaptor( subscribed_type, &secondAdapter );   
   CPPUNIT_ASSERT_EQUAL( outgoing.GetAdapters().size(), size_t(0) );

   CPPUNIT_ASSERT( outgoing.GetAdapters().empty() );
}
