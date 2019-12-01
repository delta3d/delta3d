/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2005 MOVES Institute
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
*/

#ifndef DELTA_WEATHER
#define DELTA_WEATHER

#include <dtCore/base.h>
#include <dtCore/refptr.h>
#include <dtABC/export.h>

namespace dtCore
{
   class CloudPlane;
   class DeltaDrawable;
   class Environment;
}

namespace dtABC
{
   /**
    * High level controls for representing weather.
    * The Weather class is a high-level control for weather management.  It
    * abstracts complicated weather effects into simple controls.  There are
    * two levels of settings: basic weather types and weather themes.  The
    * basic weather types allow you to quickly setup weather by setting the
    * cloud coverage, the visibility, and the wind strength.  A representable
    * weather pattern will be displayed.
    *
    * The weather themes are pre-built weather effects that have built-in
    * parameters which encompass a particular weather pattern.  The weather
    * themes override any previously set basic weather types.
    *
    * To use Weather, instantiate the class and supply either the basic types or
    * one of the themes.
    *
    * Then add the Weather's Environment to your Scene using
    * Weather::GetEnvironment().
    *
    * Make sure to add your Drawables using Weather::AddChild().  This will
    * add the Drawable to the Weather's internal Environment, which will ensure
    * that the Drawables get affected by the fog and lighting.
    */
   class DT_ABC_EXPORT Weather : public dtCore::Base
   {

   public:

      DECLARE_MANAGEMENT_LAYER(Weather)

      Weather(const std::string& textureDirectory = "");

   protected:

      virtual ~Weather();

   public:

      enum CloudType
      {
         CLOUD_CLEAR = 0,
         CLOUD_FEW = 1,
         CLOUD_SCATTERED = 2,
         CLOUD_BROKEN = 3,
         CLOUD_OVERCAST = 4
      };

      enum WindType
      {
         WIND_NONE = 0,
         WIND_LIGHT = 1,
         WIND_BREEZE = 2,
         WIND_MODERATE = 3,
         WIND_HEAVY = 4,
         WIND_SEVERE = 5,
         WIND_HURRICANE = 6
      };

      enum WindDirection
      {
         WIND_DIRECTION_NORTH = 0,
         WIND_DIRECTION_NORTH_EAST = 1,
         WIND_DIRECTION_EAST = 2,
         WIND_DIRECTION_SOUTH_EAST = 3,
         WIND_DIRECTION_SOUTH = 4,
         WIND_DIRECTION_SOUTH_WEST = 5,
         WIND_DIRECTION_WEST = 6,
         WIND_DIRECTION_NORTH_WEST = 7
      };

      enum VisibilityType
      {
         VIS_UNLIMITED = 0, ///<no restrictions
         VIS_FAR = 1,       ///<40km
         VIS_MODERATE = 2,  ///<16km
         VIS_LIMITED = 3,   ///<8km
         VIS_CLOSE = 4      ///<1.5km
      };

      enum WeatherTheme
      {
         THEME_CUSTOM = 0,///<custom weather
         THEME_CLEAR = 1,///<no clouds, good visibility, no wind
         THEME_FAIR = 2,///<light clouds, avg. vis, light winds
         THEME_FOGGY = 3,///<low visibility, no wind
         THEME_RAINY = 4///<overcast clouds, limited vis, mod winds
      };

      enum TimePeriod
      {
         TIME_DAWN = 0,  ///<sunrise
         TIME_DAY = 1,   ///<high noon
         TIME_DUSK = 2,  ///<sunset
         TIME_NIGHT = 3  ///<night
      };

      enum Season
      {
         SEASON_SPRING = 0,
         SEASON_SUMMER = 1,
         SEASON_FALL = 2,
         SEASON_WINTER = 3
      };

      ///Creates a set of clouds to represent the generic cloud description
      void SetBasicCloudType(const CloudType type, const std::string& textureDirectory = "");
      CloudType GetBasicCloudType() const {return mCloudType;}

      ///Creates wind layers to represent the wind description
      void SetBasicWindType(const WindType windType);
      WindType GetBasicWindType()const {return mWindType;}

      ///Adjusts the wind direction of the wind layers
      void SetBasicWindDirection(const WindDirection windType);
      WindDirection GetBasicWindDirection() const { return mWindDirection; }

      ///Convenience function for the Environment
      void SetBasicVisibilityType(const VisibilityType visType);
      VisibilityType GetBasicVisibilityType() const { return mVisType; }

      ///Preset weather themes which control clouds, winds, and visibility
      void SetTheme(const WeatherTheme theme, const std::string& textureDirectory = "");
      WeatherTheme GetTheme() const {return mTheme;}

      ///Get a handle to the Weather's Environment instance
      dtCore::Environment* GetEnvironment() { return mEnvironment.get(); }

      ///const version to get a handle to the Weather's Environment instance
      const dtCore::Environment* GetEnvironment() const { return mEnvironment.get(); }

      ///Set the weather's rate of change (-1.0 to 1.0)
      void SetRateOfChange(const float rate);
      float GetRateOfChange() const {return mRateOfChange;}

      ///Set the weather's time period and season
      void SetTimePeriodAndSeason(const TimePeriod period, const Season season);
      void GetTimePeriodAndSeason(TimePeriod *period, Season *season) const;

      ///Add a DeltaDrawable to be affected by this weather's lighting and fog
      bool AddChild(dtCore::DeltaDrawable* child);
      void RemoveChild(dtCore::DeltaDrawable* child);

      ///Save all generated cloud textures to file
      ///@return The number of textures saved
      int SaveCloudTextures(const std::string& textureDirectory = "");

      ///Load all pre-generated cloud textures from file
      ///@return The number of textures loaded
      int LoadCloudTextures(const std::string& textureDirectory = "");

   private:

      typedef std::vector<dtCore::RefPtr<dtCore::CloudPlane> > CloudPlaneList;
      CloudPlaneList mClouds;
      dtCore::RefPtr<dtCore::Environment> mEnvironment;
      CloudType mCloudType;
      VisibilityType mVisType;
      WindType mWindType;
      WindDirection mWindDirection;
      float mRateOfChange; ///<weather change rate (-1 to 1)
      WeatherTheme mTheme;
      TimePeriod mTimePeriod; ///<the rough time period
      Season mSeason; ///< the current season
   };
}

#endif // DELTA_WEATHER
