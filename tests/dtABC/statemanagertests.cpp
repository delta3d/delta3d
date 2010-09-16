/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2006-2008, MOVES Institute
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
*
* @author Erik Johnson
*/
#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dtABC/event.h>
#include <dtABC/state.h>
#include <dtABC/statemanager.h>
#include <dtCore/system.h>

namespace dtTest
{   
   ///some State types
   class StateTypes : public dtABC::State::Type
   {
   public:
      StateTypes(const std::string &id):dtABC::State::Type(id) 
      {}

      static const StateTypes STATE_A;
      static const StateTypes STATE_B;
   };
   const StateTypes StateTypes::STATE_A("STATE_A");
   const StateTypes StateTypes::STATE_B("STATE_B");


   ///an example State
   class State_A : public dtABC::State
   {
   public:
      State_A(const std::string &name="StateA"): 
         dtABC::State(&StateTypes::STATE_A, name),
         mShutdown(false)
         {}

         virtual void HandleEvent( dtABC::Event *event)
         {}

         virtual void Shutdown()
         {
            mShutdown = true;
         }

      bool mShutdown;
   };

   ///an example State
   class State_B : public dtABC::State
   {
   public:
      State_B(const std::string &name="StateB"): 
         dtABC::State(&StateTypes::STATE_B, name)
         {}

         virtual void HandleEvent( dtABC::Event *event)
         {}
   };

   ///some event types
   class EventTypes: public dtABC::Event::Type
   {
   public:
      EventTypes(const std::string &name):dtABC::Event::Type(name)
      {}

      static const EventTypes EVENT_A;
      static const EventTypes EVENT_B;
   };
   const EventTypes EventTypes::EVENT_A("EVENT_A");
   const EventTypes EventTypes::EVENT_B("EVENT_B");


   /// little class used to send the supplied Event
   class EventSender : public dtCore::Base
   {
   public:
      EventSender() 
      {}

      void FireEvent( const EventTypes *eventType, dtABC::StateManager *sm )
      {
         dtCore::RefPtr<dtABC::Event> ev = sm->GetEventFactory()->CreateObject(eventType);
         SendMessage("event", ev.get());
      }
   };

   //////////////////////////////////////////////////////////////////////////
   /// unit tests for dtABC::StateManager
   //////////////////////////////////////////////////////////////////////////
   class StateManagerTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE( StateManagerTests );
      CPPUNIT_TEST( TestStateManagerSetup );
      CPPUNIT_TEST( TestStateManagerTransition );
      CPPUNIT_TEST_SUITE_END();

      public:
         StateManagerTests();
         ~StateManagerTests();

         void setUp();
         void tearDown();

         void TestStateManagerSetup();
         void TestStateManagerTransition();

   private:
      dtCore::RefPtr<dtABC::StateManager> mSM;
      dtCore::RefPtr<State_A> mStateA;
      dtCore::RefPtr<State_B> mStateB;
      dtCore::RefPtr<EventSender> mSender;
   };

   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION( StateManagerTests );



   ///an example Event
   class Event_A : public dtABC::Event
   {
   public:
      Event_A(): dtABC::Event( &EventTypes::EVENT_A)
      {}
   };

   ///an example Event
   class Event_B : public dtABC::Event
   {
   public:
      Event_B(): dtABC::Event( &EventTypes::EVENT_B)
      {}
   };


   StateManagerTests::StateManagerTests()
   {}
   
   StateManagerTests::~StateManagerTests()
   {}

   void StateManagerTests::setUp()
   {
      mSM = new dtABC::StateManager();
      mSM->RegisterState<State_A>(&StateTypes::STATE_A);
      mSM->RegisterState<State_B>(&StateTypes::STATE_B);
      mSM->RegisterEvent<Event_A>(&EventTypes::EVENT_A);
      mSM->RegisterEvent<Event_B>(&EventTypes::EVENT_B);

      mStateA = new State_A();
      mStateB = new State_B();

      mSender = new EventSender();
      mSM->AddSender( mSender.get() );
   }

   void StateManagerTests::tearDown()
   {
      mSM->RemoveSender( mSender.get() );
   }


   //////////////////////////////////////////////////////////////////////////
   void StateManagerTests::TestStateManagerSetup()
   {      
      mSM->MakeCurrent(mStateA.get());
      CPPUNIT_ASSERT_MESSAGE("MakeCurrent didn't return the correct State",
                             mStateA.get() == mSM->GetCurrentState());

      bool valid = mSM->AddTransition(&EventTypes::EVENT_A, 
                                       mStateA.get(),
                                       mStateB.get() );

      CPPUNIT_ASSERT_MESSAGE("AddTransition failed",
                              valid == true);

      CPPUNIT_ASSERT_MESSAGE("Wrong number of transitions",
                             mSM->GetTransitions().size() == 1);

      CPPUNIT_ASSERT_MESSAGE("Wrong number of States",
                             mSM->GetStates().size() == 2);

      CPPUNIT_ASSERT_MESSAGE("Wrong number of Events",
                             mSM->GetNumOfEvents(mStateA.get()) == 1);

      mSM->RemoveAllStates();

      CPPUNIT_ASSERT_MESSAGE("StateManager didn't remove all States",
                             mSM->GetStates().size() == 0);
   }


   //////////////////////////////////////////////////////////////////////////
   void StateManagerTests::TestStateManagerTransition()
   {
      mSM->AddTransition(&EventTypes::EVENT_A, 
                                       mStateA.get(),
                                       mStateB.get() );
      mSM->MakeCurrent(mStateA.get());

      mSender->FireEvent(&EventTypes::EVENT_A, mSM.get() );

      dtCore::System::GetInstance().Start();
      dtCore::System::GetInstance().Step(); //need to cycle the System once

      CPPUNIT_ASSERT_MESSAGE("Didn't transition",
                              mStateB.get() == mSM->GetCurrentState() );

      CPPUNIT_ASSERT_EQUAL_MESSAGE("State::Shutdown() wasn't called",
                                    true, mStateA->mShutdown);
   }

}
