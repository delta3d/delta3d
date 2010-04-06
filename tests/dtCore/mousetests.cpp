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
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/mouse.h>
#include <dtCore/genericmouselistener.h>
#include <dtABC/application.h>
#include <dtCore/system.h>

#include <osg/io_utils>

extern dtABC::Application& GetGlobalApplication();

namespace dtTest
{
   /// unit tests for dtCore::Keyboard
   class MouseTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(MouseTests);
      CPPUNIT_TEST(TestObservers);
      CPPUNIT_TEST(TestGenericObserver);
      CPPUNIT_TEST(TestAxes);
      //CPPUNIT_TEST(TestMousePosition);
      CPPUNIT_TEST_SUITE_END();

      public:
         void setUp();
         void tearDown();

         /// tests handling and order of handling of multiple listeners for key presses and releases.
         void TestObservers();

         /// tests dtCore::GenericMouseListener
         void TestGenericObserver();

         // Test change of state on the default mouse axes
         void TestAxes();

         // test setting and getting the mouse position
         void TestMousePosition();

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
         mHit(false),
         mDiffX(0.0f),
         mDiffY(0.0f)
      {
      }

      void ResetHit() { mHit = false; }
      bool GetHit() const { return mHit; }
      dtCore::Mouse::MouseButton GetButton() const { return mButton; }

      bool HandleButtonPressed(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button)
      {
         mHit = true;
         if (mHandlerType == BUTTON)
         {
            return mButton == button;
         }

         return false;
      }

      bool HandleButtonReleased(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button)
      {
         mHit = true;
         if (mHandlerType == BUTTON)
         {
            return mButton == button;
         }

         return false;
      }

      bool HandleButtonClicked(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button, int clickCount)
      {
         mHit = true;
         if (mHandlerType == BUTTON)
         {
            return mButton == button;
         }

         return false;
      }

      bool HandleMouseMoved(const dtCore::Mouse* mouse, float x, float y)
      {
         mHit = true;

         mDiffX = x;
         mDiffY = y;

         if (mHandlerType == MOVE)
         {
            return true;
         }

         return false;
      }

      bool HandleMouseDragged(const dtCore::Mouse* mouse, float x, float y)
      {
         mHit = true;
         if (mHandlerType == DRAG)
         {
            return true;
         }

         return false;
      }

      bool HandleMouseScrolled(const dtCore::Mouse* mouse, int delta)
      {
         mHit = true;
         if (mHandlerType == SCROLL)
         {
            return true;
         }

         return false;
      }

      float GetDiffX()
      {
         return mDiffX;
      }

      float GetDiffY()
      {
         return mDiffY;
      }

   protected:
      ~MouseObserver() {}

   private:
      HandlerType mHandlerType;
      dtCore::Mouse::MouseButton mButton;
      bool mHit;
      float mDiffX;
      float mDiffY;
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
CPPUNIT_TEST_SUITE_REGISTRATION(dtTest::MouseTests);

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
   CPPUNIT_ASSERT(ms->GetListeners().empty());  // better be empty

   dtCore::RefPtr<MouseObserver> lefty(new MouseObserver(MouseObserver::BUTTON,dtCore::Mouse::LeftButton));
   ms->AddMouseListener(lefty.get());
   CPPUNIT_ASSERT_EQUAL(1, (int)ms->GetListeners().size());  // better have first observer

   // re-use variables
   float x(1.0f), y(1.0f);

   // -- test observation -- //
   CPPUNIT_ASSERT(!ms->MouseScroll(osgGA::GUIEventAdapter::SCROLL_DOWN)); // scroll: lefty should NOT handle it
   CPPUNIT_ASSERT(!ms->MouseMotion(x,y));                                 // motion: lefty should NOT handle it
   CPPUNIT_ASSERT(!ms->PassiveMouseMotion(x,y));                          // passive: lefty should NOT handle it
   CPPUNIT_ASSERT(ms->ButtonDown(x,y,lefty->GetButton()));                // press: lefty should handle it
   CPPUNIT_ASSERT(ms->DoubleButtonDown(x,y,lefty->GetButton()));          // double: lefty should handle it
   CPPUNIT_ASSERT(ms->ButtonUp(x,y,lefty->GetButton()));                  // release: lefty should handle it
   CPPUNIT_ASSERT(lefty->GetHit());                                       // better be hit

   //if we set the mouse position the listener should not get notified
   ms->SetPosition(x, y);

