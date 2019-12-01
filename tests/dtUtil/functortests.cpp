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
 * @author Aleksei Trunov
 * @author Chris Osborn
 */

#include <prefix/unittestprefix.h>
#include <dtUtil/exception.h>
#include <dtUtil/generic.h>
#include <dtUtil/funtraits.h>
#include <dtUtil/funcall.h>
#include <dtUtil/functor.h>
#include <dtUtil/funbind.h>
#include <dtUtil/command.h>
#include <dtCore/refptr.h>
#include <cppunit/extensions/HelperMacros.h>
#include <osg/Math>

#include <cstring>

using namespace dtUtil;

static int f0def() { return 0; }
static int f1def(int i) { return i; }
static int f2def(int i, long j) { return i + j; }
static int f2ii(int i, int j) { return i + j; }

char bf1(char c) { return c; }
double bf2(int i, double f) { return f + i; }
double bf3(int i, char c, double f) { return f + i + int(c); }
double bf5(int i, char c, int j, double f, int k)
{
   return f + i + int(c) + j + k;
}

/**
* @class FunctorTests
* @brief Unit tests for the generic functor implementations
*/
class FunctorTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(FunctorTests);
      CPPUNIT_TEST(TestNonMemberFunctors);
      CPPUNIT_TEST(TestMemberFunctors);
      CPPUNIT_TEST(TestBinding);
      CPPUNIT_TEST(TestMoreFunctors);
      CPPUNIT_TEST(TestCommand);
      CPPUNIT_TEST(TestValid);
   CPPUNIT_TEST_SUITE_END();

public:
   void setUp();
   void tearDown();

   void TestNonMemberFunctors();
   void TestFunctorFunctors();
   void TestMemberFunctors();
   void TestBinding();
   void TestMoreFunctors();
   void TestCommand();
   void TestValid();

private:

   struct FunT
   {
      static long instances_;
      FunT() { ++instances_; }
      FunT(FunT const&/* f*/) { ++instances_; }
      ~FunT() { --instances_; }
      int operator()() const { return 1; }
      int operator()(int i) const { return i; }
      int operator()(int i, long j) const { return i + j; }
   };

   struct FunBig
   {
      static long instances_;
      FunBig() { ++instances_; }
      FunBig(int i0, int i1, int i2) : i0_(i0), i1_(i1), i2_(i2) { ++instances_; }
      FunBig(FunBig const& f) : i0_(f.i0_), i1_(f.i1_), i2_(f.i2_)
      {
         memcpy(&c0_[0], &f.c0_[0], sizeof(c0_));
         memcpy(&c1_[0], &f.c1_[0], sizeof(c1_));
         memcpy(&c2_[0], &f.c2_[0], sizeof(c2_));
         ++instances_;
      }
      ~FunBig() { --instances_; }
      int operator()() const { return i0_; }
      int operator()(int/* i*/) const { return i1_; }
      int operator()(int/* i*/, long/* j*/) const { return i2_; }
      char c0_[16];
      int i0_;
      char c1_[16];
      int i1_;
      char c2_[16];
      int i2_;
   };

   struct A
   {
      A() {}
      virtual ~A() {}
      int f0def() { return 2; }
      int f1def(int i) { return i; }
      static int f2defstatic(int i, long j) { return i + j; }
      int f2def(int i, long j) { return i + j; }
      int f2defconst(int i, long j) const { return i + j; }
      int f2defvolatile(int i, long j) volatile { return i + j; }
      int f2defconstvolatile(int i, long j) const volatile { return i + j; }
      virtual int f2defvirt(int i, long j) { return i + j; }
      virtual int f2defvirtconst(int i, long j) const { return i + j; }
   };

   struct C : public A
   {
      virtual ~C() {}
   };

   struct M
   {
      char c_[128];
   };

   struct V
   {
      virtual ~V() {}
      virtual int f2virta(int i, long j) = 0;
      char c_[2];
   };

   struct D : public M, virtual public V, virtual public A
   {
      virtual ~D() {}
      virtual void f2() { }
      virtual int f2defvirt(int i, long j) { return i + j + 1; }

      int f2def(int i, long j) { return i + j + 3; }
      virtual int f2virta(int i, long j) { return i + j + 2; }
   };

   struct B
   {
      int f0def() { return 2; }
      int f1def(int i) { return i; }
      int f2def(int i, long j) { return i + j; }
   };

   struct Test
   {
      void test(const int &i, int &j)
      {
         j = i;
      }
   };

   typedef Functor<int, TYPELIST_0()> Functor0;
   Functor0 f0;

   typedef Functor<int, TYPELIST_1(int)> Functor1;
   typedef Functor<int, TYPELIST_2(int, long)> Functor2;

};

long FunctorTests::FunT::instances_ = 0;
long FunctorTests::FunBig::instances_ = 0;

