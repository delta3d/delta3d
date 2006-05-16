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
#include <dtCore/keyboard.h>
#include <dtABC/application.h>
#include <dtABC/applicationkeyboardlistener.h>
#include <dtABC/applicationmouselistener.h>
#include <dtCore/refptr.h>

namespace dtTest
{
   /// unit tests for dtCore::BaseABC
   class BaseABCTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE( BaseABCTests );
      CPPUNIT_TEST( TestInput );
      CPPUNIT_TEST_SUITE_END();

      public:
         void setUp() {}
         void tearDown() {}

         void TestInput();

      private:
   };

   /// a quick concrete app to test things.
   class TestApp2 : public dtABC::Application
   {
   public:
      typedef dtABC::Application BaseClass;

      TestApp2(Producer::KeyboardKey key, Producer::KeyCharacter kc): BaseClass(),
         mPressedHit(false),
         mReleasedHit(false),
         mKey(key),
         mChar(kc)
      {
      }

      ~TestApp2()
      {
      }

      void Config()
      {
         BaseClass::Config();
      }

      void ResetHits()
      {
         mPressedHit = false;
         mReleasedHit = false;
      }

      bool KeyPressed(const dtCore::Keyboard* kb, Producer::KeyboardKey key, Producer::KeyCharacter kc)
      {
         mPressedHit = true;

         if( key==mKey && kc==mChar )
         {
            return true;
         }
         return false;
      }

      bool KeyReleased(const dtCore::Keyboard* kb, Producer::KeyboardKey key, Producer::KeyCharacter kc)
      {
         mReleasedHit = true;

         if( key==mKey && kc==mChar )
         {
            return true;
         }
         return false;
      }

      Producer::KeyboardKey GetKey() const { return mKey; }
      Producer::KeyCharacter GetCharacter() const { return mChar; }
      bool GetPressedHit() const { return mPressedHit; }
      bool GetReleasedHit() const { return mReleasedHit; }

   private:
      bool mPressedHit;
      bool mReleasedHit;
      Producer::KeyboardKey mKey;
      Producer::KeyCharacter mChar;
   };

   /// a quick subclass for tests
   class UserKBL : public dtABC::ApplicationKeyboardListener
   {
   };

   /// a quick subclass for tests
   class UserMSL : public dtABC::ApplicationMouseListener
   {
   };
}

CPPUNIT_TEST_SUITE_REGISTRATION( dtTest::BaseABCTests );

using namespace dtTest;

void BaseABCTests::TestInput()
{
   // it is really weird that an app must be used to test the BaseABC function,
   // but this is how it must be when Application allocates BaseABC's members, like mDeltaWin.
   dtCore::RefPtr<TestApp2> app(new TestApp2(Producer::Key_0,Producer::KeyChar_0));

   dtCore::RefPtr<dtABC::ApplicationKeyboardListener> defaultkbl = app->GetApplicationKeyboardListener();
   dtCore::RefPtr<dtABC::ApplicationMouseListener> defaultmsl = app->GetApplicationMouseListener();

   app->SetApplicationKeyboardListener( new UserKBL() );
   app->SetApplicationMouseListener( new UserMSL() );

   dtCore::RefPtr<dtABC::ApplicationKeyboardListener> userkbl = app->GetApplicationKeyboardListener();
   dtCore::RefPtr<dtABC::ApplicationMouseListener> usermsl = app->GetApplicationMouseListener();

   CPPUNIT_ASSERT(defaultkbl != userkbl);  // should NOT be the same pointer
   CPPUNIT_ASSERT(defaultmsl != usermsl);  // should NOT be the same pointer
}
