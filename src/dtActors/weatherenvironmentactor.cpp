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
#include <dtActors/weatherenvironmentactor.h>
#include <dtDAL/exceptionenum.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtUtil/datetime.h>
#include <sstream>
#include <dtCore/environment.h>

namespace dtActors
{
   /////////////////////////////////////////////////////////////
   // Actor Proxy code
   /////////////////////////////////////////////////////////////
   WeatherEnvironmentActorProxy::WeatherEnvironmentActorProxy()
   {
      SetClassName("dtCore::Environment");
   }

   WeatherEnvironmentActorProxy::~WeatherEnvironmentActorProxy()
   {
   }

   void WeatherEnvironmentActorProxy::BuildPropertyMap()
   {
      WeatherEnvironmentActor *env = static_cast<WeatherEnvironmentActor*>(GetActor());

      dtGame::GameActorProxy::BuildPropertyMap();

      AddProperty(new dtDAL::BooleanActorProperty("Enable Fog", "Enable Fog",
         dtDAL::MakeFunctor(*env, &WeatherEnvironmentActor::EnableFog),
         dtDAL::MakeFunctorRet(*env, &WeatherEnvironmentActor::IsFogEnabled),
         "Toggles fog on and off"));

      AddProperty(new dtDAL::BooleanActorProperty("Enable Cloud Plane", "Enable Cloud Plane",
         dtDAL::MakeFunctor(*env, &WeatherEnvironmentActor::EnableCloudPlane),
         dtDAL::MakeFunctorRet(*env, &WeatherEnvironmentActor::IsCloudPlaneEnabled),
         "Toggles cloud planes on and off"));

      AddProperty(new dtDAL::EnumActorProperty<WeatherEnvironmentActor::VisibilityTypeEnum>("Visibility", "Visibility",
         dtDAL::MakeFunctor(*env, &WeatherEnvironmentActor::SetWeatherVisibility),
         dtDAL::MakeFunctorRet(*env, &WeatherEnvironmentActor::GetWeatherVisibility),
         "Sets the visibility of the scene"));

      AddProperty(new dtDAL::EnumActorProperty<WeatherEnvironmentActor::WeatherThemeEnum>("Weather Theme", "Weather Theme",
         dtDAL::MakeFunctor(*env, &WeatherEnvironmentActor::SetWeatherTheme),
         dtDAL::MakeFunctorRet(*env, &WeatherEnvironmentActor::GetWeatherTheme),
         "Sets the weather theme of the scene"));

      AddProperty(new dtDAL::EnumActorProperty<WeatherEnvironmentActor::WindTypeEnum>("Wind Type", "Wind Type",
         dtDAL::MakeFunctor(*env, &WeatherEnvironmentActor::SetWindType),
         dtDAL::MakeFunctorRet(*env, &WeatherEnvironmentActor::GetWindType),
         "Sets the type of wind in the scene"));

      AddProperty(new dtDAL::EnumActorProperty<WeatherEnvironmentActor::TimePeriodEnum>("Time Period", "Time Period",
         dtDAL::MakeFunctor(*env, &WeatherEnvironmentActor::SetTimePeriod),
         dtDAL::MakeFunctorRet(*env, &WeatherEnvironmentActor::GetTimePeriod),
         "Sets the time period in the scene"));

      AddProperty(new dtDAL::EnumActorProperty<WeatherEnvironmentActor::SeasonEnum>("Season", "Season",
         dtDAL::MakeFunctor(*env, &WeatherEnvironmentActor::SetSeason),
         dtDAL::MakeFunctorRet(*env, &WeatherEnvironmentActor::GetSeason),
         "Sets the season in the scene"));

      AddProperty(new dtDAL::StringActorProperty("Time and Date", "Time and Date",
         dtDAL::MakeFunctor(*env, &WeatherEnvironmentActor::SetTimeAndDateString),
         dtDAL::MakeFunctorRet(*env, &WeatherEnvironmentActor::GetTimeAndDateString),
         "Sets the time and date of the application. This string must be in the following UTC format: yyyy-mm-ddThh:mm:ss"));

      AddProperty(new dtDAL::ColorRgbaActorProperty("Sky Color", "Sky Color",
         dtDAL::MakeFunctor(*env, &WeatherEnvironmentActor::SetSkyColor),
         dtDAL::MakeFunctorRet(*env, &WeatherEnvironmentActor::GetSkyColor),
         "Sets the sky color of this environment"));

      AddProperty(new dtDAL::FloatActorProperty("Rate of Change", "Rate of change",
         dtDAL::MakeFunctor(*env, &WeatherEnvironmentActor::SetRateOfChange),
         dtDAL::MakeFunctorRet(*env, &WeatherEnvironmentActor::GetRateOfChange),
         "Sets the rate of change of this environment. This effects how fast clouds move, etc."));
   }