CPPUNIT_TEST_SUITE_REGISTRATION(FunctorTests);

///////////////////////////////////////////////////////////////////////////////
void FunctorTests::setUp()
{
   f0 = Functor0(&f0def);
}

///////////////////////////////////////////////////////////////////////////////
void FunctorTests::tearDown()
{
}

//////////////////////////////////////////////////////////////////////////
void FunctorTests::TestCommand()
{
   A pStruct;

   dtCore::RefPtr<dtUtil::Command0<int> > cmd = new dtUtil::Command0<int>(Command0<int>::FunctorType(&pStruct, &A::f0def));
   CPPUNIT_ASSERT(cmd->operator()() == 2);

   dtCore::RefPtr<dtUtil::Command1<int, int> > cmd1 = new dtUtil::Command1<int, int>(Command1<int, int>::FunctorType(&pStruct, &A::f1def), 2);
   CPPUNIT_ASSERT(cmd1->operator()() == 2);

   dtCore::RefPtr<dtUtil::Command2<int, int, int> > cmd2 = new dtUtil::Command2<int, int, int>(Command2<int, int, int>::FunctorType(&pStruct, &A::f2def), 5, 4);
   CPPUNIT_ASSERT(cmd2->operator()() == 9);
}

///////////////////////////////////////////////////////////////////////////////
void FunctorTests::TestValid()
{
   Functor1 f1;
   CPPUNIT_ASSERT(!f1.valid());
   f1 = Functor1(&f1def);
   CPPUNIT_ASSERT(f1.valid());
}

///////////////////////////////////////////////////////////////////////////////
void FunctorTests::TestNonMemberFunctors()
{
   CPPUNIT_ASSERT(f0() == 0);

   Functor1 f1(&f1def);
   CPPUNIT_ASSERT(f1(1) == 1);
   CPPUNIT_ASSERT(f1(2) == 2);

   Functor2 f2(&f2def);
   CPPUNIT_ASSERT(f2(1, 1) == 2);
   CPPUNIT_ASSERT(f2(1, 2) == 3);
   {
      typedef Functor<int, TYPELIST_2(int, int)> Functor2_;
      Functor2_ f2_(&f2ii);
      CPPUNIT_ASSERT(f2_(1, 1) == 2);
      CPPUNIT_ASSERT(f2_(1, 2) == 3);
   }
   Functor2 fn2def(&f2def);
   CPPUNIT_ASSERT(fn2def(1, 1) == 2);
   CPPUNIT_ASSERT(fn2def(1, 2) == 3);
   // op=
   // Functor2 fn2 = f2def;   // error - no explicit op=
   Functor2 fn2 = Functor2(&f2def);
   CPPUNIT_ASSERT(fn2(1, 1) == 2);
   CPPUNIT_ASSERT(fn2(1, 2) == 3);
   // invalid fun ctors and op()
   // Functor2 f2_(f1def);   // error - no such ctor
   Functor2 f2_(&f2def);
   // f2_();   // error - no internal implementation for such op()
   // f2_(1);   // error - no internal implementation for such op()

}

///////////////////////////////////////////////////////////////////////////////
void FunctorTests::TestFunctorFunctors()
{
   CPPUNIT_ASSERT(FunT::instances_ == 0);
   CPPUNIT_ASSERT(FunBig::instances_ == 0);
   {
      FunT fun0;
      Functor0 f0fun(fun0);
      CPPUNIT_ASSERT(f0fun() == 1);
      FunT fun1;
      Functor1 f1fun(fun1);
      CPPUNIT_ASSERT(f1fun(1) == 1);
      CPPUNIT_ASSERT(f1fun(2) == 2);
      FunT fun2;
      Functor2 f2fun(fun2);
      CPPUNIT_ASSERT(f2fun(1, 1) == 2);
      CPPUNIT_ASSERT(f2fun(1, 2) == 3);
      Functor2 f2fun_(fun2);
      CPPUNIT_ASSERT(f2fun_(1, 1) == 2);
      CPPUNIT_ASSERT(f2fun_(1, 2) == 3);
      FunBig funbig2(1, 2, 3);
      Functor2 f2funbig(funbig2);
      CPPUNIT_ASSERT(f2funbig(0, 0) == 3);
      FunBig funbig2_(2, 4, 6);
      Functor2 f2funbig_(funbig2_);
      // op= for funs
      f2fun = f2fun_;
      CPPUNIT_ASSERT(f2fun(1, 1) == 2);
      CPPUNIT_ASSERT(f2fun(1, 2) == 3);
      f2funbig = f2funbig_;
      CPPUNIT_ASSERT(f2funbig(0, 0) == 6);
   }
   CPPUNIT_ASSERT(FunT::instances_ == 0);
   CPPUNIT_ASSERT(FunBig::instances_ == 0);
}

