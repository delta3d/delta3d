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
 * Chris Osborn
 */

#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtUtil/mathdefines.h>
#include <dtCore/scene.h>
#include <dtCore/transformable.h>
#include <dtCore/transform.h>
#include <dtCore/object.h>
#include <dtCore/camera.h>
#include <dtCore/view.h>

#include <osg/MatrixTransform>
#include <osg/io_utils>
#include <ode/collision.h>
#include <sstream>
#include <limits>

using namespace dtCore;


class TransformableTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(TransformableTests);
   CPPUNIT_TEST(TestAbsoluteRelative);
   CPPUNIT_TEST(TestDefaultBools);
   CPPUNIT_TEST(TestGetCollisionGeomDimensions);
   CPPUNIT_TEST(TestSetCollisionBox);
   CPPUNIT_TEST(TestEpsilonEquals);
   CPPUNIT_TEST(TestODEInSyncOnSetTransform);
   CPPUNIT_TEST(TestSetMatrix);
   CPPUNIT_TEST(TestSetPosHPR);
   CPPUNIT_TEST(TestSetPosQuat);
   CPPUNIT_TEST(TestScale);
   CPPUNIT_TEST(TestTranslation);
   CPPUNIT_TEST(TestMove);
   CPPUNIT_TEST(TestRotationHPR);
   CPPUNIT_TEST(TestRotationQuat);
   CPPUNIT_TEST(TestTransRotScaleGetSet);
   CPPUNIT_TEST(TestConstructorTakingMatrixNode);
   CPPUNIT_TEST(TestValueOperators);
   CPPUNIT_TEST(TestValid);
   CPPUNIT_TEST(TestRows);
   CPPUNIT_TEST(TestDistance);
   CPPUNIT_TEST(TestGetTransformWithDisabledCamera);
   CPPUNIT_TEST(TestGetTransformInSceneWithNoCamera);
   CPPUNIT_TEST(TestGetTransformNotInScene);
   CPPUNIT_TEST(TestGetTransformFromInactiveTransformable);
   CPPUNIT_TEST(TestGetTransformFromInactiveParent);
   CPPUNIT_TEST_SUITE_END();

public:

   void setUp();
   void tearDown();

   void TestAbsoluteRelative();
   void TestDefaultBools();
   void TestGetCollisionGeomDimensions();
   void TestSetCollisionBox();
   void TestEpsilonEquals();
   void TestODEInSyncOnSetTransform();
   void TestSetPosHPR();
   void TestSetPosQuat();
   void TestScale();
   void TestTranslation();
   void TestMove();
   void TestRotationHPR();
   void TestRotationQuat();
   void TestTransRotScaleGetSet();
   void TestConstructorTakingMatrixNode();
   void TestSetMatrix();
   void TestValueOperators();
   void TestValid();
   void TestRows();
   void TestDistance();
   void TestGetTransformWithDisabledCamera();
   void TestGetTransformInSceneWithNoCamera();
   void TestGetTransformNotInScene();
   void TestGetTransformFromInactiveTransformable();
   void TestGetTransformFromInactiveParent();

private:
   bool CompareMatrix(const osg::Matrix& rhs, const osg::Matrix& lhs) const;
   bool CompareVector(const osg::Vec3& rhs, const osg::Vec3& lhs) const;

   RefPtr<Transformable> mParentTransformable;
   RefPtr<Transformable> mTransformable;
   Transform mTransform;

   osg::Vec3 mBoxLengths; ///Dimensions for box

   float mRadius; ///Radius for sphere or cylinder
   float mHeight; ///Height for cylinder or ray
   static const osg::Vec3::value_type TEST_EPSILON;
};

CPPUNIT_TEST_SUITE_REGISTRATION(TransformableTests);

const float TransformableTests::TEST_EPSILON(1e-5f);

void TransformableTests::setUp()
{
   mParentTransformable = new dtCore::Transformable("ParentTransformable");
   mTransformable = new dtCore::Transformable("TestTransformable");
   mTransform.Set(2.0f, 4.0f, 1.0f, 30.0f, -20.0f, 2.5f);
   mBoxLengths.set(2.0f, 3.0f, 4.0f);
   mRadius = 3.2f;
   mHeight = 6.4f;
}

