/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
#include <dtCore/keyboard.h>

namespace dtTest
{
   /// unit tests for dtCore::Keyboard
   class KeyboardTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE( KeyboardTests );
      CPPUNIT_TEST( TestObservers );
      CPPUNIT_TEST_SUITE_END();

      public:
         void setUp();
         void tearDown();

         /// tests handling and order of handling of multiple listeners for key presses and releases.
         void TestObservers();

      private:
   };

   /// keyboard test validator
   class KeyCharObserver : public dtCore::KeyboardListener
   {
   public:
      KeyCharObserver(Producer::KeyCharacter key): mKey(key), mHit(false) {}

      void ResetHit() { mHit = false; }
      bool GetHit() const { return mHit; }
      Producer::KeyCharacter GetKeyChar() const { return mKey; }

      bool HandleKeyPressed(const dtCore::Keyboard* keyboard, Producer::KeyboardKey key, Producer::KeyCharacter character)
      {
         mHit = true;
         return( mKey == character );
      }

      bool HandleKeyReleased(const dtCore::Keyboard* keyboard, Producer::KeyboardKey key, Producer::KeyCharacter character)
      {
         mHit = true;
         return( mKey == character );
      }

      bool HandleKeyTyped(const dtCore::Keyboard* keyboard, Producer::KeyboardKey key,Producer::KeyCharacter character)
      {
         mHit = true;
         return( mKey == character );
      }

   protected:
      virtual ~KeyCharObserver() {}

   private:
      Producer::KeyCharacter mKey;
      bool mHit;
   };
}

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( dtTest::KeyboardTests );

using namespace dtTest;

void KeyboardTests::setUp()
{
}

void KeyboardTests::tearDown()
{
}