   void WeatherEnvironmentActorProxy::BuildInvokables()
   {
      //WeatherEnvironmentActor *env = static_cast<WeatherEnvironmentActor*>(GetActor());

      dtGame::GameActorProxy::BuildInvokables();
   }

   void WeatherEnvironmentActorProxy::OnEnteredWorld()
   {

   }

   ////////////////////////////////////////////////////////////////////////
   // Actor code
   ////////////////////////////////////////////////////////////////////////'
   IMPLEMENT_ENUM(WeatherEnvironmentActor::CloudCoverEnum);
   WeatherEnvironmentActor::CloudCoverEnum WeatherEnvironmentActor::CloudCoverEnum::CLEAR("Clear",       dtABC::Weather::CLOUD_CLEAR);
   WeatherEnvironmentActor::CloudCoverEnum WeatherEnvironmentActor::CloudCoverEnum::FEW("Few",           dtABC::Weather::CLOUD_FEW);
   WeatherEnvironmentActor::CloudCoverEnum WeatherEnvironmentActor::CloudCoverEnum::BROKEN("Broken",     dtABC::Weather::CLOUD_BROKEN);
   WeatherEnvironmentActor::CloudCoverEnum WeatherEnvironmentActor::CloudCoverEnum::SCATTERED("Clear",   dtABC::Weather::CLOUD_SCATTERED);
   WeatherEnvironmentActor::CloudCoverEnum WeatherEnvironmentActor::CloudCoverEnum::OVERCAST("Overcast", dtABC::Weather::CLOUD_OVERCAST);

   IMPLEMENT_ENUM(WeatherEnvironmentActor::VisibilityTypeEnum);
   WeatherEnvironmentActor::VisibilityTypeEnum WeatherEnvironmentActor::VisibilityTypeEnum::VISIBILITY_UNLIMITED("Visibility Unlimited", dtABC::Weather::VIS_UNLIMITED);
   WeatherEnvironmentActor::VisibilityTypeEnum WeatherEnvironmentActor::VisibilityTypeEnum::VISIBILITY_FAR("Visibility Far", dtABC::Weather::VIS_FAR);
   WeatherEnvironmentActor::VisibilityTypeEnum WeatherEnvironmentActor::VisibilityTypeEnum::VISIBILITY_MODERATE("Visibility Moderate", dtABC::Weather::VIS_MODERATE);
   WeatherEnvironmentActor::VisibilityTypeEnum WeatherEnvironmentActor::VisibilityTypeEnum::VISIBILITY_LIMITED("Visibility Limited", dtABC::Weather::VIS_LIMITED);
   WeatherEnvironmentActor::VisibilityTypeEnum WeatherEnvironmentActor::VisibilityTypeEnum::VISIBILITY_CLOSE("Visibility Close", dtABC::Weather::VIS_CLOSE);

   IMPLEMENT_ENUM(WeatherEnvironmentActor::WeatherThemeEnum);
   WeatherEnvironmentActor::WeatherThemeEnum WeatherEnvironmentActor::WeatherThemeEnum::THEME_CUSTOM("Theme Custom", dtABC::Weather::THEME_CUSTOM);
   WeatherEnvironmentActor::WeatherThemeEnum WeatherEnvironmentActor::WeatherThemeEnum::THEME_CLEAR("Theme Clear", dtABC::Weather::THEME_CLEAR);
   WeatherEnvironmentActor::WeatherThemeEnum WeatherEnvironmentActor::WeatherThemeEnum::THEME_FAIR("Theme Fair", dtABC::Weather::THEME_FAIR);
   WeatherEnvironmentActor::WeatherThemeEnum WeatherEnvironmentActor::WeatherThemeEnum::THEME_FOGGY("Theme Foggy", dtABC::Weather::THEME_FOGGY);
   WeatherEnvironmentActor::WeatherThemeEnum WeatherEnvironmentActor::WeatherThemeEnum::THEME_RAINY("Theme Rainy", dtABC::Weather::THEME_RAINY);

