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
* Chris Osborn
*/ 
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/scene.h>
#include <dtCore/transformable.h>

#include <osg/MatrixTransform>
#include <osg/io_utils>
#include <ode/collision.h>

using namespace dtCore;

class TransformableSubClass : public Transformable
{
   public:
   TransformableSubClass()
   {
   }

   void ExecuteReplaceMatrixNode()
   {
      osg::ref_ptr<osg::MatrixTransform> mNewNode( new osg::MatrixTransform() );
      mNewNode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::ON );
      ReplaceMatrixNode( mNewNode.get() );
   }
   protected:
   virtual ~TransformableSubClass()
   {
   }
};

class TransformableTests : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE(TransformableTests);
   CPPUNIT_TEST(TestGetCollisionGeomDimensions);
   CPPUNIT_TEST(TestSetCollisionBox);
   CPPUNIT_TEST(TestSetTransform);
   CPPUNIT_TEST(TestSetMatrix);
   CPPUNIT_TEST(TestScale);
   CPPUNIT_TEST(TestTranslation);
   CPPUNIT_TEST(TestRotationHPR);
   CPPUNIT_TEST(TestTransRotScaleGetSet);
   CPPUNIT_TEST(TestReplaceMatrixNode);
   CPPUNIT_TEST_SUITE_END();

public:

   void setUp();
   void tearDown();

   void TestGetCollisionGeomDimensions();
   void TestSetCollisionBox();
   void TestSetTransform();
   void TestScale();
   void TestTranslation();
   void TestRotationHPR();
   void TestTransRotScaleGetSet();
   void TestReplaceMatrixNode();
   void TestSetMatrix();

private:
    bool CompareMatrix(const osg::Matrix& rhs, const osg::Matrix& lhs) const;
    bool CompareVector(const osg::Vec3& rhs, const osg::Vec3& lhs) const;    

   RefPtr<Transformable> mTransformable;
   Transform mTransform;
      
   osg::Vec3 mBoxLengths; ///Dimensions for box

   float mRadius; ///Radius for sphere or cylinder
   float mHeight; ///Height for cylinder or ray
   static const float TEST_EPSILON;
};

CPPUNIT_TEST_SUITE_REGISTRATION(TransformableTests);

const float TransformableTests::TEST_EPSILON(1e-5f);

void TransformableTests::setUp()
{
   mTransformable = new dtCore::Transformable("TestTransformable");
   mTransform.Set( 2.0f, 4.0f, 1.0f, 30.0f, -20.0f, 2.5f, 2.0f, 2.0f, 2.0f );
   mBoxLengths.set( 2.0f, 3.0f, 4.0f );
   mRadius = 3.2f;
   mHeight = 6.4f;
}

void TransformableTests::tearDown()
{
   mTransformable->SetCollisionMesh(NULL);
   mTransformable->SetCollisionBox( mBoxLengths[0], mBoxLengths[1], mBoxLengths[2] );
   mTransformable = NULL;
}

void TransformableTests::TestGetCollisionGeomDimensions()
{
   std::vector<float> dimensions;
   mTransformable->GetCollisionGeomDimensions(dimensions);

   CPPUNIT_ASSERT_MESSAGE( "Vector filled by GetCollisionGeomDimensions should be empty",
      dimensions.empty() );

   // Box
   mTransformable->SetCollisionBox( mBoxLengths[0], mBoxLengths[1], mBoxLengths[2] );

   dimensions.clear();
   mTransformable->GetCollisionGeomDimensions(dimensions);

   CPPUNIT_ASSERT_MESSAGE( "Vector filled by GetCollisionGeomDimensions should only have 3 elements (since it's a cube",
      dimensions.size() ==  3 );

   CPPUNIT_ASSERT_MESSAGE( "Collision geomertry dimensions should be (2.0f, 3.0f, 4.0f)", 
      osg::equivalent( dimensions[0], mBoxLengths[0], 1e-2f ) && 
      osg::equivalent( dimensions[1], mBoxLengths[1], 1e-2f ) && 
      osg::equivalent( dimensions[2], mBoxLengths[2], 1e-2f ) );
}

