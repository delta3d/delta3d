/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2006-2008, Alion Science and Technology Corporation
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
* This software was developed by Alion Science and Technology Corporation under
* circumstances in which the U. S. Government may have rights in the software.
*
* @author Matthew W. Campbell
*/
#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>

#include <vector>
#include <dtCore/refptr.h>

#include <dtUtil/exception.h>
#include <dtUtil/stringutils.h>

#include <dtCore/gameeventmanager.h>
#include <dtCore/gameevent.h>

using dtCore::RefPtr;
using std::vector;

namespace dtCore
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