   IMPLEMENT_ENUM(WeatherEnvironmentActor::TimePeriodEnum);
   WeatherEnvironmentActor::TimePeriodEnum WeatherEnvironmentActor::TimePeriodEnum::TIME_DAWN("Time Dawn", dtABC::Weather::TIME_DAWN);
   WeatherEnvironmentActor::TimePeriodEnum WeatherEnvironmentActor::TimePeriodEnum::TIME_DAY("Time Day", dtABC::Weather::TIME_DAY);
   WeatherEnvironmentActor::TimePeriodEnum WeatherEnvironmentActor::TimePeriodEnum::TIME_DUSK("Time Dusk", dtABC::Weather::TIME_DUSK);
   WeatherEnvironmentActor::TimePeriodEnum WeatherEnvironmentActor::TimePeriodEnum::TIME_NIGHT("Time Night", dtABC::Weather::TIME_NIGHT);

   IMPLEMENT_ENUM(WeatherEnvironmentActor::SeasonEnum);
   WeatherEnvironmentActor::SeasonEnum WeatherEnvironmentActor::SeasonEnum::SEASON_FALL("Season Fall", dtABC::Weather::SEASON_FALL);
   WeatherEnvironmentActor::SeasonEnum WeatherEnvironmentActor::SeasonEnum::SEASON_SPRING("Season Spring", dtABC::Weather::SEASON_SPRING);
   WeatherEnvironmentActor::SeasonEnum WeatherEnvironmentActor::SeasonEnum::SEASON_SUMMER("Season Summer", dtABC::Weather::SEASON_SUMMER);
   WeatherEnvironmentActor::SeasonEnum WeatherEnvironmentActor::SeasonEnum::SEASON_WINTER("Season Winter", dtABC::Weather::SEASON_WINTER);

   IMPLEMENT_ENUM(WeatherEnvironmentActor::WindTypeEnum);
   WeatherEnvironmentActor::WindTypeEnum WeatherEnvironmentActor::WindTypeEnum::WIND_BREEZE("Wind Breeze", dtABC::Weather::WIND_BREEZE);
   WeatherEnvironmentActor::WindTypeEnum WeatherEnvironmentActor::WindTypeEnum::WIND_NONE("Wind None", dtABC::Weather::WIND_NONE);
   WeatherEnvironmentActor::WindTypeEnum WeatherEnvironmentActor::WindTypeEnum::WIND_MODERATE("Wind Moderate", dtABC::Weather::WIND_MODERATE);
   WeatherEnvironmentActor::WindTypeEnum WeatherEnvironmentActor::WindTypeEnum::WIND_LIGHT("Wind Light", dtABC::Weather::WIND_LIGHT);
   WeatherEnvironmentActor::WindTypeEnum WeatherEnvironmentActor::WindTypeEnum::WIND_SEVERE("Wind Severe", dtABC::Weather::WIND_SEVERE);
   WeatherEnvironmentActor::WindTypeEnum WeatherEnvironmentActor::WindTypeEnum::WIND_HEAVY("Wind Heavy", dtABC::Weather::WIND_HEAVY);

   WeatherEnvironmentActor::WeatherEnvironmentActor(dtGame::GameActorProxy &proxy)
      : dtGame::IEnvGameActor(proxy)
      , mIsCloudPlaneEnabled(false)
      , mWeather(new dtABC::Weather)
      , mCloudPlane(new dtCore::CloudPlane(6, 0.5f, 6, 1, 0.3f, 0.96f, 512, 1400.0f))
      , mWeatherTheme(dtABC::Weather::THEME_CLEAR)
   {
      AddChild(mWeather->GetEnvironment());
      EnableFog(false);
      EnableCloudPlane(false);
   }

   WeatherEnvironmentActor::~WeatherEnvironmentActor()
   {
   }

   void WeatherEnvironmentActor::AddActor(dtCore::DeltaDrawable& dd)
   {
      mWeather->GetEnvironment()->AddChild(&dd);
   }

