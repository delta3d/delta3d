/* -*-c++-*-
 * testActorLibrary - testdalenvironmentactor (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2008, Alion Science and Technology Corporation
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
   while (GetNumChildren() > 0)
   {
      RemoveChild(GetChild(0));
   }
}

void TestDALEnvironmentActor::GetAllActors(std::vector<dtCore::DeltaDrawable*> &vec)
{
   vec.clear();

   for (unsigned int i = 0; i < GetNumChildren(); i++)
   {
      vec.push_back(GetChild(i));
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
   return GetNumChildren();
}
