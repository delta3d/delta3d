#include <prefix/dtutilprefix-src.h>
#include <dtUtil/datetime.h>
#include <dtUtil/macros.h>
#include <cmath>
#include <cstring>

namespace dtUtil
{

   IMPLEMENT_ENUM(DateTime::TimeOrigin);
   const DateTime::TimeOrigin DateTime::TimeOrigin::LOCAL_TIME("LOCAL_TIME");
   const DateTime::TimeOrigin DateTime::TimeOrigin::GMT_TIME("GMT_TIME");


   IMPLEMENT_ENUM(DateTime::TimeType);
   const DateTime::TimeType DateTime::TimeType::CLOCK_TIME("CLOCK_TIME");
   const DateTime::TimeType DateTime::TimeType::SIMULATION_TIME("SIMULATION_TIME");
   const DateTime::TimeType DateTime::TimeType::SCENARIO_TIME("SCENARIO_TIME");
   const DateTime::TimeType DateTime::TimeType::TRIP_TIME("TRIP_TIME");
   const DateTime::TimeType DateTime::TimeType::TIME_STAMP("TIME_STAMP");
   const DateTime::TimeType DateTime::TimeType::TIME_TYPE_OTHER("TIME_TYPE_OTHER");



   IMPLEMENT_ENUM(DateTime::TimeFormat);

   const DateTime::TimeFormat DateTime::TimeFormat::LOCAL_DATE_AND_TIME_FORMAT("LOCAL_DATE_AND_TIME_FORMAT");
   const DateTime::TimeFormat DateTime::TimeFormat::LOCAL_DATE_FORMAT("LOCAL_DATE_FORMAT");
   const DateTime::TimeFormat DateTime::TimeFormat::CLOCK_TIME_12_HOUR_FORMAT("CLOCK_TIME_12_HOUR_FORMAT");
   const DateTime::TimeFormat DateTime::TimeFormat::LEXICAL_DATE_FORMAT("LEXICAL_DATE_FORMAT");

   const DateTime::TimeFormat DateTime::TimeFormat::CALENDAR_DATE_FORMAT("CALENDAR_DATE_FORMAT");
   const DateTime::TimeFormat DateTime::TimeFormat::ORDINAL_DATE_FORMAT("ORDINAL_DATE_FORMAT");
   const DateTime::TimeFormat DateTime::TimeFormat::WEEK_DATE_FORMAT("WEEK_DATE_FORMAT");
   const DateTime::TimeFormat DateTime::TimeFormat::CLOCK_TIME_24_HOUR_FORMAT("CLOCK_TIME_24_HOUR_FORMAT");
   const DateTime::TimeFormat DateTime::TimeFormat::CALENDAR_DATE_AND_TIME_FORMAT("CALENDAR_DATE_AND_TIME_FORMAT");


   ////////////////////////////////////////////////////////////////////
   DateTime::DateTime()
   {
      ResetToDefaultValues();
   }

   ////////////////////////////////////////////////////////////////////
   DateTime::DateTime(const TimeOrigin& to)
   {
      ResetToDefaultValues();
      mTimeOrigin = &to;

      if(mTimeOrigin == &TimeOrigin::GMT_TIME)
      {
         SetToGMTTime();
      }
      else if(mTimeOrigin == &TimeOrigin::LOCAL_TIME)
      {
         SetToLocalTime();
      }
   }

   ////////////////////////////////////////////////////////////////////
   DateTime::DateTime(time_t t)
   {
      ResetToDefaultValues();
      SetTime(t);
   }

   ////////////////////////////////////////////////////////////////////
   DateTime::DateTime(const struct tm& t)
   {
      ResetToDefaultValues();
      SetTime(t);
   }


   ////////////////////////////////////////////////////////////////////
   DateTime::DateTime(const DateTime& rhs)
   {
      *this = rhs;
   }