   void WeatherEnvironmentActor::RemoveActor(dtCore::DeltaDrawable& dd)
   {
      mWeather->GetEnvironment()->RemoveChild(&dd);
   }

   bool WeatherEnvironmentActor::ContainsActor(dtCore::DeltaDrawable& dd) const
   {
      return mWeather->GetEnvironment()->GetChildIndex(&dd) < mWeather->GetEnvironment()->GetNumChildren();
   }

   void WeatherEnvironmentActor::RemoveAllActors()
   {
      while (mWeather->GetEnvironment()->GetNumChildren() > 0)
      {
         mWeather->GetEnvironment()->RemoveChild(mWeather->GetEnvironment()->GetChild(0));
      }
   }

   void WeatherEnvironmentActor::GetAllActors(std::vector<dtCore::DeltaDrawable*>& vec)
   {
      vec.clear();

      for (unsigned int i = 0; i < mWeather->GetEnvironment()->GetNumChildren(); i++)
      {
         vec.push_back(mWeather->GetEnvironment()->GetChild(i));
      }
   }

   void WeatherEnvironmentActor::GetTimeAndDate(unsigned& year, unsigned& month, unsigned& day, unsigned& hour, unsigned& min, unsigned& sec) const
   {
      mWeather->GetEnvironment()->GetDateTime(year, month, day, hour, min, sec);
   }

   void WeatherEnvironmentActor::SetTimeAndDate(const unsigned year, const unsigned month, const unsigned day,
                                              const unsigned hour, const unsigned min,   const unsigned sec)
   {
      mWeather->GetEnvironment()->SetDateTime(year, month, day, hour, min, sec);
   }

   unsigned int WeatherEnvironmentActor::GetNumEnvironmentChildren() const
   {
      return mWeather->GetEnvironment()->GetNumChildren();
   }

   void WeatherEnvironmentActor::SetTimeAndDateString(const std::string& timeAndDate)
   {
      if (timeAndDate.empty())
      {
         return;
      }

      // The time is stored in the universal format of:
      // yyyy-mm-ddThh:min:ss-some number
      // So we need to use a delimeter to ensure that we don't choke on the seperators
      if (!ValidateUTCString(timeAndDate))
      {
         LOG_ERROR("The input time and date string: " + timeAndDate + " was not formatted correctly. The correct format is: yyyy-mm-ddThh:mm:ss. Ignoring.");
      }
   }

   std::string WeatherEnvironmentActor::GetCurrentTimeAndDateString() const
   {
      return dtUtil::DateTime::ToString(dtUtil::DateTime(dtUtil::DateTime::TimeOrigin::LOCAL_TIME),
         dtUtil::DateTime::TimeFormat::CALENDAR_DATE_AND_TIME_FORMAT);
   }

   dtABC::Weather& WeatherEnvironmentActor::GetWeather()
   {
      return *mWeather;
   }

   std::string WeatherEnvironmentActor::GetTimeAndDateString() const
   {
      std::ostringstream oss;
      unsigned year, month, day, hour, min, sec;
      GetTimeAndDate(year, month, day, hour, min, sec);
      oss << year << '-';
      if (month < 10)
      {
         oss << '0' << month << '-';
      }
      else
      {
         oss << month << '-';
      }

      if (day < 10)
      {
         oss << '0' << day << 'T';
      }
      else
      {
         oss << day << 'T';
      }

      if (hour < 10)
      {
         oss << '0' << hour << ':';
      }
      else
      {
         oss << hour << ':';
      }

      if (min < 10)
      {
         oss << '0' << min << ':';
      }
      else
      {
         oss << min << ':';
      }

      if (sec < 10)
      {
         oss << '0' << sec;
      }
      else
      {
         oss << sec;
      }

      return oss.str();
   }

   void WeatherEnvironmentActor::EnableCloudPlane(bool enable)
   {
      /*if (enable == false && mIsCloudPlaneEnabled)
      {
         mWeatherTheme = mWeather->GetTheme();
      }

      mIsCloudPlaneEnabled = enable;

      if (mIsCloudPlaneEnabled)
      {
         mWeather->SetTheme(mWeatherTheme);
      }
      else
      {
         mWeather->SetTheme(dtABC::Weather::THEME_CLEAR);
      }
      */
      mIsCloudPlaneEnabled = enable;

      if (mIsCloudPlaneEnabled)
      {
         mWeather->GetEnvironment()->AddEffect(mCloudPlane.get());
      }
      else
      {
         mWeather->GetEnvironment()->RemEffect(mCloudPlane.get());
      }
   }

