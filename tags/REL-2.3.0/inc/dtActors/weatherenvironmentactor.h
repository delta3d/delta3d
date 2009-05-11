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
#ifndef weatherenvironmentactor_h__
#define weatherenvironmentactor_h__

#include <dtDAL/plugin_export.h>
#include <dtGame/environmentactor.h>
#include <dtABC/weather.h>
#include <dtCore/cloudplane.h>

namespace dtActors
{
   class DT_PLUGIN_EXPORT WeatherEnvironmentActor : public dtGame::IEnvGameActor
   {
      public:

         class DT_PLUGIN_EXPORT CloudCoverEnum : public dtUtil::Enumeration
         {
            DECLARE_ENUM(CloudCoverEnum);

            public:

               static CloudCoverEnum CLEAR;
               static CloudCoverEnum FEW;
               static CloudCoverEnum SCATTERED;
               static CloudCoverEnum BROKEN;
               static CloudCoverEnum OVERCAST;

               dtABC::Weather::CloudType GetEnumValue() const { return mValue; }

            private:

               CloudCoverEnum(const std::string &name, dtABC::Weather::CloudType tp) 
                  : dtUtil::Enumeration(name), mValue(tp) 
               {
                  AddInstance(this);
               }

               dtABC::Weather::CloudType mValue;
         }; 

         /**
          * Class that encapsulates Visibility Type
          * @see class dtCore::Weather
          */
         class DT_PLUGIN_EXPORT VisibilityTypeEnum : public dtUtil::Enumeration
         {
            DECLARE_ENUM(VisibilityTypeEnum);

            public:

               static VisibilityTypeEnum VISIBILITY_UNLIMITED;
               static VisibilityTypeEnum VISIBILITY_FAR;
               static VisibilityTypeEnum VISIBILITY_MODERATE;
               static VisibilityTypeEnum VISIBILITY_LIMITED;
               static VisibilityTypeEnum VISIBILITY_CLOSE;

               dtABC::Weather::VisibilityType GetEnumValue() const { return mType; } 

            private:

               VisibilityTypeEnum(const std::string &name, dtABC::Weather::VisibilityType type) 
                  : dtUtil::Enumeration(name), mType(type)
               {
                  AddInstance(this);
               }

               dtABC::Weather::VisibilityType mType;
         };

         class DT_PLUGIN_EXPORT WeatherThemeEnum : public dtUtil::Enumeration
         {
            DECLARE_ENUM(WeatherThemeEnum);

            public:

               static WeatherThemeEnum THEME_CUSTOM;
               static WeatherThemeEnum THEME_CLEAR;
               static WeatherThemeEnum THEME_FAIR;
               static WeatherThemeEnum THEME_FOGGY;
               static WeatherThemeEnum THEME_RAINY;

               dtABC::Weather::WeatherTheme GetEnumValue() const { return mTheme; }

            private:
               
               WeatherThemeEnum(const std::string &name, dtABC::Weather::WeatherTheme theme) 
                  : dtUtil::Enumeration(name), mTheme(theme) 
               {
                  AddInstance(this);
               }

               dtABC::Weather::WeatherTheme mTheme;
         };

         class DT_PLUGIN_EXPORT TimePeriodEnum : public dtUtil::Enumeration
         {
            DECLARE_ENUM(TimePeriodEnum);

            public:

               static TimePeriodEnum TIME_DAWN;
               static TimePeriodEnum TIME_DAY;
               static TimePeriodEnum TIME_DUSK;
               static TimePeriodEnum TIME_NIGHT;

               dtABC::Weather::TimePeriod GetEnumValue() const { return mTimePeriod; }

            private:

               TimePeriodEnum(const std::string &name, dtABC::Weather::TimePeriod tp) 
                  : dtUtil::Enumeration(name), mTimePeriod(tp) 
               {
                  AddInstance(this);
               }

               dtABC::Weather::TimePeriod mTimePeriod;
         }; 

         class DT_PLUGIN_EXPORT SeasonEnum : public dtUtil::Enumeration
         {
            DECLARE_ENUM(SeasonEnum);

