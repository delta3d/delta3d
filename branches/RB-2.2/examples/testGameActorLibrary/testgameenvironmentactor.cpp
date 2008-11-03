/* -*-c++-*-
 * testGameActorLibrary - This source file (.h & .cpp) - Using 'The MIT License'
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
 * William E. Johnson II
 */

#include "testgameenvironmentactor.h"
#include <dtUtil/datetime.h>
#include <sstream>

void TestGameEnvironmentActor::AddActor(dtCore::DeltaDrawable& dd)
{
   AddChild(&dd);
}

void TestGameEnvironmentActor::RemoveActor(dtCore::DeltaDrawable& dd)
{
   RemoveChild(&dd);
}

void TestGameEnvironmentActor::RemoveAllActors()
{
   while (GetNumChildren() > 0)
   {
      RemoveChild(GetChild(0));
   }
}

bool TestGameEnvironmentActor::ContainsActor(dtCore::DeltaDrawable& dd) const
{
   return !CanBeChild(&dd);
}

void TestGameEnvironmentActor::GetAllActors(std::vector<dtCore::DeltaDrawable*> &vec)
{
   vec.clear();

   for (unsigned int i = 0; i < GetNumChildren(); i++)
   {
      vec.push_back(GetChild(i));
   }
}

void TestGameEnvironmentActor::SetTimeAndDate(const int year, const int month, const int day,
                                              const int hour, const int min,   const int sec)
{
   mYear  = year;
   mMonth = month;
   mDay   = day;
   mHour  = hour;
   mMin   = min;
   mSec   = sec;
}

void TestGameEnvironmentActor::GetTimeAndDate(int& year, int& month, int& day, int& hour, int& min, int& sec) const
{
   year  = mYear;
   month = mMonth;
   day   = mDay;
   hour  = mHour;
   min   = mMin;
   sec   = mSec;
}

void TestGameEnvironmentActor::SetTimeAndDateString(const std::string& timeAndDate)
{
   unsigned year, month, day, hour, min, sec;
   char delimeter;
   std::istringstream iss(timeAndDate);
   // The time is stored in the universal format of:
   // yyyy-mm-ddThh:min:ss-some number
   // So we need to use a delimeter to ensure that we don't choke on the seperators
   iss >> year >> delimeter >> month >> delimeter >> day >> delimeter >> hour >> delimeter >>
      min >> delimeter >> sec;

   SetTimeAndDate(year, month, day, hour, min, sec);
}

std::string TestGameEnvironmentActor::GetCurrentTimeAndDateString() const
{
   return dtUtil::DateTime::ToString(dtUtil::DateTime(dtUtil::DateTime::TimeOrigin::LOCAL_TIME),
      dtUtil::DateTime::TimeFormat::CALENDAR_DATE_AND_TIME_FORMAT);
}

std::string TestGameEnvironmentActor::GetTimeAndDateString() const
{
   std::ostringstream oss;
   int year, month, day, hour, min, sec;
   GetTimeAndDate(year, month, day, hour, min, sec);
   oss << year << '-';
   if (month < 10)
   {
      oss << '0' << month << '-';
   }
   else
   {
      oss << month << '-';
   }

   if (day < 10)
   {
      oss << '0' << day << 'T';
   }
   else
   {
      oss << day << 'T';
   }

   if (hour < 10)
   {
      oss << '0' << hour << ':';
   }
   else
   {
      oss << hour << ':';
   }

   if (min < 10)
   {
      oss << '0' << min << ':';
   }
   else
   {
      oss << min << ':';
   }

   if (sec < 10)
   {
      oss << '0' << sec;
   }
   else
   {
      oss << sec;
   }

   return oss.str();
}