void TransformableTests::TestSetCollisionBox()
{
   dGeomID geomID = mTransformable->GetGeomID();
   CPPUNIT_ASSERT_MESSAGE( "GeomID should be valid even without a valid collision shape", 
      geomID != 0 );

   CPPUNIT_ASSERT_MESSAGE( "Collision detection should by false without a valid collision shape", 
      mTransformable->GetCollisionDetection() == false );

   CPPUNIT_ASSERT_MESSAGE( "Collision geometry type should be NONE without a collision shape", 
      mTransformable->GetCollisionGeomType() == &Transformable::CollisionGeomType::NONE );

   mTransformable->SetCollisionBox( mBoxLengths[0], mBoxLengths[1], mBoxLengths[2] );

   geomID = mTransformable->GetGeomID();
   CPPUNIT_ASSERT_MESSAGE( "GeomID should still be valid", 
      geomID != 0 );

   CPPUNIT_ASSERT_MESSAGE( "Collision detection should by true", 
      mTransformable->GetCollisionDetection() == true );

   CPPUNIT_ASSERT_MESSAGE( "Collision geometry type should be CUBE", 
      mTransformable->GetCollisionGeomType() == &Transformable::CollisionGeomType::CUBE );
}

void TransformableTests::TestSetTransform()
{
   // In order to test scale, we must have a valid collsion geom
   mTransformable->SetCollisionBox( mBoxLengths[0], mBoxLengths[1], mBoxLengths[2] );

   // Verify ODE transforms are initialized correct

   dGeomID geomID = mTransformable->GetGeomID();

   // Position
   const dReal* odePosition = dGeomGetPosition(geomID);
   CPPUNIT_ASSERT_MESSAGE( "ODE position should be (0.0f,0.0f,0.0f)",
      osg::equivalent( float(odePosition[0]), 0.0f, 1e-2f ) && 
      osg::equivalent( float(odePosition[1]), 0.0f, 1e-2f ) && 
      osg::equivalent( float(odePosition[2]), 0.0f, 1e-2f ) );

   // Rotation
   const dReal* odeRotation = dGeomGetRotation(geomID);
   CPPUNIT_ASSERT_MESSAGE( "ODE's rotation should be zero",
      osg::equivalent( double(odeRotation[0]), 1.0, 1e-2 ) && 
      osg::equivalent( double(odeRotation[1]), 0.0, 1e-2 ) && 
      osg::equivalent( double(odeRotation[2]), 0.0, 1e-2 ) && 
      osg::equivalent( double(odeRotation[4]), 0.0, 1e-2 ) && 
      osg::equivalent( double(odeRotation[5]), 1.0, 1e-2 ) && 
      osg::equivalent( double(odeRotation[6]), 0.0, 1e-2 ) && 
      osg::equivalent( double(odeRotation[8]), 0.0, 1e-2 ) && 
      osg::equivalent( double(odeRotation[9]), 0.0, 1e-2 ) && 
      osg::equivalent( double(odeRotation[10]), 1.0, 1e-2 ) );

   // Scale
   std::vector<float> dimensions;
   mTransformable->GetCollisionGeomDimensions(dimensions);
   CPPUNIT_ASSERT_MESSAGE( "Collision geomertry dimensions should be (2.0f, 3.0f, 4.0f)", 
      osg::equivalent( dimensions[0], mBoxLengths[0], 1e-2f ) && 
      osg::equivalent( dimensions[1], mBoxLengths[1], 1e-2f ) && 
      osg::equivalent( dimensions[2], mBoxLengths[2], 1e-2f ) );
   
   // Now let's set a real Transform and verify ODE knows about it
   mTransformable->SetTransform(mTransform);

   // Position
   osg::Vec3 osgPosition;
   mTransform.GetTranslation(osgPosition);
   odePosition = dGeomGetPosition(geomID);
   CPPUNIT_ASSERT_MESSAGE( "ODE and OSG's position should be in sync",
      osg::equivalent( osgPosition[0], float(odePosition[0]), 1e-2f ) && 
      osg::equivalent( osgPosition[1], float(odePosition[1]), 1e-2f ) && 
      osg::equivalent( osgPosition[2], float(odePosition[2]), 1e-2f ) );

   // Rotation
   osg::Matrix osgRotation;
   mTransform.GetRotation(osgRotation);
   odeRotation = dGeomGetRotation(geomID);
   CPPUNIT_ASSERT_MESSAGE( "ODE and OSG's rotation should be in sync",
      osg::equivalent( osgRotation(0,0), double(odeRotation[0]), 1e-2 ) && 
      osg::equivalent( osgRotation(1,0), double(odeRotation[1]), 1e-2 ) && 
      osg::equivalent( osgRotation(2,0), double(odeRotation[2]), 1e-2 ) && 
      osg::equivalent( osgRotation(0,1), double(odeRotation[4]), 1e-2 ) && 
      osg::equivalent( osgRotation(1,1), double(odeRotation[5]), 1e-2 ) && 
      osg::equivalent( osgRotation(2,1), double(odeRotation[6]), 1e-2 ) && 
      osg::equivalent( osgRotation(0,2), double(odeRotation[8]), 1e-2 ) && 
      osg::equivalent( osgRotation(1,2), double(odeRotation[9]), 1e-2 ) && 
      osg::equivalent( osgRotation(2,2), double(odeRotation[10]), 1e-2 ) );

   // Scale
   osg::Vec3 scale;
   mTransform.GetScale(scale);
   dimensions.clear();
   mTransformable->GetCollisionGeomDimensions(dimensions);
   CPPUNIT_ASSERT_MESSAGE( "Collision geomertry dimensions should be (4.0f, 6.0f, 8.0f)", 
      osg::equivalent( dimensions[0], mBoxLengths[0]*scale[0], 1e-2f ) && 
      osg::equivalent( dimensions[1], mBoxLengths[1]*scale[1], 1e-2f ) && 
      osg::equivalent( dimensions[2], mBoxLengths[2]*scale[2], 1e-2f ) );
}

