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
* @author Chris Osborn
*/

#include <cppunit/extensions/HelperMacros.h>
#include <dtDAL/functor.h>

using namespace dtDAL;

static int f0def() { return 0; }

/**
* @class DALFunctorTests
* @brief Unit tests for the original dtDAL MakeFunctors
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