void TransformableTests::tearDown()
{
   mTransformable->SetCollisionMesh(NULL);
   mTransformable->SetCollisionBox(mBoxLengths[0], mBoxLengths[1], mBoxLengths[2]);
   mTransformable = NULL;
   mParentTransformable = NULL;
}

void TransformableTests::TestDefaultBools()
{
   CPPUNIT_ASSERT(!mTransformable->GetRenderCollisionGeometry());
   CPPUNIT_ASSERT(!mTransformable->GetIsRenderingProxyNode());
   CPPUNIT_ASSERT(!mTransformable->GetCollisionDetection());
}

void TransformableTests::TestGetCollisionGeomDimensions()
{
   std::vector<float> dimensions;
   mTransformable->GetCollisionGeomDimensions(dimensions);

   CPPUNIT_ASSERT_MESSAGE("Vector filled by GetCollisionGeomDimensions should be empty",
      dimensions.empty());

   // Box
   mTransformable->SetCollisionBox(mBoxLengths[0], mBoxLengths[1], mBoxLengths[2]);

   dimensions.clear();
   mTransformable->GetCollisionGeomDimensions(dimensions);

   CPPUNIT_ASSERT_MESSAGE("Vector filled by GetCollisionGeomDimensions should only have 3 elements (since it's a cube",
      dimensions.size() ==  3);

   CPPUNIT_ASSERT_MESSAGE("Collision geometry dimensions should be (2.0f, 3.0f, 4.0f)",
      osg::equivalent(dimensions[0], mBoxLengths[0], 1e-2f) &&
      osg::equivalent(dimensions[1], mBoxLengths[1], 1e-2f) &&
      osg::equivalent(dimensions[2], mBoxLengths[2], 1e-2f));
}

void TransformableTests::TestAbsoluteRelative()
{
   dtCore::Transform xform;
   //Set the parent to transform pre-created in setup
   mParentTransformable->SetTransform(mTransform, dtCore::Transformable::ABS_CS);
   mParentTransformable->AddChild(mTransformable.get());

   mTransformable->GetTransform(xform, dtCore::Transformable::ABS_CS);
   osg::Matrix mat, matExpected;
   xform.Get(mat);
   mTransform.Get(matExpected);
   //Absolute of the child should match the absolute of the parent.
   CPPUNIT_ASSERT(CompareMatrix(mat, matExpected));

   mTransformable->GetTransform(xform, dtCore::Transformable::REL_CS);

   osg::Vec3 trans, rot, transExpected, rotExpected;
   xform.GetTranslation(trans);
   xform.GetRotation(rot);
   //make sure the relative transform of the child null.
   CPPUNIT_ASSERT(CompareVector(trans, osg::Vec3()));
   CPPUNIT_ASSERT(CompareVector(rot, osg::Vec3()));
   mTransform.GetTranslation(trans);

   //Set the translation of the child to be same as the parent, but assign it to relative, so it will
   //move twice as far in absolute space.
   xform.SetTranslation(trans);
   mTransformable->SetTransform(xform, dtCore::Transformable::REL_CS);

   //Get back the absolute transform for the child.
   mTransformable->GetTransform(xform, dtCore::Transformable::ABS_CS);

   xform.GetTranslation(trans);
   xform.GetRotation(rot);

   dtCore::Transform xformRel, xformExpected;
   //Get back the absolute transform for the child.
   mTransformable->GetTransform(xformRel, dtCore::Transformable::REL_CS);

   osg::Matrix matParent, matChild;
   xformRel.Get(matParent);
   mTransform.Get(matChild);
   xformExpected.Set(matParent*matChild);

   xformExpected.GetTranslation(transExpected);
   xformExpected.GetRotation(rotExpected);

   CPPUNIT_ASSERT(CompareVector(trans, transExpected));
   CPPUNIT_ASSERT(CompareVector(rot, rotExpected));

}

