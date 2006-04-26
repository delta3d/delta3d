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
#include <dtCore/scene.h>
#include <dtCore/transformable.h>

using namespace dtCore;

class TransformableTests : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE(TransformableTests);
   CPPUNIT_TEST(TestGetCollisionGeomDimensions);
   CPPUNIT_TEST(TestSetCollisionBox);
   CPPUNIT_TEST(TestSetTransform);
   CPPUNIT_TEST_SUITE_END();

public:

   void setUp();
   void tearDown();

   void TestGetCollisionGeomDimensions();
   void TestSetCollisionBox();
   void TestSetTransform();
   void TestSceneAddDrawable();

private:

   RefPtr<Transformable> mTransformable;
   Transform mTransform;
      
   osg::Vec3 mBoxLengths; ///Dimensions for box

   float mRadius; ///Radius for sphere or cylinder
   float mHeight; ///Height for cylinder or ray
};

CPPUNIT_TEST_SUITE_REGISTRATION(TransformableTests);

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
   mTransformable = 0;
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
   mTransformable->SetTransform(&mTransform);

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

void TransformableTests::TestSceneAddDrawable()
{
   //dtCore::RefPtr<dtCore::Scene> scene = new dtCore::Scene("TestScene"); 
   //scene->AddDrawable( mTransformable.get() );
}