void FunctorTests::TestMemberFunctors()
{
   // mem fns
   A a;
   Functor0 f0memfn(&a, &A::f0def);
   CPPUNIT_ASSERT(f0memfn() == 2);
   Functor1 f1memfn(&a, &A::f1def);
   CPPUNIT_ASSERT(f1memfn(1) == 1);
   CPPUNIT_ASSERT(f1memfn(2) == 2);
   Functor2 f2memfnstatic(&A::f2defstatic);
   CPPUNIT_ASSERT(f2memfnstatic(1, 1) == 2);
   CPPUNIT_ASSERT(f2memfnstatic(1, 2) == 3);
   Functor2 f2memfn(&a, &A::f2def);
   CPPUNIT_ASSERT(f2memfn(1, 1) == 2);
   CPPUNIT_ASSERT(f2memfn(1, 2) == 3);
   Functor2 f2memfnconst(&a, &A::f2defconst);
   CPPUNIT_ASSERT(f2memfnconst(1, 1) == 2);
   CPPUNIT_ASSERT(f2memfnconst(1, 2) == 3);
   const A ac;
   Functor2 f2memfnconst_(&ac, &A::f2defconst);
   CPPUNIT_ASSERT(f2memfnconst_(1, 1) == 2);
   CPPUNIT_ASSERT(f2memfnconst_(1, 2) == 3);
   // Functor2 _f2memfnconst_(&ac, &A::f2def);         // error - in FunctorCall<> could not convert pointer-to-member
   Functor2 f2memfnvolatile(&a, &A::f2defvolatile);
   CPPUNIT_ASSERT(f2memfnvolatile(1, 1) == 2);
   CPPUNIT_ASSERT(f2memfnvolatile(1, 2) == 3);
   Functor2 f2defconstvolatile(&a, &A::f2defconstvolatile);
   CPPUNIT_ASSERT(f2defconstvolatile(1, 1) == 2);
   CPPUNIT_ASSERT(f2defconstvolatile(1, 2) == 3);
   Functor2 f2memfnvirt(&a, &A::f2defvirt);
   CPPUNIT_ASSERT(f2memfnvirt(1, 1) == 2);
   CPPUNIT_ASSERT(f2memfnvirt(1, 2) == 3);
   Functor2 f2memfnvirtconst(&a, &A::f2defvirtconst);
   CPPUNIT_ASSERT(f2memfnvirtconst(1, 1) == 2);
   CPPUNIT_ASSERT(f2memfnvirtconst(1, 2) == 3);
   {
      C c;
      Functor2 f2memfnvirt(&c, &A::f2defvirt);
      CPPUNIT_ASSERT(f2memfnvirt(1, 1) == 2);
      CPPUNIT_ASSERT(f2memfnvirt(1, 2) == 3);
      Functor2 f2memfnvirt_(&c, &C::f2defvirt);
      CPPUNIT_ASSERT(f2memfnvirt_(1, 1) == 2);
      CPPUNIT_ASSERT(f2memfnvirt_(1, 2) == 3);
   }
   {
      D d;
      Functor2 f2memfnvirt(&d, &D::f2defvirt);
      CPPUNIT_ASSERT(f2memfnvirt(1, 1) == 3);
      CPPUNIT_ASSERT(f2memfnvirt(1, 2) == 4);
      Functor2 f2memfnvirta(&d, &D::f2virta);
      CPPUNIT_ASSERT(f2memfnvirta(1, 1) == 4);
      CPPUNIT_ASSERT(f2memfnvirta(1, 2) == 5);
      Functor2 f2memfn(&d, &D::f2def);
      CPPUNIT_ASSERT(f2memfn(1, 1) == 5);
      CPPUNIT_ASSERT(f2memfn(1, 2) == 6);
   }
   B b;
   Functor2 f2memfn_(&b, &B::f2def);
   CPPUNIT_ASSERT(f2memfn_(1, 1) == 2);
   CPPUNIT_ASSERT(f2memfn_(1, 2) == 3);
   // Functor2 _f2memfn_(&b, &A::f2def);   // error - in FunctorCall<> could not convert pointer-to-member
   f2memfn = f2memfn_;
   CPPUNIT_ASSERT(f2memfn(1, 1) == 2);
   CPPUNIT_ASSERT(f2memfn(1, 2) == 3);
   // fun copying and mutating for another callable entity
   Functor0 f0mut = f0;
   CPPUNIT_ASSERT(f0mut() == 0);
   FunT fun0;
   Functor0 f0fun(fun0);
   f0mut = f0fun;
   CPPUNIT_ASSERT(f0mut() == 1);
   f0mut = f0memfn;
   CPPUNIT_ASSERT(f0mut() == 2);
}