void TransformableTests::TestEpsilonEquals()
{
   dtCore::Transform xform1, xform2;

   xform1.MakeIdentity();
   xform2.MakeIdentity();

   CPPUNIT_ASSERT(xform1.EpsilonEquals(xform2, 0.001f));
   CPPUNIT_ASSERT(xform2.EpsilonEquals(xform1, 0.001f));

   osg::Vec3 testPos(21.2f, 23.3f, 9.6f);
   xform2.SetTranslation(testPos);

   CPPUNIT_ASSERT(!xform1.EpsilonEquals(xform2, 0.001f));
   CPPUNIT_ASSERT(!xform2.EpsilonEquals(xform1, 0.001f));

   xform1.SetTranslation(testPos);

   CPPUNIT_ASSERT(xform1.EpsilonEquals(xform2, 0.001f));
   CPPUNIT_ASSERT(xform2.EpsilonEquals(xform1, 0.001f));
}


void TransformableTests::TestSetCollisionBox()
{
   dGeomID geomID = mTransformable->GetGeomID();
   CPPUNIT_ASSERT_MESSAGE("GeomID should be valid even without a valid collision shape",
      geomID != 0);

   CPPUNIT_ASSERT_MESSAGE("Collision detection should by false without a valid collision shape",
      mTransformable->GetCollisionDetection() == false);

   CPPUNIT_ASSERT_MESSAGE("Collision geometry type should be NONE without a collision shape",
      mTransformable->GetCollisionGeomType() == &Transformable::CollisionGeomType::NONE);

   mTransformable->SetCollisionBox(mBoxLengths[0], mBoxLengths[1], mBoxLengths[2]);

   geomID = mTransformable->GetGeomID();
   CPPUNIT_ASSERT_MESSAGE("GeomID should still be valid",
      geomID != 0);

   CPPUNIT_ASSERT_MESSAGE("Collision detection should by true",
      mTransformable->GetCollisionDetection() == true);

   CPPUNIT_ASSERT_MESSAGE("Collision geometry type should be CUBE",
      mTransformable->GetCollisionGeomType() == &Transformable::CollisionGeomType::CUBE);
}

