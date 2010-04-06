/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
#include <prefix/unittestprefix.h>

#include <cppunit/extensions/HelperMacros.h>
#include <dtDIS/outgoingmessage.h>        // for testing

#include <dtDIS/imessagetopacketadapter.h>    // for base class
#include <DIS/Pdu.h>                      // for usage
#include <dtDIS/connection.h>             // for usage

#include <dtGame/actorupdatemessage.h>         // for usage
#include <dtGame/basemessages.h>          // for usage
#include <dtGame/messagefactory.h>        // for usage
#include <dtGame/message.h>

namespace dtDIS
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

using namespace dtDIS;
CPPUNIT_TEST_SUITE_REGISTRATION( OutgoingMessageTests );

void OutgoingMessageTests::TestAddRemove()
{
   OutgoingMessage outgoing( DIS::BIG, 3 );
   CPPUNIT_ASSERT( outgoing.GetAdapters().empty() );

   SampleAdapter adapter;

   outgoing.AddAdaptor( &dtGame::MessageType::INFO_ACTOR_CREATED, &adapter );
   CPPUNIT_ASSERT_EQUAL( outgoing.GetAdapters().size() , size_t(1) );

   // add another one to the same key
   {
      SampleAdapter adapter2;
      outgoing.AddAdaptor( &dtGame::MessageType::INFO_ACTOR_CREATED, &adapter2 );
      CPPUNIT_ASSERT_EQUAL( outgoing.GetAdapters().size() , size_t(2) );

      outgoing.RemoveAdaptor( &dtGame::MessageType::INFO_ACTOR_CREATED, &adapter2 );
      CPPUNIT_ASSERT_EQUAL( outgoing.GetAdapters().size() , size_t(1) );    // should have removed 1 element
   }

   // add another one to a different key
   {
      SampleAdapter adapter2;
      outgoing.AddAdaptor( &dtGame::MessageType::INFO_ACTOR_DELETED, &adapter2 );
      CPPUNIT_ASSERT_EQUAL( outgoing.GetAdapters().size() , size_t(2) );

      outgoing.RemoveAdaptor( &dtGame::MessageType::INFO_ACTOR_CREATED, &adapter2 );
      CPPUNIT_ASSERT_EQUAL( outgoing.GetAdapters().size() , size_t(2) );  // should not remove any elements

      // go ahead and clean up
      outgoing.RemoveAdaptor( &dtGame::MessageType::INFO_ACTOR_DELETED, &adapter2 );
      CPPUNIT_ASSERT_EQUAL( outgoing.GetAdapters().size() , size_t(1) );  // should not remove any elements
   }

   outgoing.RemoveAdaptor( &dtGame::MessageType::INFO_ACTOR_CREATED, &adapter );
   CPPUNIT_ASSERT( outgoing.GetAdapters().empty() );
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
   dtCore::RefPtr<dtGame::Message> dmsg;
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