   float diffX = lefty->GetDiffX();
   float diffY = lefty->GetDiffY();

   ms->SetPosition(0.25f * x, 0.5f * y);

   CPPUNIT_ASSERT_EQUAL(diffX, lefty->GetDiffX());
   CPPUNIT_ASSERT_EQUAL(diffY, lefty->GetDiffY());

   // -- test chain of responsibility -- //
   lefty->ResetHit();
   CPPUNIT_ASSERT(!lefty->GetHit());  // better NOT be hit

   dtCore::RefPtr<MouseObserver> movesy(new MouseObserver(MouseObserver::MOVE,dtCore::Mouse::LeftButton));
   ms->AddMouseListener(movesy.get());
   CPPUNIT_ASSERT_EQUAL(2, (int)ms->GetListeners().size());  // better have new observer
   CPPUNIT_ASSERT(!movesy->GetHit());  // better NOT be hit

   CPPUNIT_ASSERT(ms->ButtonDown(x,y,lefty->GetButton()));                       // press: lefty should handle it
   CPPUNIT_ASSERT(!movesy->GetHit());  // better NOT be hit

   CPPUNIT_ASSERT(ms->ButtonUp(x,y,lefty->GetButton()));                     // release: lefty should handle it
   CPPUNIT_ASSERT(!movesy->GetHit());  // better NOT be hit

   CPPUNIT_ASSERT(!movesy->GetHit());  // better be hit after down and up of same button

   movesy->ResetHit();
   CPPUNIT_ASSERT(ms->DoubleButtonDown(x,y,lefty->GetButton()));                 // double: lefty should handle it
   CPPUNIT_ASSERT(!movesy->GetHit());  // better NOT be hit

   CPPUNIT_ASSERT(!ms->MouseScroll(osgGA::GUIEventAdapter::SCROLL_DOWN)); // scroll: movesy should NOT handle it
   CPPUNIT_ASSERT(movesy->GetHit());  // better be hit

   CPPUNIT_ASSERT(!ms->MouseMotion(x,y));                                         // motion: movesy should NOT handle it
   CPPUNIT_ASSERT(ms->PassiveMouseMotion(x,y));                                  // passive: movesy should handle it
}