   ////////////////////////////////////////////////////////////////////
   DateTime& DateTime::operator=(const DateTime& rhs)
   {
      mTimeScale = rhs.mTimeScale;
      mFractionalSeconds = rhs.mFractionalSeconds;
      mSeconds = rhs.mSeconds;
      mMinutes = rhs.mMinutes;
      mHours = rhs.mHours;
      mDays = rhs.mDays;
      mMonths = rhs.mMonths;
      mYears = rhs.mYears;
      mGMTOffset = rhs.mGMTOffset;
      mTimeOrigin = rhs.mTimeOrigin;
      mTimeType = rhs.mTimeType;
      mStringFormat = rhs.mStringFormat;

      return *this;
   }

   ////////////////////////////////////////////////////////////////////
   DateTime::~DateTime()
   {

   }

   ////////////////////////////////////////////////////////////////////
   void DateTime::ResetToDefaultValues()
   {
      mTimeScale = 1.0f;
      mFractionalSeconds = 0.0f;
      mSeconds = 0;
      mMinutes = 0;
      mHours = 0;
      mDays = 0;
      mMonths = 0;
      mYears = 0;
      mGMTOffset = 0.0f;
      mTimeOrigin = &TimeOrigin::GMT_TIME;
      mTimeType = &TimeType::TIME_TYPE_OTHER;
      mStringFormat = &TimeFormat::CALENDAR_DATE_AND_TIME_FORMAT;
   }


   ////////////////////////////////////////////////////////////////////
   DateTime::operator time_t() const
   {
      return GetTime();
   }

   ////////////////////////////////////////////////////////////////////
   DateTime::operator tm() const
   {
      tm t;
      GetTime(t);
      return t;
   }

   ////////////////////////////////////////////////////////////////////
   DateTime::operator std::string() const
   {
      return ToString();
   }

   float DateTime::GetLocalGMTOffset()
   {
      //There should be a faster way to do this, but
      //it's probably not worth the effort.
      DateTime dt(TimeOrigin::LOCAL_TIME);
      return dt.GetGMTOffset();
   }

   ////////////////////////////////////////////////////////////////////
   void DateTime::IncrementClock(double seconds)
   {
      time_t timeInSeconds = *this;
      seconds *= mTimeScale;
      //accumulate fractions of a second and store off the remainder
      seconds += mFractionalSeconds;
      double floorSec = std::floor(seconds);

      timeInSeconds += time_t(floorSec);
      SetTime(timeInSeconds);

      mFractionalSeconds = seconds - floorSec;
   }

   ////////////////////////////////////////////////////////////////////
   void DateTime::AdjustTimeZone(float newGMTOffset)
   {
      float adjustmentToMake = newGMTOffset - mGMTOffset;
      double adjustmentToMakeInSec = adjustmentToMake * 60.0 * 60.0;
      IncrementClock(adjustmentToMakeInSec);
      mGMTOffset = newGMTOffset;
   }

   ////////////////////////////////////////////////////////////////////
   void DateTime::SetToLocalTime()
   {
      time_t t = time(NULL);
      struct tm timeParts;
      GetLocalTime(&t, timeParts);
      SetTime(timeParts);
      mFractionalSeconds = 0.0f;
      SetGMTOffset(timeParts, false);
   }

   ////////////////////////////////////////////////////////////////////
   void DateTime::SetToGMTTime()
   {
      time_t t = time(NULL);
      struct tm timeParts;
      GetGMTTime(&t, timeParts);
      SetTime(timeParts);
      mFractionalSeconds = 0.0f;
      mGMTOffset = 0.0f;
   }


   ////////////////////////////////////////////////////////////////////
   void DateTime::GetTime(unsigned& year, unsigned& month, unsigned& day, unsigned& hour, unsigned& min, float& sec) const
   {
      year = mYears;
      month = mMonths;
      day = mDays;
      hour = mHours;
      min = mMinutes;
      sec = mSeconds;
   }

   ////////////////////////////////////////////////////////////////////
   void DateTime::GetTime(unsigned& year, unsigned& month, unsigned& day, unsigned& hour, unsigned& min, unsigned& sec) const
   {
      year = mYears;
      month = mMonths;
      day = mDays;
      hour = mHours;
      min = mMinutes;
      sec = unsigned(floorf(mSeconds));
   }