            public:

               static SeasonEnum SEASON_SPRING;
               static SeasonEnum SEASON_FALL;
               static SeasonEnum SEASON_WINTER;
               static SeasonEnum SEASON_SUMMER;

               dtABC::Weather::Season GetEnumValue() const { return mSeason; }

            private:

               SeasonEnum(const std::string &name, dtABC::Weather::Season season) 
                  : dtUtil::Enumeration(name), mSeason(season) 
               {
                  AddInstance(this);
               }

               dtABC::Weather::Season mSeason;
         };

         class DT_PLUGIN_EXPORT WindTypeEnum : public dtUtil::Enumeration
         {
            DECLARE_ENUM(WindTypeEnum);

            public:

               static WindTypeEnum WIND_NONE;
               static WindTypeEnum WIND_BREEZE;
               static WindTypeEnum WIND_LIGHT;
               static WindTypeEnum WIND_MODERATE;
               static WindTypeEnum WIND_HEAVY;
               static WindTypeEnum WIND_SEVERE;

               dtABC::Weather::WindType GetEnumValue() const { return mWindType; }

            private:

               WindTypeEnum(const std::string &name, dtABC::Weather::WindType wt) 
                  : dtUtil::Enumeration(name), mWindType(wt) 
               {
                  AddInstance(this);
               }

               dtABC::Weather::WindType mWindType;
         };

         /// Constructor
         WeatherEnvironmentActor(dtGame::GameActorProxy &proxy);

         /**
           * Adds an actor proxy to the internal hierarchy of the environment
           * @param proxy The proxy to add
           */
         virtual void AddActor(dtCore::DeltaDrawable &dd);

         /**
           * Removes an actor proxy from the internal hierarchy
           * @param proxy The proxy to remove
           */
         virtual void RemoveActor(dtCore::DeltaDrawable &dd);

         /**
           * Removes all actors associated with this environment
           */
         virtual void RemoveAllActors();

         /**
           * Called to see if this environment has the specified proxy
           * @param proxy The proxy to look for
           * @return True if it contains it, false if not
           */
         virtual bool ContainsActor(dtCore::DeltaDrawable &dd) const;

         /**
           * const version of the above function
           * Gets all the actors associated with this environment
           * @param vec The vector to fill
           */
         virtual void GetAllActors(std::vector<dtCore::DeltaDrawable*> &vec);

         /**
           * Sets the date and time on this environment
           * @param year The year to set
           * @param month The month to set
           * @param day The day to set
           * @param hour The hour to set in military time
           * @param min The minute to set
           * @param sec The second to set
           */
         virtual void SetTimeAndDate(const unsigned year, const unsigned month, const unsigned day,
                                     const unsigned hour, const unsigned min,   const unsigned sec);

         /**
           * Gets the time and date of the current environment
           * @param year The year to get
           * @param month The month to get
           * @param day The day to get
           * @param hour The hour to get
           * @param min The minute to get
           * @param sec The second to get
           */
         virtual void GetTimeAndDate(unsigned &year, unsigned &month, unsigned &day, unsigned &hour, unsigned &min, unsigned &sec) const;

         /**
          * Gets the number of children of this environment
          */
         virtual unsigned int GetNumEnvironmentChildren() const;

         /**
          * Sets the time and date from a string
          * @param timeAndDate The string to parse
          */
         void SetTimeAndDateString(const std::string &timeAndDate);

         /**
          * Gets the time and date in a string format
          * @return The time and date
          */
         std::string GetTimeAndDateString() const;

         /**
          * Gets the current time and date in a string format
          * @return The time and date, as based on real world time
          */
         std::string GetCurrentTimeAndDateString() const;

         /**
          * Gets the weather object contained by this actor.
          * @return The weather object contained by this actor.
          */
         dtABC::Weather& GetWeather();

         /**
          * Enables a cloud plane in this environment
          * @todo Refactor this to not use hardcoded numbers
          * @param enable True to turn on, false to turn off
          */
         virtual void EnableCloudPlane(bool enable);

