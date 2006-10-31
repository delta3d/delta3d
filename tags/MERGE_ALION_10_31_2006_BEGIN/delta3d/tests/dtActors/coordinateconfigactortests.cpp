/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation.
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
 * @author David Guthrie
 */

#include <cppunit/extensions/HelperMacros.h>

#include <vector>
#include <string>
#include <iostream>

#include <osg/Vec3>
#include <osg/Vec3d>
#include <osg/Vec2d>
#include <osg/Math>

#include <dtCore/refptr.h>
#include <dtUtil/coordinates.h>
#include <dtDAL/datatype.h>
#include <dtDAL/librarymanager.h>
#include <dtGame/messagetype.h>
#include <dtActors/engineactorregistry.h>
#include <dtActors/coordinateconfigactor.h>

class CoordinateConfigActorTests : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE(CoordinateConfigActorTests);
  
   CPPUNIT_TEST(TestBasics);
   CPPUNIT_TEST(TestCoordTypes);
   CPPUNIT_TEST(TestOriginProps);
   CPPUNIT_TEST(TestWithGeoOrigin);
   CPPUNIT_TEST(TestWithoutGeoOrigin);

   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp()
      {
         dtCore::RefPtr<dtDAL::ActorProxy> proxy = dtDAL::LibraryManager::GetInstance().
            CreateActorProxy(*dtActors::EngineActorRegistry::COORDINATE_CONFIG_ACTOR_TYPE);
         
         mCoordinateProxy = dynamic_cast<dtActors::CoordinateConfigActorProxy*>(proxy.get());
         mCoordinateConfigActor = dynamic_cast<dtActors::CoordinateConfigActor*>(proxy->GetActor());
      }
      
      void tearDown()
      {
         mCoordinateProxy = NULL;
         mCoordinateConfigActor = NULL;
      }
      
      void TestBasics()
      {
         mCoordinateConfigActor->SetUTMZone(14);
         CPPUNIT_ASSERT_EQUAL(14, mCoordinateConfigActor->GetUTMZone());
         CPPUNIT_ASSERT_EQUAL(unsigned(14), mCoordinateConfigActor->GetCoordinateConverter().GetUTMZone());

         CPPUNIT_ASSERT_MESSAGE("The coordinate config actor should default to not using the Geo origin.",
            !mCoordinateConfigActor->UseGeoOrigin());
         CPPUNIT_ASSERT_MESSAGE("The coordinate config actor should default to not using the Geo origin rotation.",
            !mCoordinateConfigActor->UseGeoOriginRotation());
         
         mCoordinateConfigActor->SetUseGeoOrigin(true);
         mCoordinateConfigActor->SetUseGeoOriginRotation(true);

         CPPUNIT_ASSERT_MESSAGE("The coordinate config actor should be set to using Geo origin.",
            mCoordinateConfigActor->UseGeoOrigin());
         CPPUNIT_ASSERT_MESSAGE("The coordinate config actor should be set to using the Geo origin rotation.",
            mCoordinateConfigActor->UseGeoOriginRotation());
      }
    
      void TestCoordTypes()
      {
         CPPUNIT_ASSERT_MESSAGE("The local coordinate type should default to CARTESIAN", 
            dtUtil::LocalCoordinateType::CARTESIAN == mCoordinateConfigActor->GetLocalCoordinateType());
         CPPUNIT_ASSERT_MESSAGE("The incoming coordinate type should default to UTM", 
            dtUtil::IncomingCoordinateType::UTM == mCoordinateConfigActor->GetIncomingCoordinateType());
      }
          
      void TestOriginProps()
      {
         osg::Vec3d expected(12.1, 34.3, 89.1);
         mCoordinateConfigActor->SetGeoOrigin(expected);
         CPPUNIT_ASSERT_EQUAL(expected, mCoordinateConfigActor->GetGeoOrigin());

         osg::Vec2d expectedll(12.1, 34.3);
         mCoordinateConfigActor->SetGeoOriginRotation(expectedll);
         CPPUNIT_ASSERT_EQUAL(expectedll, mCoordinateConfigActor->GetGeoOriginRotation());
         
         mCoordinateConfigActor->SetOriginLocation(expected);
         CPPUNIT_ASSERT_EQUAL(expected, mCoordinateConfigActor->GetOriginLocation());
         
         osg::Vec3 expectedRotation(12.1, 34.3, 89.1);
         mCoordinateConfigActor->SetOriginRotation(expectedRotation);
         CPPUNIT_ASSERT_EQUAL(expectedRotation, mCoordinateConfigActor->GetOriginRotation());         
      }
      
      void TestWithGeoOrigin()
      {
         mCoordinateConfigActor->SetUseGeoOrigin(true);
         mCoordinateConfigActor->SetUseGeoOriginRotation(true);

         osg::Vec3d expected(33.197073732496300, -117.479809816835000, 0.0);
         mCoordinateConfigActor->SetGeoOrigin(expected);

         mCoordinateConfigActor->SetOriginLocation(expected);
         
         osg::Vec3d actual;
         actual = mCoordinateConfigActor->GetConvertedGeoOrigin();
         
         CPPUNIT_ASSERT_MESSAGE("The converted Geo origin should match the one set.",
            osg::equivalent(actual.x(), expected.x(), 1e-7) &&
            osg::equivalent(actual.y(), expected.y(), 1e-7) &&
            osg::equivalent(actual.z(), expected.z(), 1e-7)
            );

         mCoordinateConfigActor->SetGeoOriginRotation(osg::Vec2d(expected.x(), expected.y()));

         mCoordinateConfigActor->SetOriginLocation(expected);
         
         actual = mCoordinateConfigActor->GetCurrentOriginRotation();
         
         CPPUNIT_ASSERT_MESSAGE("The origin Rotation should NOT match one set directly.",
            !(osg::equivalent(actual.x(), expected.x(), 1e-7) &&
            osg::equivalent(actual.y(), expected.y(), 1e-7) &&
            osg::equivalent(actual.z(), expected.z(), 1e-7))
            );
            
      }
      
      void TestWithoutGeoOrigin()
      {
         mCoordinateConfigActor->SetUseGeoOrigin(false);
         mCoordinateConfigActor->SetUseGeoOriginRotation(false);

         osg::Vec3d setValue(12.1, 34.3, 89.1);
         mCoordinateConfigActor->SetGeoOrigin(setValue);

         mCoordinateConfigActor->SetOriginLocation(setValue);
         
         osg::Vec3d actual;
         actual = mCoordinateConfigActor->GetConvertedGeoOrigin();
         
         double x, y, z;
         double latitude, longitude;
         mCoordinateConfigActor->GetCoordinateConverter().GetOriginLocation(x, y, z);      

         mCoordinateConfigActor->GetCoordinateConverter().ConvertUTMToGeodetic(
            mCoordinateConfigActor->GetCoordinateConverter().GetUTMZone(), 
            x, y, latitude, longitude);

         osg::Vec3d expected(osg::RadiansToDegrees(latitude), osg::RadiansToDegrees(longitude), z);

         CPPUNIT_ASSERT_MESSAGE("The converted Geo origin should match up with the origin location.",
            osg::equivalent(actual.x(), expected.x(), 1e-3) &&
            osg::equivalent(actual.y(), expected.y(), 1e-3) &&
            osg::equivalent(actual.z(), expected.z(), 1e-3)
            );

         expected = setValue;
         mCoordinateConfigActor->SetGeoOriginRotation(osg::Vec2d(expected.x(), expected.y()));

         mCoordinateConfigActor->SetOriginRotation(expected);
         
         actual = mCoordinateConfigActor->GetCurrentOriginRotation();
         
         std::ostringstream ss;
         ss << "The origin Rotation SHOULD match the one set directly. Actual: " << actual << " Expected: " << expected;
         CPPUNIT_ASSERT_MESSAGE(ss.str(),
            osg::equivalent(actual.x(), expected.x(), 1e-3) &&
            osg::equivalent(actual.y(), expected.y(), 1e-3) &&
            osg::equivalent(actual.z(), expected.z(), 1e-3)
            );
      }
   
   private:
      
      dtCore::RefPtr<dtActors::CoordinateConfigActorProxy> mCoordinateProxy;
      dtCore::RefPtr<dtActors::CoordinateConfigActor> mCoordinateConfigActor;
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(CoordinateConfigActorTests);
