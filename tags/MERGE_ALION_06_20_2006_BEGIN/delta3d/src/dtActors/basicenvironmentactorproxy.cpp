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
#include <ctime>
#include <dtActors/basicenvironmentactorproxy.h>
#include <dtDAL/exceptionenum.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtUtil/stringutils.h>

namespace dtActors
{
   /////////////////////////////////////////////////////////////
   // Actor Proxy code
   /////////////////////////////////////////////////////////////
   BasicEnvironmentActorProxy::BasicEnvironmentActorProxy()
   {
      SetClassName("dtCore::Environment");
   }

   BasicEnvironmentActorProxy::~BasicEnvironmentActorProxy()
   {

   }

   void BasicEnvironmentActorProxy::BuildPropertyMap()
   {
      BasicEnvironmentActor *env = dynamic_cast<BasicEnvironmentActor*>(mActor.get());
      if(env == NULL)
         EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be of type dtCore::Environment");

      dtGame::GameActorProxy::BuildPropertyMap();

      AddProperty(new dtDAL::BooleanActorProperty("Enable Fog", "Enable Fog",
         dtDAL::MakeFunctor(*env, &BasicEnvironmentActor::EnableFog),
         dtDAL::MakeFunctorRet(*env, &BasicEnvironmentActor::IsFogEnabled),
         "Toggles fog on and off"));

      AddProperty(new dtDAL::BooleanActorProperty("Enable Cloud Plane", "Enable Cloud Plane",
         dtDAL::MakeFunctor(*env, &BasicEnvironmentActor::EnableCloudPlane),
         dtDAL::MakeFunctorRet(*env, &BasicEnvironmentActor::IsCloudPlaneEnabled),
         "Toggles cloud planes on and off"));

      AddProperty(new dtDAL::EnumActorProperty<BasicEnvironmentActor::VisibilityTypeEnum>("Visibility", "Visibility",
         dtDAL::MakeFunctor(*env, &BasicEnvironmentActor::SetWeatherVisibility),
         dtDAL::MakeFunctorRet(*env, &BasicEnvironmentActor::GetWeatherVisibility),
         "Sets the visibility of the scene"));

      AddProperty(new dtDAL::EnumActorProperty<BasicEnvironmentActor::WeatherThemeEnum>("Weather Theme", "Weather Theme",
         dtDAL::MakeFunctor(*env, &BasicEnvironmentActor::SetWeatherTheme),
         dtDAL::MakeFunctorRet(*env, &BasicEnvironmentActor::GetWeatherTheme),
         "Sets the weather theme of the scene"));

      AddProperty(new dtDAL::EnumActorProperty<BasicEnvironmentActor::WindTypeEnum>("Wind Type", "Wind Type",
         dtDAL::MakeFunctor(*env, &BasicEnvironmentActor::SetWindType),
         dtDAL::MakeFunctorRet(*env, &BasicEnvironmentActor::GetWindType),
         "Sets the type of wind in the scene"));

      AddProperty(new dtDAL::EnumActorProperty<BasicEnvironmentActor::TimePeriodEnum>("Time Period", "Time Period",
         dtDAL::MakeFunctor(*env, &BasicEnvironmentActor::SetTimePeriod),
         dtDAL::MakeFunctorRet(*env, &BasicEnvironmentActor::GetTimePeriod),
         "Sets the time period in the scene"));

      AddProperty(new dtDAL::EnumActorProperty<BasicEnvironmentActor::SeasonEnum>("Season", "Season",
         dtDAL::MakeFunctor(*env, &BasicEnvironmentActor::SetSeason),
         dtDAL::MakeFunctorRet(*env, &BasicEnvironmentActor::GetSeason),
         "Sets the season in the scene"));

      AddProperty(new dtDAL::StringActorProperty("Time and Date", "Time and Date",
         dtDAL::MakeFunctor(*env, &BasicEnvironmentActor::SetTimeAndDateString),
         dtDAL::MakeFunctorRet(*env, &BasicEnvironmentActor::GetTimeAndDateString),
         "Sets the time and date of the application. This string must be in the following UTC format: yyyy-mm-ddThh:mm:ss"));

      AddProperty(new dtDAL::Vec3ActorProperty("Sky Color", "Sky Color",
         dtDAL::MakeFunctor(*env, &BasicEnvironmentActor::SetSkyColor),
         dtDAL::MakeFunctorRet(*env, &BasicEnvironmentActor::GetSkyColor),
         "Sets the sky color of this environment"));

      AddProperty(new dtDAL::FloatActorProperty("Rate of Change", "Rate of change",
         dtDAL::MakeFunctor(*env, &BasicEnvironmentActor::SetRateOfChange),
         dtDAL::MakeFunctorRet(*env, &BasicEnvironmentActor::GetRateOfChange),
         "Sets the rate of change of this environment. This effects how fast clouds move, etc."));
   }

