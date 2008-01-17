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
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/keyboard.h>
#include <dtABC/application.h>
#include <dtCore/refptr.h>
#include <dtCore/deltawin.h>

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

      TestApp2(int key): BaseClass(),
         mPressedHit(false),
         mReleasedHit(false),
         mKey(key)
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

      bool KeyPressed(const dtCore::Keyboard* kb, int key)
      {
         mPressedHit = true;

         if( key==mKey)
         {
            return true;
         }
         return false;
      }

      bool KeyReleased(const dtCore::Keyboard* kb, int key)
      {
         mReleasedHit = true;

         if( key==mKey)
         {
            return true;
         }
         return false;
      }

      int GetKey() const { return mKey; }
      bool GetPressedHit() const { return mPressedHit; }
      bool GetReleasedHit() const { return mReleasedHit; }

   private:
      bool mPressedHit;
      bool mReleasedHit;
      int mKey;
   };
}

CPPUNIT_TEST_SUITE_REGISTRATION( dtTest::BaseABCTests );

using namespace dtTest;

void BaseABCTests::TestInput()
{
   // it is really weird that an app must be used to test the BaseABC function,
   // but this is how it must be when Application allocates BaseABC's members, like mDeltaWin.
   dtCore::RefPtr<TestApp2> app(new TestApp2('0'));
   app->GetWindow()->SetPosition(0, 0, 50, 50);
   CPPUNIT_ASSERT( app->GetKeyboardListener() != NULL );
}
