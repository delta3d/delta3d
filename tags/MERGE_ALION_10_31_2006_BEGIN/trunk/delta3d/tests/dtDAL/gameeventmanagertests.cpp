/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology
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
 * @author Matthew W. Campbell
 */
#include <cppunit/extensions/HelperMacros.h>

#include <vector>
#include <dtCore/refptr.h>

#include <dtUtil/exception.h>
#include <dtUtil/stringutils.h>

#include <dtDAL/gameeventmanager.h>
#include <dtDAL/gameevent.h>

using dtCore::RefPtr;
using std::vector;

namespace dtDAL
{
   
   class GameEventManagerTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(GameEventManagerTests);
         CPPUNIT_TEST(TestAddRemoveEvents);
         CPPUNIT_TEST(TestGetAllEvents);
         CPPUNIT_TEST(TestCopyConstructor);
         CPPUNIT_TEST(TestAssigmentOperator);
      CPPUNIT_TEST_SUITE_END();
   
      public:
         void setUp();
         void tearDown();
         void TestAddRemoveEvents();
         void TestGetAllEvents();
         void TestCopyConstructor();
         void TestAssigmentOperator();
   
      private:
         GameEventManager *mEventMgr;
   
         void CompareEventManagers(GameEventManager& one, GameEventManager& two);
         
         void CreateEvents(GameEventManager& gemToFill)
         {
            unsigned int i;
            for (i=0; i<50; i++)
            {
               GameEvent *newEvent = new GameEvent("Event " + dtUtil::ToString(i));
               gemToFill.AddEvent(*newEvent);
            }   
         }
   };
   
   CPPUNIT_TEST_SUITE_REGISTRATION(GameEventManagerTests);
   
   //////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////
   void GameEventManagerTests::setUp()
   {
      try
      {
         mEventMgr = &GameEventManager::GetInstance();
         CPPUNIT_ASSERT_MESSAGE("Game event manager was invalid.",mEventMgr != NULL);
      }
      catch (const dtUtil::Exception& e)
      {
         CPPUNIT_FAIL(e.ToString());
      }
   
   }
   
   //////////////////////////////////////////////////////////////////////////
   void GameEventManagerTests::tearDown()
   {
      try
      {
         mEventMgr->ClearAllEvents();
         mEventMgr = NULL;
      }
      catch (const dtUtil::Exception& e)
      {
         CPPUNIT_FAIL(e.ToString());
      }
   }
   
   //////////////////////////////////////////////////////////////////////////
   void GameEventManagerTests::TestAddRemoveEvents()
   {
      try
      {
         unsigned int i;
         for (i=0; i<50; i++)
         {
            GameEvent *newEvent = new GameEvent("Event" + dtUtil::ToString(i));
            mEventMgr->AddEvent(*newEvent);
         }
   
         CPPUNIT_ASSERT_MESSAGE("Should have 50 events in the manager.",mEventMgr->GetNumEvents() == 50);
   
         GameEvent *found = mEventMgr->FindEvent("Event10");
         CPPUNIT_ASSERT_MESSAGE("Could not find event10.",found != NULL);
   
         mEventMgr->RemoveEvent(*found);
         CPPUNIT_ASSERT_MESSAGE("Should have one less event.",mEventMgr->GetNumEvents() == 49);
      }
      catch (const dtUtil::Exception& e)
      {
         CPPUNIT_FAIL(e.ToString());
      }
   }
   
   void GameEventManagerTests::CompareEventManagers(GameEventManager& one, GameEventManager& two)
   {
      CPPUNIT_ASSERT_EQUAL(one.GetNumEvents(), two.GetNumEvents());

      vector<GameEvent* > toFillOne;
      one.GetAllEvents(toFillOne);
      
      vector<GameEvent* > toFillTwo;
      two.GetAllEvents(toFillTwo);

      CPPUNIT_ASSERT_MESSAGE(
         "The list of events in the first should be the same as the copy.",
         toFillOne == toFillTwo);      
   }
   
   //////////////////////////////////////////////////////////////////////////
   void GameEventManagerTests::TestCopyConstructor()
   {
      CreateEvents(*mEventMgr);
      RefPtr<GameEventManager> copyGEM = new GameEventManager(*mEventMgr);

      CompareEventManagers(*mEventMgr, *copyGEM);
   }
   
   //////////////////////////////////////////////////////////////////////////
   void GameEventManagerTests::TestAssigmentOperator()
   {
      CreateEvents(*mEventMgr);
      RefPtr<GameEventManager> copyGEM = new GameEventManager();
      *copyGEM = *mEventMgr;
      
      CompareEventManagers(*mEventMgr, *copyGEM);
   }
   
   //////////////////////////////////////////////////////////////////////////
   void GameEventManagerTests::TestGetAllEvents()
   {
      try
      {
         CreateEvents(*mEventMgr);
   
         vector<GameEvent* > eventList;
         mEventMgr->GetAllEvents(eventList);
         for (unsigned i=0; i<25; i++)
         {
            GameEvent *found = mEventMgr->FindEvent(eventList[i]->GetUniqueId());
            CPPUNIT_ASSERT_MESSAGE("Could not search by unique id.",found != NULL);
            mEventMgr->RemoveEvent(eventList[i]->GetUniqueId());
         }
   
         CPPUNIT_ASSERT_MESSAGE("Should have 25 events in the manager.",mEventMgr->GetNumEvents() == 25);
         mEventMgr->ClearAllEvents();
         CPPUNIT_ASSERT_MESSAGE("Should have 0 events in the manager.",mEventMgr->GetNumEvents() == 0);
      }
      catch (const dtUtil::Exception& e)
      {
         CPPUNIT_FAIL(e.ToString());
      }
   }
}
