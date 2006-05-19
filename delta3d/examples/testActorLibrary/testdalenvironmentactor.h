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
#include <dtDAL/plugin_export.h>
#include <dtDAL/environmentactor.h>
#include <dtABC/weather.h>
#include <dtCore/cloudplane.h>

class DT_PLUGIN_EXPORT TestDALEnvironmentActor : public dtDAL::EnvironmentActor, public dtCore::DeltaDrawable
{
   public:

      /// Constructor
      TestDALEnvironmentActor();

      /// Destructor
      virtual ~TestDALEnvironmentActor();

      /**
       * Adds an actor proxy to the internal hierarchy of the environment
       * @param proxy The proxy to add
       */
      virtual void AddActor(dtDAL::ActorProxy &proxy);

      /**
       * Removes an actor proxy from the internal hierarchy
       * @param proxy The proxy to remove
       */
      virtual void RemoveActor(dtDAL::ActorProxy &proxy);

      /**
       * Removes all actors associated with this environment
       */
      virtual void RemoveAllActors();

      /**
       * Called to see if this environment has the specified proxy
       * @param proxy The proxy to look for
       * @return True if it contains it, false if not
       */
      virtual bool ContainsActor(dtDAL::ActorProxy &proxy) const;

      /**
       * Gets all the actors associated with this environment
       * @param vec The vector to fill
       */
      virtual void GetAllActors(std::vector<dtDAL::ActorProxy*> &vec);

      /**
       * const version of the above function
       * Gets all the actors associated with this environment
       * @param vec The vector to fill
       */
      virtual void GetAllActors(std::vector<const dtDAL::ActorProxy*> &vec) const;

      /**
       * Sets the date and time on this environment
       * @param year The year to set
       * @param month The month to set
       * @param day The day to set
       * @param hour The hour to set in military time
       * @param min The minute to set
       * @param sec The second to set
       */
      virtual void SetTimeAndDate(const int year, const int month, const int day,
         const int hour, const int min,   const int sec);

      /**
       * Gets the time and date of the current environment
       * @param year The year to get
       * @param month The month to get
       * @param day The day to get
       * @param hour The hour to get
       * @param min The minute to get
       * @param sec The second to get
       */
      virtual void GetTimeAndDate(int &year, int &month, int &day,
               int &hour, int &min, int &sec) const;

      /**
       * Returns the number of children this environment has
       * @return The number of children
       */
      unsigned int GetNumEnvironmentChildren() const;

     private:

      dtCore::RefPtr<dtABC::Weather> mWeather;
      std::map<dtCore::RefPtr<dtDAL::ActorProxy>, dtCore::DeltaDrawable*> mAddedActors;
};

class DT_PLUGIN_EXPORT TestDALEnvironmentActorProxy : public dtDAL::ActorProxy
{
   public:

      TestDALEnvironmentActorProxy();

      virtual ~TestDALEnvironmentActorProxy();

      virtual void CreateActor() { mActor = new TestDALEnvironmentActor; }

      virtual void BuildPropertyMap();

      virtual bool IsPlaceable() const { return false; }
};
