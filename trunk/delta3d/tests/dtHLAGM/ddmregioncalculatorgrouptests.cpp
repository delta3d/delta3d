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
