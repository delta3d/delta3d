/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007, Alion Science and Technology
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
 * Bradley Anderegg 04/10/2008
 */

#ifndef DELTA_DateTime
#define DELTA_DateTime

#include <dtUtil/export.h>
#include <dtUtil/enumeration.h>

#include <osg/Referenced>

#include <string>
#include <ctime>

namespace dtUtil
{

   class DT_UTIL_EXPORT DateTime
   {
      public:

         /**
          * The TimeOrigin enumeration determines how the instance of DateTime should be interpreted.
          * When passing a TimeOrigin into the constructor of DateTime it will automatically set the clock
          * to that specific time.
          */
         class DT_UTIL_EXPORT TimeOrigin: public dtUtil::Enumeration
         {
            DECLARE_ENUM(TimeOrigin);

            public:
              static const TimeOrigin LOCAL_TIME;
              static const TimeOrigin GMT_TIME;

            protected:
               /*virtual*/ ~TimeOrigin(){}

            private:

               TimeOrigin(const std::string &name) : dtUtil::Enumeration(name)
               {
                  AddInstance(this);
               }
         };

         /**
          * The TimeType enumeration is used to identify the purpose of a DateTime instance.
          */
         class DT_UTIL_EXPORT TimeType: public dtUtil::Enumeration
         {
            DECLARE_ENUM(TimeType);

            public:
               static const TimeType CLOCK_TIME;
               static const TimeType SIMULATION_TIME;
               static const TimeType SCENARIO_TIME;
               static const TimeType TRIP_TIME;
               static const TimeType TIME_STAMP;
               static const TimeType TIME_TYPE_OTHER;

            protected:
               /*virtual*/ ~TimeType(){}

            private:

               TimeType(const std::string &name) : dtUtil::Enumeration(name)
               {
                  AddInstance(this);
               }
         };

         /**
          * The TimeFormat enumeration is used to specify how to map a DateTime object
          * to a string.  This enumeration is used in conjunction with ToString().
          */
         class DT_UTIL_EXPORT TimeFormat: public dtUtil::Enumeration
         {
            DECLARE_ENUM(TimeFormat);

            public:

               //LOCAL_DATE_AND_TIME_FORMAT: 04/18/08 13:22:50
               static const TimeFormat LOCAL_DATE_AND_TIME_FORMAT;

               //LOCAL_DATE_FORMAT: 04/18/08
               static const TimeFormat LOCAL_DATE_FORMAT;

               //CLOCK_TIME_12_HOUR_FORMAT: 01:22:50 PM
               static const TimeFormat CLOCK_TIME_12_HOUR_FORMAT;

               //CLOCK_TIME_24_HOUR_FORMAT: 13:22:50
               static const TimeFormat CLOCK_TIME_24_HOUR_FORMAT;

               //LEXICAL_DATE_FORMAT: April 18, 2008
               static const TimeFormat LEXICAL_DATE_FORMAT;

               //CALENDAR_DATE_FORMAT: 2008-04-18
               static const TimeFormat CALENDAR_DATE_FORMAT;

               //ORDINAL_DATE_FORMAT: 2008-109
               static const TimeFormat ORDINAL_DATE_FORMAT;

               //WEEK_DATE_FORMAT: 2008-W15-5
               static const TimeFormat WEEK_DATE_FORMAT;

               //CALENDAR_DATE_AND_TIME_FORMAT: 2008-04-18T13:22:50-05:00
               static const TimeFormat CALENDAR_DATE_AND_TIME_FORMAT;

            protected:
               /*virtual*/ ~TimeFormat(){}

            private:
               TimeFormat(const std::string &name) : dtUtil::Enumeration(name)
               {
                  AddInstance(this);
               }
         };

      public:

         ///The default constructor just zeros
         DateTime();

         ///This constructor takes a TimeOrigin and sets the time accordingly.
         DateTime(const TimeOrigin& initAs);

         ///create a DateTime using the c standard time_t struct
         DateTime(time_t);

         ///create a DateTime using the c standard struct tm
         DateTime(const struct tm&);

         DateTime(const DateTime&);
         DateTime& operator=(const DateTime&);

         virtual ~DateTime();

         static float GetLocalGMTOffset();

         ///changes time to be system local time
         void SetToLocalTime();

