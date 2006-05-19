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
#include "testdalenvironmentactor.h"
#include <dtCore/environment.h>

TestDALEnvironmentActorProxy::TestDALEnvironmentActorProxy()
{

}

TestDALEnvironmentActorProxy::~TestDALEnvironmentActorProxy()
{

}

void TestDALEnvironmentActorProxy::BuildPropertyMap()
{

}

TestDALEnvironmentActor::TestDALEnvironmentActor() : 
   mWeather(new dtABC::Weather)
{
   AddChild(mWeather->GetEnvironment());
}

TestDALEnvironmentActor::~TestDALEnvironmentActor()
{

}

void TestDALEnvironmentActor::AddActor(dtDAL::ActorProxy &child)
{
   mWeather->GetEnvironment()->AddChild(child.GetActor());
   mAddedActors.insert(std::make_pair(&child, child.GetActor()));
}

void TestDALEnvironmentActor::RemoveActor(dtDAL::ActorProxy &proxy)
{
   mWeather->GetEnvironment()->RemoveChild(proxy.GetActor());

   std::map<dtCore::RefPtr<dtDAL::ActorProxy>, dtCore::DeltaDrawable*>::iterator i =
      mAddedActors.find(&proxy);

   if (i != mAddedActors.end())
      mAddedActors.erase(i);
}

bool TestDALEnvironmentActor::ContainsActor(dtDAL::ActorProxy &proxy) const
{
   std::map<dtCore::RefPtr<dtDAL::ActorProxy>, dtCore::DeltaDrawable*>::const_iterator i =
      mAddedActors.find(&proxy);

   return i != mAddedActors.end();
}

void TestDALEnvironmentActor::RemoveAllActors()
{
   while(mWeather->GetEnvironment()->GetNumChildren() > 0)
      mWeather->GetEnvironment()->RemoveChild(mWeather->GetEnvironment()->GetChild(0));

   mAddedActors.clear();
}

void TestDALEnvironmentActor::GetAllActors(std::vector<dtDAL::ActorProxy*> &vec)
{
   vec.clear();

   std::map<dtCore::RefPtr<dtDAL::ActorProxy>, dtCore::DeltaDrawable*>::iterator i;
   for(i = mAddedActors.begin(); i != mAddedActors.end(); ++i)
   {
      dtCore::RefPtr<dtDAL::ActorProxy> proxy = i->first;
      vec.push_back(proxy.get());
   }
}

void TestDALEnvironmentActor::GetAllActors(std::vector<const dtDAL::ActorProxy*> &vec) const
{
   vec.clear();

   std::map<dtCore::RefPtr<dtDAL::ActorProxy>, dtCore::DeltaDrawable*>::const_iterator i;
   for(i = mAddedActors.begin(); i != mAddedActors.end(); ++i)
   {
      dtCore::RefPtr<dtDAL::ActorProxy> proxy = i->first;
      vec.push_back(proxy.get());
   }
}

void TestDALEnvironmentActor::SetTimeAndDate(const int year, const int month, const int day, const int hour, 
                                          const int min, const int sec)
{

}

void TestDALEnvironmentActor::GetTimeAndDate(int &year, int &month, int &day, 
                                          int &hour, int &min, int &sec) const
{

}

unsigned int TestDALEnvironmentActor::GetNumEnvironmentChildren() const
{
   return mAddedActors.size();
}
