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
#include <dtCore/genericmouselistener.h>

namespace dtTest
{
   /// unit tests for dtCore::Keyboard
   class MouseTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE( MouseTests );
      CPPUNIT_TEST( TestObservers );
      CPPUNIT_TEST( TestGenericObserver );
      CPPUNIT_TEST_SUITE_END();

      public:
         void setUp();
         void tearDown();

         /// tests handling and order of handling of multiple listeners for key presses and releases.
         void TestObservers();

         /// tests dtCore::GenericMouseListener
         void TestGenericObserver();

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

   // a quick class to find out if we have hit the callbacks of an arbitrary class
   class HasMouseCallbacks
   {
   public:
      HasMouseCallbacks():
         mPressedHit(false), 
         mReleasedHit(false),
         mClickedHit(false),
         mMovedHit(false),
         mDraggedHit(false),
         mScrolledHit(false)
      {
      }

      ~HasMouseCallbacks() {}

      bool PressedHit(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button) { mPressedHit=true; return true; }
      bool ReleasedHit(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button) { mReleasedHit=true; return true; }
      bool ClickedHit(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button, int clickCount) { mClickedHit=true; return true; }
      bool MovedHit(const dtCore::Mouse* mouse, float x, float y)  { mMovedHit=true; return true; }
      bool DraggedHit(const dtCore::Mouse* mouse, float x, float y) { mDraggedHit=true; return true; }
      bool ScrolledHit(const dtCore::Mouse* mouse, int delta) { mScrolledHit=true; return true; }

      bool WasPressedHit() const { return mPressedHit; }
      bool WasReleasedHit() const { return mReleasedHit; }
      bool WasClickedHit() const { return mClickedHit; }
      bool WasMovedHit() const { return mMovedHit; }
      bool WasDraggedHit() const { return mDraggedHit; }
      bool WasScrolledHit() const { return mScrolledHit; }

      void ResetAllHits()
      {
         mPressedHit=false;
         mReleasedHit=false;
         mClickedHit=false;
         mMovedHit=false;
         mDraggedHit=false;
         mScrolledHit=false;
      }

   private:
      bool mPressedHit, mReleasedHit, mClickedHit, mMovedHit, mDraggedHit, mScrolledHit;
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

void MouseTests::TestObservers()
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

   CPPUNIT_ASSERT( ms->ButtonUp(x,y,lefty->GetButton()) );                     // release: lefty should handle it
   CPPUNIT_ASSERT( !movesy->GetHit() );  // better NOT be hit

   CPPUNIT_ASSERT( !movesy->GetHit() );  // better be hit after down and up of same button

   movesy->ResetHit();
   CPPUNIT_ASSERT( ms->DoubleButtonDown(x,y,lefty->GetButton()) );                 // double: lefty should handle it
   CPPUNIT_ASSERT( !movesy->GetHit() );  // better NOT be hit

   CPPUNIT_ASSERT( !ms->MouseScroll(Producer::KeyboardMouseCallback::ScrollDown) ); // scroll: movesy should NOT handle it
   CPPUNIT_ASSERT( movesy->GetHit() );  // better be hit

