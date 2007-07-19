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

TestDALEnvironmentActor::TestDALEnvironmentActor()
{
}

TestDALEnvironmentActor::~TestDALEnvironmentActor()
{

}

void TestDALEnvironmentActor::AddActor(dtCore::DeltaDrawable &dd)
{
   AddChild(&dd);
}

void TestDALEnvironmentActor::RemoveActor(dtCore::DeltaDrawable &dd)
{
   RemoveChild(&dd);
}

bool TestDALEnvironmentActor::ContainsActor(dtCore::DeltaDrawable &dd) const
{
   return !CanBeChild(&dd);
}

void TestDALEnvironmentActor::RemoveAllActors()
{
   while(GetNumChildren() > 0)
      RemoveChild(GetChild(0));
}

void TestDALEnvironmentActor::GetAllActors(std::vector<dtCore::DeltaDrawable*> &vec)
{
   vec.clear();

   for(unsigned int i = 0; i < GetNumChildren(); i++)
      vec.push_back(GetChild(i));
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
   return GetNumChildren();
}
