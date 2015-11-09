/* -*-c++-*-
* testGameActorLibrary - testgameenvironmentactor (.h & .cpp) - Using 'The MIT License'
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
#ifndef DELTA_TEST_ENVIRONMENT_ACTOR
#define DELTA_TEST_ENVIRONMENT_ACTOR

#include "export.h"
#include <dtGame/environmentactor.h>

class DT_EXAMPLE_EXPORT TestGameEnvironmentActor : public dtGame::IEnvGameActor
{
   public:
      typedef dtGame::IEnvGameActor BaseClass;

      TestGameEnvironmentActor(dtGame::GameActorProxy& parent) :
         dtGame::IEnvGameActor(parent),  mYear(0), mMonth(0), mDay(0), mHour(0), mMin(0), mSec(0) { }

      virtual void AddActor(dtCore::DeltaDrawable& dd);

      virtual void RemoveActor(dtCore::DeltaDrawable& dd);

      virtual void RemoveAllActors();

      virtual bool ContainsActor(dtCore::DeltaDrawable& dd) const;

      virtual void GetAllActors(std::vector<dtCore::DeltaDrawable*>& vec);

      virtual void SetTimeAndDate(const int year, const int month, const int day,
                                  const int hour, const int min,   const int sec);

      virtual void GetTimeAndDate(int& year, int& month, int& day, int& hour, int& min, int& sec) const;
   
      void SetTimeAndDateString(const std::string& timeAndDate);

      std::string GetCurrentTimeAndDateString() const;

      std::string GetTimeAndDateString() const;

      unsigned int GetNumEnvironmentChildren() const { return GetNumChildren(); }

   protected:

      virtual ~TestGameEnvironmentActor() { }

   private:

      int mYear; 
      int mMonth;
      int mDay;
      int mHour; 
      int mMin;   
      int mSec;
};

class DT_EXAMPLE_EXPORT TestGameEnvironmentActorProxy : public dtGame::IEnvGameActorProxy
{
   public:

      TestGameEnvironmentActorProxy() { SetClassName("TestGameEnvironmentActorProxy"); }

      virtual void CreateDrawable()   { SetDrawable(*new TestGameEnvironmentActor(*this)); }

      virtual void BuildPropertyMap() { dtGame::GameActorProxy::BuildPropertyMap(); }

      virtual void BuildInvokables()  { dtGame::GameActorProxy::BuildInvokables();  }

      virtual void OnEnteredWorld()   { }
   protected:
      virtual ~TestGameEnvironmentActorProxy() { }

};
#endif
