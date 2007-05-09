/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2007, Alion Science and Technology, BMH Operation.
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

#include <string>
#include <dtCore/refptr.h>
#include <dtHLAGM/ddmcalculatorgeographic.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtUtil/coordinates.h>

class DDMCalculatorGeographicTests : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE(DDMCalculatorGeographicTests);
   
      CPPUNIT_TEST(TestCoordinateConverterAttribute);
      CPPUNIT_TEST(TestBaseProperties);
      CPPUNIT_TEST(TestAppSpaceProperties);
      CPPUNIT_TEST(TestRegionTypeProperties);
      CPPUNIT_TEST(TestAppSpaceValueProperties);

   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp()
      {
         mCalcGeo = new dtHLAGM::DDMCalculatorGeographic;
      }
      
      void tearDown()
      {
         mCalcGeo = NULL;
      }
      
      void TestCoordinateConverterAttribute()
      {
         CPPUNIT_ASSERT_MESSAGE("The coordinate converter should default to Geodetic for incoming.",
               mCalcGeo->GetCoordinateConverter().GetIncomingCoordinateType() == dtUtil::IncomingCoordinateType::GEODETIC);
         
         dtUtil::Coordinates coord;
         // Set a few props to make sure it assigns correctly
         coord.SetLocalCoordinateType(dtUtil::LocalCoordinateType::GLOBE);
         coord.SetIncomingCoordinateType(dtUtil::IncomingCoordinateType::GEOCENTRIC);
         coord.SetGlobeRadius(500);
         mCalcGeo->SetCoordinateConverter(coord);
         
         const dtUtil::Coordinates& assignedCoord = mCalcGeo->GetCoordinateConverter();
         
         CPPUNIT_ASSERT_EQUAL(coord.GetGlobeRadius(), assignedCoord.GetGlobeRadius());
         CPPUNIT_ASSERT(coord.GetLocalCoordinateType() == assignedCoord.GetLocalCoordinateType());
         
         CPPUNIT_ASSERT_MESSAGE("The calculator should set the incoming coordinate type to GEODETIC", 
               assignedCoord.GetIncomingCoordinateType() == dtUtil::IncomingCoordinateType::GEODETIC);
      }
      
      void TestBaseProperties()
      {   
         CPPUNIT_ASSERT_EQUAL(TestDimensionProperty(dtHLAGM::DDMRegionCalculator::PROP_FIRST_DIMENSION_NAME, "subspace"), 
               mCalcGeo->GetFirstDimensionName());

         CPPUNIT_ASSERT_EQUAL(TestDimensionProperty(dtHLAGM::DDMRegionCalculator::PROP_SECOND_DIMENSION_NAME, "one"), 
               mCalcGeo->GetSecondDimensionName());

         CPPUNIT_ASSERT_EQUAL(TestDimensionProperty(dtHLAGM::DDMRegionCalculator::PROP_THIRD_DIMENSION_NAME, "two"), 
               mCalcGeo->GetThirdDimensionName());  
      }
            
      void TestAppSpaceProperties()
      {
         dtDAL::LongActorProperty* lap = NULL;
         mCalcGeo->GetProperty(dtHLAGM::DDMCalculatorGeographic::PROP_APP_SPACE_MIN, lap);
         CPPUNIT_ASSERT(lap != NULL);

         CPPUNIT_ASSERT_EQUAL(0L, lap->GetValue());
         lap->SetValue(23);
         CPPUNIT_ASSERT_EQUAL(23L, lap->GetValue());
         CPPUNIT_ASSERT_EQUAL(23L, mCalcGeo->GetAppSpaceMinimum());
                  
         mCalcGeo->GetProperty(dtHLAGM::DDMCalculatorGeographic::PROP_APP_SPACE_MAX, lap);
         CPPUNIT_ASSERT(lap != NULL);

         CPPUNIT_ASSERT_EQUAL_MESSAGE("The maximum app space value should default to 99.", 99L, lap->GetValue());
         lap->SetValue(28);
         CPPUNIT_ASSERT_EQUAL(28L, lap->GetValue());
         CPPUNIT_ASSERT_EQUAL(28L, mCalcGeo->GetAppSpaceMaximum());
      }

      void TestRegionTypeProperties()
      {
         TestRegionTypeEnumProp(dtHLAGM::DDMCalculatorGeographic::PROP_FRIENDLY_GROUND_REGION_TYPE);
         CPPUNIT_ASSERT(dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::APP_SPACE_ONLY == mCalcGeo->GetFriendlyGroundRegionType());
         
         TestRegionTypeEnumProp(dtHLAGM::DDMCalculatorGeographic::PROP_FRIENDLY_AIR_REGION_TYPE);
         CPPUNIT_ASSERT(dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::APP_SPACE_ONLY == mCalcGeo->GetFriendlyAirRegionType());

         TestRegionTypeEnumProp(dtHLAGM::DDMCalculatorGeographic::PROP_FRIENDLY_SEA_REGION_TYPE);
         CPPUNIT_ASSERT(dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::APP_SPACE_ONLY == mCalcGeo->GetFriendlySeaRegionType());

         TestRegionTypeEnumProp(dtHLAGM::DDMCalculatorGeographic::PROP_FRIENDLY_LIFEFORM_REGION_TYPE);
         CPPUNIT_ASSERT(dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::APP_SPACE_ONLY == mCalcGeo->GetFriendlyLifeformRegionType());


         TestRegionTypeEnumProp(dtHLAGM::DDMCalculatorGeographic::PROP_NEUTRAL_GROUND_REGION_TYPE);
         CPPUNIT_ASSERT(dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::APP_SPACE_ONLY == mCalcGeo->GetNeutralGroundRegionType());
         
         TestRegionTypeEnumProp(dtHLAGM::DDMCalculatorGeographic::PROP_NEUTRAL_AIR_REGION_TYPE);
         CPPUNIT_ASSERT(dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::APP_SPACE_ONLY == mCalcGeo->GetNeutralAirRegionType());

         TestRegionTypeEnumProp(dtHLAGM::DDMCalculatorGeographic::PROP_NEUTRAL_SEA_REGION_TYPE);
         CPPUNIT_ASSERT(dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::APP_SPACE_ONLY == mCalcGeo->GetNeutralSeaRegionType());

         TestRegionTypeEnumProp(dtHLAGM::DDMCalculatorGeographic::PROP_NEUTRAL_LIFEFORM_REGION_TYPE);
         CPPUNIT_ASSERT(dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::APP_SPACE_ONLY == mCalcGeo->GetNeutralLifeformRegionType());

         
         TestRegionTypeEnumProp(dtHLAGM::DDMCalculatorGeographic::PROP_ENEMY_GROUND_REGION_TYPE);
         CPPUNIT_ASSERT(dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::APP_SPACE_ONLY == mCalcGeo->GetEnemyGroundRegionType());
         
         TestRegionTypeEnumProp(dtHLAGM::DDMCalculatorGeographic::PROP_ENEMY_AIR_REGION_TYPE);
         CPPUNIT_ASSERT(dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::APP_SPACE_ONLY == mCalcGeo->GetEnemyAirRegionType());

         TestRegionTypeEnumProp(dtHLAGM::DDMCalculatorGeographic::PROP_ENEMY_SEA_REGION_TYPE);
         CPPUNIT_ASSERT(dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::APP_SPACE_ONLY == mCalcGeo->GetEnemySeaRegionType());

         TestRegionTypeEnumProp(dtHLAGM::DDMCalculatorGeographic::PROP_ENEMY_LIFEFORM_REGION_TYPE);
         CPPUNIT_ASSERT(dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::APP_SPACE_ONLY == mCalcGeo->GetEnemyLifeformRegionType());
      }
      
      void TestAppSpaceValueProperties()
      {
         TestAppSpaceValueProp(dtHLAGM::DDMCalculatorGeographic::PROP_FRIENDLY_GROUND_APPSPACE, 7);
         CPPUNIT_ASSERT_EQUAL(173, mCalcGeo->GetFriendlyGroundAppSpace());
         
         TestAppSpaceValueProp(dtHLAGM::DDMCalculatorGeographic::PROP_FRIENDLY_AIR_APPSPACE, 1);
         CPPUNIT_ASSERT_EQUAL(173, mCalcGeo->GetFriendlyAirAppSpace());

         TestAppSpaceValueProp(dtHLAGM::DDMCalculatorGeographic::PROP_FRIENDLY_SEA_APPSPACE, 4);
         CPPUNIT_ASSERT_EQUAL(173, mCalcGeo->GetFriendlySeaAppSpace());

         TestAppSpaceValueProp(dtHLAGM::DDMCalculatorGeographic::PROP_FRIENDLY_LIFEFORM_APPSPACE, 10);
         CPPUNIT_ASSERT_EQUAL(173, mCalcGeo->GetFriendlyLifeformAppSpace());

         
         TestAppSpaceValueProp(dtHLAGM::DDMCalculatorGeographic::PROP_ENEMY_GROUND_APPSPACE, 8);
         CPPUNIT_ASSERT_EQUAL(173, mCalcGeo->GetEnemyGroundAppSpace());
         
         TestAppSpaceValueProp(dtHLAGM::DDMCalculatorGeographic::PROP_ENEMY_AIR_APPSPACE, 2);
         CPPUNIT_ASSERT_EQUAL(173, mCalcGeo->GetEnemyAirAppSpace());

         TestAppSpaceValueProp(dtHLAGM::DDMCalculatorGeographic::PROP_ENEMY_SEA_APPSPACE, 5);
         CPPUNIT_ASSERT_EQUAL(173, mCalcGeo->GetEnemySeaAppSpace());

         TestAppSpaceValueProp(dtHLAGM::DDMCalculatorGeographic::PROP_ENEMY_LIFEFORM_APPSPACE, 10);
         CPPUNIT_ASSERT_EQUAL(173, mCalcGeo->GetEnemyLifeformAppSpace());

         
         TestAppSpaceValueProp(dtHLAGM::DDMCalculatorGeographic::PROP_NEUTRAL_GROUND_APPSPACE, 9);
         CPPUNIT_ASSERT_EQUAL(173, mCalcGeo->GetNeutralGroundAppSpace());
         
         TestAppSpaceValueProp(dtHLAGM::DDMCalculatorGeographic::PROP_NEUTRAL_AIR_APPSPACE, 3);
         CPPUNIT_ASSERT_EQUAL(173, mCalcGeo->GetNeutralAirAppSpace());

         TestAppSpaceValueProp(dtHLAGM::DDMCalculatorGeographic::PROP_NEUTRAL_SEA_APPSPACE, 6);
         CPPUNIT_ASSERT_EQUAL(173, mCalcGeo->GetNeutralSeaAppSpace());

         TestAppSpaceValueProp(dtHLAGM::DDMCalculatorGeographic::PROP_NEUTRAL_LIFEFORM_APPSPACE, 10);
         CPPUNIT_ASSERT_EQUAL(173, mCalcGeo->GetNeutralLifeformAppSpace());
      }
      
   private:

      const std::string TestDimensionProperty(const std::string& name, const std::string& defaultValue)
      {
         dtDAL::StringActorProperty* sap = NULL;
         std::string testValue("myspace");
         //Mix up the value a bit to make sure reading the wrong data member doesn't happen to make the test run.
         testValue += name;
         
         mCalcGeo->GetProperty(name, sap);
         CPPUNIT_ASSERT(sap != NULL);

         CPPUNIT_ASSERT_EQUAL(defaultValue, sap->GetValue());
         sap->SetValue(testValue);
         CPPUNIT_ASSERT_EQUAL(testValue, sap->GetValue());
         return testValue;
      }

      void TestRegionTypeEnumProp(const std::string& propName)
      {
         dtDAL::EnumActorProperty<dtHLAGM::DDMCalculatorGeographic::RegionCalculationType>* eap = NULL;
         mCalcGeo->GetProperty(propName, eap);
         CPPUNIT_ASSERT(eap != NULL);
         
         CPPUNIT_ASSERT(dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::GEOGRAPHIC_SPACE == eap->GetValue());
         eap->SetValue(dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::APP_SPACE_ONLY);
         CPPUNIT_ASSERT(dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::APP_SPACE_ONLY == eap->GetValue());
      }

      void TestAppSpaceValueProp(const std::string& propName, int expectedDefault)
      {
         dtDAL::IntActorProperty* iap = NULL;
         mCalcGeo->GetProperty(propName, iap);
         CPPUNIT_ASSERT(iap != NULL);
         
         CPPUNIT_ASSERT_EQUAL(expectedDefault, iap->GetValue());
         iap->SetValue(173);
         CPPUNIT_ASSERT_EQUAL(173, iap->GetValue());
      }
      
      dtCore::RefPtr<dtHLAGM::DDMCalculatorGeographic> mCalcGeo; 
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(DDMCalculatorGeographicTests);