         /**
          * Returns if a cloud plane is enabled
          */
         virtual bool IsCloudPlaneEnabled() const;

         /**
          * Toggles fog on and off
          * @param enable True to turn on, false to turn off
          */
         void EnableFog(bool enable);

         /**
          * Returns true if fog is enabled
          */
         bool IsFogEnabled() const; 

         /**
          * Sets the visibility on the weather
          * @param visibility The new visibility to set
          */
         void SetWeatherVisibility(VisibilityTypeEnum &visibility);

         /**
          * Gets the current weather visibility
          * @return The visibility
          */
         VisibilityTypeEnum& GetWeatherVisibility();

         /**
          * Sets the weather theme for this environment
          * @param theme The theme to set
          */
         void SetWeatherTheme(WeatherThemeEnum &theme);

         /**
          * Gets the current weather theme
          * @return The weather theme
          */
         WeatherThemeEnum& GetWeatherTheme();

         /**
          * Sets the season and time period on this environment
          * @param time The time period
          * @param season The season
          */
         void SetTimePeriodAndSeason(TimePeriodEnum &time, SeasonEnum &season);

         /**
          * Sets the wind type on this environment
          * @param windType The new wind type
          */
         void SetWindType(WindTypeEnum &windType);

         /**
          * Gets the current wind type
          * @return The wind type
          */
         WindTypeEnum& GetWindType();

         /**
          * Sets the sky color on this environment
          * @param color The color to set, in RGBA format
          */
         void SetSkyColor(const osg::Vec4 &color);

         /**
          * Gets the current sky color
          * @return the sky color
          */
         osg::Vec4 GetSkyColor() const;

         /**
          * Sets the rate of change of this environment
          * @param rate The new rate of change to set. Clamped between 0 - 1.0
          */
         void SetRateOfChange(float rate);

         /**
          * Gets the rate of change of this environment
          * @return The rate of change
          */
         float GetRateOfChange() const;

         /**
          * Sets the time period on this environment
          * @param period The new time period
          */
         void SetTimePeriod(TimePeriodEnum &period);

         /**
          * Gets the current time period of this environment
          */
         TimePeriodEnum& GetTimePeriod() const;

         /**
          * Sets the season on this environment
          * @param season The new season
          */
         void SetSeason(SeasonEnum &season);

         /**
          * Gets the current season
          */
         SeasonEnum& GetSeason() const;

         /**
          * Sets the current cloud cover
          */
         void SetCloudCover(CloudCoverEnum &clouds);

         /**
          * Gets the current cloud cover
          */
         CloudCoverEnum& GetCloudCover() const;

      protected:

         /// Destructor
         virtual ~WeatherEnvironmentActor();

      private:

         /**
          * Helper method to validate a UTC string
          * @param str The string to use to parse and validate
          * @return true on success
          */
         bool ValidateUTCString(const std::string &str);

         bool mIsCloudPlaneEnabled;
         dtCore::RefPtr<dtABC::Weather> mWeather;
         dtCore::RefPtr<dtCore::CloudPlane> mCloudPlane;
         dtABC::Weather::WeatherTheme mWeatherTheme;
   };

   class DT_PLUGIN_EXPORT WeatherEnvironmentActorProxy : public dtGame::IEnvGameActorProxy
   {
      public:

         /// Constructor
         WeatherEnvironmentActorProxy();

         /**
           * Creates the actor associated with this proxy
           */
         virtual void CreateActor() 
         { 
            SetActor(*new WeatherEnvironmentActor(*this)); 
         }

         /**
           * Builds the properties of this environment actor proxy
           */
         virtual void BuildPropertyMap();

         /**
           * Builds the invokables of this environment actor proxy
           */
         virtual void BuildInvokables();

         /**
           * Called when this actor is added to the game manager
           */
         virtual void OnEnteredWorld();

         /**
          * This actor is global
          */
         virtual bool IsPlaceable() const { return false; }

      protected:

         /// Destructor
         virtual ~WeatherEnvironmentActorProxy();
   };
}
#endif //weatherenvironmentactor_h__
