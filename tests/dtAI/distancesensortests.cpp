/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2005-2008, Alion Science and Technology Corporation
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
* David Guthrie
*/
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtAI/distancesensor.h>
#include <dtAI/sensor.h>
#include <dtUtil/templateutility.h>
#include <dtUtil/stringutils.h>

#include <dtCore/refptr.h>

#include <osg/io_utils>

namespace dtAI
{
   /**
    * The structs below are used as the templated types to the dtAI::Sensor.
    * They act as first class functions in their ability to maintain state
    * while interfacing through the operator().
    */

   template <typename FloatType>
   struct TestDistanceCallbackFunctor
   {
      void operator()(FloatType& theValue)
      {
         *mValue = theValue;
      }

      FloatType* mValue;
   };


   /// Math unit tests for dtUtil
   class DistanceSensorTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(DistanceSensorTests);
      CPPUNIT_TEST(TestTransformFunctor);
      CPPUNIT_TEST(TestMatrixFunctor);
      CPPUNIT_TEST(TestVecFunctor);
      CPPUNIT_TEST_SUITE_END();
   
   public:
      void setUp()
      {
         
      }
      
      void tearDown()
      {
      }

      void TestTransformFunctor()
      {
         typedef DistanceSensor<EvaluateTransformablePosition<>, EvaluateTransformablePosition<>, 
            TestDistanceCallbackFunctor<float> > TwoTransformDistanceSensorFloat;

         dtCore::RefPtr<dtCore::Transformable> t1 = new dtCore::Transformable("jojo");
         dtCore::RefPtr<dtCore::Transformable> t2 = new dtCore::Transformable("lima");
         EvaluateTransformablePosition<osg::Vec3> etp1(*t1, dtCore::Transformable::ABS_CS), 
            etp2(*t2, dtCore::Transformable::REL_CS);

         TestDistanceCallbackFunctor<float> distanceCallback;
         float callBackValue = 1.5f;
         distanceCallback.mValue = &callBackValue;

         TwoTransformDistanceSensorFloat* distanceSensor = new TwoTransformDistanceSensorFloat(
                  etp1, etp2, 5, distanceCallback);
         distanceSensor->Evaluate();

         CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The callback should have fired", 
                  0.0f, callBackValue, 0.001f);
         CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The distance should be 0.0", 
                  0.0f, distanceSensor->GetDistance(), 0.001f);

         t1->AddChild(t2.get());
         
         osg::Vec3 testVec(5.0, 5.0, 5.0);
         osg::Vec3 testVec2(-5.0, -5.0, -5.0);
         
         dtCore::Transform xform;
         t1->GetTransform(xform, dtCore::Transformable::ABS_CS);
         xform.SetTranslation(testVec);
         t1->SetTransform(xform, dtCore::Transformable::ABS_CS);

         t2->GetTransform(xform, dtCore::Transformable::REL_CS);
         xform.SetTranslation(testVec2);
         t2->SetTransform(xform, dtCore::Transformable::REL_CS);

         callBackValue = 1.5f;

         float expectedDistance = (testVec - testVec2).length();

         distanceSensor->Evaluate();

         CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The callback should not have fired",
                  1.5f, callBackValue, 0.001f);
         CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The distance should be " + dtUtil::ToString(expectedDistance), 
                  expectedDistance, distanceSensor->GetDistance(), 0.001f);

         float distSquared = expectedDistance * expectedDistance;
         CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The distance squared should be " + dtUtil::ToString(distSquared), 
            distSquared, distanceSensor->GetDistanceSquared(), 0.001f);

         t2->GetTransform(xform, dtCore::Transformable::REL_CS);
         xform.SetTranslation(testVec);
         t2->SetTransform(xform, dtCore::Transformable::REL_CS); 

         distanceSensor->Evaluate();

         CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The callback should have fired", 
                  0.0f, callBackValue, 0.001f);
      }

      void TestMatrixFunctor()
      {
         typedef DistanceSensor<EvaluateMatrixPosition<osg::Vec3d>, EvaluateMatrixPosition<osg::Vec3d>, 
            TestDistanceCallbackFunctor<double>, double, osg::Vec3d > TwoMatrixDistanceSensorDouble;

         dtCore::RefPtr<osg::MatrixTransform> m1 = new osg::MatrixTransform;
         dtCore::RefPtr<osg::MatrixTransform> m2 = new osg::MatrixTransform;
         EvaluateMatrixPosition<osg::Vec3d> emp1(*m1), 
            emp2(*m2);

         TestDistanceCallbackFunctor<double> distanceCallback;
         double callBackValue = 1.5;
         distanceCallback.mValue = &callBackValue;

         TwoMatrixDistanceSensorDouble* distanceSensor = new TwoMatrixDistanceSensorDouble(
                  emp1, emp2, 5, distanceCallback);

         distanceSensor->Evaluate();

         CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The callback should have fired", 
                  0.0, callBackValue, 0.001);
         CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The distance should be 0.0", 
                  0.0, distanceSensor->GetDistance(), 0.001);

         osg::Vec3d testVec(5.0, 5.0, 5.0);
         osg::Vec3d testVec2(-5.0, -5.0, -5.0);
         
         osg::Matrix tmpMatrix;
         tmpMatrix.setTrans(testVec);
         m1->setMatrix(tmpMatrix);
         tmpMatrix.setTrans(testVec2);
         m2->setMatrix(tmpMatrix);

         distanceSensor->Evaluate();
      }

      void TestVecFunctor()
      {
         const osg::Vec3 testVec(0.0f, 1.0f, 2.2f);
         EvaluateVec3<> ev3(testVec);
         osg::Vec3 result;
         ev3(result);
         
         CPPUNIT_ASSERT_EQUAL(testVec, result);
      }

   private:
   };

   CPPUNIT_TEST_SUITE_REGISTRATION(DistanceSensorTests);

}
