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

class DDMCalculatorGeographicTests : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE(DDMCalculatorGeographicTests);
   
      CPPUNIT_TEST(TestProperties);

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
      
      void TestProperties()
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
      
   private:
      dtCore::RefPtr<dtHLAGM::DDMCalculatorGeographic> mCalcGeo; 
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(DDMCalculatorGeographicTests);