void TransformableTests::TestODEInSyncOnSetTransform()
{
   // In order to test scale, we must have a valid collsion geom
   mTransformable->SetCollisionBox(mBoxLengths[0], mBoxLengths[1], mBoxLengths[2]);

   // Verify ODE transforms are initialized correct

   dGeomID geomID = mTransformable->GetGeomID();

   // Position
   const dReal* odePosition = dGeomGetPosition(geomID);
   CPPUNIT_ASSERT_MESSAGE("ODE position should be (0.0f,0.0f,0.0f)",
      osg::equivalent(float(odePosition[0]), 0.0f, 1e-2f) &&
      osg::equivalent(float(odePosition[1]), 0.0f, 1e-2f) &&
      osg::equivalent(float(odePosition[2]), 0.0f, 1e-2f));

   // Rotation
   const dReal* odeRotation = dGeomGetRotation(geomID);
   CPPUNIT_ASSERT_MESSAGE("ODE's rotation should be zero",
      osg::equivalent(double(odeRotation[ 0]), 1.0, 1e-2) &&
      osg::equivalent(double(odeRotation[ 1]), 0.0, 1e-2) &&
      osg::equivalent(double(odeRotation[ 2]), 0.0, 1e-2) &&
      osg::equivalent(double(odeRotation[ 4]), 0.0, 1e-2) &&
      osg::equivalent(double(odeRotation[ 5]), 1.0, 1e-2) &&
      osg::equivalent(double(odeRotation[ 6]), 0.0, 1e-2) &&
      osg::equivalent(double(odeRotation[ 8]), 0.0, 1e-2) &&
      osg::equivalent(double(odeRotation[ 9]), 0.0, 1e-2) &&
      osg::equivalent(double(odeRotation[10]), 1.0, 1e-2));

   // Scale
   std::vector<float> dimensions;
   mTransformable->GetCollisionGeomDimensions(dimensions);
   CPPUNIT_ASSERT_MESSAGE("Collision geomertry dimensions should be (2.0f, 3.0f, 4.0f)",
      osg::equivalent(dimensions[0], mBoxLengths[0], 1e-2f) &&
      osg::equivalent(dimensions[1], mBoxLengths[1], 1e-2f) &&
      osg::equivalent(dimensions[2], mBoxLengths[2], 1e-2f));

   // Now let's set a real Transform and verify ODE knows about it
   mTransformable->SetTransform(mTransform);

   // Position
   osg::Vec3 osgPosition;
   mTransform.GetTranslation(osgPosition);
   odePosition = dGeomGetPosition(geomID);
   CPPUNIT_ASSERT_MESSAGE("ODE and OSG's position should be in sync",
      osg::equivalent(osgPosition[0], float(odePosition[0]), 1e-2f) &&
      osg::equivalent(osgPosition[1], float(odePosition[1]), 1e-2f) &&
      osg::equivalent(osgPosition[2], float(odePosition[2]), 1e-2f));

   // Rotation
   osg::Matrix osgRotation;
   mTransform.GetRotation(osgRotation);
   odeRotation = dGeomGetRotation(geomID);
   CPPUNIT_ASSERT_MESSAGE("ODE and OSG's rotation should be in sync",
      osg::equivalent(double(osgRotation(0,0)), double(odeRotation[ 0]), 1e-2) &&
      osg::equivalent(double(osgRotation(1,0)), double(odeRotation[ 1]), 1e-2) &&
      osg::equivalent(double(osgRotation(2,0)), double(odeRotation[ 2]), 1e-2) &&
      osg::equivalent(double(osgRotation(0,1)), double(odeRotation[ 4]), 1e-2) &&
      osg::equivalent(double(osgRotation(1,1)), double(odeRotation[ 5]), 1e-2) &&
      osg::equivalent(double(osgRotation(2,1)), double(odeRotation[ 6]), 1e-2) &&
      osg::equivalent(double(osgRotation(0,2)), double(odeRotation[ 8]), 1e-2) &&
      osg::equivalent(double(osgRotation(1,2)), double(odeRotation[ 9]), 1e-2) &&
      osg::equivalent(double(osgRotation(2,2)), double(odeRotation[10]), 1e-2));

   // Scale
   dimensions.clear();
   mTransformable->GetCollisionGeomDimensions(dimensions);
   CPPUNIT_ASSERT_MESSAGE("Collision geomertry dimensions should be (4.0f, 6.0f, 8.0f)",
      osg::equivalent(dimensions[0], mBoxLengths[0], 1e-2f) &&
      osg::equivalent(dimensions[1], mBoxLengths[1], 1e-2f) &&
      osg::equivalent(dimensions[2], mBoxLengths[2], 1e-2f));
}

bool HasChild(dtCore::DeltaDrawable* parent, dtCore::DeltaDrawable* child)
{
   bool result(false);
   for (unsigned i = 0; i < parent->GetNumChildren(); ++i)
   {
      result = result || parent->GetChild(i) == child;
   }
   return result;
}

bool HasChild(osg::Group* parent, osg::Node* child)
{
   return parent->containsNode(child);
}

void TransformableTests::TestSetPosHPR()
{
   osg::Vec3 hpr (10.0f, -30.0f, 23.0f);
   osg::Vec3 pos(21.0f, 33.66f, 293.9f);

   dtCore::Transform xform;
   xform.Set(pos, hpr);

   osg::Vec3 retPos, retHpr;

   xform.Get(retPos, retHpr);

   CPPUNIT_ASSERT(CompareVector(pos, retPos));
   CPPUNIT_ASSERT(CompareVector(hpr, retHpr));
}

void TransformableTests::TestSetPosQuat()
{
   osg::Quat quat (std::sqrt(0.5), 0.0, std::sqrt(0.5), 0.0);
   osg::Vec3 pos(21.0f, 33.66f, 293.9f);

   dtCore::Transform xform;
   xform.Set(pos, quat);

   osg::Vec3 retPos;
   osg::Quat retQuat;

   xform.Get(retPos, retQuat);

   CPPUNIT_ASSERT(CompareVector(pos, retPos));
   CPPUNIT_ASSERT(dtUtil::Equivalent(quat, retQuat, 4, osg::Quat::value_type(0.001f)));
}