void MouseTests::TestGenericObserver()
{
   // make an instance to test
   dtCore::RefPtr<dtCore::GenericMouseListener> mgen(new dtCore::GenericMouseListener());

   // check that the default state will not use the callbacks
   CPPUNIT_ASSERT(!mgen->IsPressedCallbackEnabled());      // better not be enabled
   CPPUNIT_ASSERT(!mgen->IsReleasedCallbackEnabled());     // better not be enabled
   CPPUNIT_ASSERT(!mgen->IsClickedCallbackEnabled());      // better not be enabled
   CPPUNIT_ASSERT(!mgen->IsMovedCallbackEnabled());        // better not be enabled
   CPPUNIT_ASSERT(!mgen->IsDraggedCallbackEnabled());      // better not be enabled
   CPPUNIT_ASSERT(!mgen->IsScrolledCallbackEnabled());     // better not be enabled

   // hook up callbacks
   HasMouseCallbacks hcb;
   mgen->SetPressedCallback(dtCore::GenericMouseListener::ButtonCallbackType(&hcb,&HasMouseCallbacks::PressedHit));
   mgen->SetReleasedCallback(dtCore::GenericMouseListener::ButtonCallbackType(&hcb,&HasMouseCallbacks::ReleasedHit));
   mgen->SetClickedCallback(dtCore::GenericMouseListener::ClickCallbackType(&hcb,&HasMouseCallbacks::ClickedHit));
   mgen->SetMovedCallback(dtCore::GenericMouseListener::MovementCallbackType(&hcb,&HasMouseCallbacks::MovedHit));
   mgen->SetDraggedCallback(dtCore::GenericMouseListener::MovementCallbackType(&hcb,&HasMouseCallbacks::DraggedHit));
   mgen->SetScrolledCallback(dtCore::GenericMouseListener::WheelCallbackType(&hcb,&HasMouseCallbacks::ScrolledHit));

   // check to see if the callbacks will be used now
   CPPUNIT_ASSERT(mgen->IsPressedCallbackEnabled());      // better be enabled
   CPPUNIT_ASSERT(mgen->IsReleasedCallbackEnabled());     // better be enabled
   CPPUNIT_ASSERT(mgen->IsClickedCallbackEnabled());      // better be enabled
   CPPUNIT_ASSERT(mgen->IsMovedCallbackEnabled());        // better be enabled
   CPPUNIT_ASSERT(mgen->IsDraggedCallbackEnabled());      // better be enabled
   CPPUNIT_ASSERT(mgen->IsScrolledCallbackEnabled());     // better be enabled

   // reusable variables
   float x(0.0f), y(0.0f);
   dtCore::Mouse::MouseButton button(dtCore::Mouse::LeftButton);

   // test if the callbacks get hit
   dtCore::RefPtr<dtCore::Mouse> ms(new dtCore::Mouse(0));
   ms->AddMouseListener(mgen.get());

   CPPUNIT_ASSERT(!hcb.WasPressedHit());  // better not be hit
   CPPUNIT_ASSERT(ms->ButtonDown(x,y,button));  // hcb is coded to handle it.
   CPPUNIT_ASSERT(hcb.WasPressedHit());  // better be hit

   CPPUNIT_ASSERT(!hcb.WasReleasedHit());  // better not be hit
   CPPUNIT_ASSERT(ms->ButtonUp(x,y,button));  // hcb is coded to handle it.
   CPPUNIT_ASSERT(hcb.WasReleasedHit());  // better be hit

   ///\todo WHY DO WE HAVE A CALLBACK WHEN WE DON'T SUPPORT IT? take off the '!'
   CPPUNIT_ASSERT(!hcb.WasClickedHit());  // better be hit after down and up of same button

   CPPUNIT_ASSERT(!hcb.WasMovedHit());  // better not be hit
   CPPUNIT_ASSERT(ms->PassiveMouseMotion(x,y));  // hcb is coded to handle it.
   CPPUNIT_ASSERT(hcb.WasMovedHit());  // better be hit

   CPPUNIT_ASSERT(!hcb.WasDraggedHit());  // better not be hit
   CPPUNIT_ASSERT(ms->MouseMotion(x,y));  // hcb is coded to handle it.
   CPPUNIT_ASSERT(hcb.WasDraggedHit());  // better be hit

   CPPUNIT_ASSERT(!hcb.WasScrolledHit());  // better not be hit
   CPPUNIT_ASSERT(ms->MouseScroll(osgGA::GUIEventAdapter::SCROLL_UP));  // hcb is coded to handle it.
   CPPUNIT_ASSERT(hcb.WasScrolledHit());  // better be hit

   // disable the callbacks
   mgen->DisablePressedCallback();
   mgen->DisableReleasedCallback();
   mgen->DisableClickedCallback();
   mgen->DisableMovedCallback();
   mgen->DisableDraggedCallback();
   mgen->DisableScrolledCallback();

   // check to see the disable worked
   CPPUNIT_ASSERT(!mgen->IsPressedCallbackEnabled());      // better not be enabled
   CPPUNIT_ASSERT(!mgen->IsReleasedCallbackEnabled());     // better not be enabled
   CPPUNIT_ASSERT(!mgen->IsClickedCallbackEnabled());        // better not be enabled
   CPPUNIT_ASSERT(!mgen->IsMovedCallbackEnabled());        // better not be enabled
   CPPUNIT_ASSERT(!mgen->IsDraggedCallbackEnabled());        // better not be enabled
   CPPUNIT_ASSERT(!mgen->IsScrolledCallbackEnabled());        // better not be enabled

   // check to see if the callbacks get hit when callbacks are disabled
   hcb.ResetAllHits();
   CPPUNIT_ASSERT(!hcb.WasPressedHit());  // better not be hit
   CPPUNIT_ASSERT(!ms->ButtonDown(x,y,button));  // mgen is coded not to handle it when disabled.
   CPPUNIT_ASSERT(!hcb.WasPressedHit());  // better not be hit

   CPPUNIT_ASSERT(!hcb.WasReleasedHit());  // better not be hit
   CPPUNIT_ASSERT(!ms->ButtonUp(x,y,button));  // mgen is coded not to handle it when disabled.
   CPPUNIT_ASSERT(!hcb.WasReleasedHit());  // better not be hit

   CPPUNIT_ASSERT(!hcb.WasClickedHit());  // better not be hit after down and up of same button

   CPPUNIT_ASSERT(!hcb.WasMovedHit());  // better not be hit
   CPPUNIT_ASSERT(!ms->PassiveMouseMotion(x,y));  // mgen is coded not to handle it when disabled.
   CPPUNIT_ASSERT(!hcb.WasMovedHit());  // better not be hit

   CPPUNIT_ASSERT(!hcb.WasDraggedHit());  // better not be hit
   CPPUNIT_ASSERT(!ms->MouseMotion(x,y));  // mgen is coded not to handle it when disabled.
   CPPUNIT_ASSERT(!hcb.WasDraggedHit());  // better be hit

   CPPUNIT_ASSERT(!hcb.WasScrolledHit());  // better not be hit
   CPPUNIT_ASSERT(!ms->MouseScroll(osgGA::GUIEventAdapter::SCROLL_UP));  // mgen is coded not to handle it when disabled.
   CPPUNIT_ASSERT(!hcb.WasScrolledHit());  // better be hit
}