void KeyboardTests::TestObservers()
{
   dtCore::RefPtr<dtCore::Keyboard> kb(new dtCore::Keyboard());
   CPPUNIT_ASSERT( kb->GetListeners().empty() );  // better be empty

   dtCore::RefPtr<KeyCharObserver> obs(new KeyCharObserver(Producer::KeyChar_H));
   CPPUNIT_ASSERT_EQUAL( Producer::KeyChar_H , obs->GetKeyChar() );  // better be H
   kb->AddKeyboardListener( obs.get() );

   CPPUNIT_ASSERT_EQUAL( 1 , (int)kb->GetListeners().size() );  // better be 1

   // ------------------------------------- //
   // test press
   CPPUNIT_ASSERT( kb->KeyPress( obs->GetKeyChar() ) );   // obs should handle it
   CPPUNIT_ASSERT( !kb->KeyPress(Producer::KeyChar_K) );  // obs should not handle it

   // test release
   CPPUNIT_ASSERT( kb->KeyRelease( obs->GetKeyChar() ) );   // obs should handle it
   CPPUNIT_ASSERT( !kb->KeyRelease(Producer::KeyChar_K) );  // obs should not handle it

   // ------------------------------------- //
   // test extra listener
   dtCore::RefPtr<KeyCharObserver> obs2(new KeyCharObserver(Producer::KeyChar_K));
   CPPUNIT_ASSERT_EQUAL( Producer::KeyChar_K , obs2->GetKeyChar() );  // better be K
   kb->AddKeyboardListener( obs2.get() );
   CPPUNIT_ASSERT_EQUAL( 2 , (int)kb->GetListeners().size() );  // better be 2

   // test press
   CPPUNIT_ASSERT( !kb->KeyPress(Producer::KeyChar_A) );  // none should handle it
   CPPUNIT_ASSERT( kb->KeyPress( obs2->GetKeyChar() ) );  // obs2 should handle it
   CPPUNIT_ASSERT( kb->KeyRelease( obs->GetKeyChar() ) );  // obs should handle it

   // test release
   CPPUNIT_ASSERT( !kb->KeyRelease(Producer::KeyChar_A) );  // none should handle it
   CPPUNIT_ASSERT( kb->KeyRelease( obs2->GetKeyChar() ) );  // obs2 should handle it
   CPPUNIT_ASSERT( kb->KeyRelease( obs->GetKeyChar() ) );  // obs should handle it

   // ------------------------------------- //
   // test ordering (chain of responsibility)
   // test press
   obs->ResetHit();
   CPPUNIT_ASSERT( !obs->GetHit() );  // better be reset
   obs2->ResetHit();
   CPPUNIT_ASSERT( !obs2->GetHit() );  // better be reset

   CPPUNIT_ASSERT( kb->KeyPress( obs->GetKeyChar() ) );       // obs should handle it
   CPPUNIT_ASSERT( obs->GetHit() );    // better have been hit since it is first in listener list.
   CPPUNIT_ASSERT( !obs2->GetHit() );   // better have NOT been hit since previous listener handles it.

   // test release
   obs->ResetHit();
   CPPUNIT_ASSERT( !obs->GetHit() );  // better be reset
   obs2->ResetHit();
   CPPUNIT_ASSERT( !obs2->GetHit() );  // better be reset

   CPPUNIT_ASSERT( kb->KeyRelease( obs->GetKeyChar() ) ); // obs should handle it
   CPPUNIT_ASSERT( obs->GetHit() );    // better have been hit since it is first in listener list.
   CPPUNIT_ASSERT( !obs2->GetHit() );   // better have NOT been hit since previous listener handles it.

   // ------------------------------------- //
   // test "special" keys
   dtCore::RefPtr<KeyCharObserver> obsEsc(new KeyCharObserver(Producer::KeyChar_Escape));
   CPPUNIT_ASSERT_EQUAL( Producer::KeyChar_Escape , obsEsc->GetKeyChar() );  // better be Escape
   kb->AddKeyboardListener( obsEsc.get() );
   CPPUNIT_ASSERT_EQUAL( 3 , (int)kb->GetListeners().size() );  // better be 3

   dtCore::RefPtr<KeyCharObserver> obsSL(new KeyCharObserver(Producer::KeyChar_Scroll_Lock));
   CPPUNIT_ASSERT_EQUAL( Producer::KeyChar_Scroll_Lock, obsSL->GetKeyChar() );  // better be Scroll Lock
   kb->AddKeyboardListener( obsSL.get() );
   CPPUNIT_ASSERT_EQUAL( 4 , (int)kb->GetListeners().size() );  // better be 3

   // test press
   obsEsc->ResetHit();
   CPPUNIT_ASSERT( !obsEsc->GetHit() );  // better be reset
   obsSL->ResetHit();
   CPPUNIT_ASSERT( !obsSL->GetHit() );  // better be reset

   CPPUNIT_ASSERT( kb->KeyPress( obsEsc->GetKeyChar() ) ); // osbEsc should handle it
   CPPUNIT_ASSERT( obsEsc->GetHit() );    // better have been hit since it handles it.
   CPPUNIT_ASSERT( !obsSL->GetHit() );    // better have NOT been hit since it is after the handler.
   CPPUNIT_ASSERT( kb->KeyPress( obsSL->GetKeyChar() ) ); // obsSL should handle it

   // test release
   obsEsc->ResetHit();
   CPPUNIT_ASSERT( !obsEsc->GetHit() );  // better be reset
   obsSL->ResetHit();
   CPPUNIT_ASSERT( !obsSL->GetHit() );  // better be reset

   CPPUNIT_ASSERT( kb->KeyRelease( obsEsc->GetKeyChar() ) ); // osbEsc should handle it
   CPPUNIT_ASSERT( obsEsc->GetHit() );    // better have been hit since it handles it.
   CPPUNIT_ASSERT( !obsSL->GetHit() );    // better have NOT been hit since it is after the handler.

   CPPUNIT_ASSERT( kb->KeyRelease( obsSL->GetKeyChar() ) );      // obsSL should handle it
}