void TransformableTests::TestSetMatrix()
{
   osg::Vec3 trans(10.0f, 7.0f, 2.0f);
   osg::Vec3 scale(3.0f, 1.0f, 5.0f);
   osg::Matrix matRotate, matScale, matTest;

   matRotate.makeRotate(osg::DegreesToRadians(45.0), osg::Vec3(1.0f, 0.0f, 0.0f));
   matScale.makeScale(scale);
   matRotate = matRotate * matScale;
   matTest = matRotate;
   matTest.setTrans(trans);

   osg::Vec3 testScale, testTrans;
   osg::Matrix testRot;

   dtCore::Transform transformTest;

   transformTest.Set(matTest);
   transformTest.GetTranslation(testTrans);
   transformTest.CalcScale(testScale);
   transformTest.GetRotation(testRot);

   CPPUNIT_ASSERT(CompareMatrix(matRotate, testRot));
   CPPUNIT_ASSERT(CompareVector(scale, testScale));
   CPPUNIT_ASSERT(CompareVector(trans, testTrans));
}

void TransformableTests::TestScale()
{
   osg::Vec3 scale(3.0f, 1.0f, 5.0f);

   dtCore::Transform transformTest;
   transformTest.Rescale(scale);
   mTransformable->SetTransform(transformTest);
   mTransformable->GetTransform(transformTest);

   osg::Vec3 testScale;
   transformTest.CalcScale(testScale);

   std::ostringstream ss;
   ss << "Value is: \"" << testScale << "\" but it should be \"" << scale << "\"";
   CPPUNIT_ASSERT_MESSAGE(ss.str(), CompareVector(scale, testScale));
}

void TransformableTests::TestTranslation()
{
   osg::Vec3 trans(10.0f, 7.0f, 2.0f);

   dtCore::Transform transformTest;
   transformTest.SetTranslation(trans);
   mTransformable->SetTransform(transformTest);
   mTransformable->GetTransform(transformTest);

   osg::Vec3 testTrans;
   transformTest.GetTranslation(testTrans);

   std::ostringstream ss;
   ss << "Value is: \"" << testTrans << "\" but it should be \"" << trans << "\"";
   CPPUNIT_ASSERT_MESSAGE(ss.str(), CompareVector(trans, testTrans));
}

void TransformableTests::TestMove()
{
   osg::Vec3 trans(10.0f, 7.0f, 2.0f);
   osg::Vec3f moveF(-1.1f, -3.6f, 21.44f);
   osg::Vec3d moveD(moveF);

   dtCore::Transform transformTest;
   transformTest.SetTranslation(trans);

   transformTest.Move(moveF);

   osg::Vec3 testTrans;
   transformTest.GetTranslation(testTrans);

   std::ostringstream ss;
   ss << "Value is: \"" << testTrans << "\" but it should be \"" << trans + moveF << "\"";
   CPPUNIT_ASSERT_MESSAGE(ss.str(), CompareVector(trans + moveF, testTrans));

   transformTest.SetTranslation(trans);

   transformTest.Move(moveD);

   transformTest.GetTranslation(testTrans);

   ss.str("");
   ss << "Value is: \"" << testTrans << "\" but it should be \"" << trans + osg::Vec3(moveD) << "\"";
   CPPUNIT_ASSERT_MESSAGE(ss.str(), CompareVector(trans + osg::Vec3(moveD), testTrans));
}

void TransformableTests::TestRotationHPR()
{
   osg::Vec3 rot(10.0f, 7.0f, 2.0f);

   dtCore::Transform transformTest;
   transformTest.SetRotation(rot);
   mTransformable->SetTransform(transformTest);
   mTransformable->GetTransform(transformTest);

   osg::Vec3 testRot;
   transformTest.GetRotation(testRot);

   std::ostringstream ss;
   ss << "Value is: \"" << testRot << "\" but it should be \"" << rot << "\"";
   CPPUNIT_ASSERT_MESSAGE(ss.str(), CompareVector(rot, testRot));
}