   void BasicEnvironmentActorProxy::BuildInvokables()
   {
      BasicEnvironmentActor *env = dynamic_cast<BasicEnvironmentActor*>(mActor.get());
      if(env == NULL)
         EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be of type dtCore::Environment");

      dtGame::GameActorProxy::BuildInvokables();
   }

   void BasicEnvironmentActorProxy::OnEnteredWorld()
   {

   }

   ////////////////////////////////////////////////////////////////////////
   // Actor code
   ////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(BasicEnvironmentActor::VisibilityTypeEnum);
   BasicEnvironmentActor::VisibilityTypeEnum BasicEnvironmentActor::VisibilityTypeEnum::VISIBILITY_UNLIMITED("Visibility Unlimited", dtABC::Weather::VIS_UNLIMITED);
   BasicEnvironmentActor::VisibilityTypeEnum BasicEnvironmentActor::VisibilityTypeEnum::VISIBILITY_FAR("Visibility Far", dtABC::Weather::VIS_FAR);
   BasicEnvironmentActor::VisibilityTypeEnum BasicEnvironmentActor::VisibilityTypeEnum::VISIBILITY_MODERATE("Visibility Moderate", dtABC::Weather::VIS_MODERATE);
   BasicEnvironmentActor::VisibilityTypeEnum BasicEnvironmentActor::VisibilityTypeEnum::VISIBILITY_LIMITED("Visibility Limited", dtABC::Weather::VIS_LIMITED);
   BasicEnvironmentActor::VisibilityTypeEnum BasicEnvironmentActor::VisibilityTypeEnum::VISIBILITY_CLOSE("Visibility Close", dtABC::Weather::VIS_CLOSE);

   IMPLEMENT_ENUM(BasicEnvironmentActor::WeatherThemeEnum);
   BasicEnvironmentActor::WeatherThemeEnum BasicEnvironmentActor::WeatherThemeEnum::THEME_CUSTOM("Theme Custom", dtABC::Weather::THEME_CUSTOM);
   BasicEnvironmentActor::WeatherThemeEnum BasicEnvironmentActor::WeatherThemeEnum::THEME_CLEAR("Theme Clear", dtABC::Weather::THEME_CLEAR);
   BasicEnvironmentActor::WeatherThemeEnum BasicEnvironmentActor::WeatherThemeEnum::THEME_FAIR("Theme Fair", dtABC::Weather::THEME_FAIR);
   BasicEnvironmentActor::WeatherThemeEnum BasicEnvironmentActor::WeatherThemeEnum::THEME_FOGGY("Theme Foggy", dtABC::Weather::THEME_FOGGY);
   BasicEnvironmentActor::WeatherThemeEnum BasicEnvironmentActor::WeatherThemeEnum::THEME_RAINY("Theme Rainy", dtABC::Weather::THEME_RAINY);

   IMPLEMENT_ENUM(BasicEnvironmentActor::TimePeriodEnum);
   BasicEnvironmentActor::TimePeriodEnum BasicEnvironmentActor::TimePeriodEnum::TIME_DAWN("Time Dawn", dtABC::Weather::TIME_DAWN);
   BasicEnvironmentActor::TimePeriodEnum BasicEnvironmentActor::TimePeriodEnum::TIME_DAY("Time Day", dtABC::Weather::TIME_DAY);
   BasicEnvironmentActor::TimePeriodEnum BasicEnvironmentActor::TimePeriodEnum::TIME_DUSK("Time Dusk", dtABC::Weather::TIME_DUSK);
   BasicEnvironmentActor::TimePeriodEnum BasicEnvironmentActor::TimePeriodEnum::TIME_NIGHT("Time Night", dtABC::Weather::TIME_NIGHT);

   IMPLEMENT_ENUM(BasicEnvironmentActor::SeasonEnum);
   BasicEnvironmentActor::SeasonEnum BasicEnvironmentActor::SeasonEnum::SEASON_FALL("Season Fall", dtABC::Weather::SEASON_FALL);
   BasicEnvironmentActor::SeasonEnum BasicEnvironmentActor::SeasonEnum::SEASON_SPRING("Season Spring", dtABC::Weather::SEASON_SPRING);
   BasicEnvironmentActor::SeasonEnum BasicEnvironmentActor::SeasonEnum::SEASON_SUMMER("Season Summer", dtABC::Weather::SEASON_SUMMER);
   BasicEnvironmentActor::SeasonEnum BasicEnvironmentActor::SeasonEnum::SEASON_WINTER("Season Winter", dtABC::Weather::SEASON_WINTER);