         ///changes time to be GMT- or Greenwich Mean Time
         void SetToGMTTime();

         /**
          * Increments the clock time by the number of seconds specified.  The fractional part of the time is saved off and
          * added in whole increments to support sub second times but getting time as a time_t or struct tm will not include
          * fractional seconds.  To get the fractional seconds use GetTime with a float for seconds or GetSecond().
          *
          * @param the number of seconds to increase the clock by.
          */
         void IncrementClock(double seconds);

         /**
          * Every date time has a GMT offset.  This method allows one to set a new
          * GMT offset, and then change the clock internally to match the newoffset.  For example
          * if the clock is in 12:00 AM Eastern Time (-5) Jan 5, 2009, setting the offset to
          * west coast time (-8) would make the date time be 9:00 PM, Jan 4, 2009.
          * @param newGMTOffset the offset in hours from GMT to change this to.
          */
         void AdjustTimeZone(float newGMTOffset);

         /**
          * Gets the time internally stored using unsigned year, month, day, hour, minute, and seconds.  The float second version
          * will include the sub second time if IncrementClock() or SetSecond() was not rounded off.  To get time modified by a GMTOffset
          * use GetGMTTime().
          *
          * @param Year- the full year, not just since 1900 epoch
          * @param Month- the month specified as 1-12
          * @param Day- the day specified as 1-31
          * @param Hour- hours since midnight 0-23
          * @param Minute- minutes after the hour 0-60
          * @param Second- seconds after the hour 0-61 (and extra second is added to support leap seconds)
          */
         void GetTime(unsigned& year, unsigned& month, unsigned& day, unsigned& hour, unsigned& min, float& sec) const;
         void GetTime(unsigned& year, unsigned& month, unsigned& day, unsigned& hour, unsigned& min, unsigned& sec) const;

         /**
          * Gets the time internally stored using unsigned year, month, day, hour, minute, and seconds.  The float second version
          * will include the sub second time if IncrementClock() or SetSecond() was not rounded off.  The GMTOffset will be added into
          * the current time to obtain the GMT Time.
          *
          * @param Year- the full year, not just since 1900 epoch
          * @param Month- the month specified as 1-12
          * @param Day- the day specified as 1-31
          * @param Hour- hours since midnight 0-23
          * @param Minute- minutes after the hour 0-60
          * @param Second- seconds after the hour 0-61 (and extra second is added to support leap seconds)
          */
         void GetGMTTime(unsigned& year, unsigned& month, unsigned& day, unsigned& hour, unsigned& min, float& sec) const;
         void GetGMTTime(unsigned& year, unsigned& month, unsigned& day, unsigned& hour, unsigned& min, unsigned& sec) const;

         /**
          * Sets the full time using year, month, day, hour, minute, and second.  A float version is provided to support sub second
          * times.
          *
          * @param Year- the full year, not just since 1900 epoch
          * @param Month- the month specified as 1-12
          * @param Day- the day specified as 1-31
          * @param Hour- hours since midnight 0-23
          * @param Minute- minutes after the hour 0-60
          * @param Second- seconds after the hour 0-61 (and extra second is added to support leap seconds)
          */
         void SetTime(unsigned year, unsigned month, unsigned day, unsigned hour, unsigned min, float sec);
         void SetTime(unsigned year, unsigned month, unsigned day, unsigned hour, unsigned min, unsigned sec);

         ///Gets the time in the standard time_t format, specified as seconds elapsed since midnight, January 1, 1970
         ///@note this format only includes whole seconds
         time_t GetTime() const;
         ///Gets the time in standard time_t format, the GMTOffset is added to the time before calculating time_t
         time_t GetGMTTime() const;
         ///sets time in standard time_t format, specified as seconds elapsed since midnight, January 1, 1970
         void SetTime(time_t);

         ///returns the total clock time in seconds elapsed since midnight, January 1, 1970
         ///this time includes fractional seconds
         double GetTimeInSeconds() const;

         ///fills a standard c struct tm with the time
         void GetTime(tm&) const;
         ///fills a standard c struct tm with the time but adds GMTOffset into time before struct tm is calculated
         void GetGMTTime(tm&) const;
         ///sets the time using the standard c struct tm
         void SetTime(const tm&);

