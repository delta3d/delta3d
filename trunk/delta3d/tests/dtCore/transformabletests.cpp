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
   CPPUNIT_TEST(TestSetCollisionBox);
   CPPUNIT_TEST_SUITE_END();

public:

   void setUp();
   void tearDown();
   void TestGetCollisionGeomDimensions();
   void TestSetCollisionBox();
   void TestSceneAddDrawable();
   void TestSetTransform();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TransformableTests);

void TransformableTests::setUp()
{

}

void TransformableTests::tearDown()
{

}

void TransformableTests::TestGetCollisionGeomDimensions()
{
   dtCore::RefPtr<dtCore::Transformable> transformable = new dtCore::Transformable("TestTransformable");

   std::vector<float> dimensions;
   transformable->GetCollisionGeomDimensions(dimensions);

   CPPUNIT_ASSERT_MESSAGE( "Vector filled by GetCollisionGeomDimensions should be empty",
      dimensions.empty() );

   // Box
   osg::Vec3 boxLengths( 2.0f, 3.0f, 4.0f );
   transformable->SetCollisionBox( boxLengths[0], boxLengths[1], boxLengths[2] );

   dimensions.clear();
   transformable->GetCollisionGeomDimensions(dimensions);

   CPPUNIT_ASSERT_MESSAGE( "Vector filled by GetCollisionGeomDimensions should only have 3 elements (since it's a cube",
      dimensions.size() ==  3 );

   CPPUNIT_ASSERT_MESSAGE( "Collision geomertry dimensions should be (2.0f, 3.0f, 4.0f)", 
      osg::equivalent( dimensions[0], boxLengths[0], 1e-2f ) && 
      osg::equivalent( dimensions[1], boxLengths[1], 1e-2f ) && 
      osg::equivalent( dimensions[2], boxLengths[2], 1e-2f ) );
}

void TransformableTests::TestSetCollisionBox()
{
   dtCore::RefPtr<dtCore::Transformable> transformable = new dtCore::Transformable("TestTransformable");

   dGeomID geomID = transformable->GetGeomID();
   CPPUNIT_ASSERT_MESSAGE( "GeomID should be valid even without a valid collision shape", 
      geomID != 0 );

   CPPUNIT_ASSERT_MESSAGE( "Collision detection should by false without a valid collision shape", 
      transformable->GetCollisionDetection() == false );

   CPPUNIT_ASSERT_MESSAGE( "Collision geometry type should be NONE without a collision shape", 
      transformable->GetCollisionGeomType() == &Transformable::CollisionGeomType::NONE );

   osg::Vec3 boxLengths( 2.0f, 3.0f, 4.0f );
   transformable->SetCollisionBox( boxLengths[0], boxLengths[1], boxLengths[2] );

   geomID = transformable->GetGeomID();
   CPPUNIT_ASSERT_MESSAGE( "GeomID should still be valid", 
      geomID != 0 );

   CPPUNIT_ASSERT_MESSAGE( "Collision detection should by true", 
      transformable->GetCollisionDetection() == true );

   CPPUNIT_ASSERT_MESSAGE( "Collision geometry type should be CUBE", 
      transformable->GetCollisionGeomType() == &Transformable::CollisionGeomType::CUBE );
}

void TransformableTests::TestSetTransform()
{
   dtCore::RefPtr<dtCore::Transformable> transformable = new dtCore::Transformable("TestTransformable");

   // In order to test scale, we must have a valid collsion geom
   osg::Vec3 boxLengths( 2.0f, 3.0f, 4.0f );
   transformable->SetCollisionBox( boxLengths[0], boxLengths[1], boxLengths[2] );

   // Verify ODE transforms are initialized correct

   dGeomID geomID = transformable->GetGeomID();

   // Position
   const dReal* odePosition = dGeomGetPosition(geomID);
   CPPUNIT_ASSERT_MESSAGE( "ODE position should be (0.0f,0.0f,0.0f)",
      osg::equivalent( odePosition[0], 0.0f, 1e-2f ) && 
      osg::equivalent( odePosition[1], 0.0f, 1e-2f ) && 
      osg::equivalent( odePosition[2], 0.0f, 1e-2f ) );

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
   transformable->GetCollisionGeomDimensions(dimensions);
   CPPUNIT_ASSERT_MESSAGE( "Collision geomertry dimensions should be (2.0f, 3.0f, 4.0f)", 
      osg::equivalent( dimensions[0], 1.0f, 1e-2f ) && 
      osg::equivalent( dimensions[1], 1.0f, 1e-2f ) && 
      osg::equivalent( dimensions[2], 1.0f, 1e-2f ) );
   
   // Now let's set a real Transform and verify ODE knows about it
   Transform transform( 2.0f, 4.0f, 1.0f, 30.0f, -20.0f, 2.5f, 2.0f, 2.0f, 2.0f );
   transformable->SetTransform(&transform);

   // Position
   osg::Vec3 osgPosition;
   transform.GetTranslation(osgPosition);
   odePosition = dGeomGetPosition(geomID);
   CPPUNIT_ASSERT_MESSAGE( "ODE and OSG's position should be in sync",
      osg::equivalent( osgPosition[0], float(odePosition[0]), 1e-2f ) && 
      osg::equivalent( osgPosition[1], float(odePosition[1]), 1e-2f ) && 
      osg::equivalent( osgPosition[2], float(odePosition[2]), 1e-2f ) );

   // Rotation
   osg::Matrix osgRotation;
   transform.GetRotation(osgRotation);
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
   transform.GetScale(scale);
   dimensions.clear();
   transformable->GetCollisionGeomDimensions(dimensions);
   CPPUNIT_ASSERT_MESSAGE( "Collision geomertry dimensions should be (2.0f, 3.0f, 4.0f)", 
      osg::equivalent( dimensions[0], boxLengths[0], 1e-2f ) && 
      osg::equivalent( dimensions[1], boxLengths[1], 1e-2f ) && 
      osg::equivalent( dimensions[2], boxLengths[2], 1e-2f ) );
}

void TransformableTests::TestSceneAddDrawable()
{
   dtCore::RefPtr<dtCore::Transformable> transformable = new dtCore::Transformable("TestTransformable");
   dtCore::RefPtr<dtCore::Scene> scene = new dtCore::Scene("TestScene"); 
   scene->AddDrawable( transformable.get() );
}