   IMPLEMENT_ENUM(BasicEnvironmentActor::WindTypeEnum);
   BasicEnvironmentActor::WindTypeEnum BasicEnvironmentActor::WindTypeEnum::WIND_BREEZE("Wind Breeze", dtABC::Weather::WIND_BREEZE);
   BasicEnvironmentActor::WindTypeEnum BasicEnvironmentActor::WindTypeEnum::WIND_NONE("Wind None", dtABC::Weather::WIND_NONE);
   BasicEnvironmentActor::WindTypeEnum BasicEnvironmentActor::WindTypeEnum::WIND_MODERATE("Wind Moderate", dtABC::Weather::WIND_MODERATE);
   BasicEnvironmentActor::WindTypeEnum BasicEnvironmentActor::WindTypeEnum::WIND_LIGHT("Wind Light", dtABC::Weather::WIND_LIGHT);
   BasicEnvironmentActor::WindTypeEnum BasicEnvironmentActor::WindTypeEnum::WIND_SEVERE("Wind Severe", dtABC::Weather::WIND_SEVERE);
   BasicEnvironmentActor::WindTypeEnum BasicEnvironmentActor::WindTypeEnum::WIND_HEAVY("Wind Heavy", dtABC::Weather::WIND_HEAVY);

   BasicEnvironmentActor::BasicEnvironmentActor(dtGame::GameActorProxy &proxy) :
      dtGame::EnvironmentActor(proxy),
      mIsCloudPlaneEnabled(false),
      mWeather(new dtABC::Weather),
      mCloudPlane(0),
      mAddedActors()
   {
      mCloudPlane = new dtCore::CloudPlane(6, 0.5f, 6, 1, 0.3f, 0.96f, 512, 1400.0f);
      AddChild(mWeather->GetEnvironment());
      EnableFog(false);
      EnableCloudPlane(false);
   }

   BasicEnvironmentActor::~BasicEnvironmentActor()
   {

   }

   void BasicEnvironmentActor::AddActor(dtDAL::ActorProxy &child)
   {
      mWeather->GetEnvironment()->AddChild(child.GetActor());
      mAddedActors.insert(std::make_pair(&child, child.GetActor()));
   }

   void BasicEnvironmentActor::RemoveActor(dtDAL::ActorProxy &proxy)
   {
      mWeather->GetEnvironment()->RemoveChild(proxy.GetActor());

      std::map<dtCore::RefPtr<dtDAL::ActorProxy>, dtCore::DeltaDrawable*>::iterator i =
         mAddedActors.find(&proxy);

      if (i != mAddedActors.end())
         mAddedActors.erase(i);
   }

   bool BasicEnvironmentActor::ContainsActor(dtDAL::ActorProxy &proxy) const
   {
      std::map<dtCore::RefPtr<dtDAL::ActorProxy>, dtCore::DeltaDrawable*>::const_iterator i =
         mAddedActors.find(&proxy);

      return i != mAddedActors.end();
   }

   void BasicEnvironmentActor::RemoveAllActors()
   {
      while(mWeather->GetEnvironment()->GetNumChildren() > 0)
         mWeather->GetEnvironment()->RemoveChild(mWeather->GetEnvironment()->GetChild(0));

      mAddedActors.clear();
   }

   void BasicEnvironmentActor::GetAllActors(std::vector<dtDAL::ActorProxy*> &vec)
   {
      vec.clear();

      std::map<dtCore::RefPtr<dtDAL::ActorProxy>, dtCore::DeltaDrawable*>::iterator i;
      for(i = mAddedActors.begin(); i != mAddedActors.end(); ++i)
      {
         dtCore::RefPtr<dtDAL::ActorProxy> proxy = i->first;
         vec.push_back(proxy.get());
      }
   }

   void BasicEnvironmentActor::GetAllActors(std::vector<const dtDAL::ActorProxy*> &vec) const
   {
      vec.clear();

      std::map<dtCore::RefPtr<dtDAL::ActorProxy>, dtCore::DeltaDrawable*>::const_iterator i;
      for(i = mAddedActors.begin(); i != mAddedActors.end(); ++i)
      {
         dtCore::RefPtr<dtDAL::ActorProxy> proxy = i->first;
         vec.push_back(proxy.get());
      }
   }

