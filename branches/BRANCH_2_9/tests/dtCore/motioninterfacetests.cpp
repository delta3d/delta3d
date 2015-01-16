/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2014 David Guthrie
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
* This software was developed by Alion Science and Technology Corporation under
* circumstances in which the U. S. Government may have rights in the software.
*
* @author David Guthrie
*/
#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/motioninterface.h>

#include <osg/Referenced>
#include <dtCore/refptr.h>
#include <dtCore/observerptr.h>

class TestMotion: public osg::Referenced, public dtCore::MotionInterface
{
public:
   DT_DECLARE_VIRTUAL_REF_INTERFACE_INLINE
   TestMotion(){ Reset(); }
   void Reset()
   {
      vCalled = false;
      aCalled = false;
      avCalled = false;
   }
   void TestNotCalled()
   {
      CPPUNIT_ASSERT(!vCalled  && !aCalled && !avCalled);
   }

   void TestCalled()
   {
      CPPUNIT_ASSERT(vCalled  && aCalled && avCalled);
   }

   virtual osg::Vec3 GetVelocity() const { vCalled = true; return osg::Vec3(); }
   virtual osg::Vec3 GetAngularVelocity() const { avCalled = true; return osg::Vec3(); }
   virtual osg::Vec3 GetAcceleration() const { aCalled = true; return osg::Vec3(); }

   mutable bool vCalled, aCalled, avCalled;
};

class MotionInterfaceTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(MotionInterfaceTests);
      CPPUNIT_TEST(TestHoldInPtr);
   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp() {}
      void tearDown() {}
      void TestHoldInPtr()
      {
         dtCore::RefPtr<TestMotion> tm = new TestMotion();
         dtCore::RefPtr<dtCore::MotionInterface> tmi = tm;
         dtCore::ObserverPtr<dtCore::MotionInterface> tmiOb = tmi.get();
         dtCore::ObserverPtr<TestMotion> tmOb = tm.get();

         CPPUNIT_ASSERT(tmi != NULL);
         CPPUNIT_ASSERT(tmiOb != NULL);

         tm->TestNotCalled();
         tmiOb->GetVelocity();
         tmiOb->GetAngularVelocity();
         tmiOb->GetAcceleration();
         tm->TestCalled();

         tm = NULL;

         CPPUNIT_ASSERT(tmi != NULL);
         CPPUNIT_ASSERT(tmiOb.valid());
         CPPUNIT_ASSERT(tmOb.valid());

         tmi = NULL;
         CPPUNIT_ASSERT(!tmOb.valid());
         CPPUNIT_ASSERT(!tmiOb.valid());
      }
};

CPPUNIT_TEST_SUITE_REGISTRATION(MotionInterfaceTests);



