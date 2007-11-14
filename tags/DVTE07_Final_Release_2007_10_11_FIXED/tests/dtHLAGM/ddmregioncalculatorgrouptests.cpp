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
#include <dtHLAGM/ddmregioncalculator.h>
#include <dtHLAGM/ddmregioncalculatorgroup.h>
#include <dtHLAGM/ddmregiondata.h>

class TestDDMRegionCalculator : public dtHLAGM::DDMRegionCalculator
{
   public:
      virtual dtCore::RefPtr<dtHLAGM::DDMRegionData> CreateRegionData() const
      {
         return new dtHLAGM::DDMRegionData;
      }
      
      virtual void CreateSubscriptionRegionData(std::vector<dtCore::RefPtr<dtHLAGM::DDMRegionData> >& toFill) const
      {
         toFill.clear();
      }

      virtual bool UpdateRegionData(dtHLAGM::DDMRegionData& ddmData) const
      {
         return false;
      }
      
};

class DDMRegionCalculatorGroupTests : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE(DDMRegionCalculatorGroupTests);
   
      CPPUNIT_TEST(TestMain);

   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp()
      {
      }
      
      void tearDown()
      {
      }
      
      void TestMain()
      {
         std::vector<dtHLAGM::DDMRegionCalculator*> calcVector;
         
         std::string calc1Name = "testName";
         dtCore::RefPtr<dtHLAGM::DDMRegionCalculator> calc1 = new TestDDMRegionCalculator;
         calc1->SetName(calc1Name);

         CPPUNIT_ASSERT(mCalcGroup.GetCalculator(calc1Name) == NULL);
         CPPUNIT_ASSERT(mCalcGroup.IsEmpty());
         CPPUNIT_ASSERT(mCalcGroup.AddCalculator(*calc1));
         CPPUNIT_ASSERT_EQUAL(calc1.get(), mCalcGroup.GetCalculator(calc1Name));
         CPPUNIT_ASSERT_EQUAL(1U, mCalcGroup.GetSize());
         CPPUNIT_ASSERT(!mCalcGroup.IsEmpty());

         CPPUNIT_ASSERT(!mCalcGroup.AddCalculator(*calc1));
         CPPUNIT_ASSERT_EQUAL_MESSAGE("Adding a duplicate should be a no-op.", 1U, mCalcGroup.GetSize());
         CPPUNIT_ASSERT(!mCalcGroup.IsEmpty());
         mCalcGroup.GetCalculators(calcVector);
         CPPUNIT_ASSERT(calcVector.size() == mCalcGroup.GetSize());
         
         
         mCalcGroup.RemoveCalculator(*calc1);
         CPPUNIT_ASSERT_EQUAL(0U, mCalcGroup.GetSize());
         CPPUNIT_ASSERT(mCalcGroup.GetCalculator(calc1Name) == NULL);

         CPPUNIT_ASSERT(mCalcGroup.AddCalculator(*calc1));
         CPPUNIT_ASSERT_EQUAL(1U, mCalcGroup.GetSize());
         CPPUNIT_ASSERT(!mCalcGroup.IsEmpty());

         dtCore::RefPtr<dtHLAGM::DDMRegionCalculator> calc2 = new TestDDMRegionCalculator;
         calc2->SetName(calc1Name);

         CPPUNIT_ASSERT(!mCalcGroup.AddCalculator(*calc2));
         CPPUNIT_ASSERT_EQUAL_MESSAGE("Adding another calculator with the same name should not work.", 1U, mCalcGroup.GetSize());
         CPPUNIT_ASSERT_EQUAL_MESSAGE("Adding another calculator with the same name should not replace the first one.", calc1.get(), mCalcGroup.GetCalculator(calc1Name));

         std::string calc2Name = "test2Name";
         calc2->SetName(calc2Name);
         CPPUNIT_ASSERT(mCalcGroup.AddCalculator(*calc2));
         CPPUNIT_ASSERT_EQUAL(2U, mCalcGroup.GetSize());
         
         mCalcGroup.GetCalculators(calcVector);
         CPPUNIT_ASSERT(calcVector.size() == mCalcGroup.GetSize());
         CPPUNIT_ASSERT_MESSAGE("The first item in a vector fetched with GetCalculators should be calc1", calc1 == calcVector[0]);
         CPPUNIT_ASSERT_MESSAGE("The first item in a vector fetched with GetCalculators should be calc1", calc1 == mCalcGroup.GetCalculators()[0]);
         CPPUNIT_ASSERT_MESSAGE("The second item in a vector fetched with GetCalculators should be calc2", calc2 == calcVector[1]);
         CPPUNIT_ASSERT_MESSAGE("The second item in a vector fetched with GetCalculators should be calc2", calc2 == mCalcGroup.GetCalculators()[1]);
         
         CPPUNIT_ASSERT(!mCalcGroup.IsEmpty());
         CPPUNIT_ASSERT_EQUAL(calc1.get(), mCalcGroup.GetCalculator(calc1Name));
         CPPUNIT_ASSERT_EQUAL(calc2.get(), mCalcGroup.GetCalculator(calc2Name));

         mCalcGroup.RemoveCalculator(calc1Name);
         CPPUNIT_ASSERT_EQUAL(1U, mCalcGroup.GetSize());
         CPPUNIT_ASSERT(!mCalcGroup.IsEmpty());
         CPPUNIT_ASSERT(mCalcGroup.GetCalculator(calc1Name) == NULL);
         CPPUNIT_ASSERT_EQUAL(calc2.get(), mCalcGroup.GetCalculator(calc2Name));

         mCalcGroup.RemoveCalculator(calc2Name);
         CPPUNIT_ASSERT_EQUAL(0U, mCalcGroup.GetSize());
         CPPUNIT_ASSERT(mCalcGroup.IsEmpty());
         CPPUNIT_ASSERT(mCalcGroup.GetCalculator(calc1Name) == NULL);
         CPPUNIT_ASSERT(mCalcGroup.GetCalculator(calc2Name) == NULL);
      }
      
   private:
      dtHLAGM::DDMRegionCalculatorGroup mCalcGroup; 
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(DDMRegionCalculatorGroupTests);