   ////////////////////////////////////////////////////////////////////
   void DateTime::GetGMTTime(unsigned& year, unsigned& month, unsigned& day, unsigned& hour, unsigned& min, float& sec) const
   {
      DateTime dt(GetGMTTime());
      year = dt.mYears;
      month = dt.mMonths;
      day = dt.mDays;
      hour = dt.mHours;
      min = dt.mMinutes;
      sec = dt.mSeconds;
   }

   ////////////////////////////////////////////////////////////////////
   void DateTime::GetGMTTime(unsigned& year, unsigned& month, unsigned& day, unsigned& hour, unsigned& min, unsigned& sec) const
   {
      DateTime dt(GetGMTTime());

      year = dt.mYears;
      month = dt.mMonths;
      day = dt.mDays;
      hour = dt.mHours;
      min = dt.mMinutes;
      sec = unsigned(floorf(dt.mSeconds));
   }

   ////////////////////////////////////////////////////////////////////
   void DateTime::SetTime(unsigned year, unsigned month, unsigned day, unsigned hour, unsigned min, float sec)
   {
      mYears = year;
      mMonths = month;
      mDays = day;
      mHours = hour;
      mMinutes = min;
      mSeconds = floorf(sec);
      mFractionalSeconds = sec - mSeconds;
   }

   ////////////////////////////////////////////////////////////////////
   void DateTime::SetTime(unsigned year, unsigned month, unsigned day, unsigned hour, unsigned min, unsigned sec)
   {
      mYears = year;
      mMonths = month;
      mDays = day;
      mHours = hour;
      mMinutes = min;
      mSeconds = sec;
      mFractionalSeconds = 0;
   }

   ////////////////////////////////////////////////////////////////////
   double DateTime::GetTimeInSeconds() const
   {
      return double(GetTime()) + mFractionalSeconds;
   }

   ////////////////////////////////////////////////////////////////////
   time_t DateTime::GetTime() const
   {
      struct tm mt;
      mt.tm_mon = mMonths - 1;
      mt.tm_mday = mDays;
      mt.tm_year = mYears - 1900;
      mt.tm_hour = mHours;
      mt.tm_min = mMinutes;
      mt.tm_sec = mSeconds;
      mt.tm_isdst = 0;


      /**
         On many systems it is ambiguous if mktime() assumes the input is in
         GMT, or local timezone.  To address this, a new function called
         timegm() is appearing.  It works exactly like mktime() but
         explicitely interprets the input as GMT.

         timegm() is available and documented under FreeBSD.  It is
         available, but completely undocumented on my current Debian 2.1
         distribution.

         In the absence of timegm() we have to guess what mktime() might do.

         Many older BSD style systems have a mktime() that assumes the input
         time in GMT.  But FreeBSD explicitly states that mktime() assumes
         local time zone

         The mktime() on many SYSV style systems (such as Linux) usually
         returns its result assuming you have specified the input time in
         your local timezone.  Therefore, in the absence if timegm() you
         have to go to extra trouble to convert back to GMT.

         If you are having problems with incorrectly positioned astronomical
         bodies, this is a really good place to start looking.
      */
#ifdef DELTA_WIN32
      // mktime on windows adjusts for time zone, and we really don't want that
      time_t ret = _mkgmtime(&mt);
#else
      time_t ret = timegm(&mt);
#endif

      return ret;
   }

   ////////////////////////////////////////////////////////////////////
   time_t DateTime::GetGMTTime() const
   {
      DateTime dt(GetTime());
      dt.IncrementClock(3600.0f * -1.0f * mGMTOffset);
      return dt.GetTime();
   }

   ////////////////////////////////////////////////////////////////////
   void DateTime::SetTime(time_t t)
   {
      struct tm timeParts;
      GetGMTTime(&t, timeParts);
      SetTime(timeParts);
   }

   ////////////////////////////////////////////////////////////////////
   void DateTime::GetTime(tm& timeParts) const
   {
      time_t t = GetTime();
      GetGMTTime(&t, timeParts);
   }

   ////////////////////////////////////////////////////////////////////
   void DateTime::GetGMTTime(tm& timeParts) const
   {
      DateTime dt(GetTime());
      dt.IncrementClock(double(3600.0f * -mGMTOffset));
      dt.GetTime(timeParts);
   }