bool HasChild( dtCore::DeltaDrawable* parent, dtCore::DeltaDrawable* child )
{
   bool result(false);
   for( unsigned i = 0; i < parent->GetNumChildren(); ++i )
   {
      result = result || parent->GetChild(i) == child;
   }
   return result;
}

bool HasChild( osg::Group* parent, osg::Node* child )
{
   return parent->containsNode(child);
}

void TransformableTests::TestSetMatrix()
{
   osg::Vec3 trans(10.0f, 7.0f, 2.0f);
   osg::Vec3 scale(3.0f, 1.0f, 5.0f);
   osg::Matrix matRotate, matScale, matTest;    

   matRotate.makeRotate(osg::DegreesToRadians(45.0), osg::Vec3(1.0f, 0.0f, 0.0f));
   matScale.makeScale(scale);
   matTest = matRotate * matScale;
   matTest.setTrans(trans);

   osg::Vec3 testScale, testTrans;
   osg::Matrix testRot;

   dtCore::Transform transformTest;
    
   transformTest.Set(matTest);
   transformTest.GetTranslation(testTrans);
   transformTest.GetScale(testScale);
   transformTest.GetRotation(testRot);

   CPPUNIT_ASSERT(CompareMatrix(matRotate, testRot));
   CPPUNIT_ASSERT(CompareVector(scale, testScale));
   CPPUNIT_ASSERT(CompareVector(trans, testTrans));
}

