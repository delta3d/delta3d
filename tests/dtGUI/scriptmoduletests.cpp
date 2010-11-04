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
* @author John K. Grant
*/
#include <prefix/unittestprefix.h>
//#include <prefix/dtgameprefix.h>
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