         /**
          * Sets the GMTOffset which is added to the time when getting GMT time,
          *
          * @param hourOffset, the number of hours to offset from GMT Time, the param is a float to support
          *        half hour time zones.
          * @param dayLightSavings, this flag is used to specify whether or not daylight savings is in effect
          *        setting this flag to true will add and extra hour to the GMTOffset
          */
         void SetGMTOffset(float hourOffset, bool dayLightSavings);

         /**
          * Sets the GMTOffset using the systems local time this offset is added to the time when getting GMT time,
          *
          * @param timeParts, the struct tm used to calculate the local timezone
          * @param factorLocalDayLightSavingsIntoGMTOffset, this flag is used to specify whether or not daylight savings
          *        should be obtained from the system clock, setting true will increment the GMTOffset by one hour
          *        if your system is currently on daylight savings.
          */
         void SetGMTOffset(tm& timeParts, bool factorLocalDayLightSavingsIntoGMTOffset);

         /**
          * Calculates the GMTOffset using a lattitude and longitude, not 100% correct due to regional time zone boundaries
          * but it works ok as an approximation.
          *
          * @param lattitude, the geographical lattitude of origin
          * @param longitude, the geographical longitude of origin
          * @param dayLightSavings, this flag is used to specify whether or not daylight savings is in effect
          *        setting this flag to true will add and extra hour to the GMTOffset
          */
         void SetGMTOffset(double lattitude, double longitude, bool dayLightSavings);

         /**
          * Gets the GMTOffset, this will be 0 unless SetGMTOffset was called or SetToLocalTime() was called,
          * or TimeOrigin::LOCAL_TIME was fed into the constructor
          *
          * @return the current offset from Greenwich mean time
          */
         float GetGMTOffset() const;

         /**
          * The TimeScale can be used to scale the time when incrementing the clock.
          * The default value for TimeScale is 1.0.
          */
         float GetTimeScale() const;
         void SetTimeScale(float percentScaleInSeconds);

         ///The TimeType enumeration is used to identify the purpose of a DateTime instance.
         const TimeType& GetTimeType() const;
         void SetTimeType(const TimeType&);

         ///The TimeOrigin enumeration determines how the instance of DateTime should be interpreted.
         const TimeOrigin& GetTimeOrigin() const;
         void SetTimeOrigin(const TimeOrigin&);

         ///The TimeFormat enumeration is used to specify how to map a DateTime object to a string.
         ///Set the TimeFormat if you would like to use the ToString() without any arguments.
         const TimeFormat& GetTimeFormat() const;
         void SetTimeFormat(const TimeFormat&);

         float GetSecond() const;
         void SetSecond(float sec);

         unsigned GetMinute() const;
         void SetMinute(unsigned min);

         unsigned GetHour() const;
         void SetHour(unsigned hour);

         unsigned GetDay() const;
         void SetDay(unsigned day);

         unsigned GetMonth() const;
         void SetMonth(unsigned month);

         unsigned GetYear() const;
         void SetYear(unsigned year);

         ///The no parameter version of ToString uses the internal TimeFormat, see the TimeFormat enumeration above
         std::string ToString() const;

         ///Converts the time to a string using a TimeFormat enumeration, see the TimeFormat enumeration above
         std::string ToString(const TimeFormat&) const;

         ///a static version of ToString() for convenience, using the conversion operators a time_t or struct tm can be passed for the DateTime
         static std::string ToString(const DateTime&, const TimeFormat&);

         //general purpose conversion operators
         operator time_t() const;
         operator tm() const;
         operator std::string() const;

         //TODO- add operators
         //DateTime& operator +=(const DateTime&);
         //DateTime& operator -=(const DateTime&);

         //bool operator<(const DateTime&) const;
         //bool operator>(const DateTime&) const;

         //std::ostream& operator >>(std::ostream&);

      private:
         void ResetToDefaultValues();
         void GetGMTTime(time_t* t, tm& timeParts) const;
         void GetLocalTime(time_t* t, tm& timeParts) const;


         float mGMTOffset;
         float mTimeScale;
         double mFractionalSeconds;
         unsigned mSeconds, mMinutes, mHours, mDays, mMonths, mYears;

         const TimeOrigin* mTimeOrigin;
         const TimeType* mTimeType;
         const TimeFormat* mStringFormat;
   };


}

#endif // DELTA_DATETIME
