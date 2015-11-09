/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2006-2008, Alion Science and Technology Corporation
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
#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>

#include <vector>
#include <string>
#include <sstream>

#include <osg/Vec3>
#include <osg/Vec3d>
#include <osg/Vec2d>
#include <osg/Math>
#include <osg/io_utils>

#include <dtCore/refptr.h>
#include <dtUtil/coordinates.h>
#include <dtCore/datatype.h>
#include <dtCore/actorfactory.h>
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
         dtCore::ActorFactory::GetInstance().LoadActorRegistry(dtCore::ActorFactory::DEFAULT_ACTOR_LIBRARY);
         dtCore::RefPtr<dtCore::BaseActorObject> proxy = dtCore::ActorFactory::GetInstance().
            CreateActor(*dtActors::EngineActorRegistry::COORDINATE_CONFIG_ACTOR_TYPE);

         mCoordinateProxy = dynamic_cast<dtActors::CoordinateConfigActorProxy*>(proxy.get());
         mCoordinateConfigActor = dynamic_cast<dtActors::CoordinateConfigActor*>(proxy->GetDrawable());
      }

      void tearDown()
      {
         mCoordinateProxy = NULL;
         mCoordinateConfigActor = NULL;
         dtCore::ActorFactory::GetInstance().UnloadActorRegistry(dtCore::ActorFactory::DEFAULT_ACTOR_LIBRARY);
      }

      void TestBasics()
      {
         mCoordinateConfigActor->SetUTMZone(14);
         CPPUNIT_ASSERT_EQUAL(14, mCoordinateConfigActor->GetUTMZone());
         CPPUNIT_ASSERT_EQUAL(unsigned(14), mCoordinateConfigActor->GetCoordinateConverter().GetUTMZone());

         CPPUNIT_ASSERT_MESSAGE("The coordinate config actor should default to not using the Geo origin.",
            !mCoordinateConfigActor->UseGeoOrigin());

         mCoordinateConfigActor->SetUseGeoOrigin(true);

         CPPUNIT_ASSERT_MESSAGE("The coordinate config actor should be set to using Geo origin.",
            mCoordinateConfigActor->UseGeoOrigin());
      }

      void TestCoordTypes()
      {
         CPPUNIT_ASSERT_MESSAGE("The local coordinate type should default to CARTESIAN_UTM",
            dtUtil::LocalCoordinateType::CARTESIAN_UTM == mCoordinateConfigActor->GetLocalCoordinateType());
         CPPUNIT_ASSERT_MESSAGE("The incoming coordinate type should default to UTM",
            dtUtil::IncomingCoordinateType::UTM == mCoordinateConfigActor->GetIncomingCoordinateType());
      }

      void TestOriginProps()
      {
         osg::Vec3d expected(12.1, 34.3, 89.1);
         mCoordinateConfigActor->SetGeoOrigin(expected);
         CPPUNIT_ASSERT_EQUAL(expected, mCoordinateConfigActor->GetGeoOrigin());

         mCoordinateConfigActor->SetOriginLocation(expected);
         CPPUNIT_ASSERT_EQUAL(expected, mCoordinateConfigActor->GetOriginLocation());
      }

      void TestWithGeoOrigin()
      {
         mCoordinateConfigActor->SetUseGeoOrigin(true);

         osg::Vec3d expected(33.197073732496300, -117.479809816835000, 0.0);
         mCoordinateConfigActor->SetGeoOrigin(expected);

         mCoordinateConfigActor->SetOriginLocation(expected);

         osg::Vec3d actual;
         actual = mCoordinateConfigActor->GetConvertedGeoOrigin();

         CPPUNIT_ASSERT_MESSAGE("The converted Geo origin should match the one set.",
                  dtUtil::Equivalent(expected, actual, 1e-7));

         mCoordinateConfigActor->SetOriginLocation(expected);

      }

      void TestWithoutGeoOrigin()
      {
         mCoordinateConfigActor->SetUseGeoOrigin(false);

         osg::Vec3d setValue(12.1, 34.3, 89.1);
         mCoordinateConfigActor->SetGeoOrigin(setValue);

         mCoordinateConfigActor->SetOriginLocation(setValue);

         osg::Vec3d actual;
         actual = mCoordinateConfigActor->GetConvertedGeoOrigin();

         osg::Vec3d localOffset;
         double latitude, longitude;
         mCoordinateConfigActor->GetCoordinateConverter().GetLocalOffset(localOffset);

         dtUtil::Coordinates::ConvertUTMToGeodetic(
            mCoordinateConfigActor->GetCoordinateConverter().GetUTMZone(),
            mCoordinateConfigActor->GetCoordinateConverter().GetUTMHemisphere(),
            localOffset.x(), localOffset.y() , latitude, longitude);

         osg::Vec3d expected(osg::RadiansToDegrees(latitude), osg::RadiansToDegrees(longitude), localOffset.z());

         CPPUNIT_ASSERT_MESSAGE("The converted Geo origin should match up with the origin location.",
                  dtUtil::Equivalent(expected, actual, 1e-3));
      }

   private:

      dtCore::RefPtr<dtActors::CoordinateConfigActorProxy> mCoordinateProxy;
      dtCore::RefPtr<dtActors::CoordinateConfigActor> mCoordinateConfigActor;
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(CoordinateConfigActorTests);
