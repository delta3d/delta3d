/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006 MOVES Institute 
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * @author Erik Johnson
 */
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

      dtCore::System::Instance()->Start();
      dtCore::System::Instance()->Step(); //need to cycle the System once

      CPPUNIT_ASSERT_MESSAGE("Didn't transition",
                              mStateB.get() == mSM->GetCurrentState() );

      CPPUNIT_ASSERT_EQUAL_MESSAGE("State::Shutdown() wasn't called",
                                    true, mStateA->mShutdown);
   }

}
