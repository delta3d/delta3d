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
#include <dtCore/mouse.h>

namespace dtTest
{
   /// unit tests for dtCore::Keyboard
   class MouseTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE( MouseTests );
      CPPUNIT_TEST( TestClickObservers );
      CPPUNIT_TEST_SUITE_END();

      public:
         void setUp();
         void tearDown();

         /// tests handling and order of handling of multiple listeners for key presses and releases.
         void TestClickObservers();

      private:
   };

   /// mouse test validator
   class MouseObserver : public dtCore::MouseListener
   {
   public:
      typedef dtCore::MouseListener BaseClass;

      enum HandlerType
      {
         BUTTON,
         MOVE,
         DRAG,
         SCROLL
      };

      MouseObserver(HandlerType handler, dtCore::Mouse::MouseButton button): BaseClass(),
         mHandlerType(handler),
         mButton(button),
         mHit(false)
      {
      }

      void ResetHit() { mHit = false; }
      bool GetHit() const { return mHit; }
      dtCore::Mouse::MouseButton GetButton() const { return mButton; }

      bool HandleButtonPressed(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button)
      {
         mHit = true;
         if( mHandlerType == BUTTON )
         {
            return( mButton == button );
         }

         return false;
      }

      bool HandleButtonReleased(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button)
      {
         mHit = true;
         if( mHandlerType == BUTTON )
         {
            return( mButton == button );
         }

         return false;
      }

      bool HandleButtonClicked(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button, int clickCount)
      {
         mHit = true;
         if( mHandlerType == BUTTON )
         {
            return( mButton == button );
         }

         return false;
      }

      bool HandleMouseMoved(const dtCore::Mouse* mouse, float x, float y)
      {
         mHit = true;

         if( mHandlerType == MOVE )
         {
            return true;
         }

         return false;
      }

      bool HandleMouseDragged(const dtCore::Mouse* mouse, float x, float y)
      {
         mHit = true;
         if( mHandlerType == DRAG )
         {
            return true;
         }

         return false;
      }

      bool HandleMouseScrolled(const dtCore::Mouse* mouse, int delta)
      {
         mHit = true;
         if( mHandlerType == SCROLL )
         {
            return true;
         }

         return false;
      }

   protected:
      ~MouseObserver() {}

   private:
      HandlerType mHandlerType;
      dtCore::Mouse::MouseButton mButton;
      bool mHit;
   };
}

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( dtTest::MouseTests );

using namespace dtTest;

void MouseTests::setUp()
{
}

void MouseTests::tearDown()
{
}

void MouseTests::TestClickObservers()
{
   dtCore::RefPtr<dtCore::Mouse> ms = new dtCore::Mouse(0);
   CPPUNIT_ASSERT( ms->GetListeners().empty() );  // better be empty

   dtCore::RefPtr<MouseObserver> lefty(new MouseObserver(MouseObserver::BUTTON,dtCore::Mouse::LeftButton));
   ms->AddMouseListener( lefty.get() );
   CPPUNIT_ASSERT_EQUAL( 1 , (int)ms->GetListeners().size() );  // better have first observer

   // re-use variables
   float x(1.0f), y(1.0f);

   // -- test observation -- //
   CPPUNIT_ASSERT( !ms->MouseScroll(Producer::KeyboardMouseCallback::ScrollDown) ); // scroll: lefty should NOT handle it
   CPPUNIT_ASSERT( !ms->MouseMotion(x,y) );                                         // motion: lefty should NOT handle it
   CPPUNIT_ASSERT( !ms->PassiveMouseMotion(x,y) );                                  // passive: lefty should NOT handle it
   CPPUNIT_ASSERT( ms->ButtonDown(x,y,lefty->GetButton()) );                     // press: lefty should handle it
   CPPUNIT_ASSERT( ms->DoubleButtonDown(x,y,lefty->GetButton()) );                 // double: lefty should handle it
   CPPUNIT_ASSERT( ms->ButtonUp(x,y,lefty->GetButton()) );                     // release: lefty should handle it
   CPPUNIT_ASSERT( lefty->GetHit() );                                               // better be hit

   // -- test chain of responsibility -- //
   lefty->ResetHit();
   CPPUNIT_ASSERT( !lefty->GetHit() );  // better NOT be hit

   dtCore::RefPtr<MouseObserver> movesy(new MouseObserver(MouseObserver::MOVE,dtCore::Mouse::LeftButton));
   ms->AddMouseListener( movesy.get() );
   CPPUNIT_ASSERT_EQUAL( 2 , (int)ms->GetListeners().size() );  // better have new observer
   CPPUNIT_ASSERT( !movesy->GetHit() );  // better NOT be hit

   CPPUNIT_ASSERT( ms->ButtonDown(x,y,lefty->GetButton()) );                       // press: lefty should handle it
   CPPUNIT_ASSERT( !movesy->GetHit() );  // better NOT be hit

   CPPUNIT_ASSERT( ms->DoubleButtonDown(x,y,lefty->GetButton()) );                 // double: lefty should handle it
   CPPUNIT_ASSERT( !movesy->GetHit() );  // better NOT be hit

   CPPUNIT_ASSERT( ms->ButtonUp(x,y,lefty->GetButton()) );                     // release: lefty should handle it
   CPPUNIT_ASSERT( !movesy->GetHit() );  // better NOT be hit

   CPPUNIT_ASSERT( !ms->MouseScroll(Producer::KeyboardMouseCallback::ScrollDown) ); // scroll: movesy should NOT handle it
   CPPUNIT_ASSERT( movesy->GetHit() );  // better be hit

   CPPUNIT_ASSERT( !ms->MouseMotion(x,y) );                                         // motion: movesy should NOT handle it
   CPPUNIT_ASSERT( ms->PassiveMouseMotion(x,y) );                                  // passive: movesy should handle it
}

