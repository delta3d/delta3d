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

         if ( key==mKey)
         {
            return true;
         }
         return false;
      }

      bool KeyReleased(const dtCore::Keyboard* kb, int key)
      {
         mReleasedHit = true;

         if ( key==mKey)
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
