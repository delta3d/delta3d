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
 * @author William E. Johnson II
 */
#include "testgameenvironmentactor.h"

void TestGameEnvironmentActor::AddActor(dtDAL::ActorProxy &proxy)
{
   mEnvironment->AddChild(proxy.GetActor());
   mAddedActors.insert(std::make_pair(&proxy, proxy.GetActor()));
}

void TestGameEnvironmentActor::RemoveActor(dtDAL::ActorProxy &proxy)
{
   mEnvironment->RemoveChild(proxy.GetActor());
   std::map<dtCore::RefPtr<dtDAL::ActorProxy>, dtCore::DeltaDrawable*>::iterator i =
      mAddedActors.find(&proxy);

   if(i != mAddedActors.end())
      mAddedActors.erase(i);
}

void TestGameEnvironmentActor::RemoveAllActors()
{
   while(mEnvironment->GetNumChildren() > 0)
      mEnvironment->RemoveChild(mEnvironment->GetChild(0));

   mAddedActors.clear();
}

bool TestGameEnvironmentActor::ContainsActor(dtDAL::ActorProxy &proxy) const
{
   std::map<dtCore::RefPtr<dtDAL::ActorProxy>, dtCore::DeltaDrawable*>::const_iterator i =
      mAddedActors.find(&proxy);

   return i != mAddedActors.end();
}

void TestGameEnvironmentActor::GetAllActors(std::vector<dtDAL::ActorProxy*> &vec)
{
   vec.clear();

   for(std::map<dtCore::RefPtr<dtDAL::ActorProxy>, dtCore::DeltaDrawable*>::iterator i =
      mAddedActors.begin(); i != mAddedActors.end(); ++i)
   {
      dtCore::RefPtr<dtDAL::ActorProxy> proxy = i->first;
      vec.push_back(proxy.get());
   }
}

void TestGameEnvironmentActor::GetAllActors(std::vector<const dtDAL::ActorProxy*> &vec) const
{
   vec.clear();

   for(std::map<dtCore::RefPtr<dtDAL::ActorProxy>, dtCore::DeltaDrawable*>::const_iterator i = 
      mAddedActors.begin(); i != mAddedActors.end(); ++i)
   {
      dtCore::RefPtr<dtDAL::ActorProxy> proxy = i->first; 
      vec.push_back(proxy.get());
   }
}

void TestGameEnvironmentActor::SetTimeAndDate(const int year, const int month, const int day, 
                                          const int hour, const int min,   const int sec)
{
   mEnvironment->SetDateTime(year, month, day, hour, min, sec);
}

void TestGameEnvironmentActor::GetTimeAndDate(int &year, int &month, int &day, int &hour, int &min, int &sec) const
{
   mEnvironment->GetDateTime(&year, &month, &day, &hour, &min, &sec);
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