   bool WeatherEnvironmentActor::IsCloudPlaneEnabled() const
   {
      return mIsCloudPlaneEnabled;
   }

   void WeatherEnvironmentActor::EnableFog(bool enable)
   {
      mWeather->GetEnvironment()->SetFogEnable(enable);
   }

   bool WeatherEnvironmentActor::IsFogEnabled() const
   {
      return mWeather->GetEnvironment()->GetFogEnable();
   }

   void WeatherEnvironmentActor::SetWeatherVisibility(WeatherEnvironmentActor::VisibilityTypeEnum &visibility)
   {
      mWeather->SetBasicVisibilityType(visibility.GetEnumValue());
   }

   WeatherEnvironmentActor::VisibilityTypeEnum& WeatherEnvironmentActor::GetWeatherVisibility()
   {
      for (unsigned int i = 0; i < WeatherEnvironmentActor::VisibilityTypeEnum::EnumerateType().size(); i++)
      {
         WeatherEnvironmentActor::VisibilityTypeEnum& v = *WeatherEnvironmentActor::VisibilityTypeEnum::EnumerateType()[i];

         if (mWeather->GetBasicVisibilityType() == v.GetEnumValue())
         {
            return v;
         }
      }
      return WeatherEnvironmentActor::VisibilityTypeEnum::VISIBILITY_UNLIMITED;
   }

   void WeatherEnvironmentActor::SetWeatherTheme(WeatherEnvironmentActor::WeatherThemeEnum& theme)
   {
      mWeather->SetTheme(theme.GetEnumValue());
   }

   WeatherEnvironmentActor::WeatherThemeEnum& WeatherEnvironmentActor::GetWeatherTheme()
   {
      for (unsigned int i = 0; i < WeatherEnvironmentActor::WeatherThemeEnum::Enumerate().size(); i++)
      {
         WeatherEnvironmentActor::WeatherThemeEnum& v = *WeatherEnvironmentActor::WeatherThemeEnum::EnumerateType()[i];

         if (mWeather->GetTheme() == v.GetEnumValue())
         {
            return v;
         }
      }
      return WeatherEnvironmentActor::WeatherThemeEnum::THEME_CUSTOM;
   }

   void WeatherEnvironmentActor::SetTimePeriodAndSeason(WeatherEnvironmentActor::TimePeriodEnum& time, WeatherEnvironmentActor::SeasonEnum& season)
   {
      mWeather->SetTimePeriodAndSeason(time.GetEnumValue(), season.GetEnumValue());
   }

   void WeatherEnvironmentActor::SetWindType(WeatherEnvironmentActor::WindTypeEnum& windType)
   {
      mWeather->SetBasicWindType(windType.GetEnumValue());
   }

   WeatherEnvironmentActor::WindTypeEnum& WeatherEnvironmentActor::GetWindType()
   {
      for (unsigned int i = 0; i < WeatherEnvironmentActor::WindTypeEnum::EnumerateType().size(); i++)
      {
         WeatherEnvironmentActor::WindTypeEnum& v = *WeatherEnvironmentActor::WindTypeEnum::EnumerateType()[i];

         if (mWeather->GetBasicWindType() == v.GetEnumValue())
         {
            return v;
         }
      }
      return WeatherEnvironmentActor::WindTypeEnum::WIND_NONE;
   }

   void WeatherEnvironmentActor::SetSkyColor(const osg::Vec4& color)
   {
      osg::Vec3 tempColor(color[0], color[1], color[2]);
      mWeather->GetEnvironment()->SetSkyColor(tempColor);
   }

   osg::Vec4 WeatherEnvironmentActor::GetSkyColor() const
   {
      osg::Vec3 color;
      mWeather->GetEnvironment()->GetSkyColor(color);
      return osg::Vec4(color[0], color[1], color[2], 0.0f);
   }

   void WeatherEnvironmentActor::SetRateOfChange(float rate)
   {
      mWeather->SetRateOfChange(rate);
   }

