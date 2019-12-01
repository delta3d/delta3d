#include <dtABC/weather.h>

#include <dtCore/cloudplane.h>
#include <dtCore/environment.h>
#include <dtCore/skydome.h>
#include <dtUtil/log.h>
#include <dtUtil/mathdefines.h>

using namespace dtUtil;

namespace dtABC
{
   IMPLEMENT_MANAGEMENT_LAYER(Weather)

   /////////////////////////////////////////////////////////////////////////////
   Weather::Weather( const std::string& textureDirectory )
      : Base("Weather")
      , mClouds()
      , mEnvironment(new dtCore::Environment("weatherEnv"))
      , mCloudType(CLOUD_CLEAR)
      , mVisType(VIS_UNLIMITED)
      , mWindType(WIND_NONE)
      , mRateOfChange(0.0f)
      , mTheme(THEME_CLEAR)
      , mTimePeriod(TIME_DAY)
      , mSeason(SEASON_SUMMER)
   {
      dtCore::SkyDome* sky = new dtCore::SkyDome();
      mEnvironment->AddEffect(sky);
      mEnvironment->SetFogEnable(true);
      mEnvironment->SetFogMode(dtCore::Environment::EXP2);

      mClouds.resize(4, NULL);

      SetTheme(mTheme, textureDirectory);

      RegisterInstance(this);
   }