void TransformableTests::TestRotationQuat()
{
   osg::Quat qrot(0.0, 0.0, 1.0, 0.0);

   dtCore::Transform transformTest;
   transformTest.SetRotation(qrot);

   osg::Quat resultQuat;

   transformTest.GetRotation(resultQuat);

   std::ostringstream ss;
   ss << "Value is: \"" << resultQuat << "\" but it should be \"" << qrot << "\"";
   CPPUNIT_ASSERT_MESSAGE(ss.str(), dtUtil::Equivalent(qrot, resultQuat, 4, 0.001));

   osg::Matrix expectedMatrix, resultMatrix;
   expectedMatrix.makeIdentity();

   expectedMatrix.setRotate(qrot);

   transformTest.GetRotation(resultMatrix);
   ss.str("Value is: \"");
   ss << expectedMatrix << "\" but it should be \"" << resultMatrix << "\"";
   CPPUNIT_ASSERT_MESSAGE(ss.str(), CompareMatrix(expectedMatrix, resultMatrix));
}

void TransformableTests::TestTransRotScaleGetSet()
{
   osg::Vec3 trans(10.0f, 7.0f, 2.0f);
   osg::Vec3 rot(10.0f, 7.0f, 2.0f);

   dtCore::Transform transformTest;
   transformTest.SetTranslation(trans);
   transformTest.SetRotation(rot);
   mTransformable->SetTransform(transformTest);
   mTransformable->GetTransform(transformTest);

   osg::Vec3 testScale, testRot, testTrans;
   transformTest.GetTranslation(testTrans);
   transformTest.GetRotation(testRot);

   std::ostringstream ss;
   ss << "rotation value is: \"" << testRot << "\" but it should be \"" << rot << "\"";
   CPPUNIT_ASSERT_MESSAGE(ss.str(), CompareVector(rot, testRot));
   ss.str("");
   ss << "translation value is: \"" << testTrans << "\" but it should be \"" << trans << "\"";
   CPPUNIT_ASSERT_MESSAGE(ss.str(), CompareVector(trans, testTrans));
}


bool TransformableTests::CompareMatrix(const osg::Matrix& rhs, const osg::Matrix& lhs) const
{
   for (int i = 0; i < 4; ++i)
   {
      for (int j = 0; j < 4; ++j)
      {
         if (std::abs(rhs(i, j) - lhs(i, j)) > TEST_EPSILON) return false;
      }
   }

   return true;
}

bool TransformableTests::CompareVector(const osg::Vec3& rhs, const osg::Vec3& lhs) const
{
   return dtUtil::Equivalent(rhs, lhs, TEST_EPSILON);
}


void TransformableTests::TestConstructorTakingMatrixNode()
{
   osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform();
   dtCore::RefPtr<dtCore::Object> object = new dtCore::Object(*mt);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Object does not contain the supplied MatrixNode",
                                 mt.get(), object->GetMatrixNode());

   CPPUNIT_ASSERT_MESSAGE("Transformable does not contain the supplied node",
                           mt.get() == object->GetOSGNode());
}