   ////////////////////////////////////////////////////////////////////
   void DateTime::SetTime(const tm& mt)
   {
      mMonths = mt.tm_mon + 1;
      mDays = mt.tm_mday;
      mYears = mt.tm_year + 1900;
      mHours = mt.tm_hour;
      mMinutes = mt.tm_min;
      mSeconds = mt.tm_sec;

      mFractionalSeconds = 0.0f;
   }

   ////////////////////////////////////////////////////////////////////
   void DateTime::SetGMTOffset(float hourOffset, bool dayLightSavings)
   {
      mGMTOffset = hourOffset + int(dayLightSavings);
   }

   ////////////////////////////////////////////////////////////////////
   void DateTime::SetGMTOffset(tm& timeParts, bool factorLocalDayLightSavingsIntoGMTOffset)
   {
      #ifdef DELTA_WIN32
            tzset();
            long tz = timezone * -1;
            mGMTOffset = tz / 3600.0f;
            if(factorLocalDayLightSavingsIntoGMTOffset)
            {
               mGMTOffset += timeParts.tm_isdst;
            }
      #else
            // If we are in daylight saving time, the gmt offset already accounts for that
            // But the dateTime wants the value without it taken into account, so we have to
            // subtract it back out.
            long tz = timeParts.tm_gmtoff;
            mGMTOffset = tz / 3600.0f;
            if(!factorLocalDayLightSavingsIntoGMTOffset)
            {
               mGMTOffset -= timeParts.tm_isdst;
            }
      #endif
   }

   ////////////////////////////////////////////////////////////////////
   void DateTime::SetGMTOffset(double lattitude, double longitude, bool dayLightSavings)
   {
      double offset = 7.5;
      if(longitude < 0.0f) offset = -offset;
      mGMTOffset = float(int(dayLightSavings) + int((longitude + offset) / 15.0));
   }

   ////////////////////////////////////////////////////////////////////
   float DateTime::GetGMTOffset() const
   {
      return mGMTOffset;
   }

   ////////////////////////////////////////////////////////////////////
   float DateTime::GetSecond() const
   {
      return float(mSeconds) + mFractionalSeconds;
   }

   ////////////////////////////////////////////////////////////////////
   void DateTime::SetSecond(float sec)
   {
      float seconds = std::floor(sec);
      mSeconds = unsigned(seconds);
      mFractionalSeconds = double(sec) - double(seconds);
   }

   ////////////////////////////////////////////////////////////////////
   unsigned DateTime::GetMinute() const
   {
      return mMinutes;
   }

   ////////////////////////////////////////////////////////////////////
   void DateTime::SetMinute(unsigned min)
   {
      mMinutes = min;
   }

   ////////////////////////////////////////////////////////////////////
   unsigned DateTime::GetHour() const
   {
      return mHours;
   }

   ////////////////////////////////////////////////////////////////////
   void DateTime::SetHour(unsigned hour)
   {
      mHours = hour;
   }


   ////////////////////////////////////////////////////////////////////
   unsigned DateTime::GetDay() const
   {
      return mDays;
   }

   ////////////////////////////////////////////////////////////////////
   void DateTime::SetDay(unsigned day)
   {
      mDays = day;
   }


   ////////////////////////////////////////////////////////////////////
   unsigned DateTime::GetMonth() const
   {
      return mMonths;
   }

   ////////////////////////////////////////////////////////////////////
   void DateTime::SetMonth(unsigned month)
   {
      mMonths = month;
   }


   ////////////////////////////////////////////////////////////////////
   unsigned DateTime::GetYear() const
   {
      return mYears;
   }

   ////////////////////////////////////////////////////////////////////
   void DateTime::SetYear(unsigned year)
   {
      mYears = year;
   }


   ////////////////////////////////////////////////////////////////////
   float DateTime::GetTimeScale() const
   {
      return mTimeScale;
   }

   ////////////////////////////////////////////////////////////////////
   void DateTime::SetTimeScale(float percentScaleInSeconds)
   {
      mTimeScale = percentScaleInSeconds;
   }


   ////////////////////////////////////////////////////////////////////
   const DateTime::TimeType& DateTime::GetTimeType() const
   {
      return *mTimeType;
   }

