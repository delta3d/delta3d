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
#ifndef DELTA_TEST_ENVIRONMENT_ACTOR
#define DELTA_TEST_ENVIRONMENT_ACTOR

#include "export.h"
#include <dtGame/environmentactor.h>
#include <dtABC/weather.h>

class DT_EXAMPLE_EXPORT TestGameEnvironmentActor : public dtGame::EnvironmentActor
{
   public:

      TestGameEnvironmentActor(dtGame::GameActorProxy &proxy) :
         dtGame::EnvironmentActor(proxy), mYear(0), mMonth(0), mDay(0), mHour(0), mMin(0), mSec(0) { }

      virtual void AddActor(dtDAL::ActorProxy &proxy);

      virtual void RemoveActor(dtDAL::ActorProxy &proxy);

      virtual void RemoveAllActors();

      virtual bool ContainsActor(dtDAL::ActorProxy &proxy) const;

      virtual void GetAllActors(std::vector<dtDAL::ActorProxy*> &vec);

      virtual void GetAllActors(std::vector<const dtDAL::ActorProxy*> &vec) const;

      virtual void SetTimeAndDate(const int year, const int month, const int day,
                                  const int hour, const int min,   const int sec);

      virtual void GetTimeAndDate(int &year, int &month, int &day, int &hour, int &min, int &sec) const;
   
      void SetTimeAndDateString(const std::string &timeAndDate);

      std::string GetCurrentTimeAndDateString() const;

      std::string GetTimeAndDateString() const;

      unsigned int GetNumEnvironmentChildren() const { return mAddedActors.size(); }

   protected:

      virtual ~TestGameEnvironmentActor() { }

   private:
      int mYear; 
      int mMonth;
      int mDay;
      int mHour; 
      int mMin;   
      int mSec;
      
      std::map<dtCore::RefPtr<dtDAL::ActorProxy>, dtCore::DeltaDrawable*> mAddedActors;
};

class DT_EXAMPLE_EXPORT TestGameEnvironmentActorProxy : public dtGame::EnvironmentActorProxy
{
   public:

      TestGameEnvironmentActorProxy() { SetClassName("TestGameEnvironmentActorProxy"); }

      virtual void CreateActor()      { mActor = new TestGameEnvironmentActor(*this); }

      virtual void BuildPropertyMap() { dtGame::GameActorProxy::BuildPropertyMap(); }

      virtual void BuildInvokables()  { dtGame::GameActorProxy::BuildInvokables();  }

      virtual void OnEnteredWorld()   { }
   protected:
      virtual ~TestGameEnvironmentActorProxy() { }

};
#endif