void TransformableTests::TestValueOperators()
{
   dtCore::Transform xform;
   xform.MakeIdentity();

   CPPUNIT_ASSERT_DOUBLES_EQUAL(osg::Matrix::value_type(1.0), xform(0,0), 0.001);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(osg::Matrix::value_type(0.0), xform(0,1), 0.001);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(osg::Matrix::value_type(1.0), xform(3,3), 0.001);

   xform(3,0) = 5.6;
   CPPUNIT_ASSERT_DOUBLES_EQUAL(osg::Matrix::value_type(5.6), xform(3,0), 0.001);
}
void TransformableTests::TestRows()
{
   dtCore::Transform transformTest;
   osg::Matrix m;
   float count = 0.0;
   for (size_t i = 0; i < 4; ++i)
   {
      for (size_t j = 0; j < 4; ++j)
      {
         m(i,j) = count;
         count++;
      }
   }

   transformTest.Set(m);

   osg::Vec3 row3, row3Expected;
   osg::Vec4 row4, row4Expected;

   transformTest.GetRow(1, row3);
   row3Expected.set(4.0f, 5.0f, 6.0f);

   CPPUNIT_ASSERT(dtUtil::Equivalent(row3Expected, row3, TEST_EPSILON));

   row3Expected.set(9.0f, 8.0f, 1.0f);
   transformTest.SetRow(2, row3Expected);
   transformTest.GetRow(2, row3);
   CPPUNIT_ASSERT(dtUtil::Equivalent(row3Expected, row3, TEST_EPSILON));

   transformTest.GetRow(1, row4);
   row4Expected.set(4.0f, 5.0f, 6.0f, 7.0f);
   CPPUNIT_ASSERT(dtUtil::Equivalent(row4Expected, row4, TEST_EPSILON));

   row4Expected.set(104.3f, 90.7f, 98.6f, 101.6f);
   transformTest.SetRow(2, row4Expected);
   transformTest.GetRow(2, row4);
   CPPUNIT_ASSERT(dtUtil::Equivalent(row4Expected, row4, TEST_EPSILON));
}

void TransformableTests::TestValid()
{
   dtCore::Transform xform;
   xform.MakeIdentity();

   for (unsigned i = 0; i < 4; ++i)
   {
      for (unsigned j = 0; j < 4; ++j)
      {
         xform(i, j) = std::numeric_limits<float>::signaling_NaN();
         CPPUNIT_ASSERT(!xform.IsValid());
         // It doesn't work for infinity
         //xform(i, j) = INFINITY;
         //CPPUNIT_ASSERT(!xform.IsValid());
         xform.MakeIdentity();
         CPPUNIT_ASSERT(xform.IsValid());
      }
   }

}
//////////////////////////////////////////////////////////////////////////
void TransformableTests::TestDistance()
{
   dtCore::Transform transform1;
   dtCore::Transform transform2;

   transform1.SetTranslation(osg::Vec3(0.f,0.f,10.f));
   transform2.SetTranslation(osg::Vec3(0.f,0.f,0.f));

   double expectedValue = 100;
   double expectedValuert = 10;

   CPPUNIT_ASSERT(dtUtil::Equivalent(expectedValue, transform1.CalcDistanceSquared(transform2)));
   CPPUNIT_ASSERT(dtUtil::Equivalent(expectedValuert, transform1.CalcDistance(transform2)));
}

//////////////////////////////////////////////////////////////////////////
void TransformableTests::TestGetTransformWithDisabledCamera()
{
   //Test if Transformable::GetTransform() works correctly with a
   //parent-child hierarchy with the Camera disabled.
   using namespace dtCore;

   RefPtr<dtCore::View> view = new dtCore::View();
   RefPtr<Camera> cam = new Camera();
   RefPtr<Scene> scene = new Scene();

   view->SetCamera(cam.get());
   view->SetScene(scene.get());

   RefPtr<Transformable> parent = new Transformable("parent");
   Transform cameraEnabledParentTransform;
   const osg::Vec3 cameraEnabledParentXYZ(1.f, 2.f, 3.f);
   cameraEnabledParentTransform.SetTranslation(cameraEnabledParentXYZ);
   parent->SetTransform(cameraEnabledParentTransform);

   RefPtr<Transformable> child = new Transformable("child");
   Transform cameraEnabledChildTransform;
   const osg::Vec3 cameraEnabledChildXYZ(10.f, 10.f, 10.f);
   cameraEnabledChildTransform.SetTranslation(cameraEnabledChildXYZ);
   child->SetTransform(cameraEnabledChildTransform);

   parent->AddChild(child.get());

   scene->AddDrawable(parent.get());

   cam->SetEnabled(false);

   Transform cameraDisabledChildTransform;
   child->GetTransform(cameraDisabledChildTransform);
   const osg::Vec3 cameraDisabledChildXYZ = cameraDisabledChildTransform.GetTranslation();

   //the child's absolute translation should be the same as the parent's + the child's
   CPPUNIT_ASSERT_MESSAGE("A Transformable's translation should not change when the Camera is disabled",
                           dtUtil::Equivalent(cameraDisabledChildXYZ, cameraEnabledChildXYZ+cameraEnabledParentXYZ, TEST_EPSILON));
}