   void BasicEnvironmentActor::GetTimeAndDate(int &year, int &month, int &day, int &hour, int &min, int &sec) const
   {
      mWeather->GetEnvironment()->GetDateTime(&year, &month, &day, &hour, &min, &sec);
   }

   void BasicEnvironmentActor::SetTimeAndDate(const int year, const int month, const int day,
                                              const int hour, const int min,   const int sec)
   {
      mWeather->GetEnvironment()->SetDateTime(year, month, day, hour, min, sec);
   }

   unsigned int BasicEnvironmentActor::GetNumEnvironmentChildren() const
   {
      return mAddedActors.size();
   }

   void BasicEnvironmentActor::SetTimeAndDateString(const std::string &timeAndDate)
   {
      if(timeAndDate.empty())
         return;

      std::istringstream iss(timeAndDate);
      // The time is stored in the universal format of:
      // yyyy-mm-ddThh:min:ss-some number
      // So we need to use a delimeter to ensure that we don't choke on the seperators
      if(!ValidateUTCString(iss, timeAndDate))
         LOG_ERROR("The input time and date string: " + timeAndDate + " was not formatted correctly. The correct format is: yyyy-mm-ddThh:mm:ss. Ignoring.");
   }

   std::string BasicEnvironmentActor::GetCurrentTimeAndDateString() const
   {
      time_t currentTime;
      time(&currentTime);
      return dtUtil::TimeAsUTC(currentTime);
   }

   std::string BasicEnvironmentActor::GetTimeAndDateString() const
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

   void BasicEnvironmentActor::EnableCloudPlane(bool enable)
   {
      mIsCloudPlaneEnabled = enable;

      if(mIsCloudPlaneEnabled)
         mWeather->GetEnvironment()->AddEffect(mCloudPlane.get());
      else
         mWeather->GetEnvironment()->RemEffect(mCloudPlane.get());
   }

   bool BasicEnvironmentActor::IsCloudPlaneEnabled() const
   {
      return mIsCloudPlaneEnabled;
   }

   void BasicEnvironmentActor::EnableFog(bool enable)
   {
      mWeather->GetEnvironment()->SetFogEnable(enable);
   }

   void BasicEnvironmentActor::SetWeatherVisibility(BasicEnvironmentActor::VisibilityTypeEnum &visibility)
   {
      mWeather->SetBasicVisibilityType(visibility.GetEnumValue());
   }

   BasicEnvironmentActor::VisibilityTypeEnum& BasicEnvironmentActor::GetWeatherVisibility()
   {
      for(unsigned int i = 0; i < BasicEnvironmentActor::VisibilityTypeEnum::Enumerate().size(); i++)
      {
         BasicEnvironmentActor::VisibilityTypeEnum &v = static_cast<BasicEnvironmentActor::VisibilityTypeEnum&>(*BasicEnvironmentActor::VisibilityTypeEnum::Enumerate()[i]);

         if(mWeather->GetBasicVisibilityType() == v.GetEnumValue())
            return v;
      }
      return BasicEnvironmentActor::VisibilityTypeEnum::VISIBILITY_UNLIMITED;
   }

   void BasicEnvironmentActor::SetWeatherTheme(BasicEnvironmentActor::WeatherThemeEnum &theme)
   {
      mWeather->SetTheme(theme.GetEnumValue());
   }

   BasicEnvironmentActor::WeatherThemeEnum& BasicEnvironmentActor::GetWeatherTheme()
   {
      for(unsigned int i = 0; i < BasicEnvironmentActor::WeatherThemeEnum::Enumerate().size(); i++)
      {
         BasicEnvironmentActor::WeatherThemeEnum &v = static_cast<BasicEnvironmentActor::WeatherThemeEnum&>(*BasicEnvironmentActor::WeatherThemeEnum::Enumerate()[i]);

         if(mWeather->GetTheme() == v.GetEnumValue())
            return v;
      }
      return BasicEnvironmentActor::WeatherThemeEnum::THEME_CUSTOM;
   }

   void BasicEnvironmentActor::SetTimePeriodAndSeason(BasicEnvironmentActor::TimePeriodEnum &time, BasicEnvironmentActor::SeasonEnum &season)
   {
      mWeather->SetTimePeriodAndSeason(time.GetEnumValue(), season.GetEnumValue());
   }