   ////////////////////////////////////////////////////////////////////
   void DateTime::SetTimeType(const TimeType& tt)
   {
      mTimeType = &tt;
   }

   ////////////////////////////////////////////////////////////////////
   const DateTime::TimeOrigin& DateTime::GetTimeOrigin() const
   {
      return *mTimeOrigin;
   }

   ////////////////////////////////////////////////////////////////////
   void DateTime::SetTimeOrigin(const TimeOrigin& to)
   {
      mTimeOrigin = &to;
   }


   ////////////////////////////////////////////////////////////////////
   const DateTime::TimeFormat& DateTime::GetTimeFormat() const
   {
      return *mStringFormat;
   }

   ////////////////////////////////////////////////////////////////////
   void DateTime::SetTimeFormat(const TimeFormat& tf)
   {
      mStringFormat = &tf;
   }

   ////////////////////////////////////////////////////////////////////
   std::string DateTime::ToString() const
   {
      return ToString(*mStringFormat);
   }

   ////////////////////////////////////////////////////////////////////
   std::string DateTime::ToString(const DateTime& dt, const TimeFormat& tf)
   {
      return dt.ToString(tf);
   }


   ////////////////////////////////////////////////////////////////////
   std::string DateTime::ToString(const TimeFormat& tf) const
   {
      char buffer[80];
      struct tm timeParts = *this; //conversion operator
      std::string str;

      if(tf == TimeFormat::CALENDAR_DATE_AND_TIME_FORMAT)
      {
         //this case we must handle specially because the msvc compiler insists on
         //writing out the time zone for %z as opposed to printing the actual offset
         //this is what we want it to look like
         //2007-05-10T16:08:18.0-05:00

         float tz = GetGMTOffset();

         int tzHour = (int)floorf(tz);
         int tzMin = (int)(tz - float(tzHour));

         snprintf(buffer, 80, "%04d-%02d-%02dT%02d:%02d:%02d%+03d:%02d",
            timeParts.tm_year + 1900, timeParts.tm_mon + 1, timeParts.tm_mday,
            timeParts.tm_hour, timeParts.tm_min, timeParts.tm_sec,
            tzHour, tzMin);
      }
      else
      {
         if(tf == TimeFormat::LOCAL_DATE_AND_TIME_FORMAT)
         {
            strftime(buffer, 80, "%c", &timeParts);
         }
         else if(tf == TimeFormat::CLOCK_TIME_12_HOUR_FORMAT)
         {
            strftime(buffer, 80, "%I:%M:%S %p", &timeParts);
         }
         else if(tf == TimeFormat::LOCAL_DATE_FORMAT)
         {
            strftime(buffer, 80, "%x", &timeParts);
         }
         else if(tf == TimeFormat::LEXICAL_DATE_FORMAT)
         {
            strftime(buffer, 80, "%B %d, %Y", &timeParts);
         }
         else if(tf == TimeFormat::CALENDAR_DATE_FORMAT)
         {
            strftime(buffer, 80, "%Y-%m-%d", &timeParts);
         }
         else if(tf == TimeFormat::CLOCK_TIME_24_HOUR_FORMAT)
         {
            strftime(buffer, 80, "%H:%M:%S", &timeParts);
         }
         else if(tf == TimeFormat::WEEK_DATE_FORMAT)
         {
            strftime(buffer, 80, "%Y-W%U-%w", &timeParts);
         }
         else if(tf == TimeFormat::ORDINAL_DATE_FORMAT)
         {
            strftime(buffer, 80, "%Y-%j", &timeParts);
         }

      }

      str.assign(buffer);
      return str;
   }



   void DateTime::GetGMTTime(time_t* t, tm& timeParts) const
   {
      if(t != NULL)
      {
         struct tm* temp = gmtime(t);
         if(temp != NULL)
         {
            timeParts = *temp;
            return;
         }
      }

      memset(&timeParts, 0, sizeof(tm));
   }


   void DateTime::GetLocalTime(time_t* t, tm& timeParts) const
   {
      if(t != NULL)
      {
         struct tm* temp = localtime(t);
         if(temp != NULL)
         {
            timeParts = *temp;
            return;
         }
      }

      memset(&timeParts, 0, sizeof(tm));
   }
}

