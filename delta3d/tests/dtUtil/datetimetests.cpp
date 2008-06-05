/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Delta3D
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
 * Bradley Anderegg
 */
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtUtil/datetime.h>

namespace dtUtil
{
   /// Math unit tests for dtUtil
   class DateTimeTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(DateTimeTests);
         CPPUNIT_TEST( TestGetSetTime );
         CPPUNIT_TEST( TestIncrementTime );
         CPPUNIT_TEST( TestTimeToString );
         CPPUNIT_TEST( TestTimeZones );
      CPPUNIT_TEST_SUITE_END();

      public:
         void setUp();
         void tearDown();

         void TestGetSetTime();
         void TestIncrementTime();
         void TestTimeToString();
         void TestTimeZones();

      private:

         void CompareTimes(const DateTime& lhs, const DateTime& rhs);
   };

   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION( DateTimeTests );

   void DateTimeTests::setUp()
   {
   }

   void DateTimeTests::tearDown()
   {
   }

   void DateTimeTests::TestGetSetTime()
   {
      unsigned yr = 1982;
      unsigned mo = 5;
      unsigned day = 1;
      unsigned hr = 14;
      unsigned min = 30;
      unsigned sec = 59;

      DateTime testSetGet1, testSetGet2;
      testSetGet1.SetTime(yr, mo, day, hr, min, sec);
      
      testSetGet2.SetYear(yr);
      testSetGet2.SetMonth(mo);
      testSetGet2.SetDay(day);
      testSetGet2.SetHour(hr);
      testSetGet2.SetMinute(min);
      testSetGet2.SetSecond(sec);
      CompareTimes(testSetGet1, testSetGet2);

      
      time_t curr;
      time(&curr);
      struct tm timeParts;
      timeParts = *gmtime(&curr);
      
      testSetGet1.SetTime(curr);
      testSetGet2.SetTime(timeParts);
      CompareTimes(testSetGet1, testSetGet2);

      //test non default constructors
      DateTime testSetGet3(curr);
      DateTime testSetGet4(timeParts);
      CompareTimes(testSetGet3, testSetGet4);
      CompareTimes(testSetGet2, testSetGet3);

      //test operator= and copy constructor
      DateTime testSetGet5(testSetGet4);
      DateTime testSetGet6;
      testSetGet6 = testSetGet5;
      CompareTimes(testSetGet3, testSetGet5);
      CompareTimes(testSetGet5, testSetGet6);

      //test conversion operators 
      CPPUNIT_ASSERT_EQUAL(curr, time_t(testSetGet1));
      CompareTimes(DateTime(timeParts), DateTime(tm(testSetGet1)));

   }

   void DateTimeTests::TestTimeZones()
   {
      DateTime dt_gmt(DateTime::TimeOrigin::GMT_TIME);
      DateTime dt_local(DateTime::TimeOrigin::LOCAL_TIME);  

      dt_local.SetGMTOffset(dt_local.GetGMTOffset(), true);

      time_t curr;
      tm local;
      time(&curr);
      local=*(localtime(&curr));

      DateTime dt_test;
      dt_test.SetTime(local);
      CompareTimes(dt_local, dt_test);

      CompareTimes(DateTime(dt_local.GetGMTTime()), dt_gmt); 
   }

   void DateTimeTests::CompareTimes(const DateTime& lhs, const DateTime& rhs)
   {
      CPPUNIT_ASSERT_EQUAL(lhs.GetYear(), rhs.GetYear()); 
      CPPUNIT_ASSERT_EQUAL(lhs.GetMonth(), rhs.GetMonth());
      CPPUNIT_ASSERT_EQUAL(lhs.GetDay(), rhs.GetDay());
      CPPUNIT_ASSERT_EQUAL(lhs.GetHour(), rhs.GetHour());
      CPPUNIT_ASSERT_EQUAL(lhs.GetMinute(), rhs.GetMinute());
      CPPUNIT_ASSERT_EQUAL(unsigned(lhs.GetSecond()), unsigned(rhs.GetSecond()));
   }

   void DateTimeTests::TestIncrementTime()
   {
      unsigned yr = 1982;
      unsigned mo = 5;
      unsigned day = 1;
      unsigned hr = 14;
      unsigned min = 30;
      unsigned sec = 30;

      DateTime testIncrementTime1;
      testIncrementTime1.SetTime(yr, mo, day, hr, min, sec);

      DateTime testIncrementTime2(testIncrementTime1);

      testIncrementTime1.IncrementClock(0.5f);
      CompareTimes(testIncrementTime1, testIncrementTime2);

      testIncrementTime1.IncrementClock(0.5f);
      testIncrementTime2.SetSecond(testIncrementTime2.GetSecond() + 1.0f);
      CompareTimes(testIncrementTime1, testIncrementTime2);

      testIncrementTime1.IncrementClock(3605.0f);
      testIncrementTime2.SetSecond(testIncrementTime2.GetSecond() + 5.0f);
      testIncrementTime2.SetHour(testIncrementTime2.GetHour() + 1.0f);
      CompareTimes(testIncrementTime1, testIncrementTime2);

      testIncrementTime1.IncrementClock(86400.0f); //24 hours
      testIncrementTime2.SetDay(testIncrementTime2.GetDay() + 1.0f);
      CompareTimes(testIncrementTime1, testIncrementTime2);

      yr = 1979;
      mo = 1;
      day = 1;
      hr = 1;
      min = 1;
      sec = 1;

      testIncrementTime1.SetTime(yr, mo, day, hr, min, sec);
      testIncrementTime2.SetTime(yr + 1, mo, day, hr, min, sec);

      for(int i = 0; i < 365; ++i)
      {
         testIncrementTime1.IncrementClock(86400.0f); //24 hours
      }

      CompareTimes(testIncrementTime1, testIncrementTime2); 

   }

   void DateTimeTests::TestTimeToString()
   {
      DateTime dt_local(DateTime::TimeOrigin::LOCAL_TIME);  

      std::cout << "TESTING TIME OUTPUT" << std::endl;

      std::cout << "LOCAL_DATE_AND_TIME_FORMAT: " << dt_local.ToString(DateTime::TimeFormat::LOCAL_DATE_AND_TIME_FORMAT) << std::endl;
      std::cout << "LOCAL_DATE_FORMAT: " << dt_local.ToString(DateTime::TimeFormat::LOCAL_DATE_FORMAT) << std::endl;
      std::cout << "CLOCK_TIME_12_HOUR_FORMAT: " << dt_local.ToString(DateTime::TimeFormat::CLOCK_TIME_12_HOUR_FORMAT) << std::endl;
      std::cout << "CLOCK_TIME_24_HOUR_FORMAT: " << dt_local.ToString(DateTime::TimeFormat::CLOCK_TIME_24_HOUR_FORMAT) << std::endl;
      std::cout << "LEXICAL_DATE_FORMAT: " << dt_local.ToString(DateTime::TimeFormat::LEXICAL_DATE_FORMAT) << std::endl;
      std::cout << "CALENDAR_DATE_FORMAT: " << dt_local.ToString(DateTime::TimeFormat::CALENDAR_DATE_FORMAT) << std::endl;
      std::cout << "ORDINAL_DATE_FORMAT: " << dt_local.ToString(DateTime::TimeFormat::ORDINAL_DATE_FORMAT) << std::endl;
      std::cout << "WEEK_DATE_FORMAT: " << dt_local.ToString(DateTime::TimeFormat::WEEK_DATE_FORMAT) << std::endl;
      std::cout << "CALENDAR_DATE_AND_TIME_FORMAT: " << dt_local.ToString(DateTime::TimeFormat::CALENDAR_DATE_AND_TIME_FORMAT) << std::endl;

      std::cout << "END TIME OUPUT TESTS" << std::endl;

   }
}