   /////////////////////////////////////////////////////////////////////////////
   Weather::~Weather()
   {
      DeregisterInstance(this);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Weather::SetTheme(const WeatherTheme theme, const std::string& textureDirectory)
   {
      if (theme == mTheme)
      {
         return;
      }

      switch (theme)
      {
         case THEME_CLEAR:
         {
            SetBasicWindType(WIND_BREEZE);
            SetBasicCloudType(CLOUD_CLEAR, textureDirectory);
            SetBasicVisibilityType(VIS_UNLIMITED);
            break;
         }
         case THEME_FAIR:
         {
            SetBasicWindType(WIND_LIGHT);
            SetBasicCloudType(CLOUD_FEW, textureDirectory);
            SetBasicVisibilityType(VIS_MODERATE);
            break;
         }
         case THEME_FOGGY:
         {
            SetBasicWindType(WIND_LIGHT);
            SetBasicCloudType(CLOUD_OVERCAST, textureDirectory);
            SetBasicVisibilityType(VIS_CLOSE);
            break;
         }
         case THEME_RAINY:
         {
            SetBasicWindType(WIND_MODERATE);
            SetBasicCloudType(CLOUD_OVERCAST, textureDirectory);
            SetBasicVisibilityType(VIS_LIMITED);
            break;
         }
         default:
         {
            break;
         }
      }

      //The theme will be set to CUSTOM because of the above,
      //this will reset it to what it's supposed to be
      mTheme = theme;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Weather::SetBasicCloudType(const CloudType type, const std::string& textureDirectory)
   {
      //create a set of cloud layers EnvEffects to represent the
      //supplied cloud type

      mTheme = THEME_CUSTOM;

      if (type == mCloudType)
      {
         return;
      }

      mCloudType = type;

      //remove any existing Clouds that have been added to the Env
      for (CloudPlaneList::iterator it = mClouds.begin(); it != mClouds.end(); ++it)
      {
         mEnvironment->RemEffect(it->get());
      }

      switch (mCloudType)
      {
         case CLOUD_CLEAR:
         {
            break;
         }
         case CLOUD_FEW:
         {
            if (!mClouds[CLOUD_FEW - 1].valid())
            {
               mClouds[CLOUD_FEW - 1] = new dtCore::CloudPlane(6, 0.75f, 2, 1.0f, 0.2f, 0.96f, 512, 1100.0f,
                  "Clouds Few", textureDirectory);//few
            }
            mEnvironment->AddEffect(mClouds[CLOUD_FEW-1].get());
            break;
         }
         case CLOUD_SCATTERED:
         {
            if (!mClouds[CLOUD_SCATTERED - 1].valid())
            {
               mClouds[CLOUD_SCATTERED - 1] = new dtCore::CloudPlane(6, 0.5f, 4, 1.0f, 0.3f, 0.97f,  512, 1000.0f,
                  "Clouds Scattered", textureDirectory);// scattered
            }
            mEnvironment->AddEffect(mClouds[CLOUD_SCATTERED - 1].get());
            break;
         }
         case CLOUD_BROKEN:
         {
            if (!mClouds[CLOUD_BROKEN - 1].valid())
            {
               mClouds[CLOUD_BROKEN - 1] = new dtCore::CloudPlane(6, 0.5f, 4, 1.0f, 0.3f, 0.96f,  512, 800.0f,
                  "Clouds Broken", textureDirectory); //broken
            }
            mEnvironment->AddEffect( mClouds[CLOUD_BROKEN - 1].get() );
            break;
         }
         case CLOUD_OVERCAST:
         {
            if (!mClouds[CLOUD_OVERCAST - 1].valid())
            {
               mClouds[CLOUD_OVERCAST - 1] = new dtCore::CloudPlane(6, 0.4f, 6, 1.0f, 0.2f, 0.98f,  512, 600.0f,
                  "Clouds Overcast", textureDirectory);//overcast
            }
            mEnvironment->AddEffect(mClouds[CLOUD_OVERCAST-1].get());
            break;
         }
      default:
         {
            break;
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void Weather::SetBasicWindType(const WindType windType)
   {
      mTheme = THEME_CUSTOM;

      if (mWindType != windType)
      {
         mWindType = windType;
         switch (mWindType)
         {
            case WIND_NONE:
               mEnvironment->SetWindSpeedMinMaxValues(0.0f, 1.5f);
               break;
            case WIND_LIGHT:
               mEnvironment->SetWindSpeedMinMaxValues(1.6f, 5.4f);
               break;
            case WIND_BREEZE:
               mEnvironment->SetWindSpeedMinMaxValues(5.5f, 10.7f);
               break;
            case WIND_MODERATE:
               mEnvironment->SetWindSpeedMinMaxValues(10.8f, 17.1f);
               break;
            case WIND_HEAVY:
               mEnvironment->SetWindSpeedMinMaxValues(17.2f, 24.4f);
               break;
            case WIND_SEVERE:
               mEnvironment->SetWindSpeedMinMaxValues(24.5f, 32.6f);
               break;
            case WIND_HURRICANE:
               mEnvironment->SetWindSpeedMinMaxValues(32.7f, 40.0f);
               break;
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void Weather::SetBasicWindDirection(const WindDirection windDirection)
   {
      mTheme = THEME_CUSTOM;

      if (mWindDirection != windDirection)
      {
         mWindDirection = windDirection;
         switch (mWindDirection)
         {
            case WIND_DIRECTION_NORTH:
            {
               mEnvironment->SetWindDirection(-osg::Y_AXIS);
               break;
            }
            case WIND_DIRECTION_NORTH_EAST:
            {
               osg::Vec3 direction(-1.0f, -1.0f, 0.0f);
               direction.normalize();
               mEnvironment->SetWindDirection(direction);
               break;
            }
            case WIND_DIRECTION_EAST:
            {
               mEnvironment->SetWindDirection(-osg::X_AXIS);
               break;
            }
            case WIND_DIRECTION_SOUTH_EAST:
            {
               osg::Vec3 direction(-1.0f, 1.0f, 0.0f);
               direction.normalize();
               mEnvironment->SetWindDirection(direction);
               break;
            }
            case WIND_DIRECTION_SOUTH:
            {
               mEnvironment->SetWindDirection(osg::Y_AXIS);
               break;
            }
            case WIND_DIRECTION_SOUTH_WEST:
            {
               osg::Vec3 direction(1.0f, 1.0f, 0.0f);
               direction.normalize();
               mEnvironment->SetWindDirection(direction);
               break;
            }
            case WIND_DIRECTION_WEST:
            {
               mEnvironment->SetWindDirection(osg::X_AXIS);
               break;
            }
            case WIND_DIRECTION_NORTH_WEST:
            {
               osg::Vec3 direction(1.0f, -1.0f, 0.0f);
               direction.normalize();
               mEnvironment->SetWindDirection(direction);
               break;
            }
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void Weather::SetBasicVisibilityType(const VisibilityType visType)
   {
      mTheme = THEME_CUSTOM;

      if (mVisType == visType)
      {
         return;
      }

      mVisType = visType;
      switch (mVisType)
      {
         case VIS_UNLIMITED:
            mEnvironment->SetVisibility(100000.0f);
            break;
         case VIS_FAR:
            mEnvironment->SetVisibility(40000.0f);
            break;
         case VIS_MODERATE:
            mEnvironment->SetVisibility(16000.0f);
            break;
         case VIS_LIMITED:
            mEnvironment->SetVisibility(8000.0f);
            break;
         case VIS_CLOSE:
            mEnvironment->SetVisibility(1500.0f);
            break;
         default:
            LOG_WARNING("Weather: unhandled visibility type" );
            break;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void Weather::SetRateOfChange(const float rate)
   {
      mRateOfChange = rate;
      dtUtil::Clamp<float>(mRateOfChange, -1.0f, 1.0f);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Weather::SetTimePeriodAndSeason(const TimePeriod period, const Season season)
   {
      if (mTimePeriod == period && mSeason == season)
      {
         return;
      }

      mTimePeriod = period;
      mSeason = season;

      unsigned yr, mo, da, hr, mi, sc;
      mEnvironment->GetDateTime(yr, mo, da, hr, mi, sc);

      //the times should probably be related to the horizon events
      //corresponding to the date
      switch (mTimePeriod)
      {
         case TIME_DAWN:  hr=6;  mi=0; sc=0; break;
         case TIME_DAY:   hr=12; mi=30;sc=0; break;
         case TIME_DUSK:  hr=18; mi=0; sc=0; break;
         case TIME_NIGHT: hr=23; mi=0; sc=0; break;
         default:         hr=12; mi=0; sc=0; break;
      }

      switch (mSeason)
      {
         case SEASON_SPRING: mo=3; da=15; break;
         case SEASON_SUMMER: mo=7; da=15; break;
         case SEASON_FALL:   mo=10; da=15;break;
         case SEASON_WINTER: mo=12; da=15;break;
         default:            mo=7; da=15; break;
      }

      mEnvironment->SetDateTime(yr, mo, da, hr, mi, sc);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Weather::GetTimePeriodAndSeason(TimePeriod* period, Season* season) const
   {
      *period = mTimePeriod;
      *season = mSeason;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Weather::AddChild(dtCore::DeltaDrawable *child)
   {
      return mEnvironment->AddChild(child);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Weather::RemoveChild(dtCore::DeltaDrawable* child)
   {
      if (child)
      {
         mEnvironment->RemoveChild(child);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   int Weather::SaveCloudTextures(const std::string& textureDirectory)
   {
      int saveCount = 0;

      if (mClouds[CLOUD_FEW-1].valid()
         && mClouds[CLOUD_FEW-1]->SaveTexture(textureDirectory+"/_WeatherCloudFew.png"))
         saveCount++;

      if (mClouds[CLOUD_SCATTERED-1].valid()
         && mClouds[CLOUD_SCATTERED-1]->SaveTexture(textureDirectory+"/_WeatherCloudScattered.png"))
         saveCount++;

      if (mClouds[CLOUD_BROKEN-1].valid()
         && mClouds[CLOUD_BROKEN-1]->SaveTexture(textureDirectory+"/_WeatherCloudBroken.png"))
         saveCount++;

      if (mClouds[CLOUD_OVERCAST-1].valid()
         && mClouds[CLOUD_OVERCAST-1]->SaveTexture(textureDirectory+"/_WeatherCloudOvercast.png"))
         saveCount++;

      return saveCount;
   }

   /////////////////////////////////////////////////////////////////////////////
   int Weather::LoadCloudTextures(const std::string& textureDirectory)
   {
      int loadCount = 0;

      if (mClouds[CLOUD_FEW-1].valid()
         && mClouds[CLOUD_FEW-1]->LoadTexture(textureDirectory+"/_WeatherCloudFew.png"))
         loadCount++;

      if (mClouds[CLOUD_SCATTERED-1].valid()
         && mClouds[CLOUD_SCATTERED-1]->LoadTexture(textureDirectory+"/_WeatherCloudScattered.png"))
         loadCount++;

      if (mClouds[CLOUD_BROKEN-1].valid()
         && mClouds[CLOUD_BROKEN-1]->LoadTexture(textureDirectory+"/_WeatherCloudBroken.png"))
         loadCount++;

      if (mClouds[CLOUD_OVERCAST-1].valid()
         && mClouds[CLOUD_OVERCAST-1]->LoadTexture(textureDirectory+"/_WeatherCloudOvercast.png"))
         loadCount++;

      return loadCount++;
   }
}