void TransformableTests::TestScale()
{
   osg::Vec3 scale(3.0f, 1.0f, 5.0f);

   dtCore::Transform transformTest;
   transformTest.SetScale(scale);
   mTransformable->SetTransform(transformTest);
   mTransformable->GetTransform(transformTest);

   osg::Vec3 testScale;
   transformTest.GetScale(testScale);

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

void TransformableTests::TestTransRotScaleGetSet()
{
   osg::Vec3 trans(10.0f, 7.0f, 2.0f);
   osg::Vec3 scale(3.0f, 1.0f, 5.0f);
   osg::Vec3 rot(10.0f, 7.0f, 2.0f);

   dtCore::Transform transformTest;
   transformTest.SetTranslation(trans);
   transformTest.SetRotation(rot);
   transformTest.SetScale(scale);
   mTransformable->SetTransform(transformTest);
   mTransformable->GetTransform(transformTest);

   osg::Vec3 testScale, testRot, testTrans;
   transformTest.GetTranslation(testTrans);
   transformTest.GetRotation(testRot);
   transformTest.GetScale(testScale);

   std::ostringstream ss;
   ss << "rotation value is: \"" << testRot << "\" but it should be \"" << rot << "\"";
   CPPUNIT_ASSERT_MESSAGE(ss.str(), CompareVector(rot, testRot));
   ss.str("");
   ss << "translation value is: \"" << testTrans << "\" but it should be \"" << trans << "\"";
   CPPUNIT_ASSERT_MESSAGE(ss.str(), CompareVector(trans, testTrans));
   ss.str("");
   ss << "scale value is: \"" << testScale << "\" but it should be \"" << scale << "\"";
   CPPUNIT_ASSERT_MESSAGE(ss.str(), CompareVector(scale, testScale));
}


bool TransformableTests::CompareMatrix(const osg::Matrix& rhs, const osg::Matrix& lhs) const
{
    for(int i = 0; i < 4; ++i)
    {   
        for(int j = 0; j < 4; ++j)
        {
            if ( fabs(rhs(i, j) - lhs(i, j)) > TEST_EPSILON ) return false;
        }
    }

    return true;
}

bool TransformableTests::CompareVector(const osg::Vec3& rhs, const osg::Vec3& lhs) const
{
    return ( (rhs[0] - lhs[0]) < TEST_EPSILON ) && ( (rhs[1] - lhs[1]) < TEST_EPSILON ) && ( (rhs[2] - lhs[2]) < TEST_EPSILON );
}

void TransformableTests::TestReplaceMatrixNode()
{
   // This subclass should immediately call SetMatrixNode in the constructor with
   // GL_LIGHTING turned on.
   dtCore::RefPtr<TransformableSubClass> testTrans( new TransformableSubClass() );
   testTrans->ExecuteReplaceMatrixNode();

   // So, the new node should...
   // 1) Have a StateSet
   osg::ref_ptr<const osg::StateSet> stateSet( testTrans->GetOSGNode()->getStateSet() );
   CPPUNIT_ASSERT( stateSet.valid() );

   // 2) Still have GL_LIGHTING turned on (from new Node)
   CPPUNIT_ASSERT( osg::StateAttribute::ON == stateSet->getMode( GL_LIGHTING ) );

   // 3) Still have GL_NORMAL_RESCALING turned on (from original Node)
   CPPUNIT_ASSERT( osg::StateAttribute::ON == stateSet->getMode( GL_RESCALE_NORMAL ) );


   //////////

   dtCore::RefPtr<TransformableSubClass> testTransTwo( new TransformableSubClass() );
   testTransTwo->SetCollisionBox(2.0f, 3.0f, 4.0f);
   
   dtCore::RefPtr<Transformable> parent( new Transformable("Parent") );
                     
   dtCore::RefPtr<Transformable> childOne( new Transformable("ChildOne") );
   dtCore::RefPtr<Transformable> childTwo( new Transformable("ChildTwo") );

   parent->AddChild( testTransTwo.get() );
   testTransTwo->AddChild( childTwo.get() );
   testTransTwo->AddChild( childOne.get() );

   // Verify Delta3D hierarchy before replacement
   CPPUNIT_ASSERT( HasChild( parent.get(), testTransTwo.get() ) );
   CPPUNIT_ASSERT( HasChild( testTransTwo.get(), childTwo.get() ) );
   CPPUNIT_ASSERT( HasChild( testTransTwo.get(), childOne.get() ) );

   // Verify OSG hierarchy before replacement
   CPPUNIT_ASSERT( HasChild( parent->GetMatrixNode(), testTransTwo->GetOSGNode() ) );
   CPPUNIT_ASSERT( HasChild( testTransTwo->GetMatrixNode(), childTwo->GetOSGNode() ) );
   CPPUNIT_ASSERT( HasChild( testTransTwo->GetMatrixNode(), childOne->GetOSGNode() ) );

   CPPUNIT_ASSERT_EQUAL( 2U, testTransTwo->GetMatrixNode()->getNumChildren() );

   bool renderingCollisionGeometry = testTransTwo->GetRenderCollisionGeometry();
   
   testTransTwo->ExecuteReplaceMatrixNode();

   CPPUNIT_ASSERT_EQUAL(   &Transformable::CollisionGeomType::CUBE, 
                           testTransTwo->GetCollisionGeomType() );

   CPPUNIT_ASSERT_EQUAL( renderingCollisionGeometry, testTransTwo->GetRenderCollisionGeometry() );

   // Verify Delta3D hierarchy after replacement
   CPPUNIT_ASSERT( HasChild( parent.get(), testTransTwo.get() ) );
   CPPUNIT_ASSERT( HasChild( testTransTwo.get(), childTwo.get() ) );
   CPPUNIT_ASSERT( HasChild( testTransTwo.get(), childOne.get() ) );

   CPPUNIT_ASSERT_EQUAL( 2U, testTransTwo->GetMatrixNode()->getNumChildren() );
   
   CPPUNIT_ASSERT( HasChild( parent->GetMatrixNode(), testTransTwo->GetOSGNode() ) );
   CPPUNIT_ASSERT( HasChild( testTransTwo->GetMatrixNode(), childTwo->GetOSGNode() ) );
   CPPUNIT_ASSERT( HasChild( testTransTwo->GetMatrixNode(), childOne->GetOSGNode() ) );
}