void TransformableTests::TestGetTransformInSceneWithNoCamera()
{
   //Make sure Transformable::GetTransform() works ok when the Transformable
   //is added to a Scene that doesn't have a Camera associated with it.
   using namespace dtCore;

   RefPtr<Transformable> transformable = new Transformable();
   transformable->SetName("testNode");

   Transform startXform;
   const osg::Vec3 startXYZ(1.f, 2.f, 3.f);
   startXform.SetTranslation(startXYZ);

   transformable->SetTransform(startXform);

   RefPtr<Scene> scene = new Scene();
   scene->AddDrawable(transformable.get());

   Transform endXform;
   transformable->GetTransform(endXform);

   CPPUNIT_ASSERT_MESSAGE("Transformable's GetTransform didn't match the SetTransform, when in a Scene without a Camera",
                          dtUtil::Equivalent(startXYZ, endXform.GetTranslation(), TEST_EPSILON));
}

void TransformableTests::TestGetTransformNotInScene()
{
   //Make sure Transformable::GetTransform() works ok when the Transformable
   //is not added to a Scene.
   using namespace dtCore;

   RefPtr<Transformable> transformable = new Transformable();
   transformable->SetName("testNode");

   Transform startXform;
   const osg::Vec3 startXYZ(1.f, 2.f, 3.f);
   startXform.SetTranslation(startXYZ);

   transformable->SetTransform(startXform);

   Transform endXform;
   transformable->GetTransform(endXform);

   CPPUNIT_ASSERT_MESSAGE("Transformable's GetTransform didn't match the SetTransform, when not in a Scene.",
                           dtUtil::Equivalent(startXYZ, endXform.GetTranslation(), TEST_EPSILON));
}

//////////////////////////////////////////////////////////////////////////
void TransformableTests::TestGetTransformFromInactiveTransformable()
{
   using namespace dtCore;

   RefPtr<Transformable> transformable = new Transformable();
   transformable->SetName("testNode");

   Transform startXform;
   const osg::Vec3 startXYZ(1.f, 2.f, 3.f);
   startXform.SetTranslation(startXYZ);

   transformable->SetTransform(startXform);

   transformable->SetActive(false);

   Transform endXform;
   transformable->GetTransform(endXform);

   CPPUNIT_ASSERT_MESSAGE("Transformable's GetTransform didn't match the SetTransform, when InActive.",
                         dtUtil::Equivalent(startXYZ, endXform.GetTranslation(), TEST_EPSILON));
}

//////////////////////////////////////////////////////////////////////////
void TransformableTests::TestGetTransformFromInactiveParent()
{
   using namespace dtCore;
   RefPtr<Transformable> parent = new Transformable();
   parent->SetName("parent");
   Transform parentStart;
   const osg::Vec3 parentStartXYZ(10.f, 10.f, 10.f);
   parentStart.SetTranslation(parentStartXYZ);
   parent->SetTransform(parentStart);
   parent->SetActive(false);

   RefPtr<Scene> scene = new dtCore::Scene();
   scene->AddDrawable(parent.get());

   RefPtr<Transformable> child = new Transformable();
   child->SetName("child");

   parent->AddChild(child.get());
   child->SetActive(true);

   Transform startXform;
   const osg::Vec3 childStartXYZ(1.f, 2.f, 3.f);
   startXform.SetTranslation(childStartXYZ);
   child->SetTransform(startXform, Transformable::REL_CS);

   Transform endXform;
   child->GetTransform(endXform, Transformable::ABS_CS);

   CPPUNIT_ASSERT_MESSAGE("A child Transformable's GetTransform() didn't add up when parent is InActive.",
      dtUtil::Equivalent(childStartXYZ+parentStartXYZ, endXform.GetTranslation(), TEST_EPSILON));
}

