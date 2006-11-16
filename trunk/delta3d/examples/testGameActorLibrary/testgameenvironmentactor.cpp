/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * William E. Johnson II
 */
#include "testgameenvironmentactor.h"
#include <dtUtil/stringutils.h>
#include <sstream>

void TestGameEnvironmentActor::AddActor(dtCore::DeltaDrawable &dd)
{
   AddChild(&dd);
}

void TestGameEnvironmentActor::RemoveActor(dtCore::DeltaDrawable &dd)
{
   RemoveChild(&dd);
}

void TestGameEnvironmentActor::RemoveAllActors()
{
   while(GetNumChildren() > 0)
      RemoveChild(GetChild(0));
}

bool TestGameEnvironmentActor::ContainsActor(dtCore::DeltaDrawable &dd) const
{
   return !CanBeChild(&dd);
}

void TestGameEnvironmentActor::GetAllActors(std::vector<dtCore::DeltaDrawable*> &vec)
{
   vec.clear();

   for(unsigned int i = 0; i < GetNumChildren(); i++)
      vec.push_back(GetChild(i));
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

void TestGameEnvironmentActor::GetTimeAndDate(int &year, int &month, int &day, int &hour, int &min, int &sec) const
{
   year  = mYear;
   month = mMonth;
   day   = mDay;
   hour  = mHour;
   min   = mMin;
   sec   = mSec;
}

void TestGameEnvironmentActor::SetTimeAndDateString(const std::string &timeAndDate)
{
   int year, month, day, hour, min, sec;
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
   time_t currentTime;
   time(&currentTime);
   return dtUtil::TimeAsUTC(currentTime);
}

std::string TestGameEnvironmentActor::GetTimeAndDateString() const
{
   std::ostringstream oss;
   int year, month, day, hour, min, sec;
   GetTimeAndDate(year, month, day, hour, min, sec);
   oss << year << '-';
   if(month < 10)
      oss << '0' << month << '-';
   else
      oss << month << '-';

   if(day < 10)
      oss << '0' << day << 'T';
   else
      oss << day << 'T';

   if(hour < 10)
      oss << '0' << hour << ':';
   else
      oss << hour << ':';

   if(min < 10)
      oss << '0' << min << ':';
   else
      oss << min << ':';

   if(sec < 10)
      oss << '0' << sec;
   else 
      oss << sec;

   return oss.str();
}
