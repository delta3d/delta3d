#include "weather.h"
#include "skydome.h"
#include "notify.h"

using namespace dtABC;

IMPLEMENT_MANAGEMENT_LAYER(Weather)

Weather::Weather(void):
mEnvironment(NULL),
mCloudType(CLOUD_CLEAR),
mWindType(WIND_NONE),
mVisType(VIS_UNLIMITED),
mTheme(THEME_CLEAR),
mRateOfChange(0.f)
{
   SetName("Weather");

   mEnvironment = new dtCore::Environment("weatherEnv");
   dtCore::SkyDome *sky = new dtCore::SkyDome();
   mEnvironment->AddEffect(sky);
   mEnvironment->SetFogEnable(true);
   mEnvironment->SetFogMode(dtCore::Environment::EXP2);

   SetTheme(mTheme);

   RegisterInstance(this);
}

Weather::~Weather(void)
{
   DeregisterInstance(this);
}

void Weather::SetTheme(const WeatherTheme theme)
{
   mTheme = theme;

   switch (mTheme) {
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
   
}

void Weather::SetBasicCloudType( const CloudType type)
{
   //create a set of cloud layers EnvEffects to represent the 
   //supplied cloud type
   mCloudType = type;

   switch (mCloudType) {
   case CLOUD_CLEAR:
      //remove any existing Clouds that have been added to the Env
      for (CloudPlaneList::iterator it = mClouds.begin();
           it != mClouds.end(); it++)
      {
         mEnvironment->RemEffect( it->get() );
      }
      mClouds.clear();
   	break;
   case CLOUD_FEW:
      {
         dtCore::CloudPlane *cld1 = new dtCore::CloudPlane(6, 0.75f, 2, 1, .2, .96, 512, 1100.f);
         mClouds.push_back(cld1);
         mEnvironment->AddEffect(cld1);
      }
   	break;
   case CLOUD_SCATTERED:
      {
         dtCore::CloudPlane *cld1 = new dtCore::CloudPlane(6, 0.5f, 4, 1, .3, .97,  512, 1000.f);
         mClouds.push_back(cld1);
         mEnvironment->AddEffect(cld1);
      }
   	break;
   case CLOUD_BROKEN:
      {
         dtCore::CloudPlane *cld1 = new dtCore::CloudPlane(6, 0.5f, 4, 1, .3, .96,  512, 800.f);
         mClouds.push_back(cld1);
         mEnvironment->AddEffect(cld1);
      }
   	break;
   case CLOUD_OVERCAST:
      {
         dtCore::CloudPlane *cld1 = new dtCore::CloudPlane(6, 0.4f, 6, 1, .2, .98,  512, 600.f);
         mClouds.push_back(cld1);
         mEnvironment->AddEffect(cld1);
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

void Weather::AddDrawable(dtCore::Drawable *drawable)
{
   if (drawable)  mEnvironment->AddDrawable(drawable);
}
