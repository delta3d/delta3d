#include "dtABC/weather.h"
#include "dtCore/skydome.h"
#include "dtCore/notify.h"

using namespace dtABC;

IMPLEMENT_MANAGEMENT_LAYER(Weather)

Weather::Weather(void):
mEnvironment(NULL),
mCloudType(CLOUD_CLEAR),
mWindType(WIND_NONE),
mVisType(VIS_UNLIMITED),
mTheme(THEME_CLEAR),
mRateOfChange(0.f),
mTimePeriod(TIME_DAY),
mSeason(SEASON_SUMMER)
{
   SetName("Weather");

   mEnvironment = new dtCore::Environment("weatherEnv");
   dtCore::SkyDome *sky = new dtCore::SkyDome();
   mEnvironment->AddEffect(sky);
   mEnvironment->SetFogEnable(true);
   mEnvironment->SetFogMode(dtCore::Environment::EXP2);

   mClouds.push_back(new dtCore::CloudPlane(6, 0.75f, 2, 1, .2, .96, 512, 1100.f));//few
   mClouds.push_back(new dtCore::CloudPlane(6, 0.5f, 4, 1, .3, .97,  512, 1000.f));//scattered
   mClouds.push_back(new dtCore::CloudPlane(6, 0.5f, 4, 1, .3, .96,  512, 800.f)); //broken
   mClouds.push_back(new dtCore::CloudPlane(6, 0.4f, 6, 1, .2, .98,  512, 600.f));//overcast

   SetTheme(mTheme);

   RegisterInstance(this);
}

Weather::~Weather(void)
{
   DeregisterInstance(this);
}

void Weather::SetTheme(const WeatherTheme theme)
{
   if (theme == mTheme) return;


   switch (theme) {
   case THEME_CLEAR:
      SetBasicWindType(WIND_BREEZE);
      SetBasicCloudType(CLOUD_CLEAR);
      SetBasicVisibilityType(VIS_UNLIMITED);
   	break;
   case THEME_FAIR:
      SetBasicWindType(WIND_LIGHT);
      SetBasicCloudType(CLOUD_FEW);
      SetBasicVisibilityType(VIS_MODERATE);
   	break;
   case THEME_FOGGY:
      SetBasicWindType(WIND_LIGHT);
      SetBasicCloudType(CLOUD_OVERCAST);
      SetBasicVisibilityType(VIS_CLOSE);
   	break;
   case THEME_RAINY:
      SetBasicWindType(WIND_MODERATE);
      SetBasicCloudType(CLOUD_OVERCAST);
      SetBasicVisibilityType(VIS_LIMITED);
   	break;
   default:
      break;
   }

   //The theme will be set to CUSTOM because of the above,
   //this will reset it to what it's supposed to be
   mTheme = theme;

   
}

void Weather::SetBasicCloudType( const CloudType type)
{
   //create a set of cloud layers EnvEffects to represent the 
   //supplied cloud type

   mTheme = THEME_CUSTOM;

   if (type == mCloudType) return;

   mCloudType = type;

   //remove any existing Clouds that have been added to the Env
   for (CloudPlaneList::iterator it = mClouds.begin();
      it != mClouds.end(); it++)
   {
      mEnvironment->RemEffect( it->get() );
   }


   switch (mCloudType) {
   case CLOUD_CLEAR:
   	break;
   case CLOUD_FEW:
      {
         mEnvironment->AddEffect( mClouds[CLOUD_FEW-1].get() );
      }
   	break;
   case CLOUD_SCATTERED:
      {
         mEnvironment->AddEffect( mClouds[CLOUD_SCATTERED-1].get() );
      }
   	break;
   case CLOUD_BROKEN:
      {
         mEnvironment->AddEffect( mClouds[CLOUD_BROKEN-1].get() );
      }
   	break;
   case CLOUD_OVERCAST:
      {
         mEnvironment->AddEffect( mClouds[CLOUD_OVERCAST-1].get() );
      }
     break;   
   default: 
      break;
   }

}

/**
 *
 */
void Weather::SetBasicWindType( const WindType windType)
{
   mTheme = THEME_CUSTOM;

   if (mWindType == windType) return;

   mWindType = windType;

   switch (mWindType) {
   case WIND_NONE:
   	break;
   case WIND_BREEZE:
   	break;
   case WIND_LIGHT:
   	break;
   case WIND_MODERATE:
   	break;
   case WIND_HEAVY:
      break;
   case WIND_SEVERE:
      break;
   default:
      break;
   }
   
}

/**
 *
 *
 */
void Weather::SetBasicVisibilityType(const VisibilityType visType)
{
   mTheme = THEME_CUSTOM;

   if (mVisType == visType) return;

   mVisType = visType;
   switch (mVisType) {
   case VIS_UNLIMITED:
      mEnvironment->SetVisibility(100000.f);
   	break;
   case VIS_FAR:
      mEnvironment->SetVisibility(50000.f);
   	break;
   case VIS_MODERATE:
      mEnvironment->SetVisibility(25000.f);
   	break;
   case VIS_LIMITED:
      mEnvironment->SetVisibility(8000.f);
   	break;
   case VIS_CLOSE:
      mEnvironment->SetVisibility(1500.f);
      break;
   default:
      dtCore::Notify(dtCore::WARN, "Weather: unhandled visibility type");
      break;
   }
   

}

/** 
 * 
 * @param rate : The rate of change from worsening to getting better
 *               (-1 = getting worse fast, 1 = getting better fast,
 *                 0 = stagnant)
 */
void Weather::SetRateOfChange(const float rate)
{
   mRateOfChange = rate;
   if (mRateOfChange<-1.f) mRateOfChange = -1.f;
   else if (mRateOfChange > 1.f) mRateOfChange = 1.f;

}

/** Set the Weather's rough time period.  This doesn't affect the date.
 */
void Weather::SetTimePeriodAndSeason(const TimePeriod period, const Season season)
{
   if (mTimePeriod == period && mSeason == season) return;

   mTimePeriod = period;
   mSeason = season;

   int yr, mo, da, hr, mi, sc;
   mEnvironment->GetDateTime(&yr, &mo, &da, &hr, &mi, &sc);

   //the times should probably be related to the horizon events
   //corresponding to the date
   switch ( mTimePeriod )
   {
   case TIME_DAWN:  hr=6;  mi=0; sc=0; break;      
   case TIME_DAY:   hr=12; mi=30;sc=0; break;
   case TIME_DUSK:  hr=18; mi=0; sc=0; break;
   case TIME_NIGHT: hr=23; mi=0; sc=0; break;
   default:         hr=12; mi=0; sc=0; break;
   }

   switch (mSeason) 
   {
   case SEASON_SPRING: yr=2004; mo=3; da=15; break;
   case SEASON_SUMMER: yr=2004; mo=7; da=15; break;
   case SEASON_FALL:   yr=2004; mo=10; da=15;break;
   case SEASON_WINTER: yr=2004; mo=12; da=15;break;
   default:            yr=2004; mo=7; da=15; break;
   }

   mEnvironment->SetDateTime(yr, mo, da, hr, mi, sc);
}

void Weather::GetTimePeriodAndSeason(TimePeriod *period, Season *season) const
{
   *period = mTimePeriod;
   *season = mSeason;
}


void Weather::AddChild(dtCore::DeltaDrawable *child)
{
   if (child)  mEnvironment->AddChild(child);
}