   float WeatherEnvironmentActor::GetRateOfChange() const
   {
      return mWeather->GetRateOfChange();
   }

   void WeatherEnvironmentActor::SetTimePeriod(TimePeriodEnum& period)
   {
      dtABC::Weather::Season s;
      dtABC::Weather::TimePeriod p;
      mWeather->GetTimePeriodAndSeason(&p, &s);
      mWeather->SetTimePeriodAndSeason(period.GetEnumValue(), s);
   }

   WeatherEnvironmentActor::TimePeriodEnum& WeatherEnvironmentActor::GetTimePeriod() const
   {
      for (unsigned int i = 0; i < WeatherEnvironmentActor::TimePeriodEnum::EnumerateType().size(); i++)
      {
         WeatherEnvironmentActor::TimePeriodEnum& v = *WeatherEnvironmentActor::TimePeriodEnum::EnumerateType()[i];

         dtABC::Weather::TimePeriod p;
         dtABC::Weather::Season s;
         mWeather->GetTimePeriodAndSeason(&p, &s);

         if (p == v.GetEnumValue())
         {
            return v;
         }
      }
      return WeatherEnvironmentActor::TimePeriodEnum::TIME_DAY;
   }

   void WeatherEnvironmentActor::SetSeason(SeasonEnum& season)
   {
      dtABC::Weather::TimePeriod p;
      dtABC::Weather::Season s;
      mWeather->GetTimePeriodAndSeason(&p, &s);
      mWeather->SetTimePeriodAndSeason(p, season.GetEnumValue());
   }

   WeatherEnvironmentActor::SeasonEnum& WeatherEnvironmentActor::GetSeason() const
   {
      for (unsigned int i = 0; i < WeatherEnvironmentActor::SeasonEnum::EnumerateType().size(); i++)
      {
         WeatherEnvironmentActor::SeasonEnum& v = *WeatherEnvironmentActor::SeasonEnum::EnumerateType()[i];

         dtABC::Weather::TimePeriod p;
         dtABC::Weather::Season s;
         mWeather->GetTimePeriodAndSeason(&p, &s);

         if (s == v.GetEnumValue())
         {
            return v;
         }
      }
      return WeatherEnvironmentActor::SeasonEnum::SEASON_SUMMER;
   }

   void WeatherEnvironmentActor::SetCloudCover(CloudCoverEnum& clouds)
   {
      mWeather->SetBasicCloudType(clouds.GetEnumValue());
   }

   WeatherEnvironmentActor::CloudCoverEnum& WeatherEnvironmentActor::GetCloudCover() const
   {
      for (unsigned int i = 0; i < WeatherEnvironmentActor::CloudCoverEnum::EnumerateType().size(); i++)
      {
         WeatherEnvironmentActor::CloudCoverEnum& currentCloud =
            *WeatherEnvironmentActor::CloudCoverEnum::EnumerateType()[i];

         if (mWeather->GetBasicCloudType() == currentCloud.GetEnumValue())
         {
            return currentCloud;
         }
      }
      return WeatherEnvironmentActor::CloudCoverEnum::CLEAR;
   }

   bool WeatherEnvironmentActor::ValidateUTCString(const std::string& str)
   {
      std::istringstream iss(str);
      int year, month, day, hour, min, sec;
      char delimeter;

      iss >> year;
      if (iss.fail())
      {
         return false;
      }

      iss >> delimeter;
      if (iss.fail())
      {
         return false;
      }

      iss >> month;
      if (iss.fail())
      {
         return false;
      }

      iss >> delimeter;
      if (iss.fail())
      {
         return false;
      }

      iss >> day;
      if (iss.fail())
      {
         return false;
      }

      iss >> delimeter;
      if (iss.fail())
      {
         return false;
      }

      iss >> hour;
      if (iss.fail())
      {
         return false;
      }

      iss >> delimeter;
      if (iss.fail())
      {
         return false;
      }

      iss >> min;
      if (iss.fail())
      {
         return false;
      }

      iss >> delimeter;
      if (iss.fail())
      {
         return false;
      }

      iss >> sec;
      if (iss.fail())
      {
         return false;
      }

      SetTimeAndDate(year, month, day, hour, min, sec);
      return true;
   }
}
