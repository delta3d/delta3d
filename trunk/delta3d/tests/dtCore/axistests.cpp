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
#include <dtCore/inputdevice.h>   // for testing the Axis definition

namespace dtTest
{
   /// unit tests for dtCore::Axis
   class AxisTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE( AxisTests );
      CPPUNIT_TEST( TestObservers );
      CPPUNIT_TEST_SUITE_END();

      public:
         void setUp();
         void tearDown();

         /// tests handling and order of handling of multiple listeners for state changes.
         void TestObservers();

      private:
   };

   /// button test validator
   template<class BaseT>
   class HitObserver2 : public BaseT
   {
   public:
      HitObserver2(): mHit(false) {}

      void ResetHit() { mHit = false; }
      bool GetHit() const { return mHit; }

      virtual ~HitObserver2() {}

   protected:
      bool mHit;
   };

   /// definition of concrete AxisListener
   template<bool>
   class AObserver {};

   /// partial specialization of AObserver
   template<>
   class AObserver<true> : public HitObserver2<dtCore::AxisListener>
   {
      bool AxisStateChanged(const dtCore::Axis* axis, double oldState, double newState, double delta)
      {
         mHit = true;
         return true;
      }
   };

   /// partial specialization of AObserver
   template<>
   class AObserver<false> : public HitObserver2<dtCore::AxisListener>
   {
      bool AxisStateChanged(const dtCore::Axis* axis, double oldState, double newState, double delta)
      {
         mHit = true;
         return false;
      }
   };
}

CPPUNIT_TEST_SUITE_REGISTRATION( dtTest::AxisTests );

using namespace dtTest;

void AxisTests::setUp()
{
}

void AxisTests::tearDown()
{
}

void AxisTests::TestObservers()
{
   typedef AObserver<true> TrueObserver;
   typedef AObserver<false> FalseObserver;

   // -- make a button -- //
   // first, make the device it might happen to belong to.
   dtCore::RefPtr<dtCore::InputDevice> my_device(new dtCore::InputDevice("my_device"));
   // finally, make the button.
   dtCore::RefPtr<dtCore::Axis> my_axis(new dtCore::Axis(my_device.get(),"my_axis"));
   my_axis->SetState(false);
   CPPUNIT_ASSERT( !my_axis->GetState() );  // should have 'false' state

   // oh yeah, add it too, even though it has an owner
   my_device->AddFeature( my_axis.get() );

   // verify it has been added
   CPPUNIT_ASSERT( my_device->GetFeatureCount()==1 );

   // assert for extra listeners
   CPPUNIT_ASSERT( my_axis->GetListeners().size()==0 );

   // -- make a listener -- //
   FalseObserver my_listener;
   CPPUNIT_ASSERT( !my_listener.GetHit() );  // better not be hit

   // -- connect the listener -- //
   my_axis->AddAxisListener( &my_listener );

   // assert for missing listeners
   CPPUNIT_ASSERT( my_axis->GetListeners().size()==1 );

   // no check to see if it was handled, don't care for this test
   CPPUNIT_ASSERT( !my_axis->SetState( my_axis->GetState()+1.0 ) );   // make sure a change occurs, should not be handled by FalseObserver
   CPPUNIT_ASSERT( my_axis->GetState() );  // should have 'true' state by now since original state was 'false'

   // check to see if my_listener was hit
   CPPUNIT_ASSERT( my_listener.GetHit() );   // better be hit

   // insert a new listener in front of the current listener
   TrueObserver my_listener2;
   my_axis->InsertAxisListener( &my_listener, &my_listener2 );
   CPPUNIT_ASSERT( my_axis->GetListeners().size()==2 );
   CPPUNIT_ASSERT( my_axis->GetListeners().front() == &my_listener2 );

   // test chain of responsibility
   my_listener.ResetHit();
   CPPUNIT_ASSERT( !my_listener.GetHit() );  // better not be hit
   my_listener2.ResetHit();
   CPPUNIT_ASSERT( !my_listener2.GetHit() );  // better not be hit

   CPPUNIT_ASSERT( my_axis->SetState( my_axis->GetState()+1.0 ) );   // make sure a change occurs, should be handled by TrueObserver
   CPPUNIT_ASSERT( !my_listener.GetHit() );  // better not be hit
   CPPUNIT_ASSERT( my_listener2.GetHit() );  // better be hit
}
