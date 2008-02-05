/*
* Delta3D Open Source Game and Simulation Engine
* Copyright 2008, Alion Science and Technology
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
* David Guthrie
*/
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtAI/distancesensor.h>
#include <dtAI/sensor.h>
#include <dtUtil/templateutility.h>
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


   typedef CompareDistanceFunc<osg::Vec3f::value_type, osg::Vec3f> CompareDistanceVec3f;
   typedef CompareDistanceFunc<osg::Vec3d::value_type, osg::Vec3d> CompareDistanceVec3d;


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
            CompareDistanceVec3f, TestDistanceCallbackFunctor<float> > TwoTransformDistanceSensorFloat;

         dtCore::RefPtr<dtCore::Transformable> t1 = new dtCore::Transformable("jojo");
         dtCore::RefPtr<dtCore::Transformable> t2 = new dtCore::Transformable("lima");
         EvaluateTransformablePosition<osg::Vec3> etp1(*t1, dtCore::Transformable::ABS_CS), 
            etp2(*t2, dtCore::Transformable::REL_CS);

         TestDistanceCallbackFunctor<float> distanceCallback;
         float callBackValue = 1.5f;
         distanceCallback.mValue = &callBackValue;

         TwoTransformDistanceSensorFloat* distanceSensor = new TwoTransformDistanceSensorFloat(
                  etp1, etp2, CompareDistanceVec3f(5), distanceCallback);
         distanceSensor->Evaluate();

         CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The callback should not have fired", 
                  1.5f, callBackValue, 0.001f);
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
         xform.SetTranslation(testVec);
         t2->SetTransform(xform, dtCore::Transformable::REL_CS);

         distanceSensor->Evaluate();

         CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The callback should not have fired", 
                  1.5f, callBackValue, 0.001f);
         CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The distance should be 0.0", 
                  0.0f, distanceSensor->GetDistance(), 0.001f);

         t2->GetTransform(xform, dtCore::Transformable::REL_CS);
         xform.SetTranslation(testVec2);
         t2->SetTransform(xform, dtCore::Transformable::REL_CS);

         distanceSensor->Evaluate();

         CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The callback should have fired", 
                  300.0f, callBackValue, 0.001f);
         CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The distance squared should be 300", 
                  callBackValue, distanceSensor->GetDistanceSquared(), 0.001f);
         CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The distance should be sqrt 300", 
                  std::sqrt(callBackValue), distanceSensor->GetDistance(), 0.001f);
      }

      void TestMatrixFunctor()
      {
         typedef DistanceSensor<EvaluateMatrixPosition<osg::Vec3d>, EvaluateMatrixPosition<osg::Vec3d>, 
            CompareDistanceVec3d, TestDistanceCallbackFunctor<double>, double, osg::Vec3d > 
            TwoMatrixDistanceSensorDouble;

         dtCore::RefPtr<osg::MatrixTransform> m1 = new osg::MatrixTransform;
         dtCore::RefPtr<osg::MatrixTransform> m2 = new osg::MatrixTransform;
         EvaluateMatrixPosition<osg::Vec3d> emp1(*m1), 
            emp2(*m2);

         TestDistanceCallbackFunctor<double> distanceCallback;
         double callBackValue = 1.5;
         distanceCallback.mValue = &callBackValue;

         TwoMatrixDistanceSensorDouble* distanceSensor = new TwoMatrixDistanceSensorDouble(
                  emp1, emp2, CompareDistanceVec3d(5), distanceCallback);

         distanceSensor->Evaluate();

         CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("The callback should not have fired", 
                  1.5, callBackValue, 0.001);
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
         const osg::Vec3 testVec(0.0, 1.0, 2.2);
         EvaluateVec3<> ev3(testVec);
         osg::Vec3 result;
         ev3(result);
         
         CPPUNIT_ASSERT_EQUAL(testVec, result);
      }

   private:
   };

   CPPUNIT_TEST_SUITE_REGISTRATION(DistanceSensorTests);

}