   void BasicEnvironmentActor::SetWindType(BasicEnvironmentActor::WindTypeEnum &windType)
   {
      mWeather->SetBasicWindType(windType.GetEnumValue());
   }

   BasicEnvironmentActor::WindTypeEnum& BasicEnvironmentActor::GetWindType()
   {
      for(unsigned int i = 0; i < BasicEnvironmentActor::WindTypeEnum::Enumerate().size(); i++)
      {
         BasicEnvironmentActor::WindTypeEnum &v = static_cast<BasicEnvironmentActor::WindTypeEnum&>(*BasicEnvironmentActor::WindTypeEnum::Enumerate()[i]);

         if(mWeather->GetBasicWindType() == v.GetEnumValue())
            return v;
      }
      return BasicEnvironmentActor::WindTypeEnum::WIND_NONE;
   }

   void BasicEnvironmentActor::SetSkyColor(const osg::Vec3 &color)
   {
      mWeather->GetEnvironment()->SetSkyColor(color);
   }

   osg::Vec3 BasicEnvironmentActor::GetSkyColor() const
   {
      osg::Vec3 color;
      mWeather->GetEnvironment()->GetSkyColor(color);
      return color;
   }

   void BasicEnvironmentActor::SetRateOfChange(float rate)
   {
      mWeather->SetRateOfChange(rate);
   }

   float BasicEnvironmentActor::GetRateOfChange() const
   {
      return mWeather->GetRateOfChange();
   }

   void BasicEnvironmentActor::SetTimePeriod(TimePeriodEnum &period)
   {
      dtABC::Weather::Season s;
      dtABC::Weather::TimePeriod p;
      mWeather->GetTimePeriodAndSeason(&p, &s);
      mWeather->SetTimePeriodAndSeason(period.GetEnumValue(), s);
   }

   BasicEnvironmentActor::TimePeriodEnum& BasicEnvironmentActor::GetTimePeriod() const
   {
      for(unsigned int i = 0; i < BasicEnvironmentActor::TimePeriodEnum::Enumerate().size(); i++)
      {
         BasicEnvironmentActor::TimePeriodEnum &v = static_cast<BasicEnvironmentActor::TimePeriodEnum&>(*BasicEnvironmentActor::TimePeriodEnum::Enumerate()[i]);

         dtABC::Weather::TimePeriod p;
         dtABC::Weather::Season s;
         mWeather->GetTimePeriodAndSeason(&p, &s);

         if(p == v.GetEnumValue())
            return v;
      }
      return BasicEnvironmentActor::TimePeriodEnum::TIME_DAY;
   }

   void BasicEnvironmentActor::SetSeason(SeasonEnum &season)
   {
      dtABC::Weather::TimePeriod p;
      dtABC::Weather::Season s;
      mWeather->GetTimePeriodAndSeason(&p, &s);
      mWeather->SetTimePeriodAndSeason(p, season.GetEnumValue());
   }

   BasicEnvironmentActor::SeasonEnum& BasicEnvironmentActor::GetSeason() const
   {
      for(unsigned int i = 0; i < BasicEnvironmentActor::SeasonEnum::Enumerate().size(); i++)
      {
         BasicEnvironmentActor::SeasonEnum &v = static_cast<BasicEnvironmentActor::SeasonEnum&>(*BasicEnvironmentActor::SeasonEnum::Enumerate()[i]);

         dtABC::Weather::TimePeriod p;
         dtABC::Weather::Season s;
         mWeather->GetTimePeriodAndSeason(&p, &s);

         if(s == v.GetEnumValue())
            return v;
      }
      return BasicEnvironmentActor::SeasonEnum::SEASON_SUMMER;
   }

   bool BasicEnvironmentActor::ValidateUTCString(std::istringstream &iss, const std::string &string)
   {
      int year, month, day, hour, min, sec;
      char delimeter;

      iss >> year;
      if(iss.fail())
         return false;

      iss >> delimeter;
      if(iss.fail())
         return false;

      iss >> month;
      if(iss.fail())
         return false;

      iss >> delimeter;
      if(iss.fail())
         return false;

      iss >> day;
      if(iss.fail())
         return false;

      iss >> delimeter;
      if(iss.fail())
         return false;

      iss >> hour;
      if(iss.fail())
         return false;

      iss >> delimeter;
      if(iss.fail())
         return false;

      iss >> min;
      if(iss.fail())
         return false;

      iss >> delimeter;
      if(iss.fail())
         return false;

      iss >> sec;
      if(iss.fail())
         return false;

      SetTimeAndDate(year, month, day, hour, min, sec);
      return true;
   }
}