   CPPUNIT_ASSERT( !ms->MouseMotion(x,y) );                                         // motion: movesy should NOT handle it
   CPPUNIT_ASSERT( ms->PassiveMouseMotion(x,y) );                                  // passive: movesy should handle it
}

void MouseTests::TestGenericObserver()
{
   // make an instance to test
   dtCore::RefPtr<dtCore::GenericMouseListener> mgen(new dtCore::GenericMouseListener());

   // check that the default state will not use the callbacks
   CPPUNIT_ASSERT( !mgen->IsPressedCallbackEnabled() );      // better not be enabled
   CPPUNIT_ASSERT( !mgen->IsReleasedCallbackEnabled() );     // better not be enabled
   CPPUNIT_ASSERT( !mgen->IsClickedCallbackEnabled() );      // better not be enabled
   CPPUNIT_ASSERT( !mgen->IsMovedCallbackEnabled() );        // better not be enabled
   CPPUNIT_ASSERT( !mgen->IsDraggedCallbackEnabled() );      // better not be enabled
   CPPUNIT_ASSERT( !mgen->IsScrolledCallbackEnabled() );     // better not be enabled

   // hook up callbacks
   HasMouseCallbacks hcb;
   mgen->SetPressedCallback(dtCore::GenericMouseListener::ButtonCallbackType(&hcb,&HasMouseCallbacks::PressedHit));
   mgen->SetReleasedCallback(dtCore::GenericMouseListener::ButtonCallbackType(&hcb,&HasMouseCallbacks::ReleasedHit));
   mgen->SetClickedCallback(dtCore::GenericMouseListener::ClickCallbackType(&hcb,&HasMouseCallbacks::ClickedHit));
   mgen->SetMovedCallback(dtCore::GenericMouseListener::MovementCallbackType(&hcb,&HasMouseCallbacks::MovedHit));
   mgen->SetDraggedCallback(dtCore::GenericMouseListener::MovementCallbackType(&hcb,&HasMouseCallbacks::DraggedHit));
   mgen->SetScrolledCallback(dtCore::GenericMouseListener::WheelCallbackType(&hcb,&HasMouseCallbacks::ScrolledHit));

   // check to see if the callbacks will be used now
   CPPUNIT_ASSERT( mgen->IsPressedCallbackEnabled() );      // better be enabled
   CPPUNIT_ASSERT( mgen->IsReleasedCallbackEnabled() );     // better be enabled
   CPPUNIT_ASSERT( mgen->IsClickedCallbackEnabled() );      // better be enabled
   CPPUNIT_ASSERT( mgen->IsMovedCallbackEnabled() );        // better be enabled
   CPPUNIT_ASSERT( mgen->IsDraggedCallbackEnabled() );      // better be enabled
   CPPUNIT_ASSERT( mgen->IsScrolledCallbackEnabled() );     // better be enabled

   // reusable variables
   float x(0.0f), y(0.0f);
   dtCore::Mouse::MouseButton button(dtCore::Mouse::LeftButton);

   // test if the callbacks get hit
   dtCore::RefPtr<dtCore::Mouse> ms(new dtCore::Mouse(0));
   ms->AddMouseListener( mgen.get() );

   CPPUNIT_ASSERT( !hcb.WasPressedHit() );  // better not be hit
   CPPUNIT_ASSERT( ms->ButtonDown(x,y,button) );  // hcb is coded to handle it.
   CPPUNIT_ASSERT( hcb.WasPressedHit() );  // better be hit

   CPPUNIT_ASSERT( !hcb.WasReleasedHit() );  // better not be hit
   CPPUNIT_ASSERT( ms->ButtonUp(x,y,button) );  // hcb is coded to handle it.
   CPPUNIT_ASSERT( hcb.WasReleasedHit() );  // better be hit

   ///\todo WHY DO WE HAVE A CALLBACK WHEN WE DON'T SUPPORT IT? take off the '!'
   CPPUNIT_ASSERT( !hcb.WasClickedHit() );  // better be hit after down and up of same button

   CPPUNIT_ASSERT( !hcb.WasMovedHit() );  // better not be hit
   CPPUNIT_ASSERT( ms->PassiveMouseMotion(x,y) );  // hcb is coded to handle it.
   CPPUNIT_ASSERT( hcb.WasMovedHit() );  // better be hit

   CPPUNIT_ASSERT( !hcb.WasDraggedHit() );  // better not be hit
   CPPUNIT_ASSERT( ms->MouseMotion(x,y) );  // hcb is coded to handle it.
   CPPUNIT_ASSERT( hcb.WasDraggedHit() );  // better be hit

   CPPUNIT_ASSERT( !hcb.WasScrolledHit() );  // better not be hit
   CPPUNIT_ASSERT( ms->MouseScroll(Producer::KeyboardMouseCallback::ScrollUp) );  // hcb is coded to handle it.
   CPPUNIT_ASSERT( hcb.WasScrolledHit() );  // better be hit

   // disable the callbacks
   mgen->DisablePressedCallback();
   mgen->DisableReleasedCallback();
   mgen->DisableClickedCallback();
   mgen->DisableMovedCallback();
   mgen->DisableDraggedCallback();
   mgen->DisableScrolledCallback();

   // check to see the disable worked
   CPPUNIT_ASSERT( !mgen->IsPressedCallbackEnabled() );      // better not be enabled
   CPPUNIT_ASSERT( !mgen->IsReleasedCallbackEnabled() );     // better not be enabled
   CPPUNIT_ASSERT( !mgen->IsClickedCallbackEnabled() );        // better not be enabled
   CPPUNIT_ASSERT( !mgen->IsMovedCallbackEnabled() );        // better not be enabled
   CPPUNIT_ASSERT( !mgen->IsDraggedCallbackEnabled() );        // better not be enabled
   CPPUNIT_ASSERT( !mgen->IsScrolledCallbackEnabled() );        // better not be enabled

   // check to see if the callbacks get hit when callbacks are disabled
   hcb.ResetAllHits();
   CPPUNIT_ASSERT( !hcb.WasPressedHit() );  // better not be hit
   CPPUNIT_ASSERT( !ms->ButtonDown(x,y,button) );  // mgen is coded not to handle it when disabled.
   CPPUNIT_ASSERT( !hcb.WasPressedHit() );  // better not be hit

   CPPUNIT_ASSERT( !hcb.WasReleasedHit() );  // better not be hit
   CPPUNIT_ASSERT( !ms->ButtonUp(x,y,button) );  // mgen is coded not to handle it when disabled.
   CPPUNIT_ASSERT( !hcb.WasReleasedHit() );  // better not be hit

   CPPUNIT_ASSERT( !hcb.WasClickedHit() );  // better not be hit after down and up of same button

   CPPUNIT_ASSERT( !hcb.WasMovedHit() );  // better not be hit
   CPPUNIT_ASSERT( !ms->PassiveMouseMotion(x,y) );  // mgen is coded not to handle it when disabled.
   CPPUNIT_ASSERT( !hcb.WasMovedHit() );  // better not be hit

   CPPUNIT_ASSERT( !hcb.WasDraggedHit() );  // better not be hit
   CPPUNIT_ASSERT( !ms->MouseMotion(x,y) );  // mgen is coded not to handle it when disabled.
   CPPUNIT_ASSERT( !hcb.WasDraggedHit() );  // better be hit

   CPPUNIT_ASSERT( !hcb.WasScrolledHit() );  // better not be hit
   CPPUNIT_ASSERT( !ms->MouseScroll(Producer::KeyboardMouseCallback::ScrollUp) );  // mgen is coded not to handle it when disabled.
   CPPUNIT_ASSERT( !hcb.WasScrolledHit() );  // better be hit
}