void MouseTests::TestAxes()
{
   dtCore::RefPtr<dtCore::GenericMouseListener> mgen(new dtCore::GenericMouseListener());

   // reusable variables
   float x(2.3f), y(8.45f);

   // test if the callbacks get hit
   dtCore::RefPtr<dtCore::Mouse> ms(new dtCore::Mouse(0));
   CPPUNIT_ASSERT_EQUAL(3, ms->GetAxisCount());
   CPPUNIT_ASSERT(ms->GetAxis(0) != NULL);
   CPPUNIT_ASSERT(ms->GetAxis(1) != NULL);
   CPPUNIT_ASSERT(ms->GetAxis(2) != NULL);
   CPPUNIT_ASSERT_EQUAL(static_cast<dtCore::Axis*>(NULL), ms->GetAxis(3));

   ms->AddMouseListener(mgen.get());

   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, ms->GetAxis(0)->GetState(), 0.001);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, ms->GetAxis(1)->GetState(), 0.001);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, ms->GetAxis(2)->GetState(), 0.001);

   ms->PassiveMouseMotion(x,y);

   CPPUNIT_ASSERT_DOUBLES_EQUAL(double(x), ms->GetAxis(0)->GetState(), 0.001);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(double(y), ms->GetAxis(1)->GetState(), 0.001);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, ms->GetAxis(2)->GetState(), 0.001);

   ms->MouseMotion(x,y);

   CPPUNIT_ASSERT_DOUBLES_EQUAL(double(x), ms->GetAxis(0)->GetState(), 0.001);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(double(y), ms->GetAxis(1)->GetState(), 0.001);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, ms->GetAxis(2)->GetState(), 0.001);

   ms->MouseScroll(osgGA::GUIEventAdapter::SCROLL_UP);

   // Verify the other axes are left untouched
   CPPUNIT_ASSERT_DOUBLES_EQUAL(double(x), ms->GetAxis(0)->GetState(), 0.001);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(double(y), ms->GetAxis(1)->GetState(), 0.001);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, ms->GetAxis(2)->GetState(), 0.001);

   ms->MouseScroll(osgGA::GUIEventAdapter::SCROLL_DOWN);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, ms->GetAxis(2)->GetState(), 0.001);

   ms->MouseScroll(osgGA::GUIEventAdapter::SCROLL_NONE);

   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, ms->GetAxis(2)->GetState(), 0.001);

   ms->MouseScroll(osgGA::GUIEventAdapter::SCROLL_UP);
   ms->MouseScroll(osgGA::GUIEventAdapter::SCROLL_UP);
   ms->MouseScroll(osgGA::GUIEventAdapter::SCROLL_UP);
   ms->MouseScroll(osgGA::GUIEventAdapter::SCROLL_DOWN);
   ms->MouseScroll(osgGA::GUIEventAdapter::SCROLL_UP);
   ms->MouseScroll(osgGA::GUIEventAdapter::SCROLL_UP);

   CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0, ms->GetAxis(2)->GetState(), 0.001);
}

void MouseTests::TestMousePosition()
{
   using namespace dtCore;

   //Had to use an Application, something to do with the osg Event queue
   //needing to be tickled.  Kind of a wacky test since it will actual
   //move the mouse pointer.  This test could fail if someone is moving
   //the mouse while the test is running.

   RefPtr<dtABC::Application> app = &GetGlobalApplication();
   System::GetInstance().Start();
   System::GetInstance().Step();

   const osg::Vec2 newPos(0.5f, 0.6f);
   app->GetMouse()->SetPosition(newPos);
   System::GetInstance().Step(); //required to get the event processed back to Delta3D

   //The Mouse position should be the same as what we just set
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Mouse was not set correctly",
      newPos, app->GetMouse()->GetPosition());
}
