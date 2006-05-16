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
 * @author John K. Grant
 */
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/inputdevice.h>   // for testing the Button definition

namespace dtTest
{
   /// unit tests for dtCore::Button
   class ButtonTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE( ButtonTests );
      CPPUNIT_TEST( TestObservers );
      CPPUNIT_TEST_SUITE_END();

      public:
         void setUp();
         void tearDown();

         /// tests handling and order of handling of multiple listeners for key presses and releases.
         void TestObservers();

      private:
   };

   /// button test validator
   class HitObserver : public dtCore::ButtonListener
   {
   public:
      HitObserver(): mHit(false) {}

      void ResetHit() { mHit = false; }
      bool GetHit() const { return mHit; }

      virtual ~HitObserver() {}

   protected:
      bool mHit;
   };

   template<bool>
   class BObserver {};

   template<>
   class BObserver<true> : public HitObserver
   {
      bool ButtonStateChanged(const dtCore::Button* bt, bool oldstate, bool newstate)
      {
         mHit = true;
         return true;
      }
   };

   template<>
   class BObserver<false> : public HitObserver
   {
      bool ButtonStateChanged(const dtCore::Button* bt, bool oldstate, bool newstate)
      {
         mHit = true;
         return false;
      }
   };
}

CPPUNIT_TEST_SUITE_REGISTRATION( dtTest::ButtonTests );

using namespace dtTest;

void ButtonTests::setUp()
{
}

void ButtonTests::tearDown()
{
}

void ButtonTests::TestObservers()
{
   typedef BObserver<true> TrueObserver;
   typedef BObserver<false> FalseObserver;

   // -- make a button -- //
   // first, make the device it might happen to belong to.
   dtCore::RefPtr<dtCore::InputDevice> my_device(new dtCore::InputDevice("my_device"));
   // finally, make the button.
   dtCore::RefPtr<dtCore::Button> my_button(new dtCore::Button(my_device.get(),"my_button"));
   my_button->SetState(false);
   CPPUNIT_ASSERT( !my_button->GetState() );  // should have 'false' state

   // oh yeah, add it too, even though it has an owner
   my_device->AddFeature( my_button.get() );

   // verify it has been added
   CPPUNIT_ASSERT( my_device->GetFeatureCount()==1 );

   // assert for extra listeners
   CPPUNIT_ASSERT( my_button->GetListeners().size()==0 );

   // -- make a listener -- //
   FalseObserver my_listener;
   CPPUNIT_ASSERT( !my_listener.GetHit() );  // better not be hit

   // -- connect the listener -- //
   my_button->AddButtonListener( &my_listener );

   // assert for missing listeners
   CPPUNIT_ASSERT( my_button->GetListeners().size()==1 );

   // no check to see if it was handled, don't care for this test
   CPPUNIT_ASSERT( !my_button->SetState( !my_button->GetState() ) );   // make sure a change occurs, should not be handled by FalseObserver
   CPPUNIT_ASSERT( my_button->GetState() );  // should have 'true' state by now since original state was 'false'

   // check to see if my_listener was hit
   CPPUNIT_ASSERT( my_listener.GetHit() );   // better be hit

   // insert a new listener in front of the current listener
   TrueObserver my_listener2;
   my_button->InsertButtonListener( &my_listener, &my_listener2 );
   CPPUNIT_ASSERT( my_button->GetListeners().size()==2 );
   CPPUNIT_ASSERT( my_button->GetListeners().front() == &my_listener2 );

   // test chain of responsibility
   my_listener.ResetHit();
   CPPUNIT_ASSERT( !my_listener.GetHit() );  // better not be hit
   my_listener2.ResetHit();
   CPPUNIT_ASSERT( !my_listener2.GetHit() );  // better not be hit

   CPPUNIT_ASSERT( my_button->SetState( !my_button->GetState() ) );   // make sure a change occurs, should be handled by TrueObserver
   CPPUNIT_ASSERT( !my_listener.GetHit() );  // better not be hit
   CPPUNIT_ASSERT( my_listener2.GetHit() );  // better be hit
}