void FunctorTests::TestBinding()
{
   // test binding
   typedef TYPELIST_3(int, char, double) TestTL3;
   Functor<double, TestTL3> bfun3(&bf3);
   CPPUNIT_ASSERT(osg::equivalent(bfun3(1, 'A', 2.1), bf3(1, 'A', 2.1)));

   typedef TYPELIST_2(Int2Type<0>, Int2Type<2>) TestIdsTL;
   typedef BoundTL2<TestTL3, TestIdsTL>::Result TestBTL2;
   typedef UnboundTL2<TestTL3, TestIdsTL>::Result TestUBTL2;
   CPPUNIT_ASSERT((Length<TestBTL2>::value) == 2);
   CPPUNIT_ASSERT((Length<TestUBTL2>::value) == 1);
   {
      Functor<double, TestBTL2> bfun2(&bf2);
      CPPUNIT_ASSERT(bfun2(1, 2.1) == bf2(1, 2.1));
      Functor<char, TestUBTL2> bfun1(&bf1);
      CPPUNIT_ASSERT(bfun1('A') == bf1('A'));
   }
   {
      Binder<Functor<char, TestTL3>, CreateTL<Int2Type<0>, Int2Type<2> >::Type>::Outgoing bfun11(&bf1);
      CPPUNIT_ASSERT(bfun11('A') == bf1('A'));
      Functor<double, TestUBTL2> bfun12 = Bind<0, 2>(bfun3, 1, 2.1);
      CPPUNIT_ASSERT(osg::equivalent(bfun12('A'), bf3(1, 'A', 2.1)));
      Functor<double, TYPELIST_1(char)> bfun13 = Bind<0, 2>(bfun3, 1, 2.1);
      CPPUNIT_ASSERT(osg::equivalent(bfun13('A'), bf3(1, 'A', 2.1)));
      Functor<double, TYPELIST_2(char, double)> bfun14 = Bind<0>(bfun3, 1);
      CPPUNIT_ASSERT(osg::equivalent(bfun14('A', 2.1), bf3(1, 'A', 2.1)));
   }
   {
      Functor<double, TYPELIST_2(char, double)> bfunbig = Bind<0, 2, 4>(Functor<double, TYPELIST_5(int, char, int, double, int)>(&bf5), 1, 2, 3);
      CPPUNIT_ASSERT(osg::equivalent(bfunbig('A', 2.1), bf5(1, 'A', 2, 2.1, 3)));
      Functor<double, TYPELIST_0()> bfun0 = Bind<0, 1, 2, 3, 4>(Functor<double, TYPELIST_5(int, char, int, double, int)>(&bf5), 1, 'A', 2, 2.1, 3);
      CPPUNIT_ASSERT(osg::equivalent(bfun0(), bf5(1, 'A', 2, 2.1, 3)));
      Functor<double, TYPELIST_5(int, char, int, double, int)> bfun5 = Bind<>(Functor<double, TYPELIST_5(int, char, int, double, int)>(&bf5));
      CPPUNIT_ASSERT(osg::equivalent(bfun5(1, 'A', 2, 2.1, 3), bf5(1, 'A', 2, 2.1, 3)));
   }
}

///////////////////////////////////////////////////////////////////////////////
void FunctorTests::TestMoreFunctors()
{
   // more complicated tests
   Test test;
   typedef Functor<void, TYPELIST_2(const int&, int&)> TestFunctor2;
   TestFunctor2 testfun(&test, &Test::test);
   int i = 10;
   int j = 100;
   CPPUNIT_ASSERT(i != j);
   testfun(i, j);
   CPPUNIT_ASSERT(i == j);
   CPPUNIT_ASSERT(MakeFunctor(f0def)() == 0);
   CPPUNIT_ASSERT(MakeFunctor(f1def)(1) == 1);
   CPPUNIT_ASSERT(MakeFunctor(f2def)(1, 1) == 2);
   {
      A a;
      CPPUNIT_ASSERT(MakeFunctor(&A::f0def, &a)() == 2);
      CPPUNIT_ASSERT(MakeFunctor(&A::f1def, &a)(1) == 1);
      CPPUNIT_ASSERT(MakeFunctor(&A::f2def, &a)(1, 1) == 2);
      CPPUNIT_ASSERT(MakeFunctor(&A::f2defconst, &a)(1, 1) == 2);
   }
   {
      FunT fun0;
      CPPUNIT_ASSERT(MakeFunctor<int (*)()>(fun0)() == 1);
      FunT fun1;
      CPPUNIT_ASSERT(MakeFunctor<int (*)(int)>(fun1)(1) == 1);
      FunT fun2;
      CPPUNIT_ASSERT(MakeFunctor<int (*)(int, long)>(fun2)(1, 1) == 2);
   }
}
