/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Delta3D
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
 * @author John K. Grant
 */

#include <cppunit/extensions/HelperMacros.h>  // for the test framework
#include <dtGUI/scriptmodule.h>               // for testing this class

namespace dtTest
{
   /// unit tests for dtGUI::ScriptModule
   class ScriptModuleTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE( ScriptModuleTests );
      CPPUNIT_TEST( TestAddCallbacks );
      CPPUNIT_TEST_SUITE_END();

      public:
         void setUp() {}
         void tearDown() {}

         /// tests adding callbacks
         void TestAddCallbacks();

      private:
   };

   class EventObserver
   {
   public:
      EventObserver() {}
      ~EventObserver() {}

      static bool StaticCallback(const CEGUI::EventArgs& ea) { return false; }
      bool MemFunCallback(const CEGUI::EventArgs& ea) { return false; }

      void DoSomething() {}
   };
}

CPPUNIT_TEST_SUITE_REGISTRATION( dtTest::ScriptModuleTests );

using namespace dtTest;

void ScriptModuleTests::TestAddCallbacks()
{
   EventObserver obs;

   {
      dtGUI::ScriptModule sm;
      CPPUNIT_ASSERT( sm.GetRegistry().size()==0 );

      // test the static overload
      sm.AddCallback("StaticCallback" , &EventObserver::StaticCallback );
      CPPUNIT_ASSERT( sm.GetRegistry().size()==1 );

      // test the functor overload
      dtGUI::ScriptModule::HandlerFunctor functor(&obs,&EventObserver::MemFunCallback);
      sm.AddCallback("Functor" , functor );
      CPPUNIT_ASSERT( sm.GetRegistry().size()==2 );

      ////// test the memfun overload
      //sm.AddCallback("MemFunCallback", &EventObserver::MemFunCallback , &obs );
      //CPPUNIT_ASSERT( sm.GetRegistry().size()==3 );
   }

   obs.DoSomething();
}
