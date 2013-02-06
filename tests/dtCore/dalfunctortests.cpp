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
* @author Chris Osborn
*/
#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/functor.h>

using namespace dtCore;

static int f0def() { return 0; }

/**
* @class DALFunctorTests
* @brief Unit tests for the original dtCore MakeFunctors
*/
class DALFunctorTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(DALFunctorTests);
   CPPUNIT_TEST(TestMakeFunctor0);
   CPPUNIT_TEST(TestMakeFunctor1);
   CPPUNIT_TEST(TestMakeFunctor0Ret);
   CPPUNIT_TEST_SUITE_END();

public:
   void setUp();
   void tearDown();

   void TestMakeFunctor0();
   void TestMakeFunctor1();
   void TestMakeFunctor0Ret();

private:

   struct A
   {
      int f0def() { return 2; }
      int f0defconst() const { return 3; }
   };

   struct TestMemberCounter
   {
      int mCount;

      TestMemberCounter() :
         mCount(0)
      {
      }

      void f0def() { mCount++; }
      void f0defconst() const { DALFunctorTests::mGlobalCounter++; }
      void f1def(int i) { mCount += i; }
      void f1defconst(int i) const { DALFunctorTests::mGlobalCounter += i; }
   };

   TestMemberCounter mMemberCounter;

   public:
   static int mGlobalCounter;

};

int DALFunctorTests::mGlobalCounter = 0;

static void voidf0def() { DALFunctorTests::mGlobalCounter++; }
static void voidf1def(int i) { DALFunctorTests::mGlobalCounter += i; }

CPPUNIT_TEST_SUITE_REGISTRATION(DALFunctorTests);

///////////////////////////////////////////////////////////////////////////////
void DALFunctorTests::setUp() 
{
   mMemberCounter.mCount = 0;
   mGlobalCounter = 0;
}

///////////////////////////////////////////////////////////////////////////////
void DALFunctorTests::tearDown() 
{
}

///////////////////////////////////////////////////////////////////////////////
void DALFunctorTests::TestMakeFunctor0()
{
   // Member, no return, non-const, 0 parameters
   MakeFunctor(mMemberCounter,&TestMemberCounter::f0def)();
   CPPUNIT_ASSERT( mMemberCounter.mCount == 1 );

   // Member, no return, const, 0 paramters
   MakeFunctor( mMemberCounter, &TestMemberCounter::f0defconst)();
   CPPUNIT_ASSERT( mGlobalCounter == 1 );

   int lastGlobalCounter = mGlobalCounter;
   // Non-member, no return, 0 paramters
   MakeFunctor(voidf0def)();
   CPPUNIT_ASSERT( mGlobalCounter == lastGlobalCounter+1 );
}

///////////////////////////////////////////////////////////////////////////////
void DALFunctorTests::TestMakeFunctor1()
{
   const int incValue = 4;

   // Member, no return, non-const, 1 parameter
   MakeFunctor(mMemberCounter,&TestMemberCounter::f1def)(incValue);
   CPPUNIT_ASSERT( mMemberCounter.mCount == incValue );

   // Member, no return, const, 1 paramter
   MakeFunctor(mMemberCounter,&TestMemberCounter::f1defconst)(incValue);
   CPPUNIT_ASSERT( mGlobalCounter == incValue );

   int lastGlobalCounter = mGlobalCounter;
   // Non-member, no return, non-const, 1 paramter
   MakeFunctor(voidf1def)(incValue);
   CPPUNIT_ASSERT( mGlobalCounter == lastGlobalCounter+incValue ); 
}

///////////////////////////////////////////////////////////////////////////////
void DALFunctorTests::TestMakeFunctor0Ret()
{
   // Member, return, non-const, no parameters
   A a;
   CPPUNIT_ASSERT(MakeFunctorRet( a, &A::f0def )() == 2);

   // Member, return, const, no parameters
   CPPUNIT_ASSERT(MakeFunctorRet( a, &A::f0defconst )() == 3);

   // Non-member, return, non-const, no parameters
   CPPUNIT_ASSERT(MakeFunctorRet(f0def)() == 0);
}
