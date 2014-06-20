/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2007-2008, Alion Science and Technology Corporation
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

#include <dtCore/refptr.h>

#include <dtCore/enumactorproperty.h>
#include <dtCore/longactorproperty.h>
#include <dtCore/stringactorproperty.h>

#include <dtHLAGM/ddmcalculatorgeographic.h>

#include <dtUtil/coordinates.h>

#include <cppunit/extensions/HelperMacros.h>

#include <string>

class DDMCalculatorGeographicTests : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE(DDMCalculatorGeographicTests);
   
      CPPUNIT_TEST(TestCoordinateConverterAttribute);
      CPPUNIT_TEST(TestBaseProperties);
      CPPUNIT_TEST(TestAppSpaceProperties);
      CPPUNIT_TEST(TestRegionTypeProperties);
      CPPUNIT_TEST(TestAppSpaceValueProperties);
      CPPUNIT_TEST(TestAppSpaceValues);

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
         dtCore::LongActorProperty* lap = NULL;
         mCalcGeo->GetProperty(dtHLAGM::DDMCalculatorGeographic::PROP_APP_SPACE_MIN, lap);
         CPPUNIT_ASSERT(lap != NULL);

         CPPUNIT_ASSERT_EQUAL(0L, lap->GetValue());
         lap->SetValue(23);
         CPPUNIT_ASSERT_EQUAL(23L, lap->GetValue());
         CPPUNIT_ASSERT_EQUAL(23L, mCalcGeo->GetAppSpaceMinimum());
                  
         mCalcGeo->GetProperty(dtHLAGM::DDMCalculatorGeographic::PROP_APP_SPACE_MAX, lap);
         CPPUNIT_ASSERT(lap != NULL);

         CPPUNIT_ASSERT_EQUAL_MESSAGE("The maximum app space value should default to 30.", 30L, lap->GetValue());
         lap->SetValue(28);
         CPPUNIT_ASSERT_EQUAL(28L, lap->GetValue());
         CPPUNIT_ASSERT_EQUAL(28L, mCalcGeo->GetAppSpaceMaximum());
      }

      void TestRegionTypeProperties()
      {

         dtCore::EnumActorProperty<dtHLAGM::DDMCalculatorGeographic::DDMObjectKind>* eap = NULL;
         mCalcGeo->GetProperty(dtHLAGM::DDMCalculatorGeographic::PROP_CALCULATOR_OBJECT_KIND, eap);
         CPPUNIT_ASSERT(eap != NULL);
         
         CPPUNIT_ASSERT(dtHLAGM::DDMCalculatorGeographic::DDMObjectKind::OBJECT_KIND_ENTITY == eap->GetValue());
         eap->SetValue(dtHLAGM::DDMCalculatorGeographic::DDMObjectKind::OBJECT_KIND_OTHER);
         CPPUNIT_ASSERT(dtHLAGM::DDMCalculatorGeographic::DDMObjectKind::OBJECT_KIND_OTHER == eap->GetValue());
         CPPUNIT_ASSERT(dtHLAGM::DDMCalculatorGeographic::DDMObjectKind::OBJECT_KIND_OTHER == mCalcGeo->GetCalculatorObjectKind());
         
         TestRegionTypeEnumProp(dtHLAGM::DDMCalculatorGeographic::PROP_FRIENDLY_REGION_TYPE);
         CPPUNIT_ASSERT(dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::APP_SPACE_ONLY == mCalcGeo->GetFriendlyRegionType());

         TestRegionTypeEnumProp(dtHLAGM::DDMCalculatorGeographic::PROP_NEUTRAL_REGION_TYPE);
         CPPUNIT_ASSERT(dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::APP_SPACE_ONLY == mCalcGeo->GetNeutralRegionType());

         TestRegionTypeEnumProp(dtHLAGM::DDMCalculatorGeographic::PROP_ENEMY_REGION_TYPE);
         CPPUNIT_ASSERT(dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::APP_SPACE_ONLY == mCalcGeo->GetEnemyRegionType());
      }
      
      void TestAppSpaceValueProperties()
      {

         TestAppSpaceValueProp(dtHLAGM::DDMCalculatorGeographic::PROP_FRIENDLY_APPSPACE, 1);
         CPPUNIT_ASSERT_EQUAL(173L, mCalcGeo->GetFriendlyAppSpace());

         TestAppSpaceValueProp(dtHLAGM::DDMCalculatorGeographic::PROP_ENEMY_APPSPACE, 2);
         CPPUNIT_ASSERT_EQUAL(173L, mCalcGeo->GetEnemyAppSpace());

         TestAppSpaceValueProp(dtHLAGM::DDMCalculatorGeographic::PROP_NEUTRAL_APPSPACE, 3);
         CPPUNIT_ASSERT_EQUAL(173L, mCalcGeo->GetNeutralAppSpace());
      }
      
      void TestAppSpaceValues()
      {
         mCalcGeo->SetFriendlyRegionType(dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::APP_SPACE_ONLY);
         mCalcGeo->SetEnemyRegionType(dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::APP_SPACE_ONLY);
         mCalcGeo->SetNeutralRegionType(dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::APP_SPACE_ONLY);
         
         std::pair<dtHLAGM::DDMCalculatorGeographic::RegionCalculationType*, long> valuePair;
         ///Ground
         valuePair = mCalcGeo->GetAppSpaceValues(dtHLAGM::DDMCalculatorGeographic::DDMForce::FORCE_ENEMY, 
               dtHLAGM::DDMCalculatorGeographic::DDMObjectKind::OBJECT_KIND_ENTITY);

         CPPUNIT_ASSERT(valuePair.first != NULL);
         CPPUNIT_ASSERT_EQUAL(mCalcGeo->GetEnemyAppSpace(), valuePair.second);
         CPPUNIT_ASSERT_EQUAL(mCalcGeo->GetEnemyRegionType(), *valuePair.first);

         valuePair = mCalcGeo->GetAppSpaceValues(dtHLAGM::DDMCalculatorGeographic::DDMForce::FORCE_FRIENDLY, 
               dtHLAGM::DDMCalculatorGeographic::DDMObjectKind::OBJECT_KIND_ENTITY);

         CPPUNIT_ASSERT(valuePair.first != NULL);
         CPPUNIT_ASSERT_EQUAL(mCalcGeo->GetFriendlyAppSpace(), valuePair.second);
         CPPUNIT_ASSERT_EQUAL(mCalcGeo->GetFriendlyRegionType(), *valuePair.first);

         valuePair = mCalcGeo->GetAppSpaceValues(dtHLAGM::DDMCalculatorGeographic::DDMForce::FORCE_NEUTRAL, 
               dtHLAGM::DDMCalculatorGeographic::DDMObjectKind::OBJECT_KIND_ENTITY);

         CPPUNIT_ASSERT(valuePair.first != NULL);
         CPPUNIT_ASSERT_EQUAL(mCalcGeo->GetNeutralAppSpace(), valuePair.second);
         CPPUNIT_ASSERT_EQUAL(mCalcGeo->GetNeutralRegionType(), *valuePair.first);


         valuePair = mCalcGeo->GetAppSpaceValues(dtHLAGM::DDMCalculatorGeographic::DDMForce::FORCE_NEUTRAL, 
               dtHLAGM::DDMCalculatorGeographic::DDMObjectKind::OBJECT_KIND_OTHER);

         CPPUNIT_ASSERT(valuePair.first != NULL);
         CPPUNIT_ASSERT_EQUAL(mCalcGeo->GetDefaultAppSpace(), valuePair.second);
         CPPUNIT_ASSERT_EQUAL(mCalcGeo->GetDefaultRegionType(), *valuePair.first);
         
      }
   private:

      const std::string TestDimensionProperty(const std::string& name, const std::string& defaultValue)
      {
         dtCore::StringActorProperty* sap = NULL;
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
         dtCore::EnumActorProperty<dtHLAGM::DDMCalculatorGeographic::RegionCalculationType>* eap = NULL;
         mCalcGeo->GetProperty(propName, eap);
         CPPUNIT_ASSERT(eap != NULL);
         
         CPPUNIT_ASSERT(dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::GEOGRAPHIC_SPACE == eap->GetValue());
         eap->SetValue(dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::APP_SPACE_ONLY);
         CPPUNIT_ASSERT(dtHLAGM::DDMCalculatorGeographic::RegionCalculationType::APP_SPACE_ONLY == eap->GetValue());
      }

      void TestAppSpaceValueProp(const std::string& propName, long expectedDefault)
      {
         dtCore::LongActorProperty* prop = NULL;
         mCalcGeo->GetProperty(propName, prop);
         CPPUNIT_ASSERT_MESSAGE("dtHLAGM::DDMCalculatorGeographic is missing the LongActorProperty:'" + propName + "'",
                                 prop != NULL);
         
         CPPUNIT_ASSERT_EQUAL(expectedDefault, prop->GetValue());
         prop->SetValue(173L);
         CPPUNIT_ASSERT_EQUAL(173L, prop->GetValue());
      }
      
      dtCore::RefPtr<dtHLAGM::DDMCalculatorGeographic> mCalcGeo; 
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(DDMCalculatorGeographicTests);
